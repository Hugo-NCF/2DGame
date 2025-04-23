#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <string>

class Hitbox; // Forward declaration

enum class EnemyState {
    IDLE,
    WALKING,
    ATTACKING,
    DEAD
};

class Enemy {
private:
    sf::Vector2f position;
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture attackTexture;
    sf::Texture deadTexture;
    sf::Sprite sprite;
    
    EnemyState currentState;
    bool facingRight;
    
    float speed;
    float health;
    float attackDamage;
    float attackRange;
    float detectionRange;
    
    // Animation properties
    int frameWidth;
    int frameHeight;
    int currentFrame;
    int totalFrames;
    float animationTimer;
    float frameDuration;
    
    // Attack cooldown
    float attackCooldown;
    float attackTimer;
    
    // For death animation
    bool isDeathAnimationFinished;
    
    Hitbox* hitbox;
    
    void updateAnimation(float deltaTime);
    void updateState(const sf::Vector2f& playerPosition);
    void loadTextures();
    void initSprite();

public:
    Enemy();
    ~Enemy();
    
    void init(float startX, bool spawnOnRight);
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
};

class EnemyManager {
private:
    std::vector<Enemy*> enemies;
    float spawnTimer;
    float spawnInterval;
    int maxEnemies;
    int currentWaveNumber;
    int enemiesPerWave;
    
public:
    EnemyManager();
    ~EnemyManager();
    
    void init(int enemiesPerWave = 5);
    void update(float deltaTime, const sf::Vector2f& playerPosition);
    void render(sf::RenderWindow& window);
    
    void spawnEnemy(float playerX);
    void removeDeadEnemies();
    
    void increaseWave();
    int getCurrentWave() const;
    int getRemainingEnemies() const;
    std::vector<Enemy*>& getEnemies();
    
    bool isWaveCleared() const;
};

#endif // ENEMY_H
