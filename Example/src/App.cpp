#include "App.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Coordinator.hpp"
#include "Rte/Ecs/Types.hpp"

#include <array>
#include <iostream>
#include <string>

Rte::Coordinator coordinator;   // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

struct TagComponent {
    std::string data;
};

struct TagSystem : public Rte::System {
    void update() {
        for (const auto& entity : this->entities) {
            const TagComponent& tag = coordinator.getComponent<TagComponent>(entity);
            std::cout << tag.data << std::endl;
        }
    }
};

void App::ecsExample() {
    // Register the TagComponent and TagSystem
    coordinator.registerComponent<TagComponent>();
    const std::shared_ptr<TagSystem>& tagSystem = coordinator.registerSystem<TagSystem>();


    // Set tagSystem signature to only have entities with TagComponent
    Rte::Signature signature;
    signature.set(coordinator.getComponentType<TagComponent>());
    coordinator.setSystemSignature<TagSystem>(signature);


    // Add 1000 entites with 1/2 of them having a TagComponent
    std::array<Rte::Entity, 1000> entities{};
    for (Rte::i16 i = 0; i < 1000; i++) {
        entities[i] = coordinator.createEntity();
        if (i % 2 == 0)
            coordinator.addComponent<TagComponent>(entities[i], TagComponent("Entity " + std::to_string(i)));
    }


    // Update the TagSystem to see the entities with TagComponent
    tagSystem->update();


    // Remove the TagComponent from the entities
    for (Rte::i16 i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            coordinator.removeComponent<TagComponent>(entities[i]);
    }


    // Update the TagSystem to see the entities without TagComponent
    tagSystem->update();


    // Destroy all the entities
    for (Rte::i16 i = 0; i < 1000; i++)
        coordinator.destroyEntity(entities[i]);
}

App::App() {
    ecsExample();
}

void App::run() {
}
