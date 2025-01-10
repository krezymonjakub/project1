#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <fstream>



class Player {
    static std::shared_ptr<sf::Texture>texture;   
    sf::Sprite sprite;     

public:
    Player() {
        if (!texture) {
            texture = std::make_shared<sf::Texture>();
            texture->loadFromFile("sprite.png");
        }
        sprite.setTexture(*texture);
        sprite.setPosition(375, 450); 
        sprite.setScale(0.17, 0.17);
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
std::shared_ptr<sf::Texture>Player::texture = nullptr;

class Enemy {
    static std::shared_ptr<sf::Texture>texture;
    sf::Sprite sprite;

public:
    Enemy(float x, float y) {
        if (!texture) {
            texture = std::make_shared<sf::Texture>();
            texture->loadFromFile("enemy.png");
        }
        sprite.setTexture(*texture);
        sprite.setPosition(x, y);
        sprite.setScale(0.5, 0.5);
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
std::shared_ptr<sf::Texture>Enemy::texture = nullptr;

class Bullet {
    static std::shared_ptr<sf::Texture>texture;
    sf::Sprite sprite;
    float speed;

public:
    Bullet(float x, float y,float speedY) : speed(speedY) {
        if (!texture) {
            texture = std::make_shared<sf::Texture>();
            texture->loadFromFile("bullet.png");
        }
        sprite.setTexture(*texture);
        sprite.setPosition(x, y);
        sprite.setScale(0.3, 0.3);
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
std::shared_ptr<sf::Texture>Bullet::texture = nullptr;
bool czyKolizja(const Enemy& newEnemy, const std::vector<Enemy>& enemies) {
    for (const auto& enemy : enemies) {
        if (newEnemy.getBounds().intersects(enemy.getBounds())) {
            return true;
        }
    }
    return false;
}
void spawnEnemies(std::vector<Enemy>& enemies, int level) {
    int enemyCount = 1 + level;
    enemies.clear();
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0;i < enemyCount;i++) {
        for (int i = 0;i < enemyCount;i++) {
            bool positionFound = false;
            while (!positionFound) {
                float x = static_cast<float>(std::rand() % 700 + 50);
                float y = static_cast<float>(std::rand() % 150 + 0);
                Enemy tempEnemy(x, y);
                if (!czyKolizja(tempEnemy, enemies)) {
                    enemies.emplace_back(x, y);
                    positionFound = true;
                }
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Invaders");
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("background.png");
    sf::Sprite background(backgroundTexture);
    sf::Vector2u textureSize = backgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    background.setScale(
        static_cast<float>(windowSize.x) / textureSize.x,
        static_cast<float>(windowSize.y) / textureSize.y
    );

    
    Player player;

   
    std::vector<Enemy> enemies;
    std::vector<Bullet> playerBullets;
    std::vector<Bullet> enemyBullets;
    int level = 0;
    spawnEnemies(enemies, level);


   
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
                playerBullets.emplace_back(player.getPosition().x + 20, player.getPosition().y, -2.0f);
                clock.restart();
            }
        }

        
        if (enemyFireClock.getElapsedTime().asSeconds() > 2.0f) {
            for (auto& enemy : enemies) {
                if (std::rand() % 2 == 0) {
                    enemyBullets.emplace_back(enemy.getPosition().x + 20,enemy.getPosition().y + 50, 1.0f);
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
        if (enemies.empty()) {
            level++;
            spawnEnemies(enemies, level);

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
