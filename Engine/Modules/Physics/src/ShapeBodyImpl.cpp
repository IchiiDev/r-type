#include "Rte/Common.hpp"

#include "ShapeBodyImpl.hpp"
#include "Rte/Physics/ShapeBody.hpp"
#include "Rte/Physics/Tool.hpp"
#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/id.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"

#include <cstdlib>
#include <iostream>
#include <numbers>

using namespace Rte::Physics;

//Where image is an array of pixels in rgba format
ShapeBodyImpl::ShapeBodyImpl(const Rte::Vec2<u16>& size, float density, float friction, const b2WorldId& worldId, const Vec2<float>& pos, float rotation, bool fixedRotation, bool isStatic, ShapeType shapeType) : m_worldId(worldId), m_size(size) {
    // Create a kinematic body using box2d
    b2BodyDef bodyDef = b2DefaultBodyDef();
    if (isStatic) {
        bodyDef.type = b2BodyType::b2_staticBody;
    } else {
        bodyDef.type = b2BodyType::b2_dynamicBody;
    }
    bodyDef.position = {(pos.x - 1920 / 2.F) / 8.F / PPM, -(pos.y - 1080 / 2.F) / 8.F / PPM};
    bodyDef.rotation = b2MakeRot(rotation * std::numbers::pi_v<float> / 180.F);
    bodyDef.fixedRotation = fixedRotation; // Fix the rotation of the body
    m_bodyId = b2CreateBody(m_worldId, &bodyDef);

    // Create a shape definition
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = density;
    shapeDef.friction = friction;
    // Create the shape
    if (shapeType == ShapeType::CAPSULE) {
        const b2Capsule capsule {
            .center1 = {0, -(static_cast<float>(size.y) / 4.F) / 8.F / PPM},
            .center2 = {0, (static_cast<float>(size.y) / 4.F) / 8.F / PPM},
            .radius = (static_cast<float>(size.x) / 2.F) / 8.F / PPM
        };
        b2CreateCapsuleShape(m_bodyId, &shapeDef, &capsule);
    } else if (shapeType == ShapeType::RECTANGLE) {
        const b2Polygon rectangle = b2MakeBox(static_cast<float>(size.x) / 8.F / PPM, static_cast<float>(size.y) / 8.F / PPM);
        b2CreatePolygonShape(m_bodyId, &shapeDef, &rectangle);
    } else if (shapeType == ShapeType::CIRCLE) {
        const b2Circle circle = {
            .center = {0, 0},
            .radius = (static_cast<float>(size.x) / 2.F) / 8.F / PPM
        };
        b2CreateCircleShape(m_bodyId, &shapeDef, &circle);
    }
    b2ShapeId shapeArray;
    b2Body_GetShapes(m_bodyId, &shapeArray, 1);
    m_shapeId = shapeArray;
    // Set the body to be affected by gravity
}

ShapeBodyImpl::~ShapeBodyImpl() {
    b2DestroyShape(m_shapeId);
    b2DestroyBody(m_bodyId);
}

void ShapeBodyImpl::applyForce(const Vec2<float>& force) {
    b2Body_ApplyLinearImpulseToCenter(m_bodyId, {force.x, force.y}, true);
}

void ShapeBodyImpl::move(const Vec2<float>& direction) {
    b2Vec2 currentVelocity = b2Body_GetLinearVelocity(m_bodyId);

    if (direction.x < 0) {
        if (currentVelocity.x > direction.x) {
            currentVelocity.x = direction.x;
        }
    } else if (direction.x > 0) {
        if (currentVelocity.x < direction.x) {
            currentVelocity.x = direction.x;
        }
    }

    if (direction.y < 0) {
        if (currentVelocity.y > direction.y) {
            currentVelocity.y = direction.y;
        }
    } else if (direction.y > 0) {
        if (currentVelocity.y < direction.y) {
            currentVelocity.y = direction.y;
        }
    }

    if (direction.x == 0) {
        currentVelocity.x = 0;
    }

    b2Body_SetLinearVelocity(m_bodyId, currentVelocity);
}

Rte::Vec2<float> ShapeBodyImpl::getVelocity() const {
    b2Vec2 velocity = b2Body_GetLinearVelocity(m_bodyId);
    return {velocity.x, velocity.y};
}

void ShapeBodyImpl::setFriction(float friction) {
    b2Shape_SetFriction(m_shapeId, friction);
}
b2BodyId ShapeBodyImpl::getBodyId() const {
    return m_bodyId;
}

b2ShapeId ShapeBodyImpl::getShapeId() const {
    return m_shapeId;
}
