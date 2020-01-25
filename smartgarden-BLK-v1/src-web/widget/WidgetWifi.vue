<template>
  <v-card>
    <v-card-title>Pengaturan WiFi</v-card-title>
    <v-list-item>
      <v-list-item-content>
        <v-list-item-title>WiFi Mode</v-list-item-title>
        <v-list-item-subtitle>
          {{ status.modeStr }}
        </v-list-item-subtitle>
      </v-list-item-content>
    </v-list-item>
    <v-list-item>
      <v-list-item-content>
        <v-list-item-title>Jaringan WiFi</v-list-item-title>
        <v-list-item-subtitle>
          <v-chip text :color="status.isConnected ? 'success' : 'gray'">
            {{
              status.isConnected
                ? status.ssid || "Terhubung"
                : "Tidak Terhubung"
            }}
          </v-chip>
        </v-list-item-subtitle>
      </v-list-item-content>
      <v-list-item-action>
        <v-dialog max-width="500" ref="connectDialog">
          <template #activator="{on}">
            <v-btn v-on="on" text rounded color="primary">
              Pindai WiFi
              <v-icon right> wifi </v-icon>
            </v-btn>
          </template>
          <WidgetConnect @done="$refs.connectDialog.isActive = false" />
        </v-dialog>
      </v-list-item-action>
    </v-list-item>
    <template v-if="status.isConnected">
      <v-subheader>Station Info</v-subheader>
      <v-list-item>
        <v-list-item-content>
          <v-list-item-title>SSID</v-list-item-title>
          <v-list-item-subtitle>
            {{ status.ssid }}
          </v-list-item-subtitle>
        </v-list-item-content>
      </v-list-item>
      <v-list-item>
        <v-list-item-content>
          <v-list-item-title>Status</v-list-item-title>
          <v-list-item-subtitle>
            {{ status.statusStr }}
          </v-list-item-subtitle>
        </v-list-item-content>
        <v-list-item-action>
          <v-btn color="error" text @click="setStaMode(false)">
            {{ status.isConnected ? "Putuskan" : "Non-Aktifkan" }}
          </v-btn>
        </v-list-item-action>
      </v-list-item>
      <v-list-item v-if="status.isConnected">
        <v-list-item-content>
          <v-list-item-title>Sinyal</v-list-item-title>
          <v-list-item-subtitle>
            <v-progress-circular :value="status.signal" color="primary">
              {{ status.signal }}
            </v-progress-circular>
          </v-list-item-subtitle>
        </v-list-item-content>
      </v-list-item>
      <v-list-item>
        <v-list-item-content>
          <v-list-item-title>IP</v-list-item-title>
          <v-list-item-subtitle>
            {{ status.ip }}
          </v-list-item-subtitle>
        </v-list-item-content>
      </v-list-item>
      <v-list-item>
        <v-list-item-content>
          <v-list-item-title>Gateway</v-list-item-title>
          <v-list-item-subtitle>
            {{ status.gateway }}
          </v-list-item-subtitle>
        </v-list-item-content>
      </v-list-item>
    </template>

    <v-subheader>Info AP</v-subheader>
    <template v-if="status.isApMode">
      <v-list-item>
        <v-list-item-content>
          <v-list-item-title>SSID</v-list-item-title>
          <v-list-item-subtitle>
            {{ status.ap_ssid }}
          </v-list-item-subtitle>
        </v-list-item-content>
        <v-list-item-action>
          <v-btn color="error" text @click="setApMode(false)">
            Matikan
          </v-btn>
        </v-list-item-action>
      </v-list-item>
      <v-list-item>
        <v-list-item-content>
          <v-list-item-title>IP</v-list-item-title>
          <v-list-item-subtitle>
            {{ status.ap_ip }}
          </v-list-item-subtitle>
        </v-list-item-content>
      </v-list-item>
      <v-list-item>
        <v-list-item-content>
          <v-list-item-title>Katasandi</v-list-item-title>
          <v-list-item-subtitle>
            {{ status.ap_psk || "(Kosong)" }}
          </v-list-item-subtitle>
        </v-list-item-content>
      </v-list-item>
      <v-list-item>
        <v-list-item-content>
          <v-list-item-title>Perangkat Terhubung</v-list-item-title>
          <v-list-item-subtitle>
            {{ status.ap_clients }}
          </v-list-item-subtitle>
        </v-list-item-content>
      </v-list-item>
    </template>
    <v-list-item v-else @click="setApMode(true)">
      <v-list-item-content>
        <v-list-item-title class="success--text"
          >Dimatikan (Klik untuk nyalakan)</v-list-item-title
        >
      </v-list-item-content>
    </v-list-item>
    <DialogConfirm :message="dialogMessage" v-model="dialogActions" />
  </v-card>
</template>
<script lang="ts">
import Vue from 'vue'
import Component from 'vue-class-component';
import { Status, ActionDialog } from '@/interfaces';
import { mapState } from 'vuex';
import WidgetConnect from "./WidgetConnect.vue";
import Api from '@/api';

@Component({
  components: { WidgetConnect },
  computed: mapState(['loading', 'status'])
})
export default class WidgetWifi extends Vue {
  dialogMessage: string = null
  dialogActions: ActionDialog[] = null
  // Typing helper
  status: Status

  /** Disconect STA */
  setStaMode(enable: boolean) {
    const action = () => {
      Api.get('wifi', {
        params: {
          sta: enable
        }
      })
    }
    if (enable) {
      // No confirmation
      return action();
    }

    this.dialogMessage = "Anda mungkin akan kehilangan koneksi perangkat.";
    this.dialogActions = [
      {
        label: 'Batal',
        color: 'success'
      },
      'spacer',
      {
        label: "Ya, putuskan",
        color: "error",
        action
      }
    ]
  }
  /** Disable AP Mode */
  setApMode(enable: boolean) {
    const action = () => {
      Api.get('wifi', {
        params: {
          ap: enable
        }
      }).then(response => this.$store.dispatch('status'))
    }
    if (enable) {
      // No confirmation
      return action();
    }

    this.dialogMessage = "You may lost connection to this device.";
    this.dialogActions = [
      {
        label: 'Cancel',
        color: 'success'
      },
      'spacer',
      {
        label: "OK, Disable it",
        color: "error",
        action
      }
    ]
  }
}
</script>
