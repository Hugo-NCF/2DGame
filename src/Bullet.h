#ifndef BULLET_H
#define BULLET_H
#include <SFML/Graphics.hpp>

class Bullet {
public:
    Bullet();
    Bullet(float x, float y, float speed, bool facingRight);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    bool isActive() const;
    void setActive(bool active);
    sf::FloatRect getBounds() const;
    void reset(float x, float y, float speed, bool facingRight);
    float getDamage() const;

private:
    sf::RectangleShape shape;
    float speed;
    bool active;
    bool facingRight;
    float damage;
};

class BulletManager {
public:
    BulletManager(int maxBullets = 20);
    ~BulletManager();
    void fireBullet(float x, float y, bool facingRight);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    int getRemainingBullets() const;
    void reload();
    Bullet* getBullets() const; // Add getter for bullets array
    int getMaxBullets() const; // Add getter for maxBullets

private:
    Bullet* bullets;
    int maxBullets;
    int remainingBullets;
};

#endif
