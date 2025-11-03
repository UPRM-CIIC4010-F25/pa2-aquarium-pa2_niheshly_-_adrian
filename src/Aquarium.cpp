#include "Aquarium.h"
#include <cstdlib>


string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        case AquariumCreatureType::PurpleFish:
            return "PurpleFish"; 
        case AquariumCreatureType::GreenFish:
            return "GreenFish";  
        default:
            return "UknownFish";
    }
}

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 10.0f, 1, sprite),
 m_baseSpeed(speed),
 m_scoreMultiplier(1) {}


void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    this->bounce();
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
}

void PlayerCreature::update() {
    this->reduceDamageDebounce();
    this->move();
}


void PlayerCreature::draw() const {
    
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if in damage debounce
    }
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
    ofSetColor(ofColor::white); // Reset color

}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }
    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}

// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 30, 1, sprite) {
    m_dx = (rand() % 3 - 1); // -1, 0, or 1
    m_dy = (rand() % 3 - 1); // -1, 0, or 1
    normalize();

    m_creatureType = AquariumCreatureType::NPCreature;
}

void NPCreature::move() {
    // Simple AI movement logic (random direction)
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce();
}

void NPCreature::draw() const {
    ofLogVerbose() << "NPCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    ofSetColor(ofColor::white);
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
}


BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    normalize();

    setCollisionRadius(60); // Bigger fish have a larger collision radius
    m_value = 5; // Bigger fish have a higher value
    m_creatureType = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}

//purple fish implementation

PurpleFish::PurpleFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed * 1.5, sprite) {

    int diagonal = rand() % 4;
    switch(diagonal) {
        case 0: m_dx = 1.0f; m_dy = 1.0f; break;
        case 1: m_dx = 1.0f; m_dy = -1.0f; break;
        case 2: m_dx = -1.0f; m_dy = 1.0f; break;
        case 3: m_dx = -1.0f; m_dy = -1.0f; break;
    }
    normalize();
    
    setCollisionRadius(45);
    m_value = 3;
    m_creatureType = AquariumCreatureType::PurpleFish;
    
    m_zigzagCounter = 0;
    m_zigzagDuration = 30 + (rand() % 30);
}

void PurpleFish::move() {
    m_zigzagCounter++;
    
    if (m_zigzagCounter >= m_zigzagDuration) {
        m_zigzagCounter = 0;
        m_zigzagDuration = 30 + (rand() % 30);
        
        m_dx = -m_dx;
        m_dy = -m_dy;
    }
    
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    
    if(m_dx < 0) {
        this->m_sprite->setFlipped(true);
    } else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void PurpleFish::draw() const {
    ofLogVerbose() << "PurpleFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}
// GreenFish implementation
GreenFish::GreenFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed * 0.4, sprite) {
    m_dx = (rand() % 2 == 0) ? 0.7f : -0.7f;
    m_dy = (rand() % 2 == 0) ? 0.7f : -0.7f;
    normalize();
    
    setCollisionRadius(55);
    m_value = 4; 
    m_creatureType = AquariumCreatureType::GreenFish;
    
    m_shakeIntensity = 20.0f;
    m_shakePhase = 0.0f;
}

void GreenFish::move() {
    m_shakePhase += 0.05f;
    m_x += m_dx * m_speed * 0.15f;
    m_y += m_dy * m_speed * 0.15f;
    
    float shakeX = sin(m_shakePhase * 4.0f) * m_shakeIntensity;
    float shakeY = cos(m_shakePhase * 3.0f) * m_shakeIntensity;
    
    m_x += shakeX;
    m_y += shakeY;
    
    if (rand() % 200 == 0) {
        m_dx = -m_dx;
        m_dy = -m_dy;
    }
    
    // Simple intensity variation
    if (rand() % 100 == 0) {
        m_shakeIntensity = 15.0f + (rand() % 25); // 15-40 intensity
    }
    
    // Stable sprite direction
    if(m_dx < 0) {
        this->m_sprite->setFlipped(true);
    } else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void GreenFish::draw() const {
    this->m_sprite->draw(this->m_x, this->m_y);
}


// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_purple_fish = std::make_shared<GameSprite>("purple-fish.png", 85, 85);
    this->m_green_fish = std::make_shared<GameSprite>("green-fish.png", 80, 80); 
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>(*this->m_big_fish);
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>(*this->m_npc_fish);
        case AquariumCreatureType::PurpleFish:
            return std::make_shared<GameSprite>(*this->m_purple_fish);
        case AquariumCreatureType::GreenFish:
            return std::make_shared<GameSprite>(*this->m_green_fish);
        return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update() {
    for (auto& creature : m_creatures) {
        creature->move();
    }
    this->Repopulate();
}

void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        ofLogVerbose() << "removing creature " << endl;
        int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
        auto npcCreature = std::static_pointer_cast<NPCreature>(creature);
        this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(npcCreature->GetType(), npcCreature->getValue());
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = rand() % this->getWidth();
    int y = rand() % this->getHeight();
    int speed = 1 + rand() % 25; // Speed between 1 and 25

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
        case AquariumCreatureType::PurpleFish:
            this->addCreature(std::make_shared<PurpleFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::PurpleFish)));
            break;
        case AquariumCreatureType::GreenFish:
            this->addCreature(std::make_shared<GreenFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::GreenFish)));
            break;
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }

}


// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");
    // lets make the levels circular
    int selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
    ofLogVerbose() << "the current index: " << selectedLevelIdx << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);


    if(level->isCompleted()){
        level->levelReset();
        this->currentLevel += 1;
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        ofLogNotice()<<"new level reached : " << selectedLevelIdx << std::endl;
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        this->clearCreatures();
    }

    
    // now lets find how many to respawn if needed 
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    if(toRespawn.size() <= 0 ){return;} // there is nothing for me to do here
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
}


// Aquarium collision detection
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;
    
    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        std::shared_ptr<Creature> npc = aquarium->getCreatureAt(i);
        if (npc && checkCollision(player, npc)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }
    return nullptr;
};

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
    std::shared_ptr<GameEvent> event;

    this->m_player->update();
    this->m_player->updatePowerUpEffects();
    this->m_player->clearExpiredEffects();

    this->m_aquarium->updatePowerUps(this->m_player);

    if (this->updateControl.tick()) {
        event = DetectAquariumCollisions(this->m_aquarium, this->m_player);
        if (event != nullptr && event->isCollisionEvent()) {
            ofLogVerbose() << "Collision detected between player and NPC!" << std::endl;
            if(event->creatureB != nullptr){
                event->print();
                if(this->m_player->getPower() < event->creatureB->getValue()){
                    ofLogNotice() << "Player is too weak to eat the creature!" << std::endl;
                    this->m_player->loseLife(3*60); // 3 frames debounce, 3 seconds at 60fps
                    if(this->m_player->getLives() <= 0){
                        this->m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, this->m_player, nullptr);
                        return;
                    }
                }
                else{

                    this->m_aquarium->spawnRandomPowerUp(event->creatureB->getX(), event->creatureB->getY());

                    this->m_aquarium->removeCreature(event->creatureB);

                    int scoreValue = event->creatureB->getValue() * this->m_player->getScoreMultiplier();

                    this->m_player->addToScore(1, scoreValue);
                    if (this->m_player->getScore() % 25 == 0){
                        this->m_player->increasePower(1);
                        ofLogNotice() << "Player power increased to " << this->m_player->getPower() << "!" << std::endl;
                    }
                    
                }
                
                

            } else {
                ofLogError() << "Error: creatureB is null in collision event." << std::endl;
            }
        }
        this->m_aquarium->update();
    }

}

void AquariumGameScene::Draw() {
    this->m_player->draw();
    this->m_aquarium->draw();
    this->paintAquariumHUD();
    this->m_aquarium->drawPowerUps();



}


void AquariumGameScene::paintAquariumHUD(){
    float panelWidth = ofGetWindowWidth() - 150;
    ofDrawBitmapString("Score: " + std::to_string(this->m_player->getScore()), panelWidth, 20);
    ofDrawBitmapString("Power: " + std::to_string(this->m_player->getPower()), panelWidth, 30);
    ofDrawBitmapString("Lives: " + std::to_string(this->m_player->getLives()), panelWidth, 40);

if (this->m_player->getScoreMultiplier() > 1) {
        ofSetColor(255, 255, 0); // Yellow for multiplier
        ofDrawBitmapString("Multiplier: x" + std::to_string(this->m_player->getScoreMultiplier()), panelWidth, 60);
        ofSetColor(255, 255, 255); // Reset to white
    }

    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth + i * 20, 50, 5);
    }
    ofSetColor(ofColor::white); // Reset color to white for other drawings
}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation == 0){
                return;
            } 
            node->currentPopulation -= 1;
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            this->m_level_score += power;
            return;
        }
    }
}

