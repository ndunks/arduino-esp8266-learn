<template>
  <v-card>
    <v-toolbar dense flat>
      <v-toolbar-title>Pompa</v-toolbar-title>
      <v-spacer />
      <v-chip v-text="statusText" dark :color="statusColor" />
      <v-dialog max-width="500" ref="dialog">
        <template #activator="{on}">
          <v-btn v-on="on" icon color="danger"><v-icon>settings</v-icon></v-btn>
        </template>
        <v-form @submit.prevent="submit" ref="form">
          <v-card>
            <v-card-title>
              Pengaturan Pompa
            </v-card-title>
            <v-card-text>
              <v-text-field
                label="Maksimal Lama Menyala"
                type="number"
                v-model="edit.maxon"
                filled
                persistent-hint
                hint="Maksimal lama pompa menyala"
              >
                <template #append>
                  Detik
                </template>
              </v-text-field>

              <v-text-field
                label="Lama Mati"
                type="number"
                v-model="edit.maxoff"
                filled
                persistent-hint
                hint="Lama pompa istirahat ketika melebihi batas maksimal nyala"
              >
                <template #append>
                  Detik
                </template>
              </v-text-field>
              <v-text-field
                label="Delay Valve Off"
                type="number"
                v-model="edit.delay_valve_off"
                filled
                persistent-hint
                hint="Tutup valve setelah delay habis untuk menghilangkan tekanan"
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
        <v-list-item-title>Auto off setelah</v-list-item-title>
        <v-list-item-subtitle>{{ maxPompa }}</v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-list-item v-if="status.pompa">
      <v-list-item-content>
        <v-list-item-title>Lama Nyala</v-list-item-title>
        <v-list-item-subtitle> {{ status.pompa_on }} </v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-list-item v-if="status.pompa_off">
      <v-list-item-content>
        <v-list-item-title>Off</v-list-item-title>
        <v-list-item-subtitle> {{ status.pompa_off }} </v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-card-actions>
      <v-dialog max-width="400" ref="dialogMatikan">
        <template #activator="{on}">
          <v-btn v-on="on" text outlined block color="warning">
            Matikan Semua
          </v-btn>
        </template>
        <v-form @submit.prevent="submitMatikan">
          <v-card>
            <v-card-title>Matikan Semua</v-card-title>
            <v-card-text>
              <v-text-field
                label="Matikan selama"
                type="number"
                v-model="lamaMati"
                filled
                persistent-hint
                hint="Matikan pompa dan semua penyiraman selama waktu yang ditentukan"
              >
                <template #append>
                  Detik
                </template>
              </v-text-field>
            </v-card-text>
            <v-card-actions>
              <v-btn
                color="error"
                text
                @click="$refs.dialogMatikan.isActive = false"
              >
                Batal
              </v-btn>
              <v-spacer />
              <v-btn color="success" text type="submit">
                Matikan Sekarang
              </v-btn>
            </v-card-actions>
          </v-card>
        </v-form>
      </v-dialog>
    </v-card-actions>
  </v-card>
</template>
<script lang="ts">
import Vue from 'vue'
import Component from 'vue-class-component';
import { Status, ActionDialog, Settings } from '@/interfaces';
import { mapState } from 'vuex';
import Api from '@/api';

@Component({
  computed: mapState(['status', 'settings'])
})
export default class WidgetPompa extends Vue {
  edit = {
    maxon: 0,
    maxoff: 0,
    delay_valve_off: 0
  }
  lamaMati = 20
  // Typing helper
  status: Status
  settings: Settings

  $refs: {
    form: Vue & any
    dialog: Vue & any
    dialogMatikan: Vue & any
  }

  mounted() {
    this.edit.maxon = this.settings.maksimal_pompa_hidup
    this.edit.maxoff = this.settings.maksimal_pompa_mati
    this.edit.delay_valve_off = this.settings.delay_valve_off
  }

  get statusText() {
    return this.status.pompa ? 'On' : 'Off'
  }

  get statusColor() {
    return this.status.pompa ? 'green' : 'gray'
  }

  get maxPompa() {
    if (this.settings.maksimal_pompa_hidup > 60) {
      return (this.settings.maksimal_pompa_hidup / 60).toFixed(2) + ' Menit'
    } else {
      return this.settings.maksimal_pompa_hidup + ' Detik'
    }
  }
  submit() {
    if (!this.$refs.form.validate()) {
      return
    }

    this.$store.dispatch('pumpSetting', this.edit).then(
      () => this.$refs.dialog.isActive = false
    )
  }
  submitMatikan() {
    this.$store.dispatch('valveAllOff', this.lamaMati).then(
      () => this.$refs.dialogMatikan.isActive = false
    )
  }
}
</script>
