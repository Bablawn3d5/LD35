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
    std::uniform_int_distribution<> dis(2, 3);
    std::uniform_int_distribution<> randRow(1, limitRow);
    std::uniform_int_distribution<> randCol(1, limitCol);

    std::set<std::pair<int, int>> created;
    // Create a block
    int blocksToCreate = dis(gen);
    ex::Entity movingEnt = em.create();
    unsigned int color_index = color_chooser(gen);
    int spawn_count = 0;
    auto creator_rand = [&](ex::Entity e) {

      // Exit early if we can't spawn any more.
      if (spawn_count > limitRow*limitCol )
        return;

      // Ensure we dont have a block created in the spot we picked.
      std::pair<int, int> p = { randRow(gen), randCol(gen) };
      while ( created.count(p) != 0 ) {
        p = { randRow(gen), randCol(gen) };
      }
      created.insert(p);
      e.assign<GameBody>(p.first, p.second, movingEnt.id());
      e.assign<Body>();
      e.assign<Sprite>(textureName.at(color_index));
      auto b = e.component<GameBody>();
    };

    auto blockWhole = movingEnt.assign<BlockWhole>();
	movingEnt.assign<InputResponder>();
	auto sdf = movingEnt.assign<Body>();
	auto asdf = sdf.get();
    while ( blocksToCreate > 0 ) {
      ex::Entity e = em.create();
      creator_rand(e);
      blockWhole->blockParts.push_back(e.id());
      blocksToCreate--;
    }
    blockWhole->pivotBlock = blockWhole->blockParts.at(0);

    // Migrate pivot to be the higest possible row.
    auto pivotId = blockWhole->blockParts.at(0);
    auto p = em.get(pivotId).component<GameBody>();
    auto highest = limitRow;
    std::pair<int, int> pair = { highest, p->column};
    // Get higest possible
    // While we're not at the top and current square is occupied
    while ( highest != 1 && created.find(pair) == created.end() ) {
      pair = { highest--, p->column};
    }

    // If the squre is unoccupied move pivot there
    if ( created.find(pair) == created.end() ) {
      created.insert(pair);
      created.erase({ p->row,p->column });
      p->row = pair.first;
      p->column = pair.second;
    }

    // Migrate all the blocks so we have less gaps.
    for ( auto blockId : blockWhole->blockParts ) {
      auto b = em.get(blockId).component<GameBody>();
      auto x2 = abs(b->row - p->row) * abs(b->row - p->row);
      auto y2 = abs(b->column - p->column) * abs(b->column - p->column);
      auto dist2 = x2+y2;
      // If we're not next to or on the piviot.
      // Move towards
      if ( dist2 > 1 ) {
        double dy = (b->row - p->row);
        double dx = (b->column - p->column);
        
        double slope = dy/dx;
        double intercept = p->row - p->column * slope;

        double vec_y = dy / sqrt(dist2);
        double vec_x = dx / sqrt(dist2);

        double x = p->column;
        double y = p->row;
        auto ads = b->column;
        auto add = b->row;
        while ( static_cast<int>(x) != b->column ) {
          std::pair<int, int> p = {static_cast<int>(slope*x+intercept),  static_cast<int>(x)};
          // If its unoccupied move it there.
          if ( created.count(p) == 0 ) {
            created.insert(p);
            created.erase({ b->row,b->column });
            b->row = p.first;
            b->column = p.second;
            
            break;
          }
          x += vec_x;
        }
      }
    }
  
    //Center the block whole around the piviot by shifting all blocks.
    // HACK(SMA) : skip grid resync since we're done moving spawning blocks.
    {
    auto p = em.get(pivotId).component<GameBody>();
    int shift = (limitCol/2) - p->column;
    for ( auto blockId : blockWhole->blockParts ) {
      auto block = em.get(blockId).component<GameBody>();
      block->column = block->column + shift;
      assert(block->column > 0);
      assert(block->column < limitCol);
    }

    }
  }
}