bool AquariumLevel::isCompleted(){
    return this->m_level_score >= this->m_targetScore;
}

//repopulation
std::vector<AquariumCreatureType> AquariumLevel::Repopulate() {
    std::vector<AquariumCreatureType> creaturesToSpawn;
    
    for (auto& node : m_levelPopulation) {
        if (node->currentPopulation < node->population) {
            creaturesToSpawn.push_back(node->creatureType);
            node->currentPopulation++;
        }
    }
    
    return creaturesToSpawn;
}


//power up implementation
PowerUp::PowerUp(float x, float y, float radius, std::unique_ptr<IPowerUpEffect> effect)
    : m_x(x), m_y(y), m_radius(radius), m_active(true), m_effect(std::move(effect)) {}

void PowerUp::draw() const {
    if (!m_active) return;
    
//visuals
    float pulse = sin(ofGetElapsedTimef() * 6.0f) * 2.0f + m_radius;
    
    ofSetColor(255, 255, 100, 80);
    ofDrawCircle(m_x, m_y, pulse + 5);
    
    ofSetColor(255, 255, 0);
    ofDrawCircle(m_x, m_y, m_radius);
    
    ofSetColor(255, 255, 150);
    ofDrawCircle(m_x - m_radius * 0.3f, m_y - m_radius * 0.3f, m_radius * 0.4f);
    
}
void PowerUp::update() {
    if (m_effect) {
        m_effect->update();
    }
}
bool PowerUp::checkCollision(std::shared_ptr<PlayerCreature> player) const {
    if (!m_active || !player) return false;

    float dx = player->getX() - m_x; 
    float dy = player->getY() - m_y;
    float distance = sqrt(dx * dx + dy * dy);
    float collisionDistance = player->getRadius() + m_radius;

    return distance <= collisionDistance;
}
void PowerUp::applyToPlayer(std::shared_ptr<PlayerCreature> player) {
    if (m_effect && player) {
        player->addPowerUpEffect(std::move(m_effect));
        m_active = false;
    }
}
PowerUpType PowerUp::getType() const {
    return m_effect ? m_effect->getType() : PowerUpType::SpeedBoost;
}
//speed boost
SpeedBoostEffect::SpeedBoostEffect(int boostAmount, int duration)
    : m_boostAmount(boostAmount), m_duration(duration), m_elapsed(0), m_applied(false), m_originalSpeed(0) {}

void SpeedBoostEffect::applyEffect(std::shared_ptr<PlayerCreature> player) {
    if (!player || m_applied) return;
    
    m_originalSpeed = player->getBaseSpeed();
    player->setTemporarySpeed(m_originalSpeed + m_boostAmount);
    m_applied = true;
    ofLogNotice() << "Speed Boost applied! Speed: " << (m_originalSpeed + m_boostAmount);
}
void SpeedBoostEffect::removeEffect(std::shared_ptr<PlayerCreature> player) {
    if (player && m_applied) {
        player->setTemporarySpeed(m_originalSpeed);
        ofLogNotice() << "Speed Boost expired. Speed restored to: " << m_originalSpeed;
    }
}
//extra life
ExtraLifeEffect::ExtraLifeEffect() : m_applied(false) {}

void ExtraLifeEffect::applyEffect(std::shared_ptr<PlayerCreature> player) {
    if (!player || m_applied) return;
    
    player->setLives(player->getLives() + 1);
    m_applied = true;
    ofLogNotice() << "Extra Life collected! Total lives: " << player->getLives();
}
void ExtraLifeEffect::removeEffect(std::shared_ptr<PlayerCreature> player) {}
//power increase
PowerIncreaseEffect::PowerIncreaseEffect(int powerIncrease) 
    : m_powerIncrease(powerIncrease), m_applied(false) {}

void PowerIncreaseEffect::applyEffect(std::shared_ptr<PlayerCreature> player) {
    if (!player || m_applied) return;
    
    player->increasePower(m_powerIncrease);
    m_applied = true;
    ofLogNotice() << "Power Increase! New power level: " << player->getPower();
}
void PowerIncreaseEffect::removeEffect(std::shared_ptr<PlayerCreature> player) {}
//score multiplier
ScoreMultiplierEffect::ScoreMultiplierEffect(int multiplier, int duration)
    : m_multiplier(multiplier), m_duration(duration), m_elapsed(0), m_applied(false) {}

