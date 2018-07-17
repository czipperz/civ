#include "stdafx.h"
#include "Resources.h"

Resources& Resources::operator+=(const Resources& other)
{
	food += other.food;
	production += other.production;
	return *this;
}
