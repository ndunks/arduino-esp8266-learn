import { ConfigRaw, Status, WifiMode, StatusRaw, WifiStatus, SensorsRaw, Settings } from './interfaces';
import { VALVE_COUNT, SPRAYER_NO } from './constant';

export function urlEncode(params: any) {
    return Object.keys(params).map(
        f => `${f}=${escape(params[f])}`
    ).join('&')
}
/**
 * Parse key-value separate by tab
 */
export function parseResponse<T = any>(res: string): T {
    const result = {} as any;
    res.split("\n").forEach(
        s => {
            const [key, value] = s.split("\t", 2);
            if ('undefined' == typeof result[key]) {
                result[key] = value
            } else {
                result[key] = Array.isArray(result[key]) ? result[key] : [result[key]]
                result[key].push(value)
            }
        }
    )
    return result;
}

export function parseConfigRaw(raw: ConfigRaw, newStatus: Status) {
    newStatus.hostname = raw.hostname
    newStatus.autoconnect = parseInt(raw.autoconnect) > 0
    newStatus.ssid = raw.ssid
    newStatus.ip = raw.ip
    newStatus.gateway = raw.gateway
    newStatus.ap_ssid = raw.ap_ssid
    newStatus.ap_ip = raw.ap_ip
    newStatus.ap_psk = raw.ap_psk
    let mode = parseInt(raw.mode)
    newStatus.isApMode = !!(mode & WifiMode["Access Point"])
    newStatus.isStaMode = !!(mode & WifiMode.Station)
}

export function parseStatusRaw(raw: StatusRaw, newStatus: Status) {

    const [free, max, frag] = raw.heap.split(' ', 3)
    newStatus.heap = {
        free: parseInt(free),
        max: parseInt(max),
        frag: parseInt(frag)
    }
    newStatus.mode = parseInt(raw.mode)
    newStatus.modeStr = WifiMode[newStatus.mode] || `Unknown ${raw.mode}`
    newStatus.status = parseInt(raw.status)
    newStatus.statusStr = WifiStatus[newStatus.status]
    newStatus.rssi = parseInt(raw.rssi)
    newStatus.signal = 100 + parseInt(raw.rssi)
    newStatus.ap_clients = parseInt(raw.ap_clients)
    newStatus.uptime = parseInt(raw.uptime)
    newStatus.isConnected = newStatus.status == WifiStatus.Connected

}

export function parseSensorRaw(raw: SensorsRaw, newStatus: Status) {
    newStatus.sensorTanah = raw.in.trim().split(' ').map(
        v => parseInt(v)
    )

    newStatus.temp = parseInt(raw.temp)
    newStatus.hum = parseInt(raw.hum)
    newStatus.cur = parseInt(raw.cur)
    // loop all valves including sprayer
    for (let i = 0; i < VALVE_COUNT; i++) {
        newStatus.valve[i] = newStatus.cur == i

    }
    newStatus.sprayer = SPRAYER_NO == newStatus.cur;
    newStatus.pompa = !!parseInt(raw.pompa);
    newStatus.pompa_on = parseInt(raw.pompa_on);
    if (typeof raw.pompa_off == 'undefined') {
        newStatus.pompa_off = null;
    } else {
        newStatus.pompa_off = parseInt(raw.pompa_off);
    }
    // Parse states
    let state = parseInt(raw.state)
    for (let i = 0; i < VALVE_COUNT; i++) {
        newStatus.needWater[i] = (state >> i & 1) > 0;
    }
    raw.laston.trim().split(' ').forEach(
        (v, i) => newStatus.lastOn[i] = parseInt(v)
    )
}

export function parseSmartGardenConfig(raw: string) {

    let cfg: Settings = {} as any;
    const rawStr = atob(raw)
    const len = rawStr.length
    let buff = new ArrayBuffer(len)

    let buffView = new Uint8Array(buff)

    for (let i = 0; i < len; i++) {
        buffView[i] = rawStr.charCodeAt(i)
    }
    let offset = 0
    cfg.valve_delay = []
    cfg.humidity_minimal = []

    // uint8
    cfg.flag = buffView[offset++]
    // uint8
    cfg.sensor_delay = buffView[offset++]
    // uint8
    cfg.temperature_max = buffView[offset++]
    // uint8 * VALVE_COUNT
    cfg.valve_delay = Array.prototype.slice.call(buffView, offset, offset + VALVE_COUNT)
    offset += VALVE_COUNT
    // uint8 * VALVE_COUNT
    cfg.humidity_minimal = Array.prototype.slice.call(buffView, offset, offset + VALVE_COUNT)
    offset += VALVE_COUNT
    // uint8 * VALVE_COUNT
    cfg.valve_gap = Array.prototype.slice.call(buffView, offset, offset + VALVE_COUNT)
    offset += VALVE_COUNT
    // unsigned long;
    let tmpLong = new Uint32Array(buffView.buffer.slice(offset, offset + 4))
    offset += 4;
    cfg.maksimal_pompa_hidup = tmpLong[0]

    // unsigned long;
    tmpLong = new Uint32Array(buffView.buffer.slice(offset, offset + 4))
    offset += 4;
    cfg.maksimal_pompa_mati = tmpLong[0]

    // char name[21];
    cfg.name = readNullString(buffView, offset, 21)
    offset += 21
    // char displayText[21];
    cfg.displayText = readNullString(buffView, offset, 21)
    offset += 21
    // char password[9];
    cfg.password = readNullString(buffView, offset, 9)
    offset += 9
    console.log(cfg)

    return cfg
}

function readNullString(buff: Uint8Array, offset: number, maxLen: number) {
    let tmp: string[] = []
    let code: number;
    while ((code = buff[offset]) && ++offset < buff.length) {
        tmp.push(String.fromCharCode(code))
    }

    return tmp.join("").trim();
}