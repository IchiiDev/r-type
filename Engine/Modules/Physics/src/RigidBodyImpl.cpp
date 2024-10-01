#include "Rte/Common.hpp"

#include "RigidBodyImpl.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/Tool.hpp"
#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/math_functions.h"

#include "box2d/id.h"
#include "box2d/types.h"
#include "poly2tri/common/shapes.h"
#include "poly2tri/sweep/cdt.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <numbers>
#include <utility>
#include <vector>

using namespace Rte::Physics;

struct Triangle {
    Rte::Vec2<float> a;
    Rte::Vec2<float> b;
    Rte::Vec2<float> c;
};

std::vector<int> convertToBinary(const std::vector<Rte::u8>& image, const Rte::Vec2<Rte::u16>& size) {
    std::vector<int> binaryImage(static_cast<size_t>(size.x * size.y));
    for (int i = 1; i < size.x * size.y + 1; i++) {
        if (image.at(i * 4 - 1) == 0)
            binaryImage.at(i - 1) = 0;
        else
            binaryImage.at(i - 1) = 1;
    }

    return binaryImage;
}

// Using the marching squares algorithm to generate outline of the binary image
std::vector<std::vector<Rte::Vec2<float>>> marchingSquares(const std::vector<int>& binaryImage, const Rte::Vec2<Rte::u16>& size) {    // NOLINT(readability-function-cognitive-complexity)
    std::vector<std::vector<Rte::Vec2<float>>> vertices;

    const Rte::Vec2<float> fSize = {static_cast<float>(size.x), static_cast<float>(size.y)};

    for (float y = -1; y < fSize.y + 1; y++) {      // NOLINT(clang-analyzer-security.FloatLoopCounter)
        for (float x = -1; x < fSize.x + 1; x++) {  // NOLINT(clang-analyzer-security.FloatLoopCounter)
            int topLeft = 0;
            int topRight = 0;
            int bottomLeft = 0;
            int bottomRight = 0;

            if (x >= 0 && x < fSize.x && y >= 0 && y < fSize.y) {
                topLeft = binaryImage.at(static_cast<size_t>(y * fSize.x + x));
                if (x + 1 < fSize.x) topRight = binaryImage.at(static_cast<size_t>(y * fSize.x + x + 1));
                if (y + 1 < fSize.y) bottomLeft = binaryImage.at(static_cast<size_t>((y + 1) * fSize.x + x));
                if (x + 1 < fSize.x && y + 1 < fSize.y) bottomRight = binaryImage.at(static_cast<size_t>((y + 1) * fSize.x + x + 1));
            } else {
                if (x < 0) {
                    topLeft = 0;
                    bottomLeft = 0;
                    if (x + 1 < fSize.x && y >= 0 && y < fSize.y) topRight = binaryImage.at(static_cast<size_t>(y * fSize.x + x + 1));
                    if (x + 1 < fSize.x && y + 1 < fSize.y) bottomRight = binaryImage.at(static_cast<size_t>((y + 1) * fSize.x + x + 1));
                }

                if (x >= fSize.x) {
                    topRight = 0;
                    bottomRight = 0;
                    if (x - 1 >= 0 && y >= 0 && y < fSize.y) topLeft = binaryImage.at(static_cast<size_t>(y * fSize.x + x - 1));
                    if (x - 1 >= 0 && y + 1 < fSize.y) bottomLeft = binaryImage.at(static_cast<size_t>((y + 1) * fSize.x + x - 1));
                }

                if (y < 0) {
                    topLeft = 0;
                    topRight = 0;
                    if (y + 1 < fSize.y && x >= 0 && x < fSize.x) bottomLeft = binaryImage.at(static_cast<size_t>((y + 1) * fSize.x + x));
                    if (y + 1 < fSize.y && x + 1 < fSize.x) bottomRight = binaryImage.at(static_cast<size_t>((y + 1) * fSize.x + x + 1));
                }

                if (y >= fSize.y) {
                    bottomLeft = 0;
                    bottomRight = 0;
                    if (y - 1 >= 0 && x >= 0 && x < fSize.x) topLeft = binaryImage.at(static_cast<size_t>((y - 1) * fSize.x + x));
                    if (y - 1 >= 0 && x + 1 < fSize.x) topRight = binaryImage.at(static_cast<size_t>((y - 1) * fSize.x + x + 1));
                }
            }

            const int configuration = topLeft * 8 + topRight * 4 + bottomRight * 2 + bottomLeft;
            switch (configuration) {
                case 1:
                case 14:
                    vertices.push_back({
                        {x + .5F, y + 1},
                        {x + 1, y + 1},
                        {x + 1, y + 1.5F}
                    });
                    break;

                case 2:
                case 13:
                    vertices.push_back({
                        {x + 1, y + 1.5F},
                        {x + 1, y + 1},
                        {x + 1.5F, y + 1}
                    });
                    break;

                case 3:
                case 12:
                    vertices.push_back({
                        {x + .5F, y + 1},
                        {x + 1.5F, y + 1}
                    });
                    break;

                case 4:
                case 11:
                    vertices.push_back({
                        {x + 1, y + .5F},
                        {x + 1, y + 1},
                        {x + 1.5F, y + 1}
                    });
                    break;

                case 5:
                    vertices.push_back({
                        {x + .5F, y + 1},
                        {x + 1, y + 1},
                        {x + 1, y + 1.5F}
                    });
                    vertices.push_back({
                        {x + 1, y + .5F},
                        {x + 1, y + 1},
                        {x + 1.5F, y + 1}
                    });
                    break;

                case 10:
                    vertices.push_back({
                        {x + 1, y + 1.5F},
                        {x + 1, y + 1},
                        {x + 1.5F, y + 1}
                    });
                    vertices.push_back({
                        {x + 1, y + .5F},
                        {x + 1, y + 1},
                        {x + .5F, y + 1}
                    });
                    break;

                case 6:
                case 9:
                    vertices.push_back({
                        {x + 1, y + .5F},
                        {x + 1, y + 1.5F}
                    });
                    break;

                case 7:
                case 8:
                    vertices.push_back({
                        {x + 1, y + .5F},
                        {x + 1, y + 1},
                        {x + .5F, y + 1}
                    });
                    break;

                default:
                    break;
            }
        }
    }

    return vertices;
}

