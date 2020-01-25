import Vue from 'vue'
import store from '@/store'
import { compareAsc, format } from 'date-fns'

const tmpDate = new Date();
Vue.filter('sinceToDate', (value: number, fallback = '-') => {
    if (!value) return fallback;
    tmpDate.setTime(store.state.deviceOn.getTime() + value * 1000)
    return tmpDate.toLocaleString()
})