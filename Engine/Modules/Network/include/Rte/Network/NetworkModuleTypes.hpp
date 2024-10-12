/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleTypes
*/

#pragma once

#include "Rte/BasicComponents.hpp"
#include "Rte/Ecs/Types.hpp"

#include <cstdint>
#include <vector>

namespace Rte::Network {
    struct PackedNewEntity {
        uint32_t id{};
        BasicComponents::Transform transform;
        std::vector<u8> pixels;
        Vec2<u16> size;
    };

    struct PackedUpdateEntity {
        uint32_t id{};
        BasicComponents::Transform transform;
    };

    struct PackedInput {
        bool fly = false;
        bool moveLeft = false;
        bool moveRight = false;
        bool shoot = false;
        float shootingAngle = 0.0F;
    };

    struct PackedTexture {
        std::vector<u8> pixels;
        Vec2<u16> size;
    };

    namespace Events {
        const EventType DISCONNECTED = "Events::DISCONNECTED"_hash;
        const EventType ENTITY_CREATED = "Events::ENTITY_CREATED"_hash;
        const EventType ENTITY_UPDATED = "Events::ENTITY_UPDATED"_hash;
        const EventType ENTITY_DELETED = "Events::ENTITY_DELETED"_hash;
        const EventType PLAYER_CREATED = "Events::PLAYER_CREATED"_hash;
        const EventType PLAYER_DELTED = "Events::PLAYER_DELETED"_hash;
        const EventType INPUT = "Events::INPUT"_hash;
    } // namespace Events

    namespace Events::Params {
        const ParamId PACKED_NEW_ENTITY = "Events::Params::PACKED_NEW_ENTITY"_hash;
        const ParamId PACKED_UPDATE_ENTITY = "Events::Params::PACKED_UPDATE_ENTITY"_hash;
        const ParamId ENTITY_ID = "Events::Params::ENTITY_UID"_hash;
        const ParamId PLAYER_ID = "Events::Params::PLAYER_ID"_hash;
        const ParamId INPUT = "Events::Params::INPUT"_hash;
    } // namespace Events::Params
};
