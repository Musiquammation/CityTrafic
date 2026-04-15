export const ApiTakeCode = {
	MAKE_MAP: 0,
	COPY_CARS: 1,
	FREE_CARS: 2,
	COPY_COORDS: 3,
	FREE_COORDS: 4,
	TAKE_MAP_EDITS: 5,
	TAKE_MAP_EDITS_ALL: 6,
	RLSE_MAP_EDITS: 7,
	TAKE_MAP_PTR: 8,
	RLSE_MAP_PTR: 9,
	PLACE_ROAD: 10,
} as const;

export type ApiTakeCode =
	typeof ApiTakeCode[keyof typeof ApiTakeCode];