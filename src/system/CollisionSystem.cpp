#include "systems/CollisionSystem.hpp"


#include <SFML/System/Vector2.hpp>

#include "components/Asteroid.hpp"
#include "components/BossSpawnedAsteroid.hpp"
#include "components/Damage.hpp"
#include "components/Health.hpp"
#include "components/Position.hpp"
#include "components/Renderable.hpp"
#include "components/Velocity.hpp"
#include "components/Invincibility.hpp"
#include "components/LaserBeam.hpp"
#include "components/PowerUpPickup.hpp"
#include "components/TrapLaser.hpp"

#include "config/SetupConfig.hpp"
#include "config/UpgradesConfig.hpp"

#include "entities/Entity.hpp"

// TODO: РЕФАКТО
struct PowerUpPickup;

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

float CollisionSystem::distanceFromPointToSegment(
    const sf::Vector2f &point,
    const sf::Vector2f &segA,
    const sf::Vector2f &segB
) {
    sf::Vector2f seg = segB - segA;
    sf::Vector2f pt = point - segA;

    float segLenSq = seg.x * seg.x + seg.y * seg.y;
    if (segLenSq == 0.f) {
        return std::sqrt(pt.x * pt.x + pt.y * pt.y);
    }

    float t = std::max(0.f, std::min(1.f, (pt.x * seg.x + pt.y * seg.y) / segLenSq));
    sf::Vector2f projection = segA + t * seg;
    sf::Vector2f diff = point - projection;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

CollisionSystem::Result CollisionSystem::update(
    Entity &player,
    Entity *playerLaser,
    std::vector<std::unique_ptr<Entity> > &bullets,
    std::vector<std::unique_ptr<Entity> > &asteroids,
    std::unique_ptr<Entity> &boss,
    const std::vector<std::unique_ptr<Entity> > &trapLasers,
    std::vector<std::unique_ptr<Entity>>& powerUpPickups
) {
    Result result;

    // bullet -> asteroid
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        auto *bulletPos = (*bulletIt)->getComponent<Position>();
        auto *bulletRender = (*bulletIt)->getComponent<Renderable>();
        auto *bulletDmg = (*bulletIt)->getComponent<Damage>();
        bool bulletHit = false;

        if (bulletPos && bulletRender && bulletDmg) {
            sf::Vector2f bulletSize = bulletRender->shape.getSize();

            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                auto *asteroidPos = (*asteroidIt)->getComponent<Position>();
                auto *asteroidRender = (*asteroidIt)->getComponent<Renderable>();
                auto *asteroidHealth = (*asteroidIt)->getComponent<Health>();
                auto *asteroidVelocity = (*asteroidIt)->getComponent<Velocity>();
                auto *isBossAsteroid = (*asteroidIt)->getComponent<BossSpawnedAsteroid>();

                if (asteroidPos && asteroidRender && asteroidHealth && asteroidVelocity) {
                    sf::Vector2f asteroidSize = asteroidRender->shape.getSize();
                    // попадание
                    if (isIntersects(
                        bulletPos->value,
                        asteroidPos->value,
                        bulletSize,
                        asteroidSize
                    )) {
                        asteroidHealth->value -= bulletDmg->value;
                        bulletHit = true;

                        if (asteroidHealth->value <= 0.f) {
                            auto *asteroidComp = (*asteroidIt)->getComponent<Asteroid>();

                            if (!isBossAsteroid && asteroidComp) {
                                result.asteroidsToSplit.emplace_back(
                                    asteroidPos->value,
                                    asteroidVelocity->value,
                                    asteroidComp->size
                                );
                                result.destroyedAsteroidPositions.push_back(asteroidPos->value);
                                result.destroyedAsteroidsCount++;
                            }
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

    // bullets -> boss
    if (boss) {
        auto *bossPos = boss->getComponent<Position>();
        auto *bossRender = boss->getComponent<Renderable>();
        auto *bossHealth = boss->getComponent<Health>();

        if (bossPos && bossRender && bossHealth) {
            sf::Vector2f bossSize = bossRender->shape.getSize();

            for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
                auto *bulletPos = (*bulletIt)->getComponent<Position>();
                auto *bulletRender = (*bulletIt)->getComponent<Renderable>();
                auto *bulletDmg = (*bulletIt)->getComponent<Damage>();

                if (bulletPos && bulletRender) {
                    if (isIntersects(bulletPos->value, bossPos->value,
                                     bulletRender->shape.getSize(), bossSize)) {
                        bossHealth->value -= bulletDmg->value;
                        result.isBossHit = true;
                        bulletIt = bullets.erase(bulletIt);
                        break;
                    }
                }
                ++bulletIt;
            }
        }
    }

    // trapLasers -> player
    if (!result.isPlayerDied) {
        auto *playerPos = player.getComponent<Position>();
        auto *playerRender = player.getComponent<Renderable>();
        auto *playerHealth = player.getComponent<Health>();
        auto *playerInv = player.getComponent<Invincibility>();

        if (playerPos && playerRender && playerHealth) {
            sf::Vector2f playerCenter = playerPos->value;
            float playerRadius = std::max(
                                     playerRender->shape.getSize().x,
                                     playerRender->shape.getSize().y
                                 ) / 2.f;

            for (const auto &laser: trapLasers) {
                auto *laserComp = laser->getComponent<TrapLaser>();
                auto *laserPos = laser->getComponent<Position>();
                auto *laserRender = laser->getComponent<Renderable>();

                if (!laserComp || !laserPos || !laserRender || !laserComp->isActive) {
                    continue; // фаза подготовки
                }

                // активен - проверка
                float length = laserRender->shape.getSize().y; // высота = длина

                sf::Transform transform = laserRender->shape.getTransform();
                sf::Vector2f localStart(0, 0); // origin
                sf::Vector2f localEnd(0, length); // вниз по локальной Y
                sf::Vector2f start = transform.transformPoint(localStart);
                sf::Vector2f end = transform.transformPoint(localEnd);

                float distance = distanceFromPointToSegment(playerCenter, start, end);
                float laserHalfWidth = laserRender->shape.getSize().x / 2.f;
                float threshold = playerRadius + laserHalfWidth;

                if (distance <= threshold) {
                    if (!playerInv || !playerInv->isActive()) {
                        playerHealth->value -= 1.f;
                        result.isPlayerHit = true;
                        if (playerHealth->value <= 0.f) {
                            result.isPlayerDied = true;
                        }
                        if (playerInv) {
                            playerInv->activate();
                        }
                        break;
                    }
                }
            }
        }
    }

    // playerLaser -> asteroids, boss
    if (playerLaser) {
        auto *laserPos = playerLaser->getComponent<Position>();
        auto *laserRender = playerLaser->getComponent<Renderable>();
        auto *laserDmg = playerLaser->getComponent<Damage>();

        if (laserPos && laserRender && laserDmg) {
            sf::Transform transform = laserRender->shape.getTransform();
            sf::Vector2f localStart(0, 0);
            sf::Vector2f localEnd(0, laserRender->shape.getSize().y);
            sf::Vector2f laserStart = transform.transformPoint(localStart);
            sf::Vector2f laserEnd = transform.transformPoint(localEnd);
            float laserHalfWidth = laserRender->shape.getSize().x / 2.f;

            // ateroids
            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                auto *asteroidPos = (*asteroidIt)->getComponent<Position>();
                auto *asteroidRender = (*asteroidIt)->getComponent<Renderable>();
                auto *asteroidHealth = (*asteroidIt)->getComponent<Health>();
                auto *asteroidVelocity = (*asteroidIt)->getComponent<Velocity>();
                auto *asteroidComp = (*asteroidIt)->getComponent<Asteroid>();
                auto *isBossAsteroid = (*asteroidIt)->getComponent<BossSpawnedAsteroid>();

                if (asteroidPos && asteroidRender && asteroidHealth && asteroidVelocity) {
                    sf::Vector2f asteroidCenter = asteroidPos->value;
                    float asteroidRadius = std::max(
                                               asteroidRender->shape.getSize().x,
                                               asteroidRender->shape.getSize().y
                                           ) / 2.f;

                    float distance = distanceFromPointToSegment(asteroidCenter, laserStart, laserEnd);
                    if (distance <= asteroidRadius + laserHalfWidth) {
                        // Попадание!
                        asteroidHealth->value -= laserDmg->value;
                        if (asteroidHealth->value <= 0.f) {
                            if (!isBossAsteroid && asteroidComp) {
                                result.asteroidsToSplit.emplace_back(
                                    asteroidPos->value,
                                    asteroidVelocity->value,
                                    asteroidComp->size
                                );
                                result.destroyedAsteroidPositions.push_back(asteroidPos->value);
                                result.destroyedAsteroidsCount++;
                            }
                            asteroidIt = asteroids.erase(asteroidIt);
                        } else {
                            ++asteroidIt;
                        }
                    } else {
                        ++asteroidIt;
                    }
                } else {
                    ++asteroidIt;
                }
            }

            // boss
            if (boss) {
                auto *bossPos = boss->getComponent<Position>();
                auto *bossRender = boss->getComponent<Renderable>();
                auto *bossHealth = boss->getComponent<Health>();

                if (bossPos && bossRender && bossHealth) {
                    sf::Vector2f bossCenter = bossPos->value;
                    float bossRadius = std::max(
                                           bossRender->shape.getSize().x,
                                           bossRender->shape.getSize().y
                                       ) / 2.f;

                    float distance = distanceFromPointToSegment(bossCenter, laserStart, laserEnd);
                    if (distance <= bossRadius + laserHalfWidth) {
                        bossHealth->value -= laserDmg->value;
                        result.isBossHit = true;
                    }
                }
            }
        }
    }

    if (playerPos && playerRender) {
        sf::Vector2f playerSize = playerRender->shape.getSize();
        sf::Vector2f playerCenter = playerPos->value;

        for (auto it = powerUpPickups.begin(); it != powerUpPickups.end();) {
            auto* pickupPos = (*it)->getComponent<Position>();
            auto* pickupRender = (*it)->getComponent<Renderable>();
            auto* pickupType = (*it)->getComponent<PowerUpPickup>();

            if (!pickupPos || !pickupRender || !pickupType) {
                ++it;
                continue;
            }

            sf::Vector2f pickupSize(POWER_UP_PICKUP_RADIUS * 2, POWER_UP_PICKUP_RADIUS * 2);
            if (isIntersects(playerCenter, pickupPos->value, playerSize, pickupSize)) {
                result.collectedPowerUps.push_back(pickupType->type);
                it = powerUpPickups.erase(it);
            } else {
                ++it;
            }
        }
    }

    return result;
}
