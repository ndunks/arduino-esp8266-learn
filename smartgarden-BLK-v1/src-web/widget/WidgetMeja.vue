<template>
  <v-card>
    <v-toolbar dense flat>
      <v-toolbar-title>Meja {{ no + 1 }}</v-toolbar-title>
      <v-spacer />
      <v-chip v-text="statusText" dark :color="statusColor" />
      <v-dialog max-width="400" ref="dialog">
        <template #activator="{on}">
          <v-btn v-on="on" icon color="danger"><v-icon>settings</v-icon></v-btn>
        </template>
        <v-form @submit.prevent="submit" ref="form">
          <v-card>
            <v-card-title>Pengaturan Meja {{ no }}</v-card-title>
            <v-card-text>
              <v-row>
                <v-col>
                  <v-text-field
                    label="Minimal Kelembaban"
                    type="number"
                    v-model="humidity"
                  >
                    <template #append>
                      %
                    </template>
                  </v-text-field>
                </v-col>
                <v-col>
                  <v-text-field
                    label="Lama Penyiraman"
                    type="number"
                    v-model="delay"
                  >
                    <template #append>
                      Detik
                    </template>
                  </v-text-field>
                </v-col>
              </v-row>
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
    <v-card-text class="text-center">
      <v-progress-circular
        class="mx-auto"
        size="100"
        width="10"
        color="primary"
        :value="status.sensorTanah[no]"
      >
        {{ status.sensorTanah[no] }}%
      </v-progress-circular>
    </v-card-text>
    <v-list-item>
      <v-list-item-content>
        <v-list-item-title>Kelembaban Tanah</v-list-item-title>
        <v-list-item-subtitle>
          {{ status.sensorTanah[no] }}%
        </v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-list-item>
      <v-list-item-content>
        <v-list-item-title>Batas Minimum</v-list-item-title>
        <v-list-item-subtitle>
          {{ settings.humidity_minimal[no] }}%
        </v-list-item-subtitle>
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
import { Status, Settings, Popup } from '@/interfaces';
import { mapState } from 'vuex';
import Api from '@/api';
import { Prop, Watch } from 'vue-property-decorator';

@Component({
  computed: mapState(['status', 'settings'])
})
export default class WidgetMeja extends Vue {
  @Prop({ type: Number })
  no: number;
  // Typing helper
  status: Status
  settings: Settings
  humidity = 0
  delay = 0
  _required = v => !!v || 'Harus di isi.'
  $refs: {
    form: Vue & any
    dialog: Vue & any
  }

  mounted() {
    this.humidity = this.settings.humidity_minimal[this.no]
    this.delay = this.settings.valve_delay[this.no]
  }

  get statusText() {
    return this.status.valve[this.no] ? 'On' : 'Off'
  }

  get statusColor() {
    return this.status.valve[this.no] ? 'green' : 'gray'
  }

  submit() {
    if (!this.$refs.form.validate()) {
      return
    }
    this.$store.dispatch('valveSetting', {
      no: this.no,
      humidity: this.humidity,
      delay: this.delay
    }).then(
      () => this.$refs.dialog.isActive = false
    )
  }
}
</script>
