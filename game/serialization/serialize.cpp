#include "serialize.hpp"

#include "jobSerializator.hpp"
#include "../utils/streams.hpp"
#include "../Game.hpp"
#include "../Character.hpp"
#include "../Car.hpp"
#include "../Building.hpp"

void serialize::save(const Game &game, WriteStream &stream) {
	// Save map
	stream.write(game.map.x);
	stream.write(game.map.y);
	stream.write(game.map.width);
	stream.write(game.map.height);

	// Copy map
	int mapLength = (game.map.width - game.map.x) *
		(game.map.height - game.map.y);
		
	stream.copy(game.map.cells, (size_t)mapLength);


	// Save cars
	stream.write(game.carHandler.cars.size());
	for (auto it: game.carHandler.cars) {
		Car* car = it.second;
		stream.write(car);
		stream.write(car);
	}

	// Save characters
	stream.write(game.characterHandler.characters.size());
	for (Character* c: game.characterHandler.characters) {
		stream.write(c);
		stream.write(*c);
	}

	// Calendar indicator
	stream.write(game.calendar.indicator);

	// Players
	stream.write(game.players.size());
	for (auto& player: game.players) {
		stream.write(player);
	}

	// Jobs
	for (const auto&[_, b]: game.map.buildings) {
		Job* job = b->getJob();
		stream.write(job);

		jobSerializator::save(stream, job);
	}

	stream.write(nullptr); // end of jobs

	// Buildings
	stream.write(game.map.buildings.size());
	for (const auto&[p, b]: game.map.buildings) {
		stream.write(p);
		b->fileSave(stream);
	}
}


void serialize::open(Game &game, ReadStream &stream) {

}


