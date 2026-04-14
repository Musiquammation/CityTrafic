import { Client } from "./Client";

interface Grid {
    ptr: number;
    view: Uint16Array;

    mapX: number;
    mapY: number;
    mapW: number;
    mapH: number;
}


export class MatchSession {
    readonly id: number;
    grid: Grid | null = null;
    clients: Client[] = [];
    
    constructor(id: number) {
        this.id = id;
    }

    
}

