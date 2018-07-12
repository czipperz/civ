#pragma once
#include <map>
#include <vector>
#include "Point.h"

class Tile;

class Unit {
public:
	int health;
	int movement;
	std::map<Point, std::pair<int, Point>> movement_tiles(Point p, const std::vector<std::vector<Tile>>& tiles);
};

enum MilitaryUnitType {
	RockSlinger,
	Clubber,
};

class MilitaryUnit : public Unit {
public:
	MilitaryUnitType type;
	int attacks;

	MilitaryUnit(MilitaryUnitType type);

	int max_health();
	int max_movement();

	int max_attack();
	bool is_melee();
};

enum CivilianUnitType {
	Worker,
};

class CivilianUnit : public Unit {
public:
	CivilianUnitType type;

	CivilianUnit(CivilianUnitType type);

	int max_health();
	int max_movement();
};
