<template>
  <v-container>
    <v-row>
      <v-col cols="12" xs="12" sm="6">
        <v-form lazy-validation ref="password" @submit.prevent="submitPassword">
          <v-card>
            <v-card-title>Ubah Katasandi</v-card-title>
            <v-card-text>
              <v-text-field
                label="Katasandi Saat ini"
                v-model="password.current"
                :rules="[v => !!v || 'Harus diisi']"
                type="password"
              />
              <v-text-field
                label="Katasandi Baru"
                v-model="password.new"
                :rules="[v => !!v || 'Harus diisi']"
                type="password"
              />
            </v-card-text>
            <v-card-actions>
              <v-spacer />
              <v-btn rounded color="success" type="submit">
                Simpan
              </v-btn>
            </v-card-actions>
          </v-card>
        </v-form>
      </v-col>
      <v-col cols="12" xs="12" sm="6">
        <v-card>
          <v-card-title>Nyalakan Ulang Perangkat</v-card-title>
          <v-list-item>
            <v-btn color="warning" block rounded text outlined @click="restart">
              Nyalakan Ulang
            </v-btn>
          </v-list-item>
          <v-list-item>
            <v-btn
              color="error"
              block
              rounded
              text
              outlined
              @click="restartAndReset"
            >
              Hapus Data
            </v-btn>
          </v-list-item>
        </v-card>
      </v-col>
    </v-row>
    <v-row>
      <v-col cols="12" xs="12" sm="6">
        <widget-wifi />
      </v-col>
      <v-col cols="12" xs="12" sm="6">
        <v-form lazy-validation ref="device" @submit.prevent="submitDevice">
          <v-card>
            <v-card-title>Perangkat</v-card-title>
            <v-card-text>
              <v-text-field
                label="Nama Perangkat"
                v-model="device.name"
                :rules="[v => !!v || 'Harus diisi']"
              />
              <v-text-field
                label="Text Display"
                v-model="device.display"
                :rules="[v => !!v || 'Harus diisi']"
              />
            </v-card-text>
            <v-card-actions>
              <v-spacer />
              <v-btn rounded color="success" type="submit">
                Simpan
              </v-btn>
            </v-card-actions>
          </v-card>
        </v-form>
      </v-col>
    </v-row>
    <DialogConfirm
      :message="dialogMessage"
      :title="dialogTitle"
      v-model="dialogActions"
    />
  </v-container>
</template>
<script lang="ts">
import Vue from 'vue'
import Component from 'vue-class-component';
import { mapState } from 'vuex';
import { Settings, Popup, ActionDialog } from '@/interfaces';
import Api from '@/api';

import WidgetWifi from "@/widget/WidgetWifi.vue";

@Component({
  computed: mapState(['settings']),
  components: { WidgetWifi }
})
export default class Dashboard extends Vue {
  password = {
    current: '',
    new: ''
  }
  device = {
    name: '',
    display: ''
  }
  dialogMessage: string = null
  dialogTitle: string = null
  dialogActions: ActionDialog[] = null

  settings: Settings
  $refs: {
    password: Vue & any
    device: Vue & any
  }
  mounted() {
    this.device.name = this.settings.name
    this.device.display = this.settings.displayText
  }
  submitPassword() {
    if (!this.$refs.password.validate()) return

    if (this.settings.password != this.password.current) {
      return this.$store.commit('popup', {
        message: 'Katasandi saat ini salah',
        color: 'warning'
      } as Popup)
    }
    Api.get('set-password', {
      params: {
        password: this.password.new
      }
    }).then(
      res => {
        this.$store.commit('popup', {
          message: res.data,
          color: res.data.indexOf('OK') >= 0 ? 'success' : 'warning'
        } as Popup)
        setTimeout(() => location.reload(), 3000)
      }
    )
  }

  submitDevice() {
    if (!this.$refs.device.validate()) return
    Api.post('settings', null, { params: this.device }).then(
      res => {
        this.$store.commit('popup', {
          message: res.data,
          color: res.data.indexOf('OK') >= 0 ? 'success' : 'warning'
        } as Popup)
        this.$store.dispatch('settings');
      }
    )
  }
  restart() {
    this.dialogTitle = "Nyalakan Ulang"
    this.dialogMessage = "Nyalakan ulang perangkat?";
    this.dialogActions = [
      {
        label: 'Batal',
        color: 'success'
      },
      'spacer',
      {
        label: "Ya",
        color: "error",
        action() {
          Api.get('reboot').then(
            response =>
              setTimeout(() => location.reload(), 2000)
          )
        }
      }
    ]
  }
  restartAndReset() {
    this.dialogTitle = "Hapus Data"
    this.dialogMessage = "Semua data dan pengaturan akan dihapus. Lanjutkan?";
    this.dialogActions = [
      {
        label: 'Batal',
        color: 'success'
      },
      'spacer',
      {
        label: "Ya, hapus semua data!",
        color: "error",
        action() {
          Api.get('reboot', {
            params: {
              factory: true
            }
          }).then(
            response =>
              setTimeout(() => location.reload(), 2000)
          )
        }
      }
    ]
  }
}
</script>