bool isLastVertexInList(const Rte::Vec2<float>& vertex, const std::vector<Rte::Vec2<float>>& vertices) {
    if (vertices.empty())
        return false;

    return vertex.x == vertices.at(vertices.size() - 1).x && vertex.y == vertices.at(vertices.size() - 1).y;
}

bool isVertexFirstInList(const Rte::Vec2<float>& vertex, const std::vector<Rte::Vec2<float>>& vertices) {
    if (vertices.empty())
        return false;

    return vertex.x == vertices.at(0).x && vertex.y == vertices.at(0).y;
}

std::vector<std::vector<Rte::Vec2<float>>> createContinuousLines(std::vector<std::vector<Rte::Vec2<float>>>& vertices) { // NOLINT(readability-function-cognitive-complexity)
    std::vector<Rte::Vec2<float>> continuousLine;
    for (const Rte::Vec2<float> i : vertices.at(0))
        continuousLine.push_back(i);

    vertices.erase(vertices.begin());
    for (size_t i = 0; i < vertices.size(); i++) {
        if (isLastVertexInList(vertices.at(i).at(0), continuousLine)) {
            vertices.at(i).erase(vertices.at(i).begin());
            for (const Rte::Vec2<float> j : vertices.at(i))
                continuousLine.push_back(j);

            vertices.erase(std::next(vertices.begin(), static_cast<std::vector<Rte::Vec2<float>>::difference_type>(i)));
            i = 0;
        } else if (isLastVertexInList(vertices.at(i).at(vertices.at(i).size() - 1), continuousLine)) {
            vertices.at(i).pop_back();
            for (int j = static_cast<int>(vertices.at(i).size() - 1); j >= 0; j--)
                continuousLine.push_back(vertices.at(i).at(j));

            vertices.erase(std::next(vertices.begin(), static_cast<std::vector<Rte::Vec2<float>>::difference_type>(i)));
            i = 0;
        } else if (isVertexFirstInList(vertices.at(i).at(0), continuousLine)) {
            vertices.at(i).erase(vertices.at(i).begin());
            for (const Rte::Vec2<float> j : vertices.at(i))
                continuousLine.insert(continuousLine.begin(), j);

            vertices.erase(std::next(vertices.begin(), static_cast<std::vector<Rte::Vec2<float>>::difference_type>(i)));
            i = 0;
        } else if (isVertexFirstInList(vertices.at(i).at(vertices.at(i).size() - 1), continuousLine)) {
            vertices.at(i).pop_back();
            for (int j = static_cast<int>(vertices.at(i).size() - 1); j >= 0; j--)
                continuousLine.insert(continuousLine.begin(), vertices.at(i).at(j));

            vertices.erase(std::next(vertices.begin(), static_cast<std::vector<Rte::Vec2<float>>::difference_type>(i)));
            i = 0;
        }
    }

    if (!vertices.empty()) {
        std::vector<std::vector<Rte::Vec2<float>>> result = createContinuousLines(vertices);
        if (continuousLine.size() > 4)
            result.push_back(continuousLine);

        return result;
    }

    if (continuousLine.size() > 4)
        return {continuousLine};

    return {};
}

