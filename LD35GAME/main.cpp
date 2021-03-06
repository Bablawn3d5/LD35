// Copyright 2015-2016 Bablawn3d5

#include <boost/python.hpp>
#include <Farquaad/Serialization.hpp>
#include <Core/JSONSerializedExtendedComponents.hpp>
#include <json/json.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <Systems/TextureRenderSystem.h>
#include <Systems/LD35InputSystem.h>
#include <Systems/GameSystem.h>
#include <Systems/SpawnSystem.h>
#include <Components/Sprite.hpp>
#include <Components/BlockWhole.hpp>
#include <Components/GameBody.hpp>
#include <Farquaad/Systems.hpp>
#include <Farquaad/Systems/PythonSystem.h>
#include <Farquaad/Components.hpp>
#include <Farquaad/Core.hpp>
#include <Farquaad/Thor/ResourceLoader.hpp>
#include <Farquaad/Box2D/SFMLB2DDebug.h>
#include <Thor/Resources.hpp>

#include <iomanip>
#include <random>

namespace fs = boost::filesystem;

// Quick test for EntityX
class Application : public entityx::EntityX {
public:
  std::shared_ptr<b2World> physWorld;
  std::shared_ptr<SFMLB2DDebug> debugDraw;
  std::shared_ptr<GameSystem> gsys;

  explicit Application(sf::RenderWindow &target, Json::Value& v) {
    b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
    physWorld = std::make_shared<b2World>(gravity);
    debugDraw = std::make_shared<SFMLB2DDebug>(target);
    debugDraw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_centerOfMassBit |
                        b2Draw::e_aabbBit | b2Draw::e_pairBit);

    systems.add<PhysicsSystem>(physWorld, debugDraw.get());
    auto inputSystem = systems.add<LD35InputSystem>(target);
    inputSystem->setKeybinds(Serializable::fromJSON<InputSystem::KeyBindMap>(v["keys"]));

    systems.add<TextureRenderSystem>(target);

    unsigned int seed = v["seed"].asUInt();
    if ( seed == 0 ) {
      std::random_device rd;
      std::mt19937 gen(rd());
      seed = gen();
      std::cout << "The seed is: " << seed << std::endl;
    }

    systems.add<SpawnSystem>(v["spawn_row"].asInt(), v["spawn_col"].asInt(), seed);
    gsys = systems.add<GameSystem>(v["time_spawn"].asDouble());
    gsys->BASE_TIME_RESET_MOVEMENT = v["move_delay"].asDouble();
    gsys->number_of_lines  = v["num_line_cleared"].asInt();
    gsys->lines_per_level = v["line_per_level"].asDouble();
    gsys->speed_diff = v["line_speed_per_level"].asDouble();
    gsys->lock_delay = v["lock_delay"].asDouble();
    systems.configure();

    //std::string path = fs::current_path().string();
    //auto pythonSystem = systems.add<PythonSystem>(&entities, path.c_str());
    //pythonSystem->add_path("Foo");

    // HACK(SMA) : Create entity right in this bloated constructor.
    thor::ResourceHolder<Json::Value, std::string> holder;
    for ( auto items : v["entities"] ) {
      auto json = holder.acquire(items.asString(),
                                 Resources::loadJSON(items.asString()));
      EntitySerializer es(json);
      auto entity = entities.create();
      es.Load(entity);
    }
  }

  ~Application() {
  }

  void update(ex::TimeDelta dt) {
    systems.update<LD35InputSystem>(dt);
    systems.update<PhysicsSystem>(dt);
    systems.update<TextureRenderSystem>(dt);
    systems.update<SpawnSystem>(dt);
    systems.update<GameSystem>(dt);
    physWorld->DrawDebugData();
  }
};

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 800), "LD35 - Shapetris");
  window.setKeyRepeatEnabled(true);

  // HACK(SMA) : Initalize these component serializers so they become registered.
  {
    Serializable::handle<Body>();
    Serializable::handle<Stats>();
    Serializable::handle<Physics>();
    Serializable::handle<Sprite>();
    Serializable::handle<InputResponder>();
  }

  thor::ResourceHolder<Json::Value, std::string> holder;
  Json::Value configs = holder.acquire("config", Resources::loadJSON("Config.json"));

  Application app(window, configs);

  sf::Clock clock;
  while ( window.isOpen() ) {
    window.clear();
    sf::Time elapsed = clock.restart();
    app.update(elapsed.asSeconds());
    window.display();
    int fps = static_cast<int>(1.f / elapsed.asSeconds());
    std::stringstream ss;
    ss << "LD 35 - Shapetris | FPS: " << fps;
    window.setTitle(ss.str());
  }

  return 0;
}
