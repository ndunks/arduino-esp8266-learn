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
    status: {
        valve: []
    } as any,
    settings: {} as any
}

export default state
