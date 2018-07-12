#pragma once

#include <vector>
#include "Tile.h"
#include <SDL.h>
#include <map>
#include "Point.h"

enum RenderMode {
	RenderMilitary, RenderCivilians,
};

class State
{
	void handle_unit_attack_move(Point pressed_point);
public:
	double xrel;
	double yrel;
	double zoom;
	bool mouse_down_unmoved;
	int width;
	int height;
	std::vector<std::vector<Tile>> tiles;
	Point selected_point;
	RenderMode render_mode;
	bool render_military() const;
	bool render_civilians() const;
	bool render_resources;

	State(int width, int height);
	int advance_state();
	std::map<Point, std::pair<int, Point>> movement_tiles(const Point& tile) const;
	const Tile& tile(Point t) const;
	Tile& tile(Point t);
};

