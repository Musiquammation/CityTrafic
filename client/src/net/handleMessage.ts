import { CLIENT_IDS } from "../../../commons/clientIds";
import { DataReader } from "../../../commons/DataReader";
import { DataWriter } from "../../../commons/DataWriter";
import { api } from "../SessionApi"

let REGION_SIZE = 1;

function net_joinCreated(reader: DataReader) {
    REGION_SIZE = reader.readUint32();
    const hash = reader.read256();
    console.log(hash);

    return null;
}

function net_joinAlive(reader: DataReader) {
    REGION_SIZE = reader.readUint32();
    const hash = reader.read256();
    console.log(hash);

    return null;
}

function net_areas(reader: DataReader) {
    const areasCount = reader.readInt32();

    for (let count = 0; count < areasCount; count++) {
        const x0 = reader.readUint32() * REGION_SIZE;
        const y0 = reader.readUint32() * REGION_SIZE;

        for (let dy = 0; dy < REGION_SIZE; dy++) {
            for (let dx = 0; dx < REGION_SIZE; dx++) {
                const data = reader.readUint16();
                api.setCell(x0 + dx, y0 + dy, data);
            }
        }

    }


    return null;
}



export function handleMessage(reader: DataReader): DataWriter | null {
    const action = reader.readUint8();

    switch (action) {
    case CLIENT_IDS.JOIN_CREATED:
        return net_joinCreated(reader);

    case CLIENT_IDS.JOIN_ALIVE:
        return net_joinAlive(reader);

    case CLIENT_IDS.AREAS:
        return net_areas(reader);

        
    default:
        throw new Error("Unknown action " + action);
    }

}