#include "systems/PlayerControlSystem.hpp"

#include "components/Position.hpp"
#include "config/PlayerConfig.hpp"
#include "entities/Entity.hpp"

void PlayerControlSystem::update(
    Entity &player,
    const sf::RenderWindow &window
) {
    auto *pos = player.getComponent<Position>();
    if (!pos) {
        return;
    }

    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    const auto mouseX = static_cast<float>(mousePos.x);
    const auto mouseY = static_cast<float>(mousePos.y);
    const float windowWidth = static_cast<float>(window.getSize().x);
    const float windowHeight = static_cast<float>(window.getSize().y);

    pos->value.x = std::clamp(mouseX, PLAYER_SIDE, windowWidth - PLAYER_SIDE);
    pos->value.y = std::clamp(mouseY, PLAYER_SIDE, windowHeight - PLAYER_SIDE);
}
