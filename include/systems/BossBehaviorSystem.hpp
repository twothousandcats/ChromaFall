#pragma once

#include <memory>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

#include "components/BossPatternData.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "config/BossConfig.hpp"
#include "entities/Entity.hpp"

class BossBehaviorSystem {
public:
    BossBehaviorSystem();

    void update(
        std::unique_ptr<Entity> &boss,
        const sf::Vector2f &playerPos,
        std::vector<std::unique_ptr<Entity> > &asteroids, // для MEDIUM/HARD
        std::vector<std::unique_ptr<Entity> > &trapLasers, // для HARD
        float deltaTime
    );

private:
    sf::Texture trapAsteroidTexture;

    void spawnUpwardAsteroids(
        std::vector<std::unique_ptr<Entity> > &asteroids,
        const sf::Vector2f &origin,
        int count = 25,
        float baseSpeed = 120.f,
        sf::Color color = sf::Color::Red
    );

    static void spawnTrapLasers(
        std::vector<std::unique_ptr<Entity> > &trapLasers,
        const sf::Vector2f &playerPosition
    );

    static bool handleEntryPhase(
        BossPatternData &pattern,
        Position &pos,
        Velocity &vel,
        float dt
    );

    static void moveWithinZone(
        BossPatternData &pattern,
        Position &pos,
        Velocity &vel,
        float dt,
        float targetChangeInterval = BOSS_CHANGE_INTERVAL,
        float stiffness = BOSS_STIFFNESS_FACTOR, // коэффициент ускорения в сторону цели
        float damping = BOSS_DAMPING, // коэффициент затухания скорости
        float arriveDistance = BOSS_ARRIVAL_DIST, // радиус покоя вокруг цели
        float arriveSpeed = BOSS_ARRIVAL_SPEED // чтобы не засчитывать пролёт мимо как достижение
    );

    static void initializeMovementZone(
        BossPatternData &pattern,
        const sf::Vector2f &center,
        const sf::Vector2f &size
    );
};
