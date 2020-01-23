import { ActionTree } from 'vuex'
import Api from '@/api'
import Axios, { AxiosInstance } from 'axios'
import { urlEncode } from '@/helper';
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
                e => Api.password = null
            )
        }
        // Load full status
        await Api.get("config").then(
            response => context.commit('status', response.data)
        )
        context.commit('bootComplete')
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