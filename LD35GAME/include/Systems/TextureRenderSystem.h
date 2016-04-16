// Copyright 2015 Bablawn3d5

#pragma once

#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>

namespace ex = entityx;

struct TextureRenderSystem : public ex::System<TextureRenderSystem> {
public:
	explicit TextureRenderSystem(sf::RenderTarget &targ) : target(targ) {}

	void update(ex::EntityManager &em, ex::EventManager &events, ex::TimeDelta dt);  // NOLINT

private:
	sf::RenderTarget& target;
};
