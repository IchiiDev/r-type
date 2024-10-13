#pragma once

#include <cstdint>

enum class MessageType : uint32_t {
    CONNECTION_REQUEST,
    CONNECTION_REPONSE,

    ENTITY_UPDATED,
    ENTITY_CREATED,
    ENTITY_DELETED,
    INPUT,
};
