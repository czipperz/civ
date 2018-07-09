#include "stdafx.h"
#include "Point.h"

Point::Point(int _x, int _y) {
	x = _x;
	y = _y;
}

bool Point::operator<(const Point& p) const
{
	return x < p.x || x == p.x && y < p.y;
}
