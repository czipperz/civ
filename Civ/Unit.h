#pragma once
#include <map>
#include <vector>
#include "Point.h"

class Player;
class Tile;

class Unit {
public:
	Player* player;
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

	MilitaryUnit(Player* player, MilitaryUnitType type);

	int max_health();
	int max_movement();

	int max_attack();
	bool is_melee();
};

enum CivilianUnitType {
	Worker,
	Settler,
};

class CivilianUnit : public Unit {
public:
	CivilianUnitType type;

	CivilianUnit(Player* player, CivilianUnitType type);

	int max_health();
	int max_movement();
};

MilitaryUnit* create_unit(Player* player, MilitaryUnitType type);
CivilianUnit* create_unit(Player* player, CivilianUnitType type);
