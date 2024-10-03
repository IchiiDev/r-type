#include "Rte/Common.hpp"

#include "ShapeBodyImpl.hpp"
#include "Rte/Physics/Tool.hpp"
#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/id.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"

#include <cstdlib>
#include <numbers>

using namespace Rte::Physics;

//Where image is an array of pixels in rgba format
ShapeBodyImpl::ShapeBodyImpl(const Rte::Vec2<u16>& size, float density, float friction, const b2WorldId& worldId, const Vec2<float>& pos, float rotation, bool fixedRotation) : m_worldId(worldId), m_size(size) {
    // Create a kinematic body using box2d
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = {(pos.x - 1920 / 2.F) / 8.F / PPM, -(pos.y - 1080 / 2.F) / 8.F / PPM};
    bodyDef.rotation = b2MakeRot(rotation * std::numbers::pi_v<float> / 180.F);
    bodyDef.fixedRotation = fixedRotation; // Fix the rotation of the body
    m_bodyId = b2CreateBody(m_worldId, &bodyDef);

    // Create a shape definition
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = density;
    shapeDef.friction = friction;

    // Create the shape
    const b2Capsule capsule {
        .center1 = {0, -(static_cast<float>(size.y) / 4.F) / 8.F / PPM},
        .center2 = {0, (static_cast<float>(size.y) / 4.F) / 8.F / PPM},
        .radius = (static_cast<float>(size.x) / 2.F) / 8.F / PPM
    };

    b2CreateCapsuleShape(m_bodyId, &shapeDef, &capsule);
    // Set the body to be affected by gravity
}

ShapeBodyImpl::~ShapeBodyImpl() {
    b2DestroyBody(m_bodyId);
}

void ShapeBodyImpl::applyForce(const Vec2<float>& force) {
    b2Body_ApplyLinearImpulseToCenter(m_bodyId, {force.x, force.y}, true);
}

void ShapeBodyImpl::move(const Vec2<float>& direction) {
    b2Vec2 currentVelocity = b2Body_GetLinearVelocity(m_bodyId);

    if (direction.x < 0) {
        if (currentVelocity.x > direction.x) {
            currentVelocity.x -= 1;
        }
    } else if (direction.x > 0) {
        if (currentVelocity.x < direction.x) {
            currentVelocity.x += 1;
        }
    }

    if (direction.y < 0) {
        if (currentVelocity.y > direction.y) {
            currentVelocity.y += 1;
        }
    } else if (direction.y > 0) {
        if (currentVelocity.y < direction.y) {
            currentVelocity.y += 1;
        }
    }

    b2Body_SetLinearVelocity(m_bodyId, currentVelocity);
}

b2BodyId ShapeBodyImpl::getBodyId() const {
    return m_bodyId;
}
