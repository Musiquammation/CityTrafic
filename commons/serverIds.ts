export enum SERVER_IDS {
	/**
	 * +00: data
	 * +32
	 */
	CONNECT,


	/**
	 * +00: x
	 * +04: y
	 * +08: w
	 * +12: h
	 * +16
	 */
	LISTEN,

	/**
	 * +00: x
	 * +04: y
	 * +08
	 */
	PLACE_SINGLE_ROAD,

	/**
	 * data only
	 */
	GAME_COMMANDS,

	/**
	 * +00: x
	 * +04: y
	 * +08: w
	 * +12: h
	 * +16
	 */
	GET_ENTITIES,

	
	ERROR
} 