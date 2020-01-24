import { ActionTree } from 'vuex'
import Api from '@/api'
import Axios, { AxiosInstance, AxiosError } from 'axios'
import { urlEncode } from '@/helper';
import { Popup } from '@/interfaces';
let ApiNoLoading: AxiosInstance;

const actions: ActionTree<State, any> = {
    async boot(context) {
        ApiNoLoading = Axios.create(Api.defaults);
        // Validating password
        if (Api.password) {
            await this.dispatch('settings').then(
                () => {
                    context.commit('login', true)
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
        if (!error) {
            context.commit('bootComplete')
        }
    },
    status(context) {
        // Dont update loading state
        return ApiNoLoading.get("status").then(
            response => context.commit('status', response.data)
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
        //send the request
        return Api.get(`valve?on=${valveNo}`).then(
            response => context.commit('status', response.data)
        )
    },
    valveSetting(context, params) {
        //send the request
        return Api.post(`valve?${urlEncode(params)}`).then(
            response => context.commit('settings', response.data)
        )
    },
    pumpSetting(context, params) {
        //send the request
        return Api.post(`pump?${urlEncode(params)}`).then(
            response => context.commit('settings', response.data)
        )
    }

}
export default actions;