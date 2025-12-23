#include "../include/systems/RenderSystem.hpp"
#include "../include/entities/Entity.hpp"
#include "../include/components/Position.hpp"
#include "../include/components/Shape.hpp"
#include "components/Textured.hpp"

void RenderSystem::render(
    sf::RenderWindow &window,
    const std::vector<Entity *> &entities
) {
    for (Entity *entity: entities) {
        if (!entity) continue;
        const auto *pos = entity->getComponent<Position>();
        auto *render = entity->getComponent<Shape>();
        if (!pos || !render) continue;

        render->shape.setPosition(pos->value);

        // текстуру, если компонент есть
        const auto *textured = entity->getComponent<Textured>();
        if (textured && textured->value) {
            render->shape.setTexture(textured->value);
        } else {
            render->shape.setTexture(nullptr); // отключаем текстуру
        }

        window.draw(render->shape);
    }
}

void RenderSystem::render(
    sf::RenderWindow &window,
    const std::vector<std::unique_ptr<Entity> > &entities
) {
    for (const auto &entity: entities) {
        if (!entity) continue;
        const auto *pos = entity->getComponent<Position>();
        auto *render = entity->getComponent<Shape>();
        if (!pos || !render) continue;

        render->shape.setPosition(pos->value);

        const auto *textured = entity->getComponent<Textured>();
        if (textured && textured->value) {
            render->shape.setTexture(textured->value);
        } else {
            render->shape.setTexture(nullptr);
        }

        window.draw(render->shape);
    }
}
