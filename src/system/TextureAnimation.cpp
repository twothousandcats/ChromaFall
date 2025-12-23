#include "systems/TextureAnimationSystem.hpp"
#include "entities/Entity.hpp"
#include "components/Shape.hpp"
#include "components/Textured.hpp"
#include "components/TextureAnimation.hpp"

void TextureAnimationSystem::update(
    std::vector<std::unique_ptr<Entity> > &entities,
    float deltaTime
) {
    for (auto &entity: entities) {
        if (!entity) continue;
        update(*entity, deltaTime);
    }
}

void TextureAnimationSystem::update(
    Entity &entity,
    float deltaTime
) {
    auto *anim = entity.getComponent<TextureAnimation>();
    auto *shape = entity.getComponent<Shape>();
    auto *textured = entity.getComponent<Textured>();

    if (!anim || !shape || !textured || !textured->value || !anim->playing) {
        return;
    }

    anim->currentTime += deltaTime;
    if (anim->currentTime >= anim->frameDuration) {
        anim->currentTime = 0.0f;
        anim->currentFrame = (anim->currentFrame + 1) % anim->frameCount;

        shape->shape.setTextureRect(
            {
                {
                    anim->currentFrame * anim->frameWidth,
                    0
                },
                {
                    anim->frameWidth,
                    anim->frameHeight
                }
            }
        );
    }
}
