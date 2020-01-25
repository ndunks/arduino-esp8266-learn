import { ActionTree } from 'vuex'
import Api, { ApiNoLoading } from '@/api'
import Axios, { AxiosInstance, AxiosError } from 'axios'
import { urlEncode } from '@/helper';
import { Popup } from '@/interfaces';


const actions: ActionTree<State, any> = {
    async boot(context) {
        // Validating password
        let isLoginValid = false;
        if (Api.password) {
            await this.dispatch('settings').then(
                () => {
                    isLoginValid = true;
                }
            ).catch(
                (e: AxiosError) => {
                    if (e.isAxiosError && e.response) {
                        Api.password = null
                    }
                }
            )
        }
        // Load full status
        let tryCount = 0
        let error: AxiosError
        do {
            error = null
            tryCount++
            await context.dispatch('config').catch(e => error = e)
            if (error) {
                // break when network error
                if (error.message.match(/network/i)) {
                    break;
                }
                console.log('Error', error.message);

                if (tryCount > 5) {
                    console.warn("Tired retrying..");
                    context.commit('popup', {
                        message: 'Gagal terhubung ke perangkat',
                        color: 'error'
                    } as Popup)
                    break;
                }
            }
        } while (error)
        if (error) {
            context.commit('popup', 'Koneksi ke perangkat GAGAL')
        } else {
            context.commit('login', isLoginValid)
            context.commit('bootComplete')
        }
    },
    status(context) {
        // Dont update loading state
        return ApiNoLoading.get("status").then(
            response => {
                const oldStatusMode = `${context.state.status.status}${context.state.status.mode}`
                context.commit('status', response.data)
                // If mode or status changed, load full status config
                if (oldStatusMode != `${context.state.status.status}${context.state.status.mode}`) {
                    this.dispatch('config')
                }
            }
        ).catch(e => console.warn('status check fail'))
    },
    settings(context) {
        return ApiNoLoading.get("settings").then(
            response => context.commit('settings', response.data)
        )
    },
    config(context) {
        return ApiNoLoading.get("config").then(
            response => context.commit('status', response.data)
        )
    },
    valveOn(context, valveNo) {
        return Api.get('valve', { params: { no: valveNo } }).then(
            response => context.commit('status', response.data)
        )
    },
    valveSetting(context, params) {
        return Api.post('valve', null, { params }).then(
            response => context.commit('settings', response.data)
        )
    },
    pumpSetting(context, params) {
        return Api.post('pump', null, { params }).then(
            response => context.commit('settings', response.data)
        )
    },
    valveAllOff(context, lamaMati) {
        return Api.get('valve', { params: { no: -1, off: lamaMati } }).then(
            response => context.commit('status', response.data)
        )
    },
}
export default actions;