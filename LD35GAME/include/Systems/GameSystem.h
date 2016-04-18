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

  explicit GameSystem(double timeSpawn);
  void update(ex::EntityManager &em, ex::EventManager &events, ex::TimeDelta dt);  // NOLINT

  double timeMovement;
  double BASE_TIME_RESET_MOVEMENT;
  double BASE_TIME_SPAWN;
private:
  KeyBindReponder responders;
  double timeSinceLastMovement;
  double timeSpawn;

  static const int MAX_ROWS = 16;
  static const int MAX_COLUMNS = 10;
  std::vector<std::vector<ex::Entity::Id>> gameGrid;
};
