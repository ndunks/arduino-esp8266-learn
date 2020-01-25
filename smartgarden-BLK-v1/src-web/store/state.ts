import { Popup, Status, Settings } from '@/interfaces';

const state: {
    bootComplete: boolean
    login: boolean
    loading: boolean
    title: string
    popups: Popup[]
    status: Status
    settings: Settings
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
        laston: []
    },
    ///@ts-ignore
    settings: {}
}

export default state
