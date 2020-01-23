import { ConfigRaw, Status, WifiMode, StatusRaw, WifiStatus, SensorsRaw, SmartGardenConfig } from './interfaces';
import { VALVE_COUNT } from './constant';
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
    newStatus.isApMode = !!(newStatus.mode & WifiMode["Access Point"])
    newStatus.isStaMode = !!(newStatus.mode & WifiMode.Station)
    newStatus.cfg = parseSmartGardenConfig(raw.cfg)
}

export function parseStatusRaw(raw: StatusRaw, newStatus: Status) {

    const [free, max, frag] = raw.heap.split(' ', 3)
    newStatus.heap = {
        free: parseInt(free),
        max: parseInt(max),
        frag: parseInt(frag)
    }
    newStatus.mode = parseInt(raw.mode)
    newStatus.modeStr = WifiMode[newStatus.mode]
    newStatus.status = parseInt(raw.status)
    newStatus.statusStr = WifiStatus[newStatus.status]
    newStatus.rssi = parseInt(raw.rssi)
    newStatus.signal = 100 + parseInt(raw.rssi)
    newStatus.ap_clients = parseInt(raw.ap_clients)
    newStatus.isConnected = newStatus.status == WifiStatus.Connected
}

export function parseSensorRaw(raw: SensorsRaw, newStatus: Status) {
    newStatus.sensorTanah = raw.in.trim().split(' ').map(
        v => parseInt(v)
    )
    let value = parseInt(raw.out);
    newStatus.valve = [];
    newStatus.valve[0] = (value & (1 >> 0)) > 0;
    newStatus.valve[1] = (value & (1 >> 1)) > 0;
    newStatus.valve[2] = (value & (1 >> 2)) > 0;
    newStatus.valve[3] = (value & (1 >> 3)) > 0;
    newStatus.valve[4] = (value & (1 >> 4)) > 0;
    newStatus.valve[5] = (value & (1 >> 5)) > 0;
    newStatus.sprayer = (value & (1 >> 6)) > 0;
    newStatus.pompa = (value & (1 >> 7)) > 0;
    newStatus.temp = parseInt(raw.temp)
    newStatus.hum = parseInt(raw.hum)
}

export function parseSmartGardenConfig(raw: string) {

    let cfg: SmartGardenConfig = {} as any;
    const tmp = raw.split(' ', 2)
    const len = parseInt(tmp[0])

    const rawStr = atob(tmp[1])
    let buff = new ArrayBuffer(len)

    let buffView = new Uint8Array(buff)
    let tmpNum: number
    for (let i = 0; i < len; i++) {
        buffView[i] = rawStr.charCodeAt(i)
        // first bit set is null
        // buffView[i] = (code & (1 << 7)) ? 0 : code
    }
    console.log(buffView);
    const firstUint8Len = 4 + VALVE_COUNT * 2
    let offset = 0
    let firstBytes = new Uint8Array(buff, offset, firstUint8Len);

    cfg.valve_delay = []
    cfg.humidity_minimal = []

    // uint8
    cfg.flag = buffView[offset++]
    // uint8
    cfg.sensor_delay = buffView[offset++]
    // uint8
    cfg.humidity_minimal_default = buffView[offset++]
    // uint8
    cfg.temperature_max = buffView[offset++]
    // uint8
    cfg.valve_delay_default = buffView[offset++]
    // uint8 * VALVE_COUNT
    cfg.valve_delay = Array.prototype.slice.call(buffView, offset, offset + VALVE_COUNT)
    offset += VALVE_COUNT
    // uint8 * VALVE_COUNT
    cfg.humidity_minimal = Array.prototype.slice.call(buffView, offset, offset + VALVE_COUNT)
    offset += VALVE_COUNT
    // have one gap in struct??
    offset++;
    // unsigned long;
    let tmpLong = new Uint32Array(buffView.buffer.slice(offset, offset + 4))
    offset += 4;
    cfg.maksimal_pompa_hidup = tmpLong[0]

    // unsigned long;
    tmpLong = new Uint32Array(buffView.buffer.slice(offset, offset + 4))
    offset += 4;
    cfg.maksimal_pompa_mati = tmpLong[0]

    // char name[21];
    // char displayText[21];
    // char password[9];
    console.log(cfg)

    return cfg
}