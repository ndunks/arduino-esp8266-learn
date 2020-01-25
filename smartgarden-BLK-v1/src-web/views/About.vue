<template>
  <v-container class="fill-height" fluid>
    <v-row align="center" justify="center">
      <v-card dark class="elevation-5" color="primary" min-width="400">
        <v-card-title>Smart Greenhouse</v-card-title>
        <v-card-subtitle>BLKP Klampok - Banjarnegara</v-card-subtitle>
        <v-list-item>
          <v-list-item-content>
            <v-list-item-title>Uptime</v-list-item-title>
            <v-list-item-subtitle>
              {{ status.uptime }} detik
            </v-list-item-subtitle>
          </v-list-item-content>
        </v-list-item>
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
            <v-list-item-title>Build Date</v-list-item-title>
            <v-list-item-subtitle>
              --
            </v-list-item-subtitle>
          </v-list-item-content>
        </v-list-item>
      </v-card>
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

