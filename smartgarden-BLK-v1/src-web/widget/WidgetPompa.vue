<template>
  <v-card>
    <v-toolbar dense flat>
      <v-toolbar-title>Pompa</v-toolbar-title>
      <v-spacer />
      <v-chip v-text="statusText" dark :color="statusColor" />
    </v-toolbar>
    <v-list-item>
      <v-list-item-content>
        <v-list-item-title>Auto off setelah</v-list-item-title>
        <v-list-item-subtitle>{{ maxPompa }}</v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-list-item v-if="status.pompa">
      <v-list-item-content>
        <v-list-item-title>Nyala Sejak</v-list-item-title>
        <v-list-item-subtitle> ... </v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-list-item v-else>
      <v-list-item-content>
        <v-list-item-title>Mati Sejak</v-list-item-title>
        <v-list-item-subtitle> ... </v-list-item-subtitle>
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

@Component({
  computed: mapState(['status', 'settings'])
})
export default class WidgetPompa extends Vue {
  // Typing helper
  status: Status
  settings: Settings

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
}
</script>
