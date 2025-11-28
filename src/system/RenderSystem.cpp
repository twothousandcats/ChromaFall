#include "../include/systems/RenderSystem.hpp"
#include "../include/entities/Entity.hpp"
#include "../include/components/Position.hpp"
#include "../include/components/Renderable.hpp"

void RenderSystem::render(
    sf::RenderWindow &window,
    const std::vector<Entity *> &entities
) {
    for (Entity *entity: entities) {
        if (!entity) {
            continue;
        }
        const auto *pos = entity->getComponent<Position>();
        auto *render = entity->getComponent<Renderable>();
        if (pos && render) {
            render->shape.setPosition(pos->value);
            window.draw(render->shape);
        }
    }
}

void RenderSystem::render(
        sf::RenderWindow &window,
        const std::vector<std::unique_ptr<Entity> > &entities
    ) {
    for (const auto &entity: entities) {
        if (!entity) {
            continue;
        }

        const auto *pos = entity->getComponent<Position>();
        auto *render = entity->getComponent<Renderable>();
        if (pos && render) {
            render->shape.setPosition(pos->value);
            window.draw(render->shape);
        }
    }
}
