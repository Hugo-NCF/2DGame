#ifndef HITBOX_H
#define HITBOX_H

#include <SFML/Graphics.hpp>

class Hitbox {
private:
    sf::RectangleShape shape;
    sf::Vector2f size;
    sf::Vector2f position;
    bool active;

public:
    Hitbox(const sf::Vector2f& size);
    ~Hitbox();

    void setPosition(float x, float y);
    void setActive(bool active);
    
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    sf::FloatRect getBounds() const;
    bool isActive() const;
    
    bool intersects(const Hitbox& other) const;
    bool intersects(const sf::FloatRect& rect) const;
    
    void draw(sf::RenderWindow& window);
};

#endif // HITBOX_H
