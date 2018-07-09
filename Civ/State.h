#pragma once

#include <vector>
#include "Tile.h"
#include <SDL.h>
#include <map>
#include "Point.h"

class State
{
public:
	double xrel;
	double yrel;
	double zoom;
	bool mouse_down;
	int width;
	int height;
	std::vector<std::vector<Tile>> tiles;
	Point selected_tile;
	bool render_military;
	bool render_civilians;
	bool render_resources;

	State(int width, int height);
	int advance_state();
	std::map<Point, std::pair<int, Point>> movement_tiles(const Point& tile) const;
	const Tile& tile(Point t) const;
	Tile& tile(Point t);
};

