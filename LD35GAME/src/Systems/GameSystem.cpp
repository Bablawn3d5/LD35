// Copyright 2015 Bablawn3d5
#include <Systems/GameSystem.h>
#include <Components/BlockWhole.hpp>
#include <Components/GameBody.hpp>
#include <Farquaad/Components.hpp>

GameSystem::GameSystem(double timeSpawn) : gameGrid(MAX_ROWS, std::vector<ex::Entity::Id>(MAX_COLUMNS)) {

  timeSinceLastMovement = 0.0;
  this->timeSpawn = timeSpawn;

  responders["+MoveUp"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.y -= 1;
    }
  };

  responders["-MoveUp"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.y += 1;
    }
  };

  responders["+MoveDown"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.y += 1;
    }
  };

  responders["-MoveDown"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.y -= 1;
    }
  };

  responders["+MoveLeft"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.x -= 1;
    }
  };

  responders["-MoveLeft"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.x += 1;
    }
  };

  responders["+MoveRight"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.x += 1;
    }
  };

  responders["-MoveRight"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.x -= 1;
    }
  };
}

void GameSystem::update(ex::EntityManager & em,
	ex::EventManager & events, ex::TimeDelta dt) {

	timeSinceLastMovement += dt;

	// Do stuff with inputs
	em.each<InputResponder>(
		[this](ex::Entity entity, InputResponder &responder) {
		for (auto event_name : responder.responds) {
			if (responders.find(event_name) != responders.end()) {
				responders[event_name](entity);
			}
		}
	});

	if (timeSinceLastMovement >= timeSpawn )
	{
		// Do stuff with moving parts
		std::set<ex::Entity::Id> entitiesToKill;

		em.each<BlockWhole>(
			[&](ex::Entity entity, BlockWhole &blockWhole) {

			for (auto entityId : blockWhole.blockParts) {
				// NOTE: row/column are 1-based indexing, but we access the game grid using 0-based index.
				ex::Entity blockPartEntity = em.get(entityId);
				auto blockPartGameBody = blockPartEntity.component<GameBody>();
				int currentRow = blockPartGameBody->row;
				// Check if we are at the bottom of the game grid.
				if (currentRow + 1 <= MAX_ROWS)
				{
					// Check if moving the block down will collide with another block.
					// Also ignore collisions with your own entity.
					ex::Entity::Id blockPartBelowId = gameGrid[currentRow][blockPartGameBody->column - 1];
					if (blockPartBelowId == ex::Entity::INVALID)
					{
						// Movement is allowed here.
					}
					else
					{
						auto blockPartBelowGameBody = em.get(blockPartBelowId).component<GameBody>();
						auto as = blockPartBelowGameBody->parentId;
						auto asd = entity.id();
						if (blockPartBelowGameBody->parentId == entity.id())
						{
							// Hello, it's me.
						}
						else
						{
							// There is a block below us, so don't move and add the block to the 'kill' queue.
							entitiesToKill.insert(entity.id());
						}
					}
				}
				else
				{
					// We are atr the bottom of the grid, so don't move and add the block to the 'kill' queue.
					entitiesToKill.insert(entity.id());
				}
			}

			if (entitiesToKill.size() == 0)
			{
				for (auto entityId : blockWhole.blockParts) {
					// NOTE: row/column are 1-based indexing, but we access the game grid using 0-based index.
					ex::Entity blockPartEntity = em.get(entityId);
					auto blockPartGameBody = blockPartEntity.component<GameBody>();
					int currentRow = blockPartGameBody->row;
					
					// All clear below - full speed ahead
					// First, remove the block from its current spot in the grid.
					gameGrid[currentRow - 1][blockPartGameBody->column - 1] = ex::Entity::INVALID;

					// Set the new position in the grid.
					blockPartGameBody->row = currentRow + 1;
					gameGrid[currentRow][blockPartGameBody->column - 1] = blockPartEntity.id();
				}
			}
		});

		for each (ex::Entity::Id entityId in entitiesToKill)
		{
			ex::Entity entityToKill = em.get(entityId);
			entityToKill.remove<BlockWhole>();
		}

		timeSinceLastMovement = 0.0;
	}
}
