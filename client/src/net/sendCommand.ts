import { CommandCode } from "../../../commons/CommandCode"
import { DataWriter } from "../../../commons/DataWriter"
import { SERVER_IDS } from "../../../commons/serverIds"
import { sendSocket } from "./sendSocket";


export function sendCommand(command: CommandCode, fill: (writer: DataWriter) => void) {
    const writer = new DataWriter();
    writer.writeUint8(SERVER_IDS.GAME_COMMAND);
    writer.writeUint8(0); // padding
    writer.writeUint16(command);

    fill(writer)

    sendSocket(writer.toArrayBuffer());
}