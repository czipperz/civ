#include "stdafx.h"
#include "Player.h"
#include "Unit.h"
#include "State.h"

Player::Player(Civilization civilization)
	: civilization(civilization)
{
}

MilitaryUnit* Player::create_military_unit(MilitaryUnitType type)
{
	military_units.push_back(std::make_unique<MilitaryUnit>(this, type));
	return military_units.back().get();
}

CivilianUnit* Player::create_civilian_unit(CivilianUnitType type)
{
	civilian_units.push_back(std::make_unique<CivilianUnit>(this, type));
	return civilian_units.back().get();
}

City* Player::create_city(State& state, int x, int y)
{
	City* city = new City(this, Point{ x, y }, state.width, state.height);
	cities.emplace_back(city);
	for (int py = y - 1; py <= y + 1; ++py) {
		if (py < 0 || py >= state.height) { continue; }
		for (int px = x - 1; px <= x + 1; ++px) {
			if (px < 0 || px >= state.width) { continue; }
			if (!state.tiles[py][px].player) {
				state.tiles[py][px].player = this;
				city->owned_points[py][px] = true;
			}
		}
	}
	return city;
}

void Player::end_turn()
{
	for (auto& m : military_units) { m->end_turn(); }
	for (auto& c : civilian_units) { c->end_turn(); }
}