float perpendicularDistance(const Rte::Vec2<float>& point, const Rte::Vec2<float>& lineStart, const Rte::Vec2<float>& lineEnd) {
    float dx = lineEnd.x - lineStart.x;
    float dy = lineEnd.y - lineStart.y;
    const float mag = std::sqrt(dx * dx + dy * dy);
    if (mag > 0.0) {
        dx /= mag;
        dy /= mag;
    }

    const float pvx = point.x - lineStart.x;
    const float pvy = point.y - lineStart.y;
    const float pvdot = dx * pvx + dy * pvy;
    const float ax = pvx - pvdot * dx;
    const float ay = pvy - pvdot * dy;

    return std::sqrt(ax * ax + ay * ay);
}

void douglasPeuckerRecursive(const std::vector<Rte::Vec2<float>>& line, float epsilon, std::vector<Rte::Vec2<float>>& result) {
    if (line.size() < 2)
        return;

    float maxDistance = 0.0;
    size_t index = 0;
    for (size_t i = 1; i < line.size() - 1; ++i) {
        const float distance = perpendicularDistance(line.at(i), line.at(0), line.at(line.size() - 1));
        if (distance > maxDistance) {
            index = i;
            maxDistance = distance;
        }
    }

    if (maxDistance > epsilon) {
        std::vector<Rte::Vec2<float>> recResults1;
        std::vector<Rte::Vec2<float>> recResults2;
        const std::vector<Rte::Vec2<float>> firstLine(line.begin(), std::next(line.begin(), static_cast<std::vector<Rte::Vec2<float>>::difference_type>(index + 1)));
        const std::vector<Rte::Vec2<float>> lastLine(std::next(line.begin(), static_cast<std::vector<Rte::Vec2<float>>::difference_type>(index)), line.end());

        douglasPeuckerRecursive(firstLine, epsilon, recResults1);
        douglasPeuckerRecursive(lastLine, epsilon, recResults2);

        result.assign(recResults1.begin(), recResults1.end() - 1);
        result.insert(result.end(), recResults2.begin(), recResults2.end());

        if (result.size() < 2) {
            result.push_back(line.at(0));
            result.push_back(line.at(line.size() - 1));
        }
    } else {
        result.push_back(line.at(0));
        result.push_back(line.at(line.size() - 1));
    }
}

std::vector<Rte::Vec2<float>> douglasPeucker(const std::vector<Rte::Vec2<float>>& line, float epsilon) {
    std::vector<Rte::Vec2<float>> result;
    douglasPeuckerRecursive(line, epsilon, result);
    return result;
}

