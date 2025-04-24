#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Hitbox.h"

enum EnemyState {
    IDLE,
    WALKING,
    ATTACKING,
    DEAD
};

class Enemy {
public:
    Enemy();
    ~Enemy();

    void init(float startX, float playerY, bool spawnOnRight);
    void loadTextures();
    void initSprite();
    void update(float deltaTime, const sf::Vector2f& playerPosition);
    void draw(sf::RenderWindow& window);

    void takeDamage(float damage);
    bool isAlive() const;
    bool canBeRemoved() const;
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    float getAttackDamage() const;
    Hitbox* getHitbox() const;
    bool isAttacking() const;

private:
    void updateState(const sf::Vector2f& playerPosition);
    void updateAnimation(float deltaTime);

    sf::Sprite sprite;
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture attackTexture;
    sf::Texture deadTexture;

    EnemyState currentState;
    bool facingRight;
    float speed;
    float health;
    float attackDamage;
    float attackRange;
    float detectionRange;

    int frameWidth;
    int frameHeight;
    int currentFrame;
    int totalFrames;
    float animationTimer;
    float frameDuration;

    float attackCooldown;
    float attackTimer;

    bool isDeathAnimationFinished;
    float deathRemoveTimer;
    float deathRemoveDelay; // e.g., 0.5 seconds
    sf::Vector2f position;

    Hitbox* hitbox;
};

class EnemyManager {
public:
    EnemyManager();
    ~EnemyManager();

    void init(int enemiesPerWave);
    void update(float deltaTime, const sf::Vector2f& playerPosition);
    void render(sf::RenderWindow& window);

    void increaseWave();
    int getCurrentWave() const;
    int getRemainingEnemies() const;
    std::vector<Enemy*>& getEnemies();
    bool isWaveCleared() const;

private:
    void spawnEnemy(const sf::Vector2f& playerPosition);
    void removeDeadEnemies();

    std::vector<Enemy*> enemies;

    float spawnTimer;
    float spawnInterval;
    int maxEnemies;
    int currentWaveNumber;
    int enemiesPerWave;
};

#endif // ENEMY_H
