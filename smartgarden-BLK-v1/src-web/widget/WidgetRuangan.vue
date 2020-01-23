<template>
  <v-card>
    <v-toolbar dense flat>
      <v-toolbar-title>Ruangan</v-toolbar-title>
      <v-spacer />
      <v-chip v-text="statusText" dark :color="statusColor" />
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
import { Status, ActionDialog } from '@/interfaces';
import { mapState } from 'vuex';
import Api from '@/api';
import { SPRAYER_NO } from '@/constant';

@Component({
  computed: mapState(['loading', 'status'])
})
export default class WidgetRuangan extends Vue {
  // Typing helper
  no = SPRAYER_NO
  status: Status
  get statusText() {
    return 'Sprayer: ' + (this.status.sprayer ? 'On' : 'Off')
  }

  get statusColor() {
    return this.status.sprayer ? 'green' : 'gray'
  }
}
</script>