std::vector<Triangle> polygoneToTriangles(std::vector<Rte::Vec2<float>> polygone) {
    std::vector<p2t::Point *> points(polygone.size());
    for (int i = 0; i < polygone.size(); i++)
        points.at(i) = new p2t::Point(polygone.at(i).x, polygone.at(i).y);  // NOLINT (cppcoreguidelines-owning-memory)

    p2t::CDT cdt(points);
    cdt.Triangulate();
    std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();
    std::vector<Triangle> convertedTriangles;
    for (auto & triangle : triangles) {
        const Triangle tri{
            .a = {
                .x = static_cast<float>(triangle->GetPoint(0)->x),
                .y = static_cast<float>(triangle->GetPoint(0)->y)
            },
            .b = {
                .x = static_cast<float>(triangle->GetPoint(1)->x),
                .y = static_cast<float>(triangle->GetPoint(1)->y)
            },
            .c = {
                .x = static_cast<float>(triangle->GetPoint(2)->x),
                .y = static_cast<float>(triangle->GetPoint(2)->y)
            }
        };

        convertedTriangles.push_back(tri);
    }

    return convertedTriangles;
}

bool isPointInPolygon(const Rte::Vec2<float>& point, const std::vector<Rte::Vec2<float>>& polygon) {
    bool inside = false;
    const size_t polygonCount = polygon.size();
    for (size_t i = 0, j = polygonCount - 1; i < polygonCount; j = i++) {
        if (((polygon.at(i).y > point.y) != (polygon.at(j).y > point.y)) &&
            (point.x < (polygon.at(j).x - polygon.at(i).x) * (point.y - polygon.at(i).y) / (polygon.at(j).y - polygon.at(i).y) + polygon.at(i).x)) {
            inside = !inside;
        }
    }

    return inside;
}

bool isPolygonInsidePolygon(const std::vector<Rte::Vec2<float>>& polygon1, const std::vector<Rte::Vec2<float>>& polygon2) {
    return std::ranges::all_of(polygon2, [&polygon1](const auto& point) {
        return isPointInPolygon(point, polygon1);
    });
}

void findHolesInPolygons(std::vector<std::vector<Rte::Vec2<float>>>& polygons) {
    for (size_t i = 0; i < polygons.size(); i++) {
        for (size_t j = 0; j < polygons.size(); j++) {
            if (i != j && isPolygonInsidePolygon(polygons.at(i), polygons.at(j))) {
                polygons.erase(std::next(polygons.begin(), static_cast<std::vector<Rte::Vec2<float>>::difference_type>(j)));
                i = 0;
            }
        }
    }
}

Rte::Vec2<float> rotate(Rte::Vec2<float> point, const Rte::Vec2<float>& center, float angle) {
    const float s = std::sin(angle);
    const float c = std::cos(angle);
    point.x -= center.x;
    point.y -= center.y;

    const float xnew = point.x * c - point.y * s;
    const float ynew = point.x * s + point.y * c;
    point.x = xnew + center.x;
    point.y = ynew + center.y;

    return point;
}

std::vector<std::vector<PixelCringe>> RigidBodyImpl::getRotatedPixels() const {
    const Rte::Vec2<float> center = {static_cast<float>(m_size.x) / 2.0F, static_cast<float>(m_size.y) / 2.0F};
    const float angle = -getRotation();
    std::vector<std::vector<PixelCringe>> rotatedPixels(m_size.y, std::vector<PixelCringe>(m_size.x));

    for (int y = 0; y < m_size.y; y++) {
        for (int x = 0; x < m_size.x; x++) {
            const Rte::Vec2<float> rotatedPos = rotate({static_cast<float>(x), static_cast<float>(y)}, center, angle);
            rotatedPixels.at(y).at(x) = {m_pixels.at((static_cast<long long>(y * m_size.x + x)) * 4), m_pixels.at((y * m_size.x + x) * 4 + 1), m_pixels.at((y * m_size.x + x) * 4 + 2), m_pixels.at((y * m_size.x + x) * 4 + 3), (rotatedPos.x - center.x), (rotatedPos.y - center.y)};
        }
    }

    return rotatedPixels;
}

float RigidBodyImpl::getRotation() const {
    return b2Rot_GetAngle(b2Body_GetRotation(m_bodyId));
}

Rte::Vec2<float> RigidBodyImpl::getPosition() const {
    const b2Vec2 position = b2Body_GetPosition(m_bodyId);
    return {position.x, position.y};
}