void ScoreMultiplierEffect::applyEffect(std::shared_ptr<PlayerCreature> player) {
    if (!player || m_applied) return;
    
    player->setScoreMultiplier(m_multiplier);
    m_applied = true;
    ofLogNotice() << "Score Multiplier x" << m_multiplier << " activated for " << m_duration << " frames!";
}
void ScoreMultiplierEffect::removeEffect(std::shared_ptr<PlayerCreature> player) {
    if (player && m_applied) {
        player->setScoreMultiplier(1);
        ofLogNotice() << "Score Multiplier expired.";
    }
}
std::shared_ptr<PowerUp> PowerUpFactory::createRandomPowerUp(float x, float y) {
    // Randomly select a power-up type
    PowerUpType type = static_cast<PowerUpType>(rand() % 4);
    return createPowerUp(x, y, type);
}
//creating power up
std::shared_ptr<PowerUp> PowerUpFactory::createPowerUp(float x, float y, PowerUpType type) {
    std::unique_ptr<IPowerUpEffect> effect;
    float radius = 15.0f;
    
    switch (type) {
        case PowerUpType::SpeedBoost:
            effect = std::make_unique<SpeedBoostEffect>(10, 300); //+10 speed for 5 seconds
            break;
        case PowerUpType::ExtraLife:
            effect = std::make_unique<ExtraLifeEffect>();
            break;
        case PowerUpType::PowerIncrease:
            effect = std::make_unique<PowerIncreaseEffect>(1); //+1 power level
            break;
        case PowerUpType::ScoreMultiplier:
            effect = std::make_unique<ScoreMultiplierEffect>(2, 600); //2x multiplier for 10 seconds
            break;
    }
    
    return std::make_shared<PowerUp>(x, y, radius, std::move(effect));
}
void PlayerCreature::addPowerUpEffect(std::unique_ptr<IPowerUpEffect> effect) {
    if (effect) {
        effect->applyEffect(shared_from_this());
        m_activeEffects.push_back(std::move(effect));
        ofLogNotice() << "Power-up effect added: " << m_activeEffects.back()->getName();
    }
}
void PlayerCreature::updatePowerUpEffects() {
    for (auto& effect : m_activeEffects) {
        effect->update();
    }
}
void PlayerCreature::clearExpiredEffects() {
    auto it = m_activeEffects.begin();
    while (it != m_activeEffects.end()) {
        if ((*it)->isExpired()) {
            (*it)->removeEffect(shared_from_this());
            ofLogNotice() << "Power-up effect expired: " << (*it)->getName();
            it = m_activeEffects.erase(it);
        } else {
            ++it;
        }
    }
}
void PlayerCreature::setTemporarySpeed(int speed) {
    m_speed = speed;
}
void Aquarium::addPowerUp(std::shared_ptr<PowerUp> p) {
    m_powerUps.push_back(p);
}
void Aquarium::drawPowerUps() const {
    forEachActivePowerUp([](const std::shared_ptr<PowerUp>& powerUp) {
        powerUp->draw();
    });
}
void Aquarium::updatePowerUps(std::shared_ptr<PlayerCreature> player) {
    if (!player) return;

    for (auto& powerUp : m_powerUps) {
        if (powerUp->isActive()) {
            powerUp->update();
        }
    }

    auto it = m_powerUps.begin();
    while (it != m_powerUps.end()) {
        if ((*it)->isActive() && (*it)->checkCollision(player)) {
            ofLogNotice() << "Power-up collected! Type: " << static_cast<int>((*it)->getType());
            (*it)->applyToPlayer(player);
            it = m_powerUps.erase(it);
        } else {
            ++it;
        }
    }
}
void Aquarium::spawnRandomPowerUp(float x, float y) {
    if (rand() % 100 < 15) {
        auto powerUp = PowerUpFactory::createRandomPowerUp(x, y);
        addPowerUp(powerUp);
        ofLogNotice() << "Power-up spawned at (" << x << ", " << y << ")";
    }
}
void Aquarium::removeInactivePowerUps() {
    auto it = m_powerUps.begin();
    while (it != m_powerUps.end()) {
        if (!(*it)->isActive()) {
            it = m_powerUps.erase(it);
        } else {
            ++it;
        }
    }
}