<template>
  <v-container fluid>
    <v-row>
      <v-col>
        <widget-ruangan />
      </v-col>
      <v-col>
        <widget-pompa />
      </v-col>
    </v-row>
    <v-row>
      <v-col cols="12" xs="12" sm="6" md="4">
        <widget-meja :no="0" />
      </v-col>
      <v-col cols="12" xs="12" sm="6" md="4">
        <widget-meja :no="1" />
      </v-col>
      <v-col cols="12" xs="12" sm="6" md="4">
        <widget-meja :no="2" />
      </v-col>
      <v-col cols="12" xs="12" sm="6" md="4">
        <widget-meja :no="3" />
      </v-col>
      <v-col cols="12" xs="12" sm="6" md="4">
        <widget-meja :no="4" />
      </v-col>
      <v-col cols="12" xs="12" sm="6" md="4">
        <widget-meja :no="5" />
      </v-col>
    </v-row>
  </v-container>
</template>
<script lang="ts">
import Vue from 'vue'
import Component from 'vue-class-component';
import WidgetMeja from "@/widget/WidgetMeja.vue";
import WidgetRuangan from "@/widget/WidgetRuangan.vue";
import WidgetPompa from "@/widget/WidgetPompa.vue";

@Component({
  components: { WidgetMeja, WidgetRuangan, WidgetPompa }
})
export default class Dashboard extends Vue {

}
</script>
