// Copyright 2016 Bablawn3d5

#include <Systems/LD35InputSystem.h>
#include <Farquaad/Components.hpp>
#include <string>

void LD35InputSystem::setKeybinds(const KeyBindMap & binds) {
  this->keyBinds = binds;
}

void LD35InputSystem::bindEventToKeyPress(const std::string string, sf::Keyboard::Key key) {
  KeyInput bind;
  bind.userInputType = KeyboardInput;
  bind.eventType = sf::Event::KeyPressed;
  bind.keyCode = key;
  keyBinds["+" + string] = bind;

  bind.eventType = sf::Event::KeyReleased;
  bind.keyCode = key;
  keyBinds["-" + string] = bind;

  return;
}

void LD35InputSystem::bindEventToKeyPress(const std::string string, sf::Mouse::Button button) {
  KeyInput bind;
  bind.userInputType = MouseInput;
  bind.eventType = sf::Event::MouseButtonPressed;
  bind.mouseButton = button;
  keyBinds["+" + string] = bind;

  bind.eventType = sf::Event::MouseButtonReleased;
  bind.mouseButton = button;
  keyBinds["-" + string] = bind;

  return;
}

bool LD35InputSystem::testEvent(const std::string eventName, sf::Event e) {
  try {
    (void)keyBinds.at(eventName);
  }
  catch ( std::out_of_range e ) {
    std::cerr << "[Error] Keybind for event: '" << eventName << "' does not exist!" << std::endl;
    std::cerr << "[Warning] A placeholder unknown keybind will be created." << std::endl;
  }

  KeyInput& k = keyBinds[eventName];

  // Mouse event
  if ( k.eventType == e.type &&
      k.mouseButton == e.mouseButton.button ) {
    return (true);
  }

  // Keyboard event
  if ( k.eventType == e.type &&
      k.keyCode == e.key.code ) {
    return (true);
  }
  return (false);
}

void LD35InputSystem::update(ex::EntityManager &em,
                             ex::EventManager &events, ex::TimeDelta dt) {
  sf::Event Event;

  // Store responders in a refrence vec, and clear old responses
  std::vector<std::reference_wrapper<InputResponder>> responders;
  em.each<InputResponder>(
    [&](ex::Entity entity, InputResponder &responder) {
    responder.responds.clear();
    responders.push_back(responder);
  });

  // Poll window Events
  while ( window.pollEvent(Event) ) {
    // Specail Case: Window closed
    // TODO(SMA) : Handle window close.
    if ( Event.type == sf::Event::Closed ) {
      window.close();
    }

    // HACK (SMA) : Push event name into all responders
    // This looks cancerous but it works.
    for ( auto responder : responders ) {
      for ( auto pairs : keyBinds ) {
        if ( testEvent(pairs.first, Event) ) {
          responder.get().responds.push_back(pairs.first);
        }
      }
    }
  }
}
