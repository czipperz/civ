#pragma once

#include <vector>
#include <memory>

class MilitaryUnit;
class CivilianUnit;

enum Civilization {
	America, Britain,
};

class Player
{
public:
	Civilization civilization;
	std::vector<std::unique_ptr<MilitaryUnit>> military_units;
	std::vector<std::unique_ptr<CivilianUnit>> civilian_units;

	Player(Civilization civilization);
	~Player();
};

