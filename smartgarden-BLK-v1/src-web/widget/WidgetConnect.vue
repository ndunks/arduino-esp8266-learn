<template>
  <v-card>
    <v-toolbar short flat>
      <v-toolbar-title>Jaringan WiFi</v-toolbar-title>
      <v-spacer />
      <v-progress-circular
        v-if="loading"
        color="warning"
        indeterminate
        class="mx-auto"
      />
      <v-btn @click="scan" v-else text icon color="primary">
        <v-icon>refresh</v-icon>
      </v-btn>
    </v-toolbar>
    <v-list-item v-if="isScanning">
      <v-list-item-content>
        <v-list-item-title class="grey--text">
          Memindai jaringan WiFi..
        </v-list-item-title>
      </v-list-item-content>
    </v-list-item>
    <template v-else>
      <template v-if="wifiList.length">
        <v-list-item
          v-for="wifi in wifiList"
          :key="wifi.ssid"
          :disabled="loading"
          @click="click(wifi)"
        >
          <v-list-item-avatar>
            <v-progress-circular :value="wifi.signal" color="primary">
              {{ wifi.signal }}
            </v-progress-circular>
          </v-list-item-avatar>
          <v-list-item-content>
            <v-list-item-title v-text="wifi.ssid" />
            <v-list-item-subtitle v-text="wifi.security" />
          </v-list-item-content>
          <v-list-item-action v-if="isConnected(wifi)" class="success--text">
            Connected
          </v-list-item-action>
        </v-list-item>
      </template>
      <v-list-item v-else>
        <v-list-item-content>
          <v-list-item-title class="grey--text">
            Tidak ada jaringan WiFi ditemukan..
          </v-list-item-title>
        </v-list-item-content>
      </v-list-item>
    </template>
    <DialogConfirm
      :message="dialogMessage"
      :title="dialogTitle"
      :input-message="dialogInput"
      input-type="password"
      v-model="dialogActions"
    />
  </v-card>
</template>
<script lang="ts">
import Vue from 'vue'
import Component from 'vue-class-component';
import { Wifi, Status, ActionDialog } from '@/interfaces';
import Api, { ApiNoLoading } from '@/api';
import { mapState } from 'vuex';

@Component({
  computed: mapState(['loading', 'status'])
})
export default class WidgetConnect extends Vue {
  dialogTitle: string = null
  dialogInput: string = null
  dialogMessage: string = null
  dialogActions: ActionDialog[] = null
  wifiList: Wifi[] = []
  // Typing helper
  status: Status
  isScanning = false

  isConnected(wifi: Wifi) {
    return this.status.isConnected && this.status.ssid == wifi.ssid
  }

  scan() {
    this.isScanning = true
    Api.get("scan", {
      timeout: 20000
    }).then(
      response => {
        this.wifiList.splice(0)
        const data = response.data as string;
        let tmp;
        if (!data || !data.length) return;
        data.split("\n").filter(v => !!v).forEach(
          (v, i) => {
            // RSSI Security  SSID
            tmp = v.split("\t");
            this.wifiList.push({
              id: i,
              signal: 100 + parseInt(tmp[0]),
              security: tmp[1],
              ssid: tmp[2]
            })
          }
        )
      }
    ).finally(() => this.isScanning = false)
  }

  click(wifi: Wifi) {
    if (this.isConnected(wifi)) {
      this.disconnect(wifi)
    } else {
      this.connect(wifi)
    }
  }
  connect(wifi: Wifi) {
    const action = (pass?: string) => {

      ApiNoLoading.get('wifi', {
        params: {
          connect: wifi.id,
          pass
        },
        timeout: 10000
      })
      this.$store.commit('popup', `Menghubungkan ke ${wifi.ssid}`)
      this.$emit('done');
    }

    if (!this.status.isConnected && wifi.security == 'none') {
      return action()
    }
    if (wifi.security != 'none') {
      this.dialogInput = "Wifi password"
    } else {
      this.dialogInput = null
    }
    this.dialogTitle = `Connect to ${wifi.ssid}?`
    this.dialogMessage = "Device IP may be changed and You may lost connection."
    this.dialogActions = [
      {
        label: 'Cancel',
        color: 'error'
      },
      'spacer',
      {
        label: `OK, Connect to ${wifi.ssid}`,
        color: "success",
        isSubmitAction: true,
        action
      }
    ]
  }

  disconnect(wifi: Wifi) {
    this.dialogInput = null
    this.dialogTitle = `Disconnect from ${wifi.ssid}?`
    this.dialogMessage = "You may lost connection to this device."
    this.dialogActions = [
      {
        label: 'Cancel',
        color: 'success'
      },
      'spacer',
      {
        label: "OK, Disconnect it",
        color: "error",
        action: async () => {
          await Api.get('wifi', { params: { disconnect: true } })
          this.$store.dispatch('status')
          this.$emit('done');
        }
      }
    ]
  }
  mounted() {
    this.scan()
  }
}
</script>
