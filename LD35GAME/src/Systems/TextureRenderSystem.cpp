// Copyright 2015 Bablawn3d5
#include <Systems/TextureRenderSystem.h>
#include <Components/Sprite.hpp>
#include <Components/GameBody.hpp>
#include <Farquaad/Components.hpp>

void TextureRenderSystem::update(ex::EntityManager & em,
								 ex::EventManager & events, ex::TimeDelta dt) {

	em.each<Body, GameBody>(
		[this](ex::Entity entity, Body &body, GameBody &gameBody) {
		body.position.x = gameBody.column * PIXELS_PER_GRID;
		body.position.y = gameBody.row * PIXELS_PER_GRID;
	});

	em.each<Body, Sprite>(
		[this](ex::Entity entity, Body &body, Sprite &sprite) {
		sprite.sprite.setPosition(body.position);
		target.draw(sprite.sprite);
	});
}
