/*
Temat:Space Invaders

*/
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <fstream>

class Player {
	sf::Texture texture;
	sf::Sprite sprite;
public:
	Player() {
		texture.loadFromFile("yamal.png");
		sprite.setTexture(texture);
		sprite.setPosition(375, 400);
		sprite.setScale(0.15, 0.15);

	}
	void move(float dx, float dy) {
		sprite.move(dx, dy);
	}
	void draw(sf::RenderWindow& window) {
		window.draw(sprite);
	}
	sf::Vector2f getPosition() const {
		return sprite.getPosition();
	}
};
class Enemy {
	sf::Texture texture;
	sf::Sprite sprite;
public:
	Enemy(float x,float y) {
		texture.loadFromFile("vini.png");
		sprite.setTexture(texture);
		sprite.setPosition(x, y);
		sprite.setScale(0.25, 0.25);

	}
	void move(float dx, float dy) {
		sprite.move(dx, dy);
	}
	void draw(sf::RenderWindow& window) {
		window.draw(sprite);
	}
	sf::FloatRect getBounds() const {
		return sprite.getGlobalBounds();
	}
};
class Bullet {
	sf::Texture texture;
	sf::Sprite sprite;
	float speed;
public:
	Bullet(float x,float y):speed(-5.0f) {
		texture.loadFromFile("pilka.png");
		sprite.setTexture(texture);
		sprite.setPosition(x,y);
		sprite.setScale(0.06, 0.06);
	}
	void update() {
		sprite.move(0, speed);
	}
	void draw(sf::RenderWindow& window) {
		window.draw(sprite);
	}
	sf::FloatRect getBounds() const {
		return sprite.getGlobalBounds();
	}
};
int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "El Clasico");
	Player player;
	Enemy enemy(100,100);
	Bullet bullet(400, 500);
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			// Ruch gracza
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				player.move(-5.0f, 0);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				player.move(5.0f, 0);
			}
		}
		bullet.update();
		window.clear();
		player.draw(window);
		enemy.draw(window);
		bullet.draw(window);
		window.display();
	}



	return 0;
}