import '@/style.scss'
import '@/plugin'
import '@/filter'
import '@/dialog'
import Vue from 'vue'
import router from "@/router";
import store from "@/store";
import App from '@/App.vue'
import Vuetify from 'vuetify/lib'
//import DialogConfirm from "@/dialog/DialogConfirm.vue";

Vue.config.productionTip = false

const vuetify = new Vuetify({
  icons: {
    iconfont: 'md',
  },
});

new Vue({
  router,
  store,
  ///@ts-ignore
  vuetify,
  render: h => h(App)
}).$mount('#app')

store.dispatch('boot')