
#include "pathfinder.hpp"

#include "PathHandler.hpp"
#include "Map.hpp"
#include "Direction.hpp"
#include "direction.hpp"

#include <queue>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <limits.h>
#include <unordered_map>
#include <queue>
#include <vector>
#include <tuple>
#include <functional>

// #define printPath(fmt, ...) printf("[PATH] " fmt "\n", ##__VA_ARGS__)
#define printPath(fmt, ...) 

struct Node {
	int x, y;
	int g, h;
	int parentX, parentY;
	char dir;

	int f() const { return g + h; }
	bool operator>(const Node& other) const { return f() > other.f(); }
};

struct Pos {
	int x, y;
	bool operator==(const Pos& other) const { return x == other.x && y == other.y; }
};

struct PosHash {
	std::size_t operator()(const Pos& p) const {
		std::size_t h1 = std::hash<int>{}(p.x);
		std::size_t h2 = std::hash<int>{}(p.y);
		// Boost-style hash_combine
		return h1 ^ (h2 * 2654435761ULL + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
	}
};


// Finds the nearest walkable cell (NONE or ROAD) to (x, y) using expanding square rings.
// Returns {INT_MIN, INT_MIN} if no walkable cell is found.
static Pos findNearestWalkable(const Map& map, int x, int y) {
	const Cell* c = map.getCell(x, y);
	if (c && (c->getType() == CellType::NONE || c->getType() == CellType::ROAD))
		return {x, y};

	constexpr int RADIUS = 12;
	for (int radius = 1; radius < RADIUS; ++radius) {
		for (int ox = -radius; ox <= radius; ++ox) {
			for (int oy = -radius; oy <= radius; ++oy) {
				if (std::abs(ox) != radius && std::abs(oy) != radius)
					continue;
				const Cell* nc = map.getCell(x + ox, y + oy);
				if (nc && (nc->getType() == CellType::NONE || nc->getType() == CellType::ROAD))
					return {x + ox, y + oy};
			}
		}
	}
	return {INT_MIN, INT_MIN};
}

char* makePedestranPath(const Map& map, int startX, int startY, int destX, int destY) {
	const int dx[]        = { 1,  1,  0, -1, -1, -1,  0,  1};
	const int dy[]        = { 0, -1, -1, -1,  0,  1,  1,  1};
	const int baseCosts[] = {10, 14, 10, 14, 10, 14, 10, 14};
	const int ROAD_MULT   = 3;

	Pos src = findNearestWalkable(map, startX, startY);
	Pos dst = findNearestWalkable(map, destX,  destY);

	printPath("Request: (%d,%d) -> (%d,%d)\n", startX, startY, destX, destY);
	printPath("Snapped: src=(%d,%d) dst=(%d,%d)\n", src.x, src.y, dst.x, dst.y);

	if (src.x == INT_MIN || dst.x == INT_MIN) {
		printPath("ERROR: could not find walkable cell near src or dst\n");
		return nullptr;
	}

	auto getH = [&](int x, int y) -> int {
		int dX = std::abs(x - dst.x);
		int dY = std::abs(y - dst.y);
		return 10 * (dX + dY) - 6 * std::min(dX, dY);
	};

	struct Node {
		int x, y, g, f;
		int parentX, parentY;
		char dir;
		bool operator>(const Node& o) const { return f > o.f; }
	};

	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
	std::unordered_map<Pos, Node, PosHash> closedList;

	openList.push({src.x, src.y, 0, getH(src.x, src.y), INT_MIN, INT_MIN, 8});
	printPath("Start node pushed: (%d,%d) g=0 h=%d\n", src.x, src.y, getH(src.x, src.y));

	int iterations = 0;

	while (!openList.empty()) {
		Node current = openList.top();
		openList.pop();
		iterations++;

		Pos curPos = {current.x, current.y};

		auto it = closedList.find(curPos);
		if (it != closedList.end() && it->second.g <= current.g) {
			printPath("  iter=%d skip (%d,%d) already closed with g=%d <= current g=%d\n",
				iterations, current.x, current.y, it->second.g, current.g);
			continue;
		}

		closedList[curPos] = current;
		printPath("  iter=%d settle (%d,%d) g=%d f=%d parent=(%d,%d) dir=%d\n",
			iterations, current.x, current.y, current.g, current.f,
			current.parentX, current.parentY, (int)current.dir);

		if (current.x == dst.x && current.y == dst.y) {
			std::vector<char> tempPath;

			// 1) Chemin A* : dst -> src (reconstruction normale)
			Pos bt = curPos;
			while (closedList[bt].parentX != INT_MIN) {
				Node& n = closedList[bt];
				tempPath.push_back(n.dir);
				bt = {n.parentX, n.parentY};
			}
			// tempPath est dst->src, on va le reverser plus bas

			// 2) Tronçon snap dst->destX/destY (ajouté en tête, avant inversion)
			//    On marche de dst vers (destX, destY) en direction opposée
			{
				int cx = dst.x, cy = dst.y;
				while (cx != destX || cy != destY) {
					int stepX = (destX > cx) ? 1 : (destX < cx) ? -1 : 0;
					int stepY = (destY > cy) ? 1 : (destY < cy) ? -1 : 0;
					// Trouve la direction correspondant à (stepX, stepY)
					for (int i = 0; i < 8; ++i) {
						if (dx[i] == stepX && dy[i] == stepY) {
							// Ce tronçon va APRÈS le chemin A* (donc inséré avant inversion)
							tempPath.insert(tempPath.begin(), (char)i);
							break;
						}
					}
					cx += stepX;
					cy += stepY;
				}
			}

			// 3) Tronçon startX/startY->src (ajouté en queue, après inversion)
			std::vector<char> prefixPath;
			{
				int cx = startX, cy = startY;
				while (cx != src.x || cy != src.y) {
					int stepX = (src.x > cx) ? 1 : (src.x < cx) ? -1 : 0;
					int stepY = (src.y > cy) ? 1 : (src.y < cy) ? -1 : 0;
					for (int i = 0; i < 8; ++i) {
						if (dx[i] == stepX && dy[i] == stepY) {
							prefixPath.push_back((char)i);
							break;
						}
					}
					cx += stepX;
					cy += stepY;
				}
			}

			// Assemblage final : prefix (start->src) + A* inversé (src->dst) + suffix (dst->dest)
			size_t totalSize = prefixPath.size() + tempPath.size() + 1;
			char* result = (char*)malloc(totalSize);
			if (!result) return nullptr;

			size_t idx = 0;
			// prefix : déjà dans le bon sens
			for (char c : prefixPath)
				result[idx++] = c;
			// A* inversé
			for (size_t i = tempPath.size(); i-- > 0; )
				result[idx++] = tempPath[i];
			result[idx] = 8;

			return result;
		}


		for (int i = 0; i < 8; ++i) {
			int nx = current.x + dx[i];
			int ny = current.y + dy[i];

			const Cell* nextCell = map.getCell(nx, ny);
			if (!nextCell) continue;

			CellType type = nextCell->getType();
			if (type != CellType::NONE && type != CellType::ROAD)
				continue;

			int mult   = (type == CellType::ROAD) ? ROAD_MULT : 1;
			int gScore = current.g + baseCosts[i] * mult;

			Pos nPos = {nx, ny};
			auto nit = closedList.find(nPos);
			if (nit != closedList.end() && nit->second.g <= gScore) continue;

			printPath("     push neighbor (%d,%d) dir=%d g=%d f=%d\n",
				nx, ny, i, gScore, gScore + getH(nx, ny));
			openList.push({nx, ny, gScore, gScore + getH(nx, ny), current.x, current.y, (char)i});
		}
	}

	printPath("No path found after %d iterations\n", iterations);
	return nullptr;
}










// Hash for (x, y, dir)
struct StateHash {
	size_t operator()(const std::tuple<int,int,Direction>& k) const {
		auto h1 = std::hash<int>{}(std::get<0>(k));
		auto h2 = std::hash<int>{}(std::get<1>(k));
		auto h3 = std::hash<uint8_t>{}((uint8_t)std::get<2>(k));
		return h1 ^ (h2 * 2654435761u) ^ (h3 * 40503u);
	}
};

// Converts a relative direction (front=1, right=2, left=3) into an absolute direction
static Direction applyRelative(Direction current, int relative) {
	// relative: 1=front, 2=right (of current), 3=left (of current)
	switch (relative) {
		case 1: return current;                          // front = same direction
		case 2: return Direction_getRight(current);      // turn right
		case 3: return Direction_getLeft(current);       // turn left
		default: return current;
	}
}





bool makeCarPath(
	const Map& map, PathHandler<true>& path,
	int startX, int startY, int destX, int destY,
	Direction startDir
) {
	using Key = std::tuple<int, int, Direction>;
	using State = std::pair<int, Key>;

	std::unordered_map<Key, int, StateHash> dist;
	std::unordered_map<Key, Key, StateHash> prev;

	std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

	Key startKey = {startX, startY, startDir};
	dist[startKey] = 0;
	pq.push({0, startKey});

	Key destKey = {-1, -1, Direction::RIGHT};
	bool found = false;

	// --- Dijkstra Search ---
	while (!pq.empty()) {
		State top = pq.top();
		pq.pop();

		int cost = top.first;
		Key curKey = top.second;

		int x = std::get<0>(curKey);
		int y = std::get<1>(curKey);
		Direction dir = std::get<2>(curKey);

		if (dist.count(curKey) && dist[curKey] < cost)
			continue;

		if (x == destX && y == destY) {
			destKey = curKey;
			found = true;
			break;
		}

		const Cell* cell = map.getCell(x, y);
		CellType type = cell->getType();

		auto tryNeighbor = [&](Direction outDir, int extraWeight) {
			auto dv = DIRECTION_VECTORS[(int)outDir];
			int nx = x + dv.x;
			int ny = y + dv.y;

			const Cell* ncell = map.getCell(nx, ny);
			CellType ntype = ncell->getType();

			if (ntype != CellType::ROAD && ntype != CellType::DIRECTION && ntype != CellType::PARKING)
				return;

			Key nextKey = {nx, ny, outDir};
			int weight = (ntype == CellType::ROAD) ? ((ncell->data >> 8) & 0x7F) : 0;
			int newCost = cost + 1 + extraWeight + weight;

			if (!dist.count(nextKey) || dist[nextKey] > newCost) {
				dist[nextKey] = newCost;
				prev[nextKey] = curKey;
				pq.push({newCost, nextKey});
			}
		};

		if (type == CellType::ROAD || type == CellType::PARKING) {
			tryNeighbor(dir, 0);
		} else if (type == CellType::DIRECTION) {
			int side = direction::getSide(cell->data, (int)dir);
			static const int SIDE_DECOMP[8][3] = {
				{1, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0},
				{1, 2, 0}, {1, 3, 0}, {2, 3, 0}, {1, 2, 3},
			};

			for (int i = 0; i < 3; i++) {
				int rel = SIDE_DECOMP[side][i];
				if (rel == 0) break;
				tryNeighbor(applyRelative(dir, rel), 0);
			}
		}
	}

	if (!found) return false;

	// --- Path Reconstruction (Compression Logic) ---
	std::vector<Vector<int>> finalPos;
	std::vector<Direction> finalDir;

	Key cur = destKey;

	// 1. Start by adding the destination
	finalPos.push_back({std::get<0>(cur), std::get<1>(cur)});
	finalDir.push_back(std::get<2>(cur));

	while (cur != startKey) {
		Key parent = prev[cur];
		
		// A "pivot" occurs if the direction to leave 'parent' is different 
		// from the direction that was used to leave 'parent-of-parent'.
		// We also explicitly keep the startKey to define the beginning of the path.
		if (parent == startKey || std::get<2>(cur) != std::get<2>(parent)) {
			finalPos.push_back({std::get<0>(parent), std::get<1>(parent)});
			
			// The direction associated with this coordinate is the one 
			// used to MOVE TOWARDS the next point in the path.
			finalDir.push_back(std::get<2>(cur));
		}
		cur = parent;
	}

	// 2. Reverse to get Start -> End order
	std::reverse(finalPos.begin(), finalPos.end());
	std::reverse(finalDir.begin(), finalDir.end());

	// 3. Fixup the initial direction at the starting point
	if (!finalDir.empty()) {
		finalDir[0] = startDir;
	}

	// --- Encoding & Buffer Allocation ---
	int len = (int)finalPos.size();
	Vector<int>* posArray = new Vector<int>[len];
	std::copy(finalPos.begin(), finalPos.end(), posArray);

	int byteCount = (2 * len + 7) / 8;
	uint8_t* compactDirs = (uint8_t*) calloc(byteCount, 1);

	for (int i = 0; i < len; i++) {
		int bitIndex = 2 * i;
		compactDirs[bitIndex / 8] |= uint8_t(((uint8_t)finalDir[i] & 0b11) << (bitIndex % 8));
	}

	path.fill(posArray, compactDirs, len);
	return true;
}