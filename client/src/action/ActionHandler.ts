import { PlayState } from "../play/PlayState";
import { Action } from "./Action";
import { ActionConstructor } from "./ActionConstructor";
import { ACTION_CONSTRUCTORS } from "./ACTION_CONSTRUCTORS";

export class ActionHandler {
    private currentAction: Action;
    private hand = [0];
    private readonly playState: PlayState;

    constructor(playState: PlayState) {
        this.playState = playState;
        this.currentAction = ACTION_CONSTRUCTORS[0].construct();
        this.currentAction.select();
    }


    changeAction(actionConstructor: ActionConstructor) {
        this.currentAction.deselect();
        this.currentAction = actionConstructor.construct();
        this.currentAction.select();
    }

    takeFromHand(idx: number) {
        if (idx < 0 || idx >= this.hand.length)
            return;

        const hand = this.hand[idx];
        if (hand < 0)
            return;

        this.changeAction(ACTION_CONSTRUCTORS[hand]);
    }

    getAction() {
        return this.currentAction;
    }
}
