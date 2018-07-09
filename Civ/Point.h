#pragma once
#include <SDL.h>

struct Point : public SDL_Point
{
public:
	Point(int x, int y);
	Point() = default;

	bool operator<(const Point&) const;
};

