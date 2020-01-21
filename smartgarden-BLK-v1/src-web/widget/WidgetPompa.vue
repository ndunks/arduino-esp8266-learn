<template>
  <v-card>
    <v-card-title>
      Pompa
    </v-card-title>
    <v-list-item>
      <v-list-item-content>
        <v-list-item-title>Pompa</v-list-item-title>
        <v-list-item-subtitle> {{ statusPompa }}</v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-list-item v-if="status.pump">
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
import { Status, ActionDialog } from '@/interfaces';
import { mapState } from 'vuex';
import Api from '@/api';

@Component({
  computed: mapState(['status'])
})
export default class WidgetPompa extends Vue {
  // Typing helper
  status: Status

  get statusPompa() {
    return status.pump ? 'Nyala' : 'Mati';
  }
}
</script>
