import Vue from 'vue'
import store from '@/store'
import { compareAsc, format, formatDistanceToNow } from 'date-fns'
import { id as locale } from "date-fns/locale";

const tmpDate = new Date();
Vue.filter('sinceToDate', (value: number, fallback = '-') => {
    if (!value) return fallback;
    tmpDate.setTime(store.state.deviceOn.getTime() + value * 1000)
    let str = formatDistanceToNow(tmpDate, {
        includeSeconds: true,
        addSuffix: true,
        locale,
    });
    return str
})