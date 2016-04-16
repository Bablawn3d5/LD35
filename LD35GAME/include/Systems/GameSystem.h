// Copyright 2016 Bablawn3d5

#pragma once

#include <Farquaad/Systems/InputSystem.h>
#include <entityx/entityx.h>
#include <SFML/Window.hpp>
#include <map>
#include <string>

namespace ex = entityx;

struct GameSystem : public ex::System<GameSystem> {
public:
  typedef std::map<const std::string, std::function<void(ex::Entity)>>  KeyBindReponder;

  explicit GameSystem();
  void update(ex::EntityManager &em, ex::EventManager &events, ex::TimeDelta dt);  // NOLINT

private:
  KeyBindReponder responders;
  double timeSinceLastMovement;
};
