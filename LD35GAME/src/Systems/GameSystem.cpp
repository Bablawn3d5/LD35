// Copyright 2015 Bablawn3d5
#include <Systems/GameSystem.h>
#include <Components/BlockWhole.hpp>
#include <Components/GameBody.hpp>
#include <Farquaad/Components.hpp>

GameSystem::GameSystem(double timeSpawn) : gameGrid(MAX_ROWS, std::vector<ex::Entity::Id>(MAX_COLUMNS)) {

  timeSinceLastMovement = 0.0;
  this->number_of_lines = 0;
  this->BASE_TIME_SPAWN = timeSpawn;
  this->time_lock = 0;
  this->is_instant = false;

  for (unsigned int i = 0; i < gameGrid.size(); i++) {
	  for (unsigned int j = 0; j < gameGrid[i].size(); j++)
	  {
		  gameGrid[i][j] = ex::Entity::INVALID;
	  }
  }

  responders["+MoveUp"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.y = -1;
    }
  };

  responders["-MoveUp"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.y = 0;
    }
  };

  responders["+MoveDown"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.y = 1;
    }
  };

  responders["-MoveDown"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.y = 0;
    }
  };

  responders["+MoveLeft"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.x = -1;
    }
  };

  responders["-MoveLeft"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.x = 0;
    }
  };

  responders["+MoveRight"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
      e.component<Body>()->direction.x = 1;
    }
  };

  responders["-MoveRight"] = [](ex::Entity e) {
    if ( e.has_component<Body>() ) {
        e.component<Body>()->direction.x = 0;
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

	// Sync up the game grid
	em.each<BlockWhole>(
		[&](ex::Entity entity, BlockWhole &blockWhole) {
		for (auto entityId : blockWhole.blockParts) {
			ex::Entity blockPartEntity = em.get(entityId);
			auto blockPartGameBody = blockPartEntity.component<GameBody>();
			if (gameGrid[blockPartGameBody->row - 1][blockPartGameBody->column - 1] != ex::Entity::INVALID &&
				gameGrid[blockPartGameBody->row - 1][blockPartGameBody->column - 1] != entityId)
			{
				// We spawned on top of something, delete what was there before we replace it.
				// This can also be a game-over condition.
				em.destroy(gameGrid[blockPartGameBody->row - 1][blockPartGameBody->column - 1]);
			}

			gameGrid[blockPartGameBody->row - 1][blockPartGameBody->column - 1] = entityId;
		}
	});

	// Move the moveable things left-right
	em.each<Body, BlockWhole>(
		[&](ex::Entity entity, Body &body, BlockWhole &blockWhole) {

    // (1/4)th spawn time if we're holding down
    if ( body.direction.y == 1 ) {
      this->timeSpawn = BASE_TIME_SPAWN/4;
    } else if ( body.direction.y == -1 ) {
      // Instant drop if we're holding up
      this->timeSpawn = 0;
      this->is_instant = true;
    } else {
      this->timeSpawn = BASE_TIME_SPAWN;
    }

		if (body.direction.x == 0)
		{
			return;
		}

		bool allowHorizontalMovement = true;
		for (auto entityId : blockWhole.blockParts) {
			// NOTE: row/column are 1-based indexing, but we access the game grid using 0-based index.
			ex::Entity blockPartEntity = em.get(entityId);
			auto blockPartGameBody = blockPartEntity.component<GameBody>();
			int currentColumn = blockPartGameBody->column;

			// Check if we are at the edge of the grid
			if ((currentColumn + body.direction.x) <= MAX_COLUMNS && (currentColumn + body.direction.x) > 0)
			{
				// Check if moving the block will collide with another block.
				// Also ignore collisions with your own entity.
				ex::Entity::Id blockPartBesideId = gameGrid[blockPartGameBody->row - 1][currentColumn + static_cast<int>(body.direction.x) - 1];
				if (blockPartBesideId == ex::Entity::INVALID)
				{
					// Movement is allowed here.
				}
				else
				{
					auto blockPartBesideGameBody = em.get(blockPartBesideId).component<GameBody>();
					if (blockPartBesideGameBody->parentId == entity.id())
					{
						// Hello, it's me.
					}
					else
					{
						// There is a block in our way, so don't move
						allowHorizontalMovement = false;
						break;
					}
				}
			}
			else
			{
				// We are at the right side of the grid, so don't move
				allowHorizontalMovement = false;
				break;
			}
		}

    // If valid move, then check if we've moved already
    if ( timeMovement > 0 ) {
      timeMovement -= dt;
      allowHorizontalMovement = false;
    }


		if (allowHorizontalMovement == true)
		{
      this->timeMovement = this->BASE_TIME_RESET_MOVEMENT;
			for (auto entityId : blockWhole.blockParts) {
				// NOTE: row/column are 1-based indexing, but we access the game grid using 0-based index.
				ex::Entity blockPartEntity = em.get(entityId);
				auto blockPartGameBody = blockPartEntity.component<GameBody>();
				int currentColumn = blockPartGameBody->column;

				// All clear below - full speed to the right
				// First, remove the block from its current spot in the grid.
				gameGrid[blockPartGameBody->row - 1][currentColumn - 1] = ex::Entity::INVALID;

				// Set the new position in the grid.
				blockPartGameBody->column = currentColumn + static_cast<int>(body.direction.x);
				gameGrid[blockPartGameBody->row - 1][blockPartGameBody->column - 1] = blockPartEntity.id();
			}
		}
	});
  
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
				// We are at the bottom of the grid, so don't move and add the block to the 'kill' queue.
				entitiesToKill.insert(entity.id());
			}
		}

		if (entitiesToKill.size() == 0) {
      // Move block dwn
      // Move it faster as we clear more lines
      if ( timeSinceLastMovement >= (timeSpawn - ((int)(this->number_of_lines / this->lines_per_level) * this->speed_diff)) ) {
        for ( auto entityId : blockWhole.blockParts ) {
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
        timeSinceLastMovement = 0.0;
        time_lock = 0;
      }
		}
	});

  // Only remove input if we've locked the blocks.
  if ( entitiesToKill.size() != 0 ) {
    if (  is_instant || ( time_lock >= lock_delay ) ) {
      is_instant = false;
      time_lock = 0;
    } else {
      time_lock += dt;
    }

    if ( time_lock == 0 ) {
      for each (ex::Entity::Id entityId in entitiesToKill) {
        ex::Entity entityToKill = em.get(entityId);
        entityToKill.remove<BlockWhole>();
        entityToKill.remove<InputResponder>();
      }
    }
  }

	// Now we check if there are any lines
	for (unsigned int i = 0; i < gameGrid.size(); i++)
	{
		bool isRowFull = true;

		for each(ex::Entity::Id entityId in gameGrid[i])
		{
			if (entityId == ex::Entity::INVALID)
			{
				isRowFull = false;
			}
			else
			{
				ex::Entity currentEntity = em.get(entityId);
				auto currentEntityGameBody = currentEntity.component<GameBody>();
				ex::Entity parentEntity = em.get(currentEntityGameBody->parentId);
				if (parentEntity.has_component<BlockWhole>() == true)
				{
					isRowFull = false;
				}
			}

			if (isRowFull == false)
			{
				// No need to keep processing the row if it's missing a square.
				break;
			}
		}

		if (isRowFull == true)
		{
      this->number_of_lines++;
			// Clear this row
			for (unsigned int j = 0; j < MAX_COLUMNS; j++)
			{
				em.destroy(gameGrid[i][j]);
				gameGrid[i][j] = ex::Entity::INVALID;
			}

			// Move down all blocks above the row
			for (int k = i - 1; k >= 0; k--)
			{
				for (int l = 0; l < MAX_COLUMNS; l++)
				{
					if (gameGrid[k][l] != ex::Entity::INVALID)
					{
						ex::Entity blockPartEntity = em.get(gameGrid[k][l]);
						auto blockPartGameBody = blockPartEntity.component<GameBody>();
						blockPartGameBody->row++;
            // Resync gird
            gameGrid[k][l] = ex::Entity::INVALID;
            assert(gameGrid[k + 1][l] == ex::Entity::INVALID);
            gameGrid[k+1][l] = blockPartEntity.id();
					}
				}
			}
		}
	}


}
