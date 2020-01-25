import { Popup, Status, Settings } from '@/interfaces';

const state: {
    bootComplete: boolean
    login: boolean
    loading: boolean
    title: string
    popups: Popup[]
    status: Status
    settings: Settings
    deviceOn: Date
} = {
    bootComplete: false,
    login: false,
    loading: false,
    title: 'Smart Device..',
    popups: [],
    ///@ts-ignore
    status: {
        valve: [],
        sensorTanah: [],
        needWater: [],
        lastOn: []
    },
    ///@ts-ignore
    settings: {},
    deviceOn: null
}

export default state
