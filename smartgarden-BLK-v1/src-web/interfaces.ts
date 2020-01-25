export interface Popup {
    id?: string,
    color?: 'error' | 'info' | 'success' | 'warning' | string,
    message: string
}

export interface Wifi {
    id: number
    ssid: string
    security: 'none' | 'wep' | 'tkip' | 'ccmp' | 'auto'
    signal: number
}
export interface ActionDialogObject {
    label: string
    action?: (inputValue?: string) => void | boolean | Promise<any>
    color?: string
    isSubmitAction?: boolean
}

export type ActionDialog = ActionDialogObject | string

export interface ConfigDetailRaw {
}

export interface ConfigRaw extends SensorsRaw {
    hostname: string
    autoconnect: string
    ssid: string
    ip: string
    gateway: string
    ap_ssid: string
    ap_ip: string
    ap_psk: string
    // in booth config & status response //
    heap: string
    mode: string
    status: string
    rssi: string
    ap_clients: string
    uptime: string
}

export type StatusRaw = Pick<ConfigRaw, 'heap' | 'mode' | 'status' | 'rssi' | 'ap_clients' | 'uptime'>

// Config/Status type guad
export function isConfigRaw(raw): raw is ConfigRaw {
    return (raw as ConfigRaw).hostname !== undefined;
}
export function isStatusRaw(raw): raw is StatusRaw {
    return (raw as ConfigRaw).status !== undefined;
}

export function isSensorsRaw(raw): raw is SensorsRaw {
    return (raw as ConfigRaw).in !== undefined;
}

/*
typedef enum WiFiMode 
{
    WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3,
    WIFI_SHUTDOWN = 4, WIFI_RESUME = 8
} WiFiMode_t;
*/

export enum WifiMode {
    Off, Station, 'Access Point', 'Access Point & Station',
    Shutdown, 'Wifi Resume' = 8
}
/*
ESP8266WiFi/src/include/wl_definitions.h
typedef enum {
    WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6
} wl_status_t;
*/
export enum WifiStatus {
    Idle,
    'No SSID Available',
    'Scan Completed',
    Connected,
    'Connect Failed',
    'Connection Lost',
    Disconnected,
    'No Shield' = 255,
}
export interface Config {
    heap: {
        free: number
        max: number
        frag: number
    }
    mode: WifiMode
    hostname: string
    autoconnect: boolean
    // If Mode Station is active
    status?: WifiStatus
    ssid?: string
    // if status connected
    rssi?: number
    ip?: string
    gateway?: string
    // If mode AP Enabled
    ap_ssid?: string
    ap_ip?: string
    ap_psk?: string
    ap_clients: number

}

export interface Status extends Config, Sensors {

    signal?: number // converted from 100 + rssi (rssi is negative)
    isConnected: boolean // from status 
    modeStr: string
    statusStr?: string
    isApMode: boolean // bitwise check from mode
    isStaMode: boolean // bitwise check from mode
    uptime: number
}

export interface SensorsRaw {
    /** Sensor separated by space */
    in: string
    /** 8 bit valve & other output state */
    // out: string
    temp: string
    hum: string
    cur: string
    pompa: string
    /** Lama pompa ON */
    pompa_on?: string
    /** Sisa pompa off */
    pompa_off?: string
}

export interface Sensors extends Pick<SensorsRaw, 'in'> {
    sensorTanah: number[]
    valve: boolean[]
    sprayer: boolean
    // Pompa status
    pompa: boolean
    temp: number
    hum: number
    // Current valve on
    cur: number
    /** Lama pompa ON */
    pompa_on?: number
    /** Sisa pompa off */
    pompa_off?: number
}

export interface Settings {
    // Boot status flag
    flag: number
    // Delay membaca sensor analog
    sensor_delay: number
    // max temperatur to trigger sprayer in celcius
    temperature_max: number
    // Delay valve in second
    valve_delay: number[]
    // Delay space before next check
    valve_gap: number[]
    // minimal kelembaban utk men trigger valve
    humidity_minimal: number[]
    // Maksimal pompa menyala (detik)
    maksimal_pompa_hidup: number
    // Maksimal pompa mati/istirahat (detik)
    maksimal_pompa_mati: number
    // Device name / hostname
    name: string
    // LCD Text when iddle
    displayText: string
    // Web Password
    password: string
};