// Copyright 2015-2016 Bablawn3d5

#pragma once

#include <Farquaad/Core/SeralizeableComponentMap.h>
#include <Farquaad/Core/JSONSerializedPrimitiveTypes.hpp>
#include <Components/Sprite.hpp>
#include <Components/GameBody.hpp>
#include <SFML/Graphics.hpp>

template<>
class SerializableHandle<sf::Texture> {
public:
  // Workaround: DR253: http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#253
  // Define these so class becomes non POD for const initalizaiton
  SerializableHandle() {}
  ~SerializableHandle() {}

  inline sf::Texture fromJSON(const Json::Value &v) const {
    sf::Texture texture;
    texture.loadFromFile(v.asString());
    return texture;
  }

  inline Json::Value toJSON(const sf::Texture& component) const {
    Json::Value v = Json::nullValue;
    return v;
  }

  inline void initPy(py::class_<sf::Texture>& py) {
  }
};

template<>
class SerializableHandle<Sprite> :
  public MappedComponent<Sprite> {
public:
  SerializableHandle() :
    MappedComponent("sprite") {
  }

  inline Sprite fromJSON(const Json::Value &v) const {
    Sprite sprite(v["file"].asString());
    return sprite;
  }

  inline Json::Value toJSON(const Sprite& component) const {
    Json::Value v = Json::nullValue;
    return v;
  }

  inline void initPy(py::class_<Sprite>& py) {
  }
};

template<>
class SerializableHandle<GameBody> : public SerializeFromRegistry<GameBody>,
  public MappedComponent<GameBody> {
public:
  SerializableHandle() : SerializeFromRegistry<GameBody>(this->GenerateMap()),
    MappedComponent("gamebody") {
  }

  inline const SerializeFromRegistry<GameBody>::MemberMap GenerateMap() {
    SerializeFromRegistry<GameBody>::MemberMap map;
    AddMember(map, "row", &GameBody::row);
    AddMember(map, "column", &GameBody::column);
    return map;
  }
};
