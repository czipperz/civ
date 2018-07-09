#include "stdafx.h"
#include "Unit.h"
#include <SDL.h>
#include <utility>
#include <map>
#include "Tile.h"

Unit::Unit(UnitType type)
	: type(type)
	, attacks(1)
{
	health = max_health();
	movement = max_movement();
}

bool Unit::is_melee()
{
	switch (type) {
	case RockSlinger:
		return false;
	case Clubber:
		return true;
	}
}

int Unit::max_health()
{
	switch (type) {
	case RockSlinger:
		return 10;
	case Clubber:
		return 13;
	}
}

int Unit::max_attack()
{
	switch (type) {
	case RockSlinger:
		return 4;
	case Clubber:
		return 5;
	}
}

int Unit::max_movement()
{
	switch (type) {
	case RockSlinger:
	case Clubber:
		return 8;
	}
}

std::map<Point, std::pair<int, Point>> Unit::movement_tiles(Point p, const std::vector<std::vector<Tile>>& tiles)
{
	std::map<Point, std::pair<int, Point>> paths1;
	std::map<Point, std::pair<int, Point>> paths2;
	auto* paths_in = &paths1;
	auto* paths_out = &paths2;

	paths_out->insert(std::make_pair(p, std::make_pair(movement, Point {})));

	bool force_continue;
	do {
		force_continue = false;
		std::swap(paths_in, paths_out);
		paths_out->clear();
		for (const auto& path : *paths_in) {
			auto it = paths_out->find(path.first);
			if (it == paths_out->end()) {
				paths_out->insert(path);
			}
			else if (it->second.first < path.second.first) {
				// replace only if have higher movement points left
				it->second = path.second;
			}
			paths_out->insert(path);
			if (path.second.first > 0) {
				for (int i = 0; i < 4; ++i) {
					auto p = path.first;
					if (i == 0 && p.x >= 1) { --p.x; }
					else if (i == 1 && p.y >= 1) { --p.y; }
					else if (i == 2 && p.x + 1 < tiles[0].size()) { ++p.x; }
					else if (i == 3 && p.y + 1 < tiles.size()) { ++p.y; }
					else { continue; }
					auto movement_points = path.second.first - tiles[p.y][p.x].terrain_cost();
					auto it = paths_out->find(p);
					if (it == paths_out->end()) {
						paths_out->insert(std::make_pair(p, std::make_pair(movement_points, path.first)));
					}
					else if (it->second.first < movement_points) {
						// replace only if have higher movement points left
						it->second = std::make_pair(movement_points, path.first);
						force_continue = true;
					}
				}
			}
		}
	} while (force_continue || paths_in->size() != paths_out->size());

	paths_out->erase(p);
	return *paths_out;
}

int unit_attack(Unit*& u, Unit*& other, int other_cost)
{
	--u->attacks;
	printf("Before: %d, %d ", u->health, other->health);
	other->health -= u->max_attack() * u->health / u->max_health();
	if (u->is_melee()) {
		if (other->health > 0) {
			u->health -= other->max_attack() * other->health / other->max_health();
			if (u->health <= 0) { 
				u = NULL;
				printf("After: dead, %d\n", other->health);
				return 2;
			}
			else {
				printf("After: %d, %d\n", u->health, other->health);
			}
		}
		else {
			u->movement -= other_cost;
			other = u;
			u = NULL;
			printf("After: %d, dead\n", other->health);
			return 1;
		}
	}
	else {
		if (other->health <= 0) {
			other = NULL;
			printf("After: %d, dead\n", u->health);
		}
		else {
			printf("After: %d, %d\n", u->health, other->health);
		}
	}
	return 0;
}
