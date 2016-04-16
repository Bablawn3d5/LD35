// Copyright 2015 Bablawn3d5

#pragma once

#include <entityx/entityx.h>

struct GameBody {
public:
	explicit GameBody(int row = 0, int column = 0, ex::Entity::Id parentId = ex::Entity::INVALID) {
		this->column = column;
		this->row = row;
		this->parentId = parentId;
	}

	~GameBody() {}

	int column;
	int row;
	ex::Entity::Id parentId;
};
