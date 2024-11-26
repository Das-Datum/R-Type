#pragma once

#include <memory>
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"
#include "Types.hpp"

/**
 * @class Coordinator
 * @brief Coordinates entities, components, and systems.
 */
class Coordinator
{
    public:
        void init()
        {
            componentManager = std::make_unique<ComponentManager>();
            entityManager = std::make_unique<EntityManager>();
            systemManager = std::make_unique<SystemManager>();
        }

        /**
         * @brief Creates a new entity.
         * @return The new entity ID.
         */
        Entity createEntity()
        {
            return entityManager->createEntity();
        }

        /**
         * @brief Destroys an entity.
         * @param entity The entity to destroy.
         */
        void destroyEntity(Entity entity)
        {
            entityManager->destroyEntity(entity);
            componentManager->entityDestroyed(entity);
            systemManager->entityDestroyed(entity);
        }

        /**
         * @brief Registers a component type.
         * @tparam T Component type.
         */
        template <typename T>
        void registerComponent()
        {
            componentManager->registerComponent<T>();
        }

        /**
         * @brief Adds a component to an entity.
         * @tparam T Component type.
         * @param entity The entity.
         * @param component The component data.
         */
        template <typename T>
        void addComponent(Entity entity, T component)
        {
            componentManager->addComponent<T>(entity, component);

            auto signature = entityManager->getSignature(entity);
            signature.set(componentManager->getComponentTypeID<T>(), true);
            entityManager->setSignature(entity, signature);

            systemManager->entitySignatureChanged(entity, signature);
        }

        /**
         * @brief Removes a component from an entity.
         * @tparam T Component type.
         * @param entity The entity.
         */
        template <typename T>
        void removeComponent(Entity entity)
        {
            componentManager->removeComponent<T>(entity);

            auto signature = entityManager->getSignature(entity);
            signature.set(componentManager->getComponentTypeID<T>(), false);
            entityManager->setSignature(entity, signature);

            systemManager->entitySignatureChanged(entity, signature);
        }

        /**
         * @brief Gets a component from an entity.
         * @tparam T Component type.
         * @param entity The entity.
         * @return Reference to the component data.
         */
        template <typename T>
        T& getComponent(Entity entity)
        {
            return componentManager->getComponent<T>(entity);
        }

        /**
         * @brief Gets the component type ID.
         * @tparam T Component type.
         * @return The component type ID.
         */
        template <typename T>
        ComponentTypeID getComponentTypeID()
        {
            return componentManager->getComponentTypeID<T>();
        }

        // System methods
        /**
         * @brief Registers a system.
         * @tparam T System type.
         * @return A shared pointer to the system.
         */
        template <typename T>
        std::shared_ptr<T> registerSystem()
        {
            return systemManager->registerSystem<T>();
        }

        /**
         * @brief Sets the signature for a system.
         * @tparam T System type.
         * @param signature The signature.
         */
        template <typename T>
        void setSystemSignature(Signature signature)
        {
            systemManager->setSignature<T>(signature);
        }

        /**
         * @brief Checks if an entity has a component.
         * @tparam T Component type.
         * @param entity The entity.
         * @return True if the entity has the component.
         */
        template <typename T>
        bool hasComponent(Entity entity)
        {
            auto signature = entityManager->getSignature(entity);
            auto ComponentTypeID = componentManager->getComponentTypeID<T>();
            return signature.test(ComponentTypeID);
        }

        /**
         * @brief Gets the signature of an entity.
         * @param entity The entity.
         * @return The entity's signature.
         */
        Signature getEntitySignature(Entity entity)
        {
            return entityManager->getSignature(entity);
        }

    private:
        std::unique_ptr<ComponentManager> componentManager;
        std::unique_ptr<EntityManager> entityManager;
        std::unique_ptr<SystemManager> systemManager;
};
