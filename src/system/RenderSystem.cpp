#include "../include/systems/RenderSystem.hpp"
#include "../include/entities/Entity.hpp"
#include "../include/components/Position.hpp"
#include "../include/components/Renderable.hpp"

void RenderSystem::render(
    sf::RenderWindow &window,
    std::vector<std::unique_ptr<Entity>> &entities
) {
    window.clear(sf::Color::Black);
    for (auto &entity: entities) {
        auto *pos = entity->getComponent<Position>();
        auto *render = entity->getComponent<Renderable>();
        if (pos && render) {
            render->shape.setPosition(pos->value);
            window.draw(render->shape);
        }
    }
    window.display();
}