bool RigidBodyImpl::isDynamic() const {
    return m_isDynamic;
}

std::pair<std::vector<MaterialType>, MaterialDef> createMaterialMap(Rte::Vec2<Rte::u16> size , const Rte::u8* pixels) {
    std::vector<MaterialType> materialMap(static_cast<size_t>(size.x * size.y), MaterialType::AIR);

    MaterialDef averageProperties{};
    averageProperties.isDynamic = true;
    for (int i = 0; i < size.x * size.y; i++) {
        const int pixelColor = pixels[static_cast<ptrdiff_t>(i * 4)] * 1 + pixels[i * 4 + 1] * 2 + pixels[i * 4 + 2] * 4 + pixels[i * 4 + 3] * 8;

        switch (matColors.at(pixelColor)) {
            case MaterialType::AIR:
                materialMap.at(i) = MaterialType::AIR;
                break;
            case MaterialType::STATIC_WOOD:
                materialMap.at(i) = MaterialType::STATIC_WOOD;
                break;
            case MaterialType::DYNAMIC_WOOD:
                materialMap.at(i) = MaterialType::DYNAMIC_WOOD;
                break;
            default:
                break;
        }

        if (materialMap.at(i) != MaterialType::AIR) {
            averageProperties.density += mats.at(materialMap.at(i)).density;
            averageProperties.friction += mats.at(materialMap.at(i)).friction;
            if (!mats.at(materialMap.at(i)).isDynamic)
                averageProperties.isDynamic = false;
        }
    }

    averageProperties.density /= static_cast<float>(size.x * size.y);
    averageProperties.friction /= static_cast<float>(size.x * size.y);

    return {materialMap, averageProperties};
}

RigidBodyImpl::RigidBodyImpl(const u8* pixels, const Vec2<u16>& size, const b2WorldId& worldId, const Vec2<float>& pos, float rotation) : m_size(size), m_worldId(worldId) {
    m_pixels.resize((static_cast<size_t>(size.x * size.y) * 4));
    std::copy(pixels, pixels + static_cast<ptrdiff_t>(size.x * size.y * 4), m_pixels.begin());

    const std::pair<std::vector<MaterialType>, MaterialDef> result = createMaterialMap(size, pixels);
    m_materials = result.first;
    const MaterialDef properties = result.second;
    // Create a body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();

    if (properties.isDynamic) {
        m_isDynamic = true;
        bodyDef.type = b2BodyType::b2_dynamicBody;
    } else {
        m_isDynamic = false;
        bodyDef.type = b2BodyType::b2_staticBody;
    }

    bodyDef.position = {(pos.x - 1920 / 2.F) / 8.F / PPM, -(pos.y - 1080 / 2.F) / 8.F / PPM};
    bodyDef.rotation = b2MakeRot(rotation * std::numbers::pi_v<float> / 180.F);

    // Create the polygons from the image
    const std::vector<int> binaryImage = convertToBinary(m_pixels, size);
    std::vector<std::vector<Rte::Vec2<float>>> vertices = marchingSquares(binaryImage, size);
    std::vector<std::vector<Rte::Vec2<float>>> continuousLines = createContinuousLines(vertices);
    findHolesInPolygons(continuousLines);
    for (std::vector<Rte::Vec2<float>>& continuousLine : continuousLines)
        continuousLine = douglasPeucker(continuousLine, 0.75);

    const std::vector<Triangle> triangles = polygoneToTriangles(continuousLines.at(0));

    m_bodyId = b2CreateBody(worldId, &bodyDef);

    // Create attach the triangles to the body
    for (const auto& tri : triangles) {
        const std::array<b2Vec2, 3> vertices{
            b2Vec2{(tri.a.x - static_cast<float>(size.x) / 2) / PPM, -(tri.a.y - static_cast<float>(size.y) / 2) / PPM},
            b2Vec2{(tri.b.x - static_cast<float>(size.x) / 2) / PPM, -(tri.b.y - static_cast<float>(size.y) / 2) / PPM},
            b2Vec2{(tri.c.x - static_cast<float>(size.x) / 2) / PPM, -(tri.c.y - static_cast<float>(size.y) / 2) / PPM}
        };

        const b2Hull hull = b2ComputeHull(vertices.data(), 3);
        const b2Polygon triangle = b2MakePolygon(&hull, 0);

        // Create a shape definition
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = properties.density;
        shapeDef.friction = properties.friction;

        // Create the shape
        b2CreatePolygonShape(m_bodyId, &shapeDef, &triangle);
    }
}

