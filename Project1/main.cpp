#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <fstream>


void initializeRandom() {
    std::srand(static_cast<unsigned>(std::time(nullptr))); 
}


int getRandomX() {
    return std::rand() % 750; 
}


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
    Enemy(float x, float y) {
        texture.loadFromFile("vini.png");
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setScale(0.25, 0.25);
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
    Bullet(float x, float y) : speed(-10.0f) {
        texture.loadFromFile("pilka.png");
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setScale(0.03, 0.03);
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
    initializeRandom(); 

    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Invaders");
    window.setFramerateLimit(60);

    
    Player player;

   
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;

   
    sf::Clock enemySpawnClock;
    sf::Clock bulletClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.move(-5.0f, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.move(5.0f, 0);
        }

        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bulletClock.getElapsedTime().asSeconds() > 0.5f) {
            sf::Vector2f playerPosition = player.getPosition();
            bullets.emplace_back(playerPosition.x + 25.0f, playerPosition.y); 
            bulletClock.restart();
        }

        
        if (enemySpawnClock.getElapsedTime().asSeconds() >= 5.0f) {
            int randomX = getRandomX();
            enemies.emplace_back(randomX, 100); 
            enemySpawnClock.restart();
        }

        
        for (auto it = bullets.begin(); it != bullets.end(); ) {
            it->update();
            if (it->getBounds().top + it->getBounds().height < 0) { 
                it = bullets.erase(it);
            }
            else {
                ++it;
            }
        }

        
        window.clear();
        player.draw(window); 

        for (auto& enemy : enemies) {
            enemy.draw(window); 
        }

        for (auto& bullet : bullets) {
            bullet.draw(window); 
        }

        window.display();
    }

    return 0;
}
