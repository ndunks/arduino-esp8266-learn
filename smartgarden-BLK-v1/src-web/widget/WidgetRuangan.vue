<template>
  <v-card>
    <v-toolbar dense flat>
      <v-toolbar-title>Ruangan</v-toolbar-title>
      <v-spacer />
      <v-chip v-text="statusText" dark :color="statusColor" />
      <v-dialog max-width="600" ref="dialog">
        <template #activator="{on}">
          <v-btn v-on="on" icon color="danger"><v-icon>settings</v-icon></v-btn>
        </template>
        <v-form @submit.prevent="submit" ref="form">
          <v-card>
            <v-card-title
              >Pengaturan Suhu &amp; Kelembaban Ruangan</v-card-title
            >
            <v-card-text>
              <v-text-field
                label="Maksimal Suhu"
                type="number"
                v-model="edit.temp"
                filled
                persistent-hint
                hint="Pengembunan secara otomatis ketika suhu mencapai batas yang ditentukan"
              >
                <template #append>
                  &deg;C
                </template>
              </v-text-field>

              <v-text-field
                label="Minimal Kelembaban"
                type="number"
                v-model="edit.humidity"
                filled
                persistent-hint
                hint="Pengembunan secara otomatis ketika kelembaban kurang dari batas yang ditentukan"
              >
                <template #append>
                  %
                </template>
              </v-text-field>

              <v-text-field
                label="Lama Pengembunan"
                type="number"
                v-model="edit.delay"
                filled
                persistent-hint
                hint="Lama waktu dalam satu sesi pengembunan"
              >
                <template #append>
                  Detik
                </template>
              </v-text-field>
              <v-text-field
                label="Delay Pengembunan"
                type="number"
                v-model="edit.gap"
                filled
                persistent-hint
                hint="Delay spasi pengembunan sebelum melakukan pengembunan selanjutnya"
              >
                <template #append>
                  Detik
                </template>
              </v-text-field>
            </v-card-text>
            <v-card-actions>
              <v-btn color="error" text @click="$refs.dialog.isActive = false">
                Batal
              </v-btn>
              <v-spacer />
              <v-btn color="success" text @click="submit">
                Simpan
              </v-btn>
            </v-card-actions>
          </v-card>
        </v-form>
      </v-dialog>
    </v-toolbar>
    <v-list-item>
      <v-list-item-content>
        <v-list-item-title>Suhu</v-list-item-title>
        <v-list-item-subtitle> {{ status.temp }}&deg;C </v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-list-item>
      <v-list-item-content>
        <v-list-item-title>Kelembaban</v-list-item-title>
        <v-list-item-subtitle> {{ status.hum }}% </v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-list-item>
      <v-list-item-content>
        <v-btn
          @click="$store.dispatch('valveOn', no)"
          color="success"
          block
          rounded
        >
          Nyalakan
        </v-btn>
      </v-list-item-content>
    </v-list-item>
  </v-card>
</template>
<script lang="ts">
import Vue from 'vue'
import Component from 'vue-class-component';
import { Status, ActionDialog, Settings } from '@/interfaces';
import { mapState } from 'vuex';
import Api from '@/api';
import { SPRAYER_NO } from '@/constant';

@Component({
  computed: mapState(['loading', 'status', 'settings'])
})
export default class WidgetRuangan extends Vue {
  edit = {
    no: SPRAYER_NO,
    temp: 0,
    humidity: 0,
    delay: 0,
    gap: 0
  }

  // Typing helper
  no = SPRAYER_NO
  status: Status
  settings: Settings
  $refs: {
    form: Vue & any
    dialog: Vue & any
  }

  mounted() {
    this.edit.humidity = this.settings.humidity_minimal[this.no]
    this.edit.delay = this.settings.valve_delay[this.no]
    this.edit.gap = this.settings.valve_gap[this.no]
    this.edit.temp = this.settings.temperature_max
  }

  get statusText() {
    return 'Sprayer: ' + (this.status.sprayer ? 'On' : 'Off')
  }

  get statusColor() {
    return this.status.sprayer ? 'green' : 'gray'
  }
  submit() {
    if (!this.$refs.form.validate()) {
      return
    }
    this.$store.dispatch('valveSetting', this.edit).then(
      () => this.$refs.dialog.isActive = false
    )
  }
}
</script>
