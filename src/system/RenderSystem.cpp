#include "../include/systems/RenderSystem.hpp"
#include "../include/entities/Entity.hpp"
#include "../include/components/Position.hpp"
#include "../include/components/Renderable.hpp"

void RenderSystem::render(
    sf::RenderWindow &window,
    const std::vector<std::unique_ptr<Entity> > &entities
) {
    for (auto &entity: entities) {
        const auto *pos = entity->getComponent<Position>();
        if (auto *render = entity->getComponent<Renderable>(); pos && render) {
            render->shape.setPosition(pos->value);
            window.draw(render->shape);
        }
    }
}
