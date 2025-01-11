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
void generateEnemies(std::vector<Enemy>& enemies, int count) {
    enemies.clear();
    for (int i = 0;i < count;++i) {
        float x, y;
        bool validPosition;
        do {
            validPosition = true;
            x = 50 + std::rand() % 700;
            y = 0 + std::rand() % 150;
            for (const auto& enemy : enemies) {
                if (sf::FloatRect(x, y, 64, 64).intersects(enemy.getBounds())) {
                    validPosition = false;
                    break;
                }
            }
        } while (!validPosition);
        enemies.emplace_back(x, y);
    }
}
int loadMaxLevel(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        int maxLevel;
        file >> maxLevel;
        return maxLevel;

    }
    return 1;
}
void saveMaxLevel(const std::string& filename, int maxLevel) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << maxLevel;

    }
}
int loadSavedLevel(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        int savedLevel;
        file >> savedLevel;
        return savedLevel;

    }
    return 1;
}
void saveCurrentLevel(const std::string& filename, int level) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << level;
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
    
    const std::string saveFile="max_level.txt";
    int maxLevel = loadMaxLevel(saveFile);
    const std::string currentLevelFile = "current_level.txt";
    int level = loadSavedLevel(currentLevelFile);

    
    generateEnemies(enemies, 2);


   
    sf::Clock clock;
    sf::Clock enemyFireClock;
    
    sf::Font font;
    font.loadFromFile("arial.ttf");
   
    sf::Text levelText;
    levelText.setFont(font);
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::White);
    levelText.setString("Level: 1");
    levelText.setPosition(10, 10);

    

    sf::Text maxLevelText;
    maxLevelText.setFont(font);
    maxLevelText.setCharacterSize(24);
    maxLevelText.setFillColor(sf::Color::White);
    maxLevelText.setString("Max Level: " + std::to_string(maxLevel));
    maxLevelText.setPosition(10, 30);


    bool isPaused=false;
    bool showMenu = false;
    int menuIndex = 0;
    int menuState = 0;
    std::vector<std::string>menuOptions = { "Wznow gre","Sterowanie","Wyjdz bez zapisywania","Wyjdz i zapisz" };
    sf::Text menuText[4];
    for (int i = 0;i < 4;i++) {
        menuText[i].setFont(font);
        menuText[i].setCharacterSize(30);
        menuText[i].setFillColor(i == menuIndex ? sf::Color::Yellow : sf::Color::White);
        menuText[i].setString(menuOptions[i]);
        menuText[i].setPosition(300, 200 + i * 50);

    }

    while (window.isOpen()) {
        sf::Event event;
        bool keyProceseed = false;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (menuState == 0) {
                    isPaused = !isPaused;
                    showMenu = isPaused;
                }
                else {
                    menuState = 0;
                }
            }
            if (showMenu) {
                if (menuState == 0) {
                    if (event.type == sf::Event::KeyPressed && !keyProceseed) {
                        if (event.key.code == sf::Keyboard::Up) {
                            menuIndex = (menuIndex - 1 + 4) % 4;
                        }
                        if (event.key.code == sf::Keyboard::Down) {
                            menuIndex = (menuIndex + 1) % 4;
                        }
                    }
                    if (event.type == sf::Event::KeyReleased) {
                        keyProceseed = false;
                    }
                    if (event.key.code == sf::Keyboard::Enter) {
                        switch (menuIndex) {
                        case 0:
                            isPaused = false;
                            showMenu = false;
                            break;
                        case 1:
                            menuState = 1;
                            break;
                        case 2:
                            menuState = 2;
                            break;
                        case 3:
                            saveCurrentLevel(currentLevelFile, level);
                            window.close();
                            break;
                        }
                    }
                }
                else if (menuState == 1) {
                    if (event.type == sf::Event::KeyPressed && event.key.code==sf::Keyboard::Escape) {
                        menuState = 0;
                    }
                }
                else if (menuState==2) {
                    if (event.type == sf::Event::KeyPressed && !keyProceseed) {
                        if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) {
                            menuIndex = (menuIndex + 1) % 2;
                            keyProceseed = true;
                        }
                        if (event.key.code == sf::Keyboard::Enter) {
                            if (menuIndex == 0) {
                                window.close();
                            }
                            else {
                                menuState = 0;
                            }
                        }
                    }
                    if (event.type == sf::Event::KeyReleased) {
                        keyProceseed = false;
                    }
                }
            }
        }
        if (!isPaused) {

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && player.getPosition().x > 0) {
                player.move(-1.0f, 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && player.getPosition().x < 750) {
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
                        enemyBullets.emplace_back(enemy.getPosition().x + 20, enemy.getPosition().y + 50, 1.0f);
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
                ++level;
                levelText.setString("Level: " + std::to_string(level));
                generateEnemies(enemies,1+level);

                if (level > maxLevel) {
                    maxLevel = level;
                    maxLevelText.setString("Max Level: " + std::to_string(maxLevel));
                    saveMaxLevel(saveFile, maxLevel);
                }
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
        window.draw(levelText);
        window.draw(maxLevelText);
        if (showMenu) {
            sf::RectangleShape overlay(sf::Vector2f(800, 600));
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);
            if (menuState == 0) {
                for (int i = 0;i < 4;++i) {
                    menuText[i].setFillColor(i == menuIndex ? sf::Color::Yellow : sf::Color::White);
                    window.draw(menuText[i]);
                }
            }
            else if (menuState == 1) {
                sf::Text controlsText;
                controlsText.setFont(font);
                controlsText.setCharacterSize(24);
                controlsText.setFillColor(sf::Color::White);
                controlsText.setString("Sterowanie\n\nRuch w lewo:Strzalka w lewo\nRuch w prawo:Strzalka w prawo\nStrzal:Spacja\nPauza:ESC");
                controlsText.setPosition(100, 100);
                window.draw(controlsText);
            }
            else if (menuState == 2) {
                sf::Text confirmText;
                confirmText.setFont(font);
                confirmText.setCharacterSize(24);
                confirmText.setFillColor(sf::Color::White);
                confirmText.setString("Czy na pewno chcesz wyjsc? Stracisz caly postep.");
                confirmText.setPosition(150, 200);
                window.draw(confirmText);
                sf::Text textOption[2];
                std::vector<std::string>options = { "Tak","Nie" };
                for (int i = 0;i < 2;i++) {
                    textOption[i].setFont(font);
                    textOption[i].setCharacterSize(24);
                    textOption[i].setFillColor(i == menuIndex ? sf::Color::Yellow : sf::Color::White);
                    textOption[i].setString(options[i]);
                    textOption[i].setPosition(375, 250 + i * 50);
                    window.draw(textOption[i]);
                }
            }

        }
        window.display();
    }

    return 0;
}
