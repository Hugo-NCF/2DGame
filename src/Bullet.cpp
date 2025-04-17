#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include <iostream>

// Bullet implementation
Bullet::Bullet() : speed(0), active(false), facingRight(true) {
    shape.setSize(sf::Vector2f(10.f, 5.f));
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(0, 0);
}

Bullet::Bullet(float x, float y, float speed, bool facingRight) : speed(speed), active(true), facingRight(facingRight) {
    shape.setSize(sf::Vector2f(10.f, 5.f));
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(x, y);
}

void Bullet::update(float deltaTime) {
    if (active) {
        float direction = facingRight ? 1.0f : -1.0f;
        shape.move(direction * speed * deltaTime, 0);

        // Check if bullet is off-screen
        if (shape.getPosition().x < 0 || shape.getPosition().x > 1280) {
            active = false;
        }
    }
}

void Bullet::render(sf::RenderWindow& window) {
    if (active) {
        window.draw(shape);
    }
}

bool Bullet::isActive() const {
    return active;
}

void Bullet::setActive(bool active) {
    this->active = active;
}

sf::FloatRect Bullet::getBounds() const {
    return shape.getGlobalBounds();
}

void Bullet::reset(float x, float y, float speed, bool facingRight) {
    this->speed = speed;
    this->active = true;
    this->facingRight = facingRight;
    shape.setPosition(x - 28, y - 18);
}

// BulletManager implementation
BulletManager::BulletManager(int maxBullets) : maxBullets(maxBullets), remainingBullets(maxBullets) {
    bullets = new Bullet[maxBullets];
    // Initialize all bullets as inactive
    for (int i = 0; i < maxBullets; i++) {
        bullets[i].setActive(false);
    }
}

BulletManager::~BulletManager() {
    delete[] bullets;
}

void BulletManager::fireBullet(float x, float y, bool facingRight) {
    if (remainingBullets <= 0) {
        std::cout << "Out of ammo! Press R to reload." << std::endl;
        return;
    }

    // Find an inactive bullet to reuse
    for (int i = 0; i < maxBullets; i++) {
        if (!bullets[i].isActive()) {
            bullets[i].reset(x, y, 600.f, facingRight);
            remainingBullets--;
            return;
        }
    }
    
    // If we get here, all bullets are active (shouldn't happen with proper management)
    std::cout << "No inactive bullets available!" << std::endl;
}

void BulletManager::update(float deltaTime) {
    for (int i = 0; i < maxBullets; i++) {
        if (bullets[i].isActive()) {
            bullets[i].update(deltaTime);
        }
    }
}

void BulletManager::render(sf::RenderWindow& window) {
    for (int i = 0; i < maxBullets; i++) {
        if (bullets[i].isActive()) {
            bullets[i].render(window);
        }
    }
}

int BulletManager::getRemainingBullets() const {
    return remainingBullets;
}

void BulletManager::reload() {
    remainingBullets = maxBullets;
    std::cout << "Reloaded! Bullets: " << remainingBullets << std::endl;
}
