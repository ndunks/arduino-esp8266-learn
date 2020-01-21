<template>
  <v-card>
    <v-card-title>
      Ruangan
    </v-card-title>
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
  </v-card>
</template>
<script lang="ts">
import Vue from 'vue'
import Component from 'vue-class-component';
import { Status, ActionDialog } from '@/interfaces';
import { mapState } from 'vuex';
import Api from '@/api';

@Component({
  computed: mapState(['loading', 'status'])
})
export default class WidgetRuangan extends Vue {
  // Typing helper
  status: Status
}
</script>
