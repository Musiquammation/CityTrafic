#pragma once

#include "declarations.hpp"
#include "calendar_t.hpp"
#include "Vector.hpp"
#include "BuildingInfo.hpp"
#include "BuildingType.hpp"
#include "ActionExecutor.hpp"
#include "CharacterStatus.hpp"
#include "actions/ActionCode.hpp"

namespace actionNodes::character {
	struct CharacterFriend;
}

enum class CharacterState {
	CLIENT,
	WALK,
	INSIDE,
	OUTSIDE,
	DRIVE,
	WAIT
};

class Character {
	static constexpr float SPEED = .04f;
	static constexpr float MAX_SEEDS = 50;
	static constexpr float CHECK_SEEDS = 10;

	Car* car = nullptr;
	ActionExecutor executor;
	Vector<int> jobLoc = {INT32_MIN, INT32_MIN};
	Vector<int> home = {INT32_MIN, INT32_MIN};
	CharacterState state;
	CharacterStatus status = CharacterStatus::IDLE;
	int money = 0;
	int waitingJoyMoney = 0;
	int salaryEstimation = 0;
	unsigned int pointId;
	calendar_t nextRentPayMonth = 0;
	float seeds = MAX_SEEDS;



	union {
		struct {

		} client;

		struct {
			char* path;
			Vector<int> anchor;
			int position;
			float step;
		} walk;

		struct {
			int index;

			struct {
				int delay;
			} grocery;
		} inside;

		struct {

		} outside;

		struct {
			ActionCode state;
		} drive;

		struct {
			int cooldown;
		} wait;

	} data;

	void cleanupState();
	void setState(CharacterState next);
	ActionCode walk(Game& game);

	friend struct actionNodes::character::CharacterFriend;
	friend struct serialize;
public:
	float x;
	float y;

	static Character* createClientCharacter(float x, float y);
	static Character* spawnCharacter(const Map& map, int x, int y);
	static float evalFullLiterSafetyCost(float completion);

	bool makeWalk(Game& game, int destX, int destY);
	bool makeDrive(Map& map, int destX, int destY);
	bool makeInside(Game& game);
	void makeOutside(Game& game);

	void notifyDrive();

	BuildingInfo getWorkBuilding(Game& game) const;
	BuildingInfo getHomeBuilding(const Map& map) const;
	BuildingInfo getCurrentBuilding(const Map& map, BuildingType type) const;
	bool orientBuilding(Game& game, BuildingInfo info);
	bool locateBuilding(Map& map, BuildingInfo info);

	void frame(Game& game);
	int takeRandomPointId(int modulo);

	CharacterState getState() const;
	Vector<int> getPos() const;

	auto getHome() const {return this->home;}
	void kickFromHouse();

	Car* getCar() const;
	bool setCar(Car* car);

	bool takeJob(
		Vector<int> loc,
		const JobOffer& offer,
		Game& game
	);
	void leaveJob(Game& game);
	Job* getJob(Game& game) const;
	Vector<int> getJobLoc() const;
	bool hasJob() const;
	bool isInside() const;

	void give(int money);
	int pay(int money);

	bool isAtHome(const Game& game) const;

	uint32_t* sendData(uint32_t* ptr);
	void saveData(WriteStream& stream);
	void openData(ReadStream& stream);

	Character();
	~Character();
};