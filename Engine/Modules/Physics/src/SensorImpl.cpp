#include "Rte/Common.hpp"

#include "SensorImpl.hpp"
#include "Rte/Physics/Sensor.hpp"
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
SensorImpl::SensorImpl(const Rte::Vec2<u16>& size, const b2WorldId& worldId, const Vec2<float>& pos, float rotation, SensorType shapeType) : m_worldId(worldId), m_size(size) {
    // Create a kinematic body using box2d
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_staticBody;
    
    bodyDef.position = {(pos.x - 1920 / 2.F) / 8.F / PPM, -(pos.y - 1080 / 2.F) / 8.F / PPM};
    bodyDef.rotation = b2MakeRot(rotation * std::numbers::pi_v<float> / 180.F);
    m_bodyId = b2CreateBody(m_worldId, &bodyDef);
    
    // Create a shape definition
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    // Create the shape
    if (shapeType == SensorType::CAPSULE) {
        const b2Capsule capsule {
            .center1 = {0, -(static_cast<float>(size.y) / 4.F) / 8.F / PPM},
            .center2 = {0, (static_cast<float>(size.y) / 4.F) / 8.F / PPM},
            .radius = (static_cast<float>(size.x) / 2.F) / 8.F / PPM
        };
        b2CreateCapsuleShape(m_bodyId, &shapeDef, &capsule);
    } else if (shapeType == SensorType::RECTANGLE) {
        const b2Polygon rectangle = b2MakeBox(static_cast<float>(size.x) / 8.F / PPM, static_cast<float>(size.y) / 8.F / PPM);
        b2CreatePolygonShape(m_bodyId, &shapeDef, &rectangle);
    } else if (shapeType == SensorType::CIRCLE) {
        const b2Circle circle = {
            .center = {0, 0},
            .radius = (static_cast<float>(size.x) / 2.F) / 8.F / PPM
        };
        b2CreateCircleShape(m_bodyId, &shapeDef, &circle);
    }
    b2ShapeId shapeArray;
    b2Body_GetShapes(m_bodyId, &shapeArray, 1);
    m_shapeId = shapeArray;

    b2Shape_EnableSensorEvents(m_shapeId, true);
}

SensorImpl::~SensorImpl() {
    b2DestroyShape(m_shapeId);
    b2DestroyBody(m_bodyId);
}

b2BodyId SensorImpl::getBodyId() const {
    return m_bodyId;
}

b2ShapeId SensorImpl::getShapeId() const {
    return m_shapeId;
}
