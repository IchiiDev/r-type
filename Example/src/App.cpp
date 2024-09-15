#include "App.hpp"
#include "Ecs/Coordinator.hpp"
#include "Ecs/Types.hpp"

#include <iostream>
#include <string>
#include <array>

Rte::Coordinator coordinator;   // NOLINT

struct TagComponent {
    std::string data;
};

struct TagSystem : public Rte::System {
    void update() {
        for (auto const& entity : this->entities) {
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
    for (int i = 0; i < 1000; i++) {
        entities[i] = coordinator.createEntity();
        if (i % 2 == 0)
            coordinator.addComponent<TagComponent>(entities[i], TagComponent("Entity " + std::to_string(i)));
    }


    // Update the TagSystem to see the entities with TagComponent
    tagSystem->update();


    // Remove the TagComponent from the entities
    for (int i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            coordinator.removeComponent<TagComponent>(entities[i]);
    }


    // Update the TagSystem to see the entities without TagComponent
    tagSystem->update();


    // Destroy all the entities
    for (int i = 0; i < 1000; i++)
        coordinator.destroyEntity(entities[i]);
}

App::App() {
    m_window = Rte::createWindow(800, 600, "Example");

    // Lambda event listener to quit the app
    coordinator.addEventListener(LAMBDA_LISTENER(Rte::Events::Window::QUIT, [this](const Rte::Event& /* UNUSED */) {
        m_running = false;
    }));

    // Run the ECS example
    ecsExample();
}

void App::run() {
    while (m_running) {
        m_window->clear();
        m_window->update();
        m_window->display();
    }
}
