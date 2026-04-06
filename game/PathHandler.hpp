#pragma once


typedef struct {
	int x;
	int y;
	Direction dir;
} PathPoint;

class PathHandler {
	PathPoint* array = nullptr;
	int step = 0;
	int length = 0;


public:
	~PathHandler();

	void fill(PathPoint* array, int length);
	PathPoint* seek();
	void next();
};
