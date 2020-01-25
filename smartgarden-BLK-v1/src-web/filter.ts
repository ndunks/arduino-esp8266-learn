import Vue from 'vue'
import store from '@/store'
import { compareAsc, format, formatDistanceToNow } from 'date-fns'
import { id as locale } from "date-fns/locale";

const tmpDate = new Date();

function sinceToDate(value: number, fallback = '-') {
    if (!value) return fallback;
    tmpDate.setTime(store.state.deviceOn.getTime() + value * 1000)
    return filterSince(tmpDate, fallback)
}

function filterSince(value: Date, fallback = '-') {
    let str = formatDistanceToNow(value, {
        includeSeconds: true,
        addSuffix: true,
        locale,
    });
    return str
}
Vue.filter('sinceToDate', sinceToDate)
Vue.filter('since', filterSince)