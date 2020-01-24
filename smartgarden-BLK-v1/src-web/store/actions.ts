import { ActionTree } from 'vuex'
import Api from '@/api'
import Axios, { AxiosInstance } from 'axios'
import { urlEncode } from '@/helper';
import { Popup } from '@/interfaces';
let ApiNoLoading: AxiosInstance;

const actions: ActionTree<State, any> = {
    async boot(context) {
        ApiNoLoading = Axios.create(Api.defaults);
        // Validating password
        if (Api.password) {
            await Api.get('ping').then(
                resp => {
                    context.commit('login', true)
                    // auto load setttings too
                    return this.dispatch('settings')
                }
            ).catch(
                e => {
                    console.log(e)
                    Api.password = null
                }
            )
        }
        // Load full status
        let tryCount = 0;
        let error = null
        do {
            error = null
            tryCount++
            await context.dispatch('config').catch(e => error = e)
            if (error) {
                console.log('Try', tryCount, error, error.data, error.response);
            }
            if (tryCount > 5) {
                console.warn("Tired retrying..");
                context.commit('popup', {
                    message: 'Gagal terhubung ke perangkat',
                    color: 'error'
                } as Popup)
                break;
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
        )
    },
    settings(context) {
        return Api.get("settings").then(
            response => context.commit('settings', response.data)
        )
    },
    config(context) {
        return Api.get("config").then(
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