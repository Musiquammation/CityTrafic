import { Unit } from "./Unit";

export class NamedUnit {
    name: string;
    unit: Unit<any>;

    constructor(name: string, unit: Unit<any>) {
        this.name = name;
        this.unit = unit;
    }
}