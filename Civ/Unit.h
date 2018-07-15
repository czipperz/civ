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

	std::map<Point, std::pair<int, Point>> movement_tiles(Point p, const std::vector<std::vector<Tile>>& tiles) const;
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

	int max_health() const;
	int max_movement() const;

	int max_attack() const;
	int attack_range() const;
	bool is_melee() const;
	int max_num_attacks() const;
	std::map<Point, std::pair<int, Point>> attack_tiles(Point p, const std::vector<std::vector<Tile>>& tiles) const;

	void end_turn();
	void kill();
};

enum CivilianUnitType {
	Worker,
	Settler,
};

class CivilianUnit : public Unit {
public:
	CivilianUnitType type;

	CivilianUnit(Player* player, CivilianUnitType type);

	int max_health() const;
	int max_movement() const;

	void end_turn();
	void kill();
};
