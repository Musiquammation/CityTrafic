import { PanelDescriptor } from "./PanelDescriptor";
import { NamedUnit } from "./NamedUnit";
import { units } from "./Unit";
import { PANEL_IDS } from "../../shared/PanelId";

export const PANEL_MAP = new Map<number, PanelDescriptor>();

PANEL_MAP.set(PANEL_IDS.BUILDING_HOME, {
    title: "Home",
    fields: [
        new NamedUnit("Rent", units.i32),
        new NamedUnit("Capacity", units.i32)
    ]
});

PANEL_MAP.set(PANEL_IDS.BUILDING_OIL_FIELD, {
    title: "Oil field",
    fields: [
        new NamedUnit("Crude", units.f32),
        new NamedUnit("Left", units.f32)
    ]
});



