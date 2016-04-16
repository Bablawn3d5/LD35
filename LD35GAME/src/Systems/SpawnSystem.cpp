// Copyright 2016 Bablawn3d5
#include <Systems/SpawnSystem.h>
#include <Components/GameBody.hpp>
#include <Components/BlockWhole.hpp>
#include <Components/Sprite.hpp>
#include <Farquaad/Components.hpp>

SpawnSystem::SpawnSystem(int limitRow, int limitColumn, unsigned int startSeed) :
limitRow(limitRow), limitCol(limitColumn), seed(startSeed) {
  std::mt19937 gen(seed);
  color_chooser = std::uniform_int_distribution<>{0,4};
  textureName.push_back("./Resources/BlockG.png");
  textureName.push_back("./Resources/BlockR.png");
  textureName.push_back("./Resources/BlockY.png");
  textureName.push_back("./Resources/BlockP.png");
  textureName.push_back("./Resources/BlockT.png");
};

void SpawnSystem::update(ex::EntityManager & em,
                         ex::EventManager & events, ex::TimeDelta dt) {
  // Check if BlockWhole exists
  bool needToSpawn = true;
  em.each<BlockWhole>(
    [&](ex::Entity entity, BlockWhole &responder) {
    needToSpawn = false;
  }
  );

  // Spawn something within the top limitRow rows of the game.
  // Spawn stuff within limitColnums columns
  if ( needToSpawn ) {
    std::mt19937 gen(seed);
    seed = gen();
    std::uniform_int_distribution<> dis(2, 16);
    std::uniform_int_distribution<> randRow(1, limitRow);
    std::uniform_int_distribution<> randCol(1, limitCol);

    std::set<std::pair<int, int>> created;
    // Create a block
    int blocksToCreate = dis(gen);
    ex::Entity movingEnt = em.create();
    auto creator_rand = [&](ex::Entity e) {
      // Ensure we dont have a block created in the spot we picked.
      std::pair<int, int> p = { randRow(gen), randCol(gen) };
      while ( created.find(p) != created.end() ) {
        p = { randRow(gen), randCol(gen) };
      }
      created.insert(p);
      e.assign<GameBody>(p.first, p.second, movingEnt.id());
      e.assign<Body>();
      unsigned int color_index = color_chooser(gen);
      e.assign<Sprite>(textureName.at(color_index));
      auto b = e.component<GameBody>();
    };

    auto blockWhole = movingEnt.assign<BlockWhole>();
    while ( blocksToCreate > 0 ) {
      ex::Entity e = em.create();
      creator_rand(e);
      blockWhole->blockParts.push_back(e.id());
      blocksToCreate--;
    }
    blockWhole->pivotBlock = blockWhole->blockParts.at(0);
  }
}
