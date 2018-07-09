#pragma once
#include <map>
#include <vector>
#include "Point.h"

class Tile;

enum UnitType {
	RockSlinger,
	Clubber,
};

class Unit
{
public:
	UnitType type;
	int health;
	int movement;
	int attacks;

	Unit(UnitType type);

	bool is_melee();
	int max_health();
	int max_attack();
	int max_movement();
	std::map<Point, std::pair<int, Point>> movement_tiles(Point p, const std::vector<std::vector<Tile>>& tiles);
};

int unit_attack(Unit*& u, Unit*& other, int other_cost);