RigidBodyImpl::RigidBodyImpl(const std::shared_ptr<RigidBodyImpl>& rigidBody, const u8* pixels, const Rte::Vec2<u16>& size, const b2WorldId& worldId) : m_size(size), m_worldId(worldId) {
    m_pixels.resize((static_cast<size_t>(size.x * size.y) * 4));
    std::copy(pixels, pixels + static_cast<ptrdiff_t>(size.x * size.y * 4), m_pixels.begin());

    // Copy the body definition from the existing rigid body
    const std::pair<std::vector<MaterialType>, MaterialDef> result = createMaterialMap(size, pixels);
    m_materials = result.first;
    const MaterialDef properties = result.second;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    if (properties.isDynamic) {
        m_isDynamic = true;
        bodyDef.type = b2BodyType::b2_dynamicBody;
    } else {
        m_isDynamic = false;
        bodyDef.type = b2BodyType::b2_staticBody;
    }

    bodyDef.position = b2Body_GetPosition(rigidBody->getBodyId());
    bodyDef.rotation = b2Body_GetRotation(rigidBody->getBodyId());
    bodyDef.linearVelocity = b2Body_GetLinearVelocity(rigidBody->getBodyId());
    bodyDef.angularVelocity = b2Body_GetAngularVelocity(rigidBody->getBodyId());
    bodyDef.linearDamping = b2Body_GetLinearDamping(rigidBody->getBodyId());
    bodyDef.angularDamping = b2Body_GetAngularDamping(rigidBody->getBodyId());
    bodyDef.fixedRotation = b2Body_IsFixedRotation(rigidBody->getBodyId());
    bodyDef.gravityScale = b2Body_GetGravityScale(rigidBody->getBodyId());

    // Create the polygons from the image
    const std::vector<int> binaryImage = convertToBinary(m_pixels, size);
    std::vector<std::vector<Rte::Vec2<float>>> vertices = marchingSquares(binaryImage, size);
    std::vector<std::vector<Rte::Vec2<float>>> continuousLines = createContinuousLines(vertices);
    findHolesInPolygons(continuousLines);
    for (std::vector<Rte::Vec2<float>>& continuousLine : continuousLines)
        continuousLine = douglasPeucker(continuousLine, 0.75);

    const std::vector<Triangle> triangles = polygoneToTriangles(continuousLines.at(0));

    m_bodyId = b2CreateBody(m_worldId, &bodyDef);

    // Create attach the triangles to the body
    for (const auto& tri : triangles) {
        const std::array<b2Vec2, 3> vertices = {
            b2Vec2{(tri.a.x - static_cast<float>(size.x) / 2) / PPM, -(tri.a.y - static_cast<float>(size.y) / 2) / PPM},
            b2Vec2{(tri.b.x - static_cast<float>(size.x) / 2) / PPM, -(tri.b.y - static_cast<float>(size.y) / 2) / PPM},
            b2Vec2{(tri.c.x - static_cast<float>(size.x) / 2) / PPM, -(tri.c.y - static_cast<float>(size.y) / 2) / PPM}
        };

        const b2Hull hull = b2ComputeHull(vertices.data(), 3);
        const b2Polygon triangle = b2MakePolygon(&hull, 0);

        // Create a shape definition
        b2ShapeDef shapeDef = b2DefaultShapeDef();

        shapeDef.density = properties.density;
        shapeDef.friction = properties.friction;

        // Create the shape
        b2CreatePolygonShape(m_bodyId, &shapeDef, &triangle);
    }
}

RigidBodyImpl::~RigidBodyImpl() {
    b2DestroyBody(m_bodyId);
}

b2BodyId RigidBodyImpl::getBodyId() const {
    return m_bodyId;
}
