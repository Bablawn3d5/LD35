// Copyright 2015 Bablawn3d5

#pragma once

#include <entityx/entityx.h>
#include <vector>

struct BlockWhole {
public:
	explicit BlockWhole() {
	}

	~BlockWhole() {}

	std::vector<entityx::Entity::Id> blockParts;
	entityx::Entity::Id pivotBlock;
};
