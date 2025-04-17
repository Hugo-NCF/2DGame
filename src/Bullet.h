#ifndef BULLET_H
#define BULLET_H
#include <SFML/Graphics.hpp>
#include <vector>

class Bullet {
public:
    Bullet(float x, float y, float speed, bool facingRight);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    bool isActive() const;
    void setActive(bool active);
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;
    float speed;
    bool active;
    bool facingRight;
};

class BulletManager {
public:
    BulletManager(int maxBullets = 20);
    void fireBullet(float x, float y, bool facingRight);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    int getRemainingBullets() const;
    void reload();

private:
    std::vector<Bullet> bullets;
    int maxBullets;
    int remainingBullets;
};

#endif
