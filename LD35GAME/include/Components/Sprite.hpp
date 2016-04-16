// Copyright 2015 Bablawn3d5

#pragma once

#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>
#include <string>

struct Sprite {
public:
	explicit Sprite(std::string filename = "") {
		sf::Texture t;
		if (!t.loadFromFile(filename))
		{

			// TODO: Do something if this fails.
		}

		texture = t;
		sprite.setTexture(texture);
	}

	Sprite(const Sprite& other) : sprite(other.sprite), texture(other.texture) {
		sprite.setTexture(texture);
	}

	~Sprite() {}

	sf::Sprite sprite;
	sf::Texture texture;
};
