#include "jobSerializator.hpp"

#include <iosfwd>

#include "../jobs/AgricultorJob.hpp"
#include "../utils/streams.hpp"


enum class Types: int {
	AGRICULTOR
};

void jobSerializator::save(WriteStream &stream, const Job *job) {
	if (auto* agricultor = dynamic_cast<const AgricultorJob*>(job)) {
		stream.write(Types::AGRICULTOR);
		stream.write(agricultor->salaryPerSeed);
		stream.write(agricultor->pricePerSeed);
		return;
	}

	throw std::runtime_error{"Job not implemented"};
}

Job *jobSerializator::load(ReadStream &stream) {
	auto type = stream.read<Types>();

	switch (type) {
		case Types::AGRICULTOR: {
			float salaryPerSeed = stream.read<float>();
			float pricePerSeed = stream.read<float>();
			auto agricultor = new AgricultorJob{salaryPerSeed, pricePerSeed};
			return agricultor;
		}
	}


	throw std::runtime_error{"Job not implemented"};
}
