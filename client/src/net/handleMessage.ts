import { CLIENT_IDS } from "../../../commons/clientIds";
import { DataReader } from "../../../commons/DataReader";
import { DataWriter } from "../../../commons/DataWriter";
import { getGameHandler } from "../gameHandler"
import { PlayState } from "../play/PlayState";
import { postWorker } from "../worker/askWorker";

let REGION_SIZE = 1;

function net_joinCreated(reader: DataReader) {
    REGION_SIZE = reader.readUint32();
    const hash = reader.read256();
    console.log(hash);

    getGameHandler().setState(new PlayState());
    return null;
}

function net_joinAlive(reader: DataReader) {
    REGION_SIZE = reader.readUint32();
    const hash = reader.read256();
    console.log(hash);

    getGameHandler().setState(new PlayState());
    return null;
}

function net_areas(reader: DataReader) {
    reader.readUint8(); // for 16bits padding
    const areasCount = reader.readInt32();

    for (let count = 0; count < areasCount; count++) {
        const x0 = reader.readUint32() * REGION_SIZE;
        const y0 = reader.readUint32() * REGION_SIZE;

        const list = new Uint16Array(REGION_SIZE * REGION_SIZE);
        for (let i = 0; i < REGION_SIZE * REGION_SIZE; i++)
            list[i] = reader.readUint16();

        postWorker('setArea', [x0, y0, REGION_SIZE, REGION_SIZE, list], [list.buffer]);
    }


    return null;
}

function net_edits(reader: DataReader) {
    reader.readUint8(); // for 16bits padding
    reader.readUint16(); // for 32bits padding
    const length = reader.readUint32();
    const array = reader.readUint32Array(length-1);
    postWorker('applyEdits', [array], [array.buffer]);

    return null;
}

function net_getEntities(reader: DataReader) {
    /// TODO: read entities
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

    case CLIENT_IDS.EDITS:
        return net_edits(reader);

    case CLIENT_IDS.GET_ENTITIES:
        return net_getEntities(reader);
        
    default:
        throw new Error("Unknown action " + action);
    }

}