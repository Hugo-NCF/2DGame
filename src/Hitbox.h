#ifndef HITBOX_H
#define HITBOX_H

#include <SFML/Graphics.hpp>

class Hitbox {
public:
    // Constructor accepts a size and optionally a position
    Hitbox(const sf::Vector2f& size = sf::Vector2f(0.0f, 0.0f));

    ~Hitbox();

    // Set the position of the hitbox
    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f& position);

    // Set whether the hitbox is active
    void setActive(bool active);

    // Get the position of the hitbox
    sf::Vector2f getPosition() const;

    // Get the size of the hitbox
    sf::Vector2f getSize() const;

    // Get the global bounds of the hitbox
    sf::FloatRect getBounds() const;

    // Check if the hitbox is active
    bool isActive() const;

    // Check for intersection with another hitbox
    bool intersects(const Hitbox& other) const;

    // Check for intersection with a rectangle
    bool intersects(const sf::FloatRect& rect) const;

    // Check for collision with another rectangle
    bool checkCollision(const sf::FloatRect& otherBounds) const; // Add this method

    // Draw the hitbox (useful for debugging)
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape shape;  // Shape used to represent the hitbox visually
    sf::Vector2f size;        // Size of the hitbox
    sf::Vector2f position;    // Position of the hitbox
    bool active;              // Whether the hitbox is active or not
};

#endif // HITBOX_H
