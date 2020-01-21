<template>
  <v-card>
    <v-card-title> Meja {{ no }} </v-card-title>
    <v-card-text class="text-center">
      <v-progress-circular
        class="mx-auto"
        size="100"
        width="10"
        color="primary"
        :value="status[`S${no}`]"
      >
        {{ status[`S${no}`] }}%
      </v-progress-circular>
    </v-card-text>
    <v-list-item>
      <v-list-item-content>
        <v-list-item-title>Kelembaban Tanah</v-list-item-title>
        <v-list-item-subtitle> {{ status[`S${no}`] }}% </v-list-item-subtitle>
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
import { Prop } from 'vue-property-decorator';

@Component({
  computed: mapState(['status'])
})
export default class WidgetMeja extends Vue {
  @Prop({ type: Number })
  no: number;
  // Typing helper
  status: Status
}
</script>
