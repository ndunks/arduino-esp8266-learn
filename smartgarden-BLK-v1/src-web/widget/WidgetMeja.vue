<template>
  <v-card>
    <v-toolbar dense flat>
      <v-toolbar-title>Meja {{ no + 1 }}</v-toolbar-title>
      <v-spacer />
      <v-chip v-text="statusText" dark :color="statusColor" />
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
        <v-btn
          @click="$store.commit('valveOn', no)"
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
import { Status } from '@/interfaces';
import { mapState } from 'vuex';
import Api from '@/api';
import { Prop, Watch } from 'vue-property-decorator';

@Component({
  computed: mapState(['status'])
})
export default class WidgetMeja extends Vue {
  @Prop({ type: Number })
  no: number;
  // Typing helper
  status: Status

  get statusText() {
    return this.status.valve[this.no] ? 'On' : 'Off'
  }

  get statusColor() {
    return this.status.valve[this.no] ? 'green' : 'gray'
  }
}
</script>
