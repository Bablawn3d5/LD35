// Copyright 2015 Bablawn3d5

#pragma once

#include <entityx/entityx.h>

struct GameBody {
public:
	explicit GameBody(int row = 0, int column = 0) {
	}

	~GameBody() {}

	int column;
	int row;
};
