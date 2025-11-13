#pragma once
#include "../include/core/Component.hpp"
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

class Entity {
public:
    template<typename T>
    void addComponent(std::unique_ptr<T> component) {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
        components[typeid(T)] = std::move(component);
    }

    template<typename T>
    T* getComponent() {
        auto it = components.find(typeid(T));
        return (it != components.end()) ? static_cast<T*>(it->second.get()) : nullptr;
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};