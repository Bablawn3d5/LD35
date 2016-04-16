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
				ex::Entity blockPartEntity = em.get(entityId);
				auto blockPartGameBody = blockPartEntity.component<GameBody>();
				int currentRow = blockPartGameBody->row;
				if (currentRow + 1 <= MAX_ROWS)
				{
					blockPartGameBody->row = currentRow + 1;
					gameGrid[blockPartGameBody->row - 1][blockPartGameBody->column - 1] = blockPartEntity.id();
				}
				else
				{
					entitiesToKill.insert(entity.id());
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
