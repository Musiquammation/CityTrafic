#pragma once

#include "Calendar.hpp"
#include "Map.hpp"
#include "CarHandler.hpp"
#include "CharacterHandler.hpp"
#include "BuildingInfo.hpp"
#include "Player.hpp"

#include "declarations.hpp"


class Game {
private:
	Map map;
	CarHandler carHandler{};
	CharacterHandler characterHandler{};
	Calendar calendar;
	std::vector<Player> players;
	int frameCount = 0;
	uint64_t updateJobsDate = 0;
	void test();
	int searchPlayer(hash_t key);

	
public:
	friend struct serialize;
	friend struct GameCommand;
	friend class Api;
	friend class Server;
	friend uint32_t* updateNet_helper_write(
		Game& game,
		int x, int y, int w, int h,
		uint8_t clientRequestId,
		int money,
		bool updateClientJobs
	);
	friend void updateNet_helper_read(Game& game, void* args);

	friend int main();


	void frame();

	Cell* getEditCell(int x, int y);
	const Cell* getCell(int x, int y);
	Car* spawnCar(int x, int y, Direction direction);
	Car* getCar(int x, int y);
	BuildingInfo getBuilding(int x, int y) const;
	void destroyBuilding(BuildingInfo info);

	const Map& getMap() const {return this->map;}
	Map& getMap() {return this->map;}
	const Calendar& getCalendar() const {return this->calendar;}


	int getFrame() const;
	bool checkBounds(int x, int y, int width, int height) const;

	Player* getPlayer(int id);
	int getPlayerId(Player* player);

	Vector<int> searchJob(
		Character* c,
		JobOffer& bestOffer
	);

	Vector<int> searchHome(
		int maxRent,
		int cx,
		int cy
	);

	Game(int width, int height);
	Game(): Game(32,32) {}
	~Game();
};
