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
        new NamedUnit("Refined", units.f32),
        new NamedUnit("Entry hour", units.i32),
        new NamedUnit("Leave hour", units.i32),
        new NamedUnit("Salary (per liter)", units.f32),
        new NamedUnit("Sold prince (per liter)", units.f32),
        new NamedUnit("Current employees", units.i32),
        new NamedUnit("Goal employees", units.i32),
    ]
});


PANEL_MAP.set(PANEL_IDS.BUILDING_OIL_FIELD, {
    title: "Plantation",
    fields: [
        new NamedUnit("Test", units.f32),
    ]
});
