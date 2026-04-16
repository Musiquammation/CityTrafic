import { Action } from "./Action";

export abstract class ActionConstructor {
    abstract construct(): Action;
}