#pragma once

#include <memory>
#include <vector>
#include <SFML/Graphics/Texture.hpp>

#include "components/Boss.hpp"
#include "entities/Entity.hpp"

class BossSystem {
public:
    BossSystem(); // чисто для инициализации текстур

    std::unique_ptr<Entity> spawnBoss(
        BossType type
    ) const;

private:
    sf::Texture easyBossTexture;
    sf::Texture mediumBossTexture;
    sf::Texture hardBossTexture;
};