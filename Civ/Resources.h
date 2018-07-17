#pragma once
class Resources
{
public:
	int food;
	int production;

	Resources& operator+=(const Resources& other);
};

