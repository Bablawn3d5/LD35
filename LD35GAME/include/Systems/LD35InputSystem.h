// Copyright 2016 Bablawn3d5

#pragma once

#include <Farquaad/Systems/InputSystem.h>
#include <entityx/entityx.h>
#include <SFML/Window.hpp>
#include <map>
#include <string>

namespace ex = entityx;

struct LD35InputSystem : public ex::System<LD35InputSystem> {
public:
  typedef std::map<const std::string, KeyInput> KeyBindMap;

  explicit LD35InputSystem(sf::Window &target) : window(target) {}

  void setKeybinds(const KeyBindMap &);
  void bindEventToKeyPress(const std::string string, const sf::Keyboard::Key key);
  void bindEventToKeyPress(const std::string string, const sf::Mouse::Button button);

  bool testEvent(const std::string eventName, sf::Event e);

  void update(ex::EntityManager &em, ex::EventManager &events, ex::TimeDelta dt);  // NOLINT

private:
  KeyBindMap keyBinds;
  sf::Window& window;
};
