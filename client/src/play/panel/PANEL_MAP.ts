import { PanelDescriptor } from "./PanelDescriptor";
import { NamedUnit } from "./NamedUnit";
import { units } from "./Unit";
import { PANEL_IDS } from "../../shared/PanelId";

export const PANEL_MAP = new Map<number, PanelDescriptor>();

PANEL_MAP.set(PANEL_IDS.BUILDING_HOME, {
	title: "Home",
	fields: [
		new NamedUnit("Rent", units.mut_i32),
		new NamedUnit("Capacity", units.i32),
		new NamedUnit("Destroy", units.button)
	]
});


PANEL_MAP.set(PANEL_IDS.BUILDING_OIL_FIELD, {
	title: "Oil field",
	fields: [
		new NamedUnit("Crude", units.f32),
		new NamedUnit("Refined", units.f32),
		new NamedUnit("Entry hour", units.mut_i32),
		new NamedUnit("Leave hour", units.mut_i32),
		new NamedUnit("Salary (per liter)", units.mut_f32),
		new NamedUnit("Sold price (per liter)", units.mut_f32),
		new NamedUnit("Current employees", units.i32),
		new NamedUnit("Goal employees", units.mut_i32),
	]
});


PANEL_MAP.set(PANEL_IDS.BUILDING_PLANTATION, {
	title: "Plantation",
	fields: [
		new NamedUnit("Test", units.f32),
	]
});


PANEL_MAP.set(PANEL_IDS.BUILDING_GROCERY, {
	title: "Grocery",
	fields: [
		new NamedUnit("Clients", units.i32),
		new NamedUnit("Cashiers", units.i32),
		new NamedUnit("Stock", units.f32),
		new NamedUnit("Efficiency", units.mut_f32),
		new NamedUnit("Salary (per hour)", units.mut_f32),
	]
});


PANEL_MAP.set(PANEL_IDS.BUILDING_CONSTRUCTION, {
	title: "Construction",
	fields: [
		new NamedUnit("Type", units.i32),
		new NamedUnit("Completion", units.i32),
		new NamedUnit("Total to reach", units.i32),
		new NamedUnit("Current employees", units.i32),
		new NamedUnit("Goal employees", units.mut_i32),
	]
});

PANEL_MAP.set(PANEL_IDS.BUILDING_WAREHOUSE, {
	title: "Warehouse",
	fields: [
		new NamedUnit("Type", units.i32),
		new NamedUnit("Current employees", units.i32),
		new NamedUnit("Goal employees", units.mut_i32),
	]
});

