// Copyright 2016 Bablawn3d5

#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

struct SpawnSystem : public ex::System<SpawnSystem> {
public:
  explicit SpawnSystem();
  void update(ex::EntityManager &em, ex::EventManager &events, ex::TimeDelta dt);  // NOLINT
};
