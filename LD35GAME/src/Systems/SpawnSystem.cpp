// Copyright 2016 Bablawn3d5
#include <Systems/SpawnSystem.h>
#include <Components/GameBody.hpp>
#include <Components/BlockWhole.hpp>
#include <Farquaad/Components.hpp>

void SpawnSystem::update(ex::EntityManager & em,
                         ex::EventManager & events, ex::TimeDelta dt) {
  // Check if BlockWhole exists
  bool needToSpawn = false;
  em.each<BlockWhole>(
    [&](ex::Entity entity, BlockWhole &responder) {
    needToSpawn = true;
  }
  );

  // Spawn something within the top limitRow rows of the game.
  // Spawn stuff within limitColnums columns
  if ( needToSpawn ) {
    std::mt19937 gen(seed);
    seed = gen.max();
    std::uniform_int_distribution<> dis(1, 6);
    std::uniform_int_distribution<> randRow(1, limitRow);
    std::uniform_int_distribution<> randCol(1, limitCol);
    // Create a block

    int blocksToCreate = dis(gen);
    ex::Entity movingEnt = em.create();
    auto blockWhole = movingEnt.assign<BlockWhole>();
    while ( blocksToCreate > 0 ) {
      ex::Entity e = em.create();
      blockCreator.Load(e);
      auto b = e.component<GameBody>();
      b->row = randRow(gen);
      b->column = randCol(gen);
      blockWhole->blockParts.push_back(e.id());
      blocksToCreate--;
    }
    blockWhole->pivotBlock = blockWhole->blockParts.at(0);
  }
}
