// Copyright 2015 Bablawn3d5
#include <Systems/GameSystem.h>
#include <Components/Sprite.hpp>
#include <Farquaad/Components.hpp>

GameSystem::GameSystem() {
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
  // Do stuff with inputs
  em.each<InputResponder>(
    [this](ex::Entity entity, InputResponder &responder) {
    for ( auto event_name : responder.responds ) {
      if ( responders.find(event_name) != responders.end() ) {
        responders[event_name](entity);
      }
    }
  });
}
