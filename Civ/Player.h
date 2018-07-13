#pragma once

#include <vector>
#include <memory>
#include "Unit.h"
#include "City.h"

enum Civilization {
	America, Britain,
};

class State;

class Player
{
public:
	Civilization civilization;
	std::vector<std::unique_ptr<MilitaryUnit>> military_units;
	std::vector<std::unique_ptr<CivilianUnit>> civilian_units;
	std::vector<std::unique_ptr<City>> cities;

	Player(Civilization civilization);

	MilitaryUnit* create_military_unit(MilitaryUnitType type);
	CivilianUnit* create_civilian_unit(CivilianUnitType type);
	City* create_city(State&, int x, int y);
};

