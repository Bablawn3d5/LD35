// Copyright 2015 Bablawn3d5
#include <Systems/TextureRenderSystem.h>
#include <Components/Sprite.hpp>
#include <Farquaad/Components.hpp>

void TextureRenderSystem::update(ex::EntityManager & em,
								 ex::EventManager & events, ex::TimeDelta dt) {
	em.each<Body, Sprite>(
		[this](ex::Entity entity, Body &body, Sprite &sprite) {
		sprite.sprite.setPosition(body.position);
		target.draw(sprite.sprite);
	});
}
