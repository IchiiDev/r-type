# Contribution Guidelines & Documentation

## Documentation

An Entity Component System (ECS) is a design pattern that allows for flexible and efficient entity management. It is a way to organize your game objects in a way that is easy to understand and maintain. The ECS pattern is a way to separate the data from the logic in your game. This makes it easier to manage your game objects and allows for more flexibility in how you design your game.

![](https://i.imgur.com/YHS0Ti9.png)

### Entity

An entity is a game object that is represented by a unique ID. An entity is just an ID and does not contain any data or logic. Entities are used to group components together to create game objects.

```cpp
// Create an entity
Entity myEntity = coordinator.createEntity();

// Entity Destruction
coordinator.destroyEntity(myEntity);
```

### Component

A component is a piece of data that is attached to an entity. Components are used to store data about an entity, such as its position, velocity, or health. Components are used to define the behavior of an entity.

```cpp
struct TransformComponent {
    Vec3<float> position;
    Vec3<float> rotation;
    Vec3<float> scale;
};

void func() {
    // Register a component (mandatory in order to use it)
    coordinator.registerComponent<TransformComponent>();

    // Add a component to an entity
    coordinator.addComponent(
        myEntity,
        TransformComponent{
            .position = Vec3(0, 0, 0),
            .rotation = Vec3(0, 0, 0),
            .scale = Vec3(1, 1, 1)
        }
    );

    // Remove a component from an entity
    coordinator.removeComponent<TransformComponent>(myEntity);

    // Get reference to a component (for read/write)
    TransformComponent& transform = coordinator.getComponent<TransformComponent>(myEntity);
}
```

### System

A system is a piece of logic that operates on entities that have specific components. Systems are used to update the state of the game world. Systems are used to define the behavior of the game.

```cpp
class PhysicsSystem : public Rte::System {  // Must inherit from Rte::System
    void update(float deltaTime) {
        // Entities regrouping correct components are stored in parent class System
        for (auto const& entity : this->entities) {
            // Fetch entities components
            const TransformComponent& transform = coordinator.getComponent<TransformComponent>(entity);
            const RigidBodyComponent& rigidBody = coordinator.getComponent<RigidBodyComponent>(entity);

            // Update components
            transform.position += rigidBody.velocity * deltaTime;
        }
    }
};

void func() {
    // Register a system
    const std::shared_ptr<PhysicsSystem> physicsSystem = coordinator.registerSystem<PhysicsSystem>();

    // Get the signature of the system
    Signature signature;
    signature.set(coordinator.getComponentType<TransformComponent>());  // To depend on the transform component
    signature.set(coordinator.getComponentType<RigidBodyComponent>());  // To depend on the rigid body component
    coordinator.setSystemSignature<PhysicsSystem>(signature);  // Apply components signature to the system

    // Update the system
    physicsSystem->update(deltaTime);
}
```

## How to contribute

### Pull Requests

1. Fork the repository
2. Create a new branch for your feature or bugfix
3. Make your changes
4. Test your changes
5. Open a pull request

### Code Style
Use clang-tidy to check the code style:
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DRTE_CLANG_TIDY=ON
cmake --build build
```

### Commit Messages
We are following the conventional commits specification for commit messages. You can find more information [here](https://www.conventionalcommits.org/en/v1.0.0/).

### Code Review
All submissions, including submissions by project members, require review. We use GitHub pull requests for this purpose. Consult [GitHub Help](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/about-pull-requests) for more information on using pull requests.
You will have to ask for a review from a project member in order to merge your pull request. (`IchiiDev`, `Asti0s`, `emartin2109`, `YohanDecamps`)

### License

By contributing, you agree that your contributions will be licensed under the [MIT License](LICENSE). This means that anyone can use, modify, and distribute your work for any purpose, including commercial use, without any other conditions or requirements.
See: [Open Source Initiative](https://opensource.org/licenses/MIT)
