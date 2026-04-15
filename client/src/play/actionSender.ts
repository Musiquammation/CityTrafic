import { DataWriter } from "../../../commons/DataWriter";
import { SERVER_IDS } from "../../../commons/serverIds";
import { sendSocket } from "../net/sendSocket";

export const actionSender = {
    placeBlock(x: number, y: number) {
        const buffer = new DataWriter();
        buffer.writeUint8(SERVER_IDS.PLACE_SINGLE_ROAD);
        buffer.writeInt32(x);
        buffer.writeInt32(y);
        sendSocket(buffer.toArrayBuffer());
    }
};