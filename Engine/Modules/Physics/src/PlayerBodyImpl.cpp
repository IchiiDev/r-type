#include "Rte/Common.hpp"

#include "PlayerBodyImpl.hpp"
#include "Rte/Physics/Tool.hpp"
#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/math_functions.h"
#include "poly2tri/poly2tri.h"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace Rte::Physics;

//Where image is an array of pixels in rgba format
PlayerBodyImpl::PlayerBodyImpl(Rte::Vec2<u16> size, float density, float friction, b2WorldId worldId, Vec2<float> pos, Vec2<float> scale, float rotation) : m_worldId(worldId), m_size(size) {
    // Create a kinematic body using box2d
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = {(pos.x - 1920 / 2.F) / 8.F / PPM, -(pos.y - 1080 / 2.F) / 8.F / PPM};
    bodyDef.rotation = b2MakeRot(rotation * b2_pi / 180.F);
    bodyDef.fixedRotation = true; // Fix the rotation of the body
    m_bodyId = b2CreateBody(m_worldId, &bodyDef);

    // Create a shape definition
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = density;
    shapeDef.friction = friction;

    // Create the shape
    b2Capsule capsule;
    capsule.center1 = {-(size.x / 2.F) / 8.F / PPM, 0};
    capsule.center2 = {(size.x / 2.F) / 8.F / PPM, 0};
    capsule.radius = (size.y / 2.F) / 8.F / PPM;

    b2CreateCapsuleShape(m_bodyId, &shapeDef, &capsule);
    // Set the body to be affected by gravity
}

PlayerBodyImpl::~PlayerBodyImpl() {
    b2DestroyBody(m_bodyId);
}

void PlayerBodyImpl::applyForce(Vec2<float> force) {
    b2Body_ApplyForceToCenter(m_bodyId, {force.x, force.y}, true);
}

b2BodyId PlayerBodyImpl::getBodyId() const {
    return m_bodyId;
}
