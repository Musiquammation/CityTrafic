import { sendCommand } from '../../net/sendCommand';
import { COMMAND_CODES } from '../../shared/CommandCode';
import {HandButton} from './handtypes'

function erase(x: number, y: number, btn: number) {
    
}

export const handlist = {
    erase: new HandButton(
        "assets/icons/erase.png",

        // enable
        ()=>{},

        // diseable
        ()=>{},

        // mouseUp
        (x, y, btn, current) => {
            sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
                writer.writeInt32(Math.floor(x));
                writer.writeInt32(Math.floor(y));
            });
        },

        // mouseDown
        (x, y, btn, current) => {
            sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
                writer.writeInt32(Math.floor(x));
                writer.writeInt32(Math.floor(y));
            });
        },

        // mouseMove
        (prevX, prevY, x, y, btn, current) => {
            sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
                writer.writeInt32(Math.floor(x));
                writer.writeInt32(Math.floor(y));
            });
        },
    ),

    road: new HandButton(
        "assets/icons/road.png",

        // enable
        ()=>{},

        // diseable
        ()=>{},

        // mouseUp
        (x, y, btn, current) => {
            sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
                writer.writeInt32(Math.floor(x));
                writer.writeInt32(Math.floor(y));
            });
        },

        // mouseDown
        (x, y, btn, current) => {
            sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
                writer.writeInt32(Math.floor(x));
                writer.writeInt32(Math.floor(y));
            });
        },

        // mouseMove
        (prevX, prevY, x, y, btn, current) => {
            sendCommand(COMMAND_CODES.PLACE_SINGLE_ROAD, writer => {
                writer.writeInt32(Math.floor(x));
                writer.writeInt32(Math.floor(y));
            });
        },
    )
}