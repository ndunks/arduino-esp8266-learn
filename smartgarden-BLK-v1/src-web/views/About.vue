<template>
  <v-container class="fill-height">
    <v-row align="center" justify="center">
      <v-col xs="12" sm="10" lg="6">
        <v-card dark class="elevation-5" color="primary">
          <v-card-title>Smart Greenhouse</v-card-title>
          <v-card-subtitle>BLKP Klampok - Banjarnegara</v-card-subtitle>
          <v-list-item>
            <v-list-item-content>
              <v-list-item-title>Versi</v-list-item-title>
              <v-list-item-subtitle>
                1.0
              </v-list-item-subtitle>
            </v-list-item-content>
          </v-list-item>
          <v-list-item>
            <v-list-item-content>
              <v-list-item-title>Perangkat Nyala Sejak</v-list-item-title>
              <v-list-item-subtitle>
                {{ $store.state.deviceOn.toLocaleString() }} -
                {{ $store.state.deviceOn | since }}
              </v-list-item-subtitle>
            </v-list-item-content>
          </v-list-item>
          <v-list-item>
            <v-list-item-content>
              <v-list-item-title>Uptime</v-list-item-title>
              <v-list-item-subtitle>
                {{ status.uptime }} detik
              </v-list-item-subtitle>
            </v-list-item-content>
          </v-list-item>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>
<script lang="ts">
import Vue from 'vue'
import Component from 'vue-class-component';
import { mapState } from 'vuex';

@Component({
  computed: mapState(['status'])
})
export default class About extends Vue {
}
</script>

