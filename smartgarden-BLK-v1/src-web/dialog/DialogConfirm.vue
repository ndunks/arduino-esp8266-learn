<template>
  <v-dialog :value="visible" max-width="450">
    <v-card>
      <v-card-title v-text="title" />
      <v-card-text v-text="message" />
      <v-card-text v-if="hasInput">
        <v-text-field
          autofocus
          autocomplete="off"
          v-model="inputValue"
          :type="inputType"
          :label="inputMessage"
          @keydown.enter="submit"
        />
      </v-card-text>
      <v-card-actions v-if="visible">
        <template v-for="action in value">
          <v-spacer :key="action.label" v-if="typeof action == 'string'" />
          <v-btn
            v-else
            text
            :key="action.label"
            v-text="action.label"
            @click="doAction(action.action)"
            :color="action.color"
          />
        </template>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>
<script lang="ts">
import Vue from 'vue'
import Component from 'vue-class-component';
import { Prop, Watch } from 'vue-property-decorator';
import { ActionDialog, ActionDialogObject } from '../interfaces';

@Component
export default class DialogConfirm extends Vue {

  @Prop({ type: String, default: "Konfirmasi" })
  title: string

  @Prop({ type: String, default: null })
  inputMessage: string

  @Prop({ type: String, default: "text" })
  inputType: string

  @Prop({ type: String, default: "" })
  message: string

  @Prop({ type: Array, default: undefined })
  value: ActionDialog[]

  inputValue: string = null

  get visible() {
    return Array.isArray(this.value) && this.value.length > 0
  }

  get hasInput() {
    return 'string' == typeof this.inputMessage && this.inputMessage.length > 0
  }

  async doAction(action: (inputValue: any) => void | boolean | Promise<any>) {
    if ('undefined' == typeof action) {
      return this.$emit('input', null)
    }
    let result = action(this.inputValue);
    if (result instanceof Promise) {
      console.log('Confirm wait action');

      result = await result;
    }
    console.log('Confirm result', result);
    if (result !== false) {
      // Close the dialog
      return this.$emit('input', null)
    }

  }
  submit() {
    const act =
      this.value.find((v: ActionDialogObject) => v.isSubmitAction) as ActionDialogObject
    if (act) {
      this.doAction(act.action as any)
    }

  }
}
</script>
