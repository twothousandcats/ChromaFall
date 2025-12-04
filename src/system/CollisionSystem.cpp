#include "systems/CollisionSystem.hpp"


#include <SFML/System/Vector2.hpp>

#include "components/Asteroid.hpp"
#include "components/Health.hpp"
#include "components/Position.hpp"
#include "components/Renderable.hpp"
#include "components/Velocity.hpp"
#include "components/Invincibility.hpp"

#include "config/SetupConfig.hpp"

#include "entities/Entity.hpp"

bool CollisionSystem::isIntersects(
    const sf::Vector2f &aPos,
    const sf::Vector2f &bPos,
    const sf::Vector2f &aSize,
    const sf::Vector2f &bSize
) {
    const float aLeft = aPos.x - aSize.x / HALF_DIVISOR;
    const float aRight = aPos.x + aSize.x / HALF_DIVISOR;
    const float aTop = aPos.y - aSize.y / HALF_DIVISOR;
    const float aBottom = aPos.y + aSize.y / HALF_DIVISOR;

    const float bLeft = bPos.x - bSize.x / HALF_DIVISOR;
    const float bRight = bPos.x + bSize.x / HALF_DIVISOR;
    const float bTop = bPos.y - bSize.y / HALF_DIVISOR;
    const float bBottom = bPos.y + bSize.y / HALF_DIVISOR;

    return aRight > bLeft
           && aLeft < bRight
           && aBottom > bTop
           && aTop < bBottom;
}

CollisionSystem::Result CollisionSystem::update(
    Entity &player,
    std::vector<std::unique_ptr<Entity> > &bullets,
    std::vector<std::unique_ptr<Entity> > &asteroids
) {
    Result result;

    // bullet -> asteroid
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        auto *bulletPos = (*bulletIt)->getComponent<Position>();
        auto *bulletRender = (*bulletIt)->getComponent<Renderable>();
        bool bulletHit = false;

        if (bulletPos && bulletRender) {
            sf::Vector2f bulletSize = bulletRender->shape.getSize();

            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                auto *asteroidPos = (*asteroidIt)->getComponent<Position>();
                auto *asteroidRender = (*asteroidIt)->getComponent<Renderable>();
                auto *asteroidHealth = (*asteroidIt)->getComponent<Health>();
                auto *asteroidVelocity = (*asteroidIt)->getComponent<Velocity>();

                if (asteroidPos && asteroidRender && asteroidHealth && asteroidVelocity) {
                    sf::Vector2f asteroidSize = asteroidRender->shape.getSize();
                    // попадание
                    if (isIntersects(
                        bulletPos->value,
                        asteroidPos->value,
                        bulletSize,
                        asteroidSize
                    )) {
                        asteroidHealth->value -= 1.f;
                        bulletHit = true;

                        if (asteroidHealth->value <= 0.f) {
                            auto *asteroidComp = (*asteroidIt)->getComponent<Asteroid>();
                            if (asteroidComp) {
                                result.asteroidsToSplit.emplace_back(
                                    asteroidPos->value,
                                    asteroidVelocity->value,
                                    asteroidComp->size
                                );
                            }

                            result.destroyedAsteroidsCount++;
                            asteroidIt = asteroids.erase(asteroidIt);
                        } else {
                            ++asteroidIt;
                        }
                        break;
                    } else {
                        ++asteroidIt;
                    }
                } else {
                    ++asteroidIt;
                }
            }
        }

        if (bulletHit) {
            bulletIt = bullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    // asteroid -> player
    auto *playerPos = player.getComponent<Position>();
    auto *playerRender = player.getComponent<Renderable>();
    auto *playerHealth = player.getComponent<Health>();
    auto *playerInvisibility = player.getComponent<Invincibility>();

    if (playerPos && playerRender && playerHealth) {
        const sf::Vector2f playerSize = playerRender->shape.getSize();

        for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end(); ++asteroidIt) {
            auto *asteroidPos = (*asteroidIt)->getComponent<Position>();
            auto *asteroidRender = (*asteroidIt)->getComponent<Renderable>();
            if (asteroidPos && asteroidRender) {
                sf::Vector2f asteroidSize = asteroidRender->shape.getSize();
                if (isIntersects(
                    playerPos->value,
                    asteroidPos->value,
                    playerSize,
                    asteroidSize
                )) {
                    if (playerInvisibility && playerInvisibility->isActive()) {
                        continue;
                    }
                    playerHealth->value -= 1.f;
                    result.isPlayerHit = true;

                    if (playerHealth->value <= 0.f) {
                        result.isPlayerDied = true;
                    }

                    if (playerInvisibility) {
                        playerInvisibility->activate();
                    }

                    // std::cout << "player hit!" << playerHealth->value << std::endl;
                }
            }
        }
    }

    return result;
}