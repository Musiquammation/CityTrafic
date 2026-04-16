import { CommandCode } from "../commons/CommandCode";
import { DataReader } from "../commons/DataReader";

export function getCommandLength(code: CommandCode, reader: DataReader): number {
    switch (code) {
    case CommandCode.TEST:
        return 4;

    case CommandCode.PLACE_SINGLE_ROAD:
        return 2;
    }

    throw new Error("Given code is not a command");
}