#include "Rte/Common.hpp"

#include "RigidBodyImpl.hpp"
#include "Rte/Physics/Tool.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace Rte::Physics;

RigidBodyImpl::RigidBodyImpl(BodyType type, const std::vector<u8>& pixels, float density, float friction, b2WorldId worldId, Vec2<float> pos, Vec2<float> size)
    : m_type(type), m_pixels(pixels), m_density(density), m_friction(friction) {
    
    // Create a body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    if (type == BodyType::STATIC) {
        bodyDef.type = b2BodyType::b2_staticBody;
    } else if (type == BodyType::DYNAMIC) {
        bodyDef.type = b2BodyType::b2_dynamicBody;
    } else if (type == BodyType::KINEMATIC) {
        bodyDef.type = b2BodyType::b2_kinematicBody;
    }
    bodyDef.position = {pos.x - 1920 / 2.F, -(pos.y - 1080 / 2.F)};
    std::cout << "Position: " << bodyDef.position.x << ", " << bodyDef.position.y << std::endl;
    // Create the body
    m_bodyId = b2CreateBody(worldId, &bodyDef);

    // Create a polygon shape
    b2Polygon box = b2MakeBox(size.x / 2.F, size.y / 2.F);

    // Create a shape definition
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = m_density;
    shapeDef.friction = m_friction;
    
    // Create the shape
    b2CreatePolygonShape(m_bodyId, &shapeDef, &box);
}

b2BodyId RigidBodyImpl::getBodyId() const {
    return m_bodyId;
}
