// Copyright 2016 Bablawn3d5

#pragma once

#include <entityx/entityx.h>
#include <random>

namespace ex = entityx;

struct SpawnSystem : public ex::System<SpawnSystem> {
public:
  explicit SpawnSystem(int limitRow, int limitColumn, unsigned int startSeed = 42) :
    limitRow(limitRow), limitCol(limitColumn), seed(startSeed) {
    std::mt19937 gen(seed);
  };
  void update(ex::EntityManager &em, ex::EventManager &events, ex::TimeDelta dt);  // NOLINT

private:
  int limitCol, limitRow;
  unsigned int seed;
};
