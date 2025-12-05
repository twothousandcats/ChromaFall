#pragma once

#include <memory>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "components/BossPatternData.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "entities/Entity.hpp"

class BossBehaviorSystem {
public:
    static void update(
        std::unique_ptr<Entity> &boss,
        std::vector<std::unique_ptr<Entity> > &asteroids, // для MEDIUM/HARD
        std::vector<std::unique_ptr<Entity> > &trapLasers, // для HARD
        float deltaTime
    );

private:
    static void spawnUpwardAsteroids(
        std::vector<std::unique_ptr<Entity>>& asteroids,
        const sf::Vector2f& origin,
        int count = 25,
        float baseSpeed = 120.f,
        sf::Color color = sf::Color::Red
    );

    static void spawnTrapLasers(
        std::vector<std::unique_ptr<Entity>>& trapLasers
    );


    static bool handleEntryPhase(
        BossPatternData& pattern,
        Position& pos,
        Velocity& vel,
        float dt
    );
};
