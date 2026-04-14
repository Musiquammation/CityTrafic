import { Client } from "./Client";

interface Grid {
    ptr: number;
    view: Uint16Array;

    mapX: number;
    mapY: number;
    mapW: number;
    mapH: number;
}


export class Match {
    readonly id: number;
    grid: Grid;
    clients: Client[] = [];
    
    constructor(id: number, grid: Grid) {
        this.id = id;
        this.grid = grid;
    }

}

