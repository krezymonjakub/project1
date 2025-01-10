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
    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
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

    sf::Vector2f getPosition() const {
        return sprite.getPosition();
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
    Bullet(float x, float y,float speedY) : speed(speedY) {
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
    bool isOffScreen() const {
        return sprite.getPosition().y < 0 || sprite.getPosition().y>600;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Invaders");
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("background.png");
    sf::Sprite background(backgroundTexture);

    
    Player player;

   
    std::vector<Enemy> enemies;
    std::vector<Bullet> playerBullets;
    std::vector<Bullet> enemyBullets;
    for (int i = 0;i < 5; i++) {
        enemies.emplace_back(100 + i * 100, 100);
    }

   
    sf::Clock clock;
    sf::Clock enemyFireClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)&&player.getPosition().x>0) {
            player.move(-1.0f, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)&&player.getPosition().x<750) {
            player.move(1.0f, 0);
        }

        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (clock.getElapsedTime().asSeconds() > 0.5f) {
                playerBullets.emplace_back(player.getPosition().x + 20, player.getPosition().y, -10.0f);
                clock.restart();
            }
        }

        
        if (enemyFireClock.getElapsedTime().asSeconds() > 2.0f) {
            for (auto& enemy : enemies) {
                if (std::rand() % 2 == 0) {
                    enemyBullets.emplace_back(enemy.getPosition().x + 20,enemy.getPosition().y + 50, 5.0f);
                }
            }
            enemyFireClock.restart();
        }

        for (auto it = playerBullets.begin();it != playerBullets.end();) {
            it->update();
            if (it->isOffScreen()) {
                it = playerBullets.erase(it);
            }
            else {
                bool hit = false;
                for (auto enemyIt = enemies.begin();enemyIt != enemies.end();) {
                    if (it->getBounds().intersects(enemyIt->getBounds())) {
                        enemyIt = enemies.erase(enemyIt);
                        hit = true;
                        break;

                    }
                    else {
                        ++enemyIt;
                    }
                }
                if (hit) {
                    it = playerBullets.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        for (auto it = enemyBullets.begin();it != enemyBullets.end();) {
            it->update();
            if (it->isOffScreen()) {
                it = enemyBullets.erase(it);
            }
            else if (it->getBounds().intersects(player.getBounds())) {
                window.close();
            }
            else {
                ++it;
            }
        }

        
        window.clear();
        window.draw(background);
        player.draw(window); 

        for (auto& enemy : enemies) {
            enemy.draw(window); 
        }

        for (auto& bullet : playerBullets) {
            bullet.draw(window); 
        }
        for (auto& bullet : enemyBullets) {
            bullet.draw(window);
        }
        window.display();
    }

    return 0;
}
