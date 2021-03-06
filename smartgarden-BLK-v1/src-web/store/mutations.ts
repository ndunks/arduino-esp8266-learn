import { Store } from 'vuex'
import { Popup, StatusRaw, ConfigRaw, isConfigRaw, isSensorsRaw, isStatusRaw, SensorsRaw } from '@/interfaces';
import { parseResponse, parseSensorRaw, parseConfigRaw, parseStatusRaw, parseSmartGardenConfig } from '@/helper';

const mutations: {
    [name: string]: (this: Store<State>, state: State, payload?: any) => any
} = {
    loading(state, loading: boolean) {
        ///@ts-ignore
        state.loading = (window.loading.counter += loading ? 1 : -1) > 0;
    },
    login(state, login: boolean) {
        state.login = login
    },
    /*     deviceOn(state, deviceOn) {
            state.deviceOn = deviceOn
        }, */
    bootComplete(state) {
        state.bootComplete = true
        state.deviceOn = new Date(Date.now() - (state.status.uptime * 1000));
        // Update device on
        //Default loading is 1 on page load, hide it now
        this.commit('loading', false);
    },
    title(state, payload: string) {
        state.title = payload
        document.title = payload
    },
    popup(state, payload: Popup | string) {
        if (typeof payload != 'object') {
            payload = {
                color: 'warning',
                message: payload
            } as Popup
        }
        payload.id = Math.round(Math.random() * 100000).toString(32) + Date.now().toString(32)
        Object.defineProperty(payload, 'state', {
            get() {
                return true
            },
            set: (v) => {
                this.commit('removePopup', payload)
            }
        })
        state.popups.push(payload)
    },
    removePopup(state, payload: Popup) {
        const index = state.popups.indexOf(payload)
        if (index >= 0) {
            state.popups.splice(index, 1)
        }
    },
    status(state, payload: string) {
        const raw = parseResponse<StatusRaw | ConfigRaw | SensorsRaw>(payload);
        const newStatus = Object.assign({}, state.status);

        // status is always included
        if (isStatusRaw(raw)) {
            parseStatusRaw(raw, newStatus)
        }
        // status sensors
        if (isSensorsRaw(raw)) {
            parseSensorRaw(raw, newStatus)
        }

        if (isConfigRaw(raw)) {
            parseConfigRaw(raw, newStatus)
        }

        this.state.status = newStatus;
    },
    settings(state, payload: string) {
        this.state.settings = parseSmartGardenConfig(payload);
    }
}
export default mutations;