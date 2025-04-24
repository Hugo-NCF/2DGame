#include "Hitbox.h"

Hitbox::Hitbox(const sf::Vector2f& size) :
    size(size),
    position(0.0f, 0.0f),
    active(true)
{
    shape.setSize(size);
    shape.setOrigin(size.x / 2.0f, size.y / 2.0f);  // Center the origin of the hitbox
    shape.setPosition(position);
    shape.setFillColor(sf::Color(255, 0, 0, 100)); // Semi-transparent red for debugging
    shape.setOutlineColor(sf::Color::Red);
    shape.setOutlineThickness(1.0f);
}

Hitbox::~Hitbox() {
    // Nothing to clean up
}

void Hitbox::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
    shape.setPosition(position);
}

void Hitbox::setPosition(const sf::Vector2f& position) {
    setPosition(position.x, position.y);
}

void Hitbox::setActive(bool active) {
    this->active = active;
}

sf::Vector2f Hitbox::getPosition() const {
    return position;
}

sf::Vector2f Hitbox::getSize() const {
    return size;
}

sf::FloatRect Hitbox::getBounds() const {
    return shape.getGlobalBounds();
}

bool Hitbox::isActive() const {
    return active;
}

bool Hitbox::intersects(const Hitbox& other) const {
    if (!active || !other.isActive()) {
        return false;
    }
    return getBounds().intersects(other.getBounds());
}

bool Hitbox::intersects(const sf::FloatRect& rect) const {
    if (!active) {
        return false;
    }
    return getBounds().intersects(rect);
}

void Hitbox::draw(sf::RenderWindow& window) {
    if (active) {
        window.draw(shape);
    }
}