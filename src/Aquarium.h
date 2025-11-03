#define NOMINMAX // To avoid min/max macro conflict on Windows

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include "Core.h"
#include "ofMain.h"

class PowerUp;
class IPowerUpEffect;
class PlayerCreature;


enum class AquariumCreatureType {
    NPCreature,
    BiggerFish,
    PurpleFish,
    GreenFish
};

string AquariumCreatureTypeToString(AquariumCreatureType t);

class AquariumLevelPopulationNode{
    public:
        AquariumLevelPopulationNode() = default;
        AquariumLevelPopulationNode(AquariumCreatureType creature_type, int population) {
            this->creatureType = creature_type;
            this->population = population;
            this->currentPopulation = 0;
        };
        AquariumCreatureType creatureType;
        int population;
        int currentPopulation;
};

class AquariumLevel : public GameLevel {
    public:
        AquariumLevel(int levelNumber, int targetScore)
        : GameLevel(levelNumber), m_level_score(0), m_targetScore(targetScore){};
        void ConsumePopulation(AquariumCreatureType creature, int power);
        bool isCompleted() override;
        void populationReset();
        void levelReset(){m_level_score=0;this->populationReset();}
        virtual std::vector<AquariumCreatureType> Repopulate() = 0;
    protected:
        std::vector<std::shared_ptr<AquariumLevelPopulationNode>> m_levelPopulation;
        int m_level_score;
        int m_targetScore;

};


class PlayerCreature : public Creature, public std::enable_shared_from_this<PlayerCreature> {
public:

    PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move();
    void draw() const;
    void update();
    void changeSpeed(int speed);
    void setLives(int lives) { m_lives = lives; }
    void setDirection(float dx, float dy);
    float isXDirectionActive() { return m_dx != 0; }
    float isYDirectionActive() {return m_dy != 0; }
    float getDx() { return m_dx; }
    float getDy() { return m_dy; }

    int getScore()const { return m_score; }
    int getLives() const { return m_lives; }
    int getPower() const { return m_power; }
    
    void addToScore(int amount, int weight=1) { m_score += amount * weight; }
    void loseLife(int debounce);
    void increasePower(int value) { m_power += value; }
    void reduceDamageDebounce();

    //powerup

    void addPowerUpEffect(std::unique_ptr<IPowerUpEffect> effect);
    void updatePowerUpEffects();
    void clearExpiredEffects();

    int getBaseSpeed() const { return m_baseSpeed; }
    void setTemporarySpeed(int speed);
    int getScoreMultiplier() const { return m_scoreMultiplier; }
    void setScoreMultiplier(int multiplier) { m_scoreMultiplier = multiplier; }
    
    float getRadius() const { return m_collisionRadius; }
    
private:
    int m_score = 0;
    int m_lives = 3;
    int m_power = 1; // mark current power lvl
    int m_damage_debounce = 0; // frames to wait after eating

    int m_baseSpeed;
    int m_scoreMultiplier = 1;
    std::vector<std::unique_ptr<IPowerUpEffect>> m_activeEffects;
};

class NPCreature : public Creature {
public:
    NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    AquariumCreatureType GetType() {return this->m_creatureType;}
    void move() override;
    void draw() const override;
protected:
    AquariumCreatureType m_creatureType;

};

class BiggerFish : public NPCreature {
public:
    BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};


class AquariumSpriteManager {
    public:
        AquariumSpriteManager();
        ~AquariumSpriteManager() = default;
        std::shared_ptr<GameSprite>GetSprite(AquariumCreatureType t);
    private:
        std::shared_ptr<GameSprite> m_npc_fish;
        std::shared_ptr<GameSprite> m_big_fish;
        std::shared_ptr<GameSprite> m_purple_fish;
        std::shared_ptr<GameSprite> m_green_fish;
};


class Aquarium{
public:
    Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager);
    void addCreature(std::shared_ptr<Creature> creature);
    void addAquariumLevel(std::shared_ptr<AquariumLevel> level);
    void removeCreature(std::shared_ptr<Creature> creature);
    void clearCreatures();
    void update();
    void draw() const;
    void setBounds(int w, int h) { m_width = w; m_height = h; }
    void setMaxPopulation(int n) { m_maxPopulation = n; }
    void Repopulate();
    void SpawnCreature(AquariumCreatureType type);

    //powerups
    void addPowerUp(std::shared_ptr<PowerUp> p);
    void drawPowerUps() const;
    void updatePowerUps(std::shared_ptr<PlayerCreature> player);
    void spawnRandomPowerUp(float x, float y);

    void removeInactivePowerUps();
        template<typename Func>
    void forEachActivePowerUp(Func func) const {
        std::for_each(m_powerUps.begin(), m_powerUps.end(), 
            [&func](const std::shared_ptr<PowerUp>& powerUp) {
                if (powerUp->isActive()) {
                    func(powerUp);
                }
            });
    }

    
    std::shared_ptr<Creature> getCreatureAt(int index);
    int getCreatureCount() const { return m_creatures.size(); }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }


private:
    int m_maxPopulation = 0;
    int m_width;
    int m_height;
    int currentLevel = 0;
    std::vector<std::shared_ptr<Creature>> m_creatures;
    std::vector<std::shared_ptr<Creature>> m_next_creatures;
    std::vector<std::shared_ptr<AquariumLevel>> m_aquariumlevels;
    std::shared_ptr<AquariumSpriteManager> m_sprite_manager;

    std::vector<std::shared_ptr<PowerUp>> m_powerUps;

};


std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player);


class AquariumGameScene : public GameScene {
    public:
        AquariumGameScene(std::shared_ptr<PlayerCreature> player, std::shared_ptr<Aquarium> aquarium, string name)
        : m_player(std::move(player)) , m_aquarium(std::move(aquarium)), m_name(name){}
        std::shared_ptr<GameEvent> GetLastEvent(){return m_lastEvent;}
        void SetLastEvent(std::shared_ptr<GameEvent> event){this->m_lastEvent = event;}
        std::shared_ptr<PlayerCreature> GetPlayer(){return this->m_player;}
        std::shared_ptr<Aquarium> GetAquarium(){return this->m_aquarium;}
        string GetName()override {return this->m_name;}
        void Update() override;
        void Draw() override;
    private:
        void paintAquariumHUD();
        std::shared_ptr<PlayerCreature> m_player;
        std::shared_ptr<Aquarium> m_aquarium;
        std::shared_ptr<GameEvent> m_lastEvent;
        string m_name;
        AwaitFrames updateControl{5};
};


class Level_0 : public AquariumLevel  {
    public:
        Level_0(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::GreenFish, 20));
            

        };
        std::vector<AquariumCreatureType> Repopulate() override;

};
class Level_1 : public AquariumLevel  {
    public:
        Level_1(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 15));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::PurpleFish, 5));

        };
        std::vector<AquariumCreatureType> Repopulate() override;


};
class Level_2 : public AquariumLevel  {
    public:
        Level_2(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 25));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 5));

        };
        std::vector<AquariumCreatureType> Repopulate() override;

};
class Level_3 : public AquariumLevel  {
    public:
        Level_3(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::PurpleFish, 30));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 5));

        };
        std::vector<AquariumCreatureType> Repopulate() override;

};
class Level_4 : public AquariumLevel  {
    public:
        Level_4(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::GreenFish, 3));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 7));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 10));

        };
        std::vector<AquariumCreatureType> Repopulate() override;

};
class Level_5 : public AquariumLevel  {
    public:
        Level_5(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::GreenFish, 5));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 5));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::PurpleFish, 10));

        };
        std::vector<AquariumCreatureType> Repopulate() override;

};
class Level_6 : public AquariumLevel  {
    public:
        Level_6(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::GreenFish, 10));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 5));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::PurpleFish, 15));

        };
        std::vector<AquariumCreatureType> Repopulate() override;

};
enum class PowerUpType {
    SpeedBoost,
    ExtraLife, 
    PowerIncrease,
    ScoreMultiplier
};
class IPowerUpEffect {
public:
    virtual ~IPowerUpEffect() = default;
    virtual void applyEffect(std::shared_ptr<PlayerCreature> player) = 0;
    virtual void removeEffect(std::shared_ptr<PlayerCreature> player) = 0;
    virtual bool isExpired() const = 0;
    virtual void update() = 0;
    virtual std::string getName() const = 0;
    virtual PowerUpType getType() const = 0;
};
class PowerUp {
private:
    float m_x;
    float m_y;
    float m_radius;
    bool m_active;
    std::unique_ptr<IPowerUpEffect> m_effect;


public:

    PowerUp(float x, float y, float radius, std::unique_ptr<IPowerUpEffect> effect);
    virtual ~PowerUp() = default;
    
    void draw() const;
    void update();
    bool checkCollision(std::shared_ptr<PlayerCreature> player) const;
    void applyToPlayer(std::shared_ptr<PlayerCreature> player);

    bool isActive() const { return m_active; }
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getRadius() const { return m_radius; }
    PowerUpType getType() const;
    
    void deactivate() { m_active = false; }
};
class SpeedBoostEffect : public IPowerUpEffect {
private:
    int m_originalSpeed;
    int m_boostAmount;
    int m_duration;
    int m_elapsed;
    bool m_applied;
    
public:
    SpeedBoostEffect(int boostAmount, int duration);
    void applyEffect(std::shared_ptr<PlayerCreature> player) override;
    void removeEffect(std::shared_ptr<PlayerCreature> player) override;
    bool isExpired() const override { return m_elapsed >= m_duration; }
    void update() override { if (m_applied && m_elapsed < m_duration) m_elapsed++; }
    std::string getName() const override { return "Speed Boost"; }
    PowerUpType getType() const override { return PowerUpType::SpeedBoost; }
};

class ExtraLifeEffect : public IPowerUpEffect {
private:
    bool m_applied;
public:
    ExtraLifeEffect();
    void applyEffect(std::shared_ptr<PlayerCreature> player) override;
    void removeEffect(std::shared_ptr<PlayerCreature> player) override;
    bool isExpired() const override { return m_applied; } // Instant effect
    void update() override {} 
    std::string getName() const override { return "Extra Life"; }
    PowerUpType getType() const override { return PowerUpType::ExtraLife; }
};
class PowerIncreaseEffect : public IPowerUpEffect {
private:
    int m_powerIncrease;
    bool m_applied;
public:
    PowerIncreaseEffect(int powerIncrease);
    void applyEffect(std::shared_ptr<PlayerCreature> player) override;
    void removeEffect(std::shared_ptr<PlayerCreature> player) override;
    bool isExpired() const override { return m_applied; } // Permanent effect
    void update() override {} 
    std::string getName() const override { return "Power Increase"; }
    PowerUpType getType() const override { return PowerUpType::PowerIncrease; }
};
class ScoreMultiplierEffect : public IPowerUpEffect {
private:
    int m_multiplier;
    int m_duration;
    int m_elapsed;
    bool m_applied;
public:
    ScoreMultiplierEffect(int multiplier, int duration);
    void applyEffect(std::shared_ptr<PlayerCreature> player) override;
    void removeEffect(std::shared_ptr<PlayerCreature> player) override;
    bool isExpired() const override { return m_elapsed >= m_duration; }
    void update() override { if (m_applied && m_elapsed < m_duration) m_elapsed++; }
    std::string getName() const override { return "Score Multiplier"; }
    PowerUpType getType() const override { return PowerUpType::ScoreMultiplier; }
};
class PowerUpFactory {
public:
    static std::shared_ptr<PowerUp> createRandomPowerUp(float x, float y);
    static std::shared_ptr<PowerUp> createPowerUp(float x, float y, PowerUpType type);
};
// new fish
class PurpleFish : public NPCreature {
public:
    PurpleFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
private:
    int m_zigzagCounter;
    int m_zigzagDuration;
};
class GreenFish : public NPCreature {
public:
    GreenFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
private:
    float m_shakeIntensity;
    float m_shakePhase;
};
