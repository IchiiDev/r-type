#include "Rte/Common.hpp"

#include "RigidBodyImpl.hpp"
#include "Rte/Physics/Materials.hpp"
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


using Tri = struct {
    Rte::Vec2<float> a;
    Rte::Vec2<float> b;
    Rte::Vec2<float> c;
};

//Where image is an array of pixels in rgba format
std::vector<int> convertToBinary(const Rte::u8* image, Rte::Vec2<Rte::u16> size) {
    std::vector<int> binaryImage;
    for (int i = 1; i < size.x * size.y + 1; i++)
    {
        if (image[i * 4 - 1] == 0)
        {
            binaryImage.push_back(0);
        }
        else
        {
            binaryImage.push_back(1);
        }
    }
    return binaryImage;
}

// Using the marching squares algorithm to generate outline of the binary image
std::vector<std::vector<Rte::Vec2<float>>> marchingSquares (std::vector<int> binaryImage, Rte::Vec2<Rte::u16> size) {
    std::vector<std::vector<Rte::Vec2<float>>> vertices;
    for (float y = -1; y < size.y + 1; y++)
    {
        for (float x = -1; x < size.x + 1; x++)
        {
            int topLeft = 0;
            int topRight = 0;
            int bottomLeft = 0;
            int bottomRight = 0;
            if (x >= 0 && x < size.x && y >= 0 && y < size.y)
            {
                topLeft = binaryImage[y * size.x + x];
                if (x + 1 < size.x) topRight = binaryImage[y * size.x + x + 1];
                if (y + 1 < size.y) bottomLeft = binaryImage[(y + 1) * size.x + x];
                if (x + 1 < size.x && y + 1 < size.y) bottomRight = binaryImage[(y + 1) * size.x + x + 1];
            }
            else
            {
                if (x < 0)
                {
                    topLeft = 0;
                    bottomLeft = 0;
                    if (x + 1 < size.x && y >= 0 && y < size.y) topRight = binaryImage[y * size.x + x + 1];
                    if (x + 1 < size.x && y + 1 < size.y) bottomRight = binaryImage[(y + 1) * size.x + x + 1];
                }
                if (x >= size.x)
                {
                    topRight = 0;
                    bottomRight = 0;
                    if (x - 1 >= 0 && y >= 0 && y < size.y) topLeft = binaryImage[y * size.x + x - 1];
                    if (x - 1 >= 0 && y + 1 < size.y) bottomLeft = binaryImage[(y + 1) * size.x + x - 1];
                }
                if (y < 0)
                {
                    topLeft = 0;
                    topRight = 0;
                    if (y + 1 < size.y && x >= 0 && x < size.x) bottomLeft = binaryImage[(y + 1) * size.x + x];
                    if (y + 1 < size.y && x + 1 < size.x) bottomRight = binaryImage[(y + 1) * size.x + x + 1];
                }
                if (y >= size.y)
                {
                    bottomLeft = 0;
                    bottomRight = 0;
                    if (y - 1 >= 0 && x >= 0 && x < size.x) topLeft = binaryImage[(y - 1) * size.x + x];
                    if (y - 1 >= 0 && x + 1 < size.x) topRight = binaryImage[(y - 1) * size.x + x + 1];
                }
            }
            int configuration = topLeft * 8 + topRight * 4 + bottomRight * 2 + bottomLeft;
            switch (configuration)
            {
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
            }
        }
    }
    return vertices;
}

bool isVertexInList(Rte::Vec2<float> vertex, std::vector<Rte::Vec2<float>> vertices) {
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (vertex.x == vertices[i].x && vertex.y == vertices[i].y)
        {
            return true;
        }
    }
    return false;
}

bool isVertexLastInList(Rte::Vec2<float> vertex, std::vector<Rte::Vec2<float>> vertices) {
    if (vertices.size() == 0)
    {
        return false;
    }
    return vertex.x == vertices[vertices.size() - 1].x && vertex.y == vertices[vertices.size() - 1].y;
}

bool isVertexFirstInList(Rte::Vec2<float> vertex, std::vector<Rte::Vec2<float>> vertices) {
    if (vertices.size() == 0)
    {
        return false;
    }
    return vertex.x == vertices[0].x && vertex.y == vertices[0].y;
}

std::vector<Rte::Vec2<float>> pushIfNotInList(Rte::Vec2<float> vertex, std::vector<Rte::Vec2<float>> vertices) {
    if (!isVertexInList(vertex, vertices))
    {
        vertices.push_back(vertex);
    }
    return vertices;
}

std::vector<std::vector<Rte::Vec2<float>>> createContinuousLines(std::vector<std::vector<Rte::Vec2<float>>> vertices) {
    std::vector<Rte::Vec2<float>> continuousLine;
    for (size_t i = 0; i < vertices[0].size(); i++)
    {
        continuousLine.push_back(vertices[0][i]);
    }
    vertices.erase(vertices.begin());
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (isVertexLastInList(vertices[i][0], continuousLine))
        {
            vertices[i].erase(vertices[i].begin());
            for (size_t j = 0; j < vertices[i].size(); j++)
            {
                continuousLine.push_back(vertices[i][j]);
            }
            vertices.erase(vertices.begin() + i);
            i = 0;
        }
        else if (isVertexLastInList(vertices[i][vertices[i].size() - 1], continuousLine))
        {
            vertices[i].pop_back();
            for (int j = vertices[i].size() - 1; j >= 0; j--)
            {
                continuousLine.push_back(vertices[i][j]);
            }
            vertices.erase(vertices.begin() + i);
            i = 0;
        }
        else if (isVertexFirstInList(vertices[i][0], continuousLine))
        {
            vertices[i].erase(vertices[i].begin());
            for (size_t j = 0; j < vertices[i].size(); j++)
            {
                continuousLine.insert(continuousLine.begin(), vertices[i][j]);
            }
            vertices.erase(vertices.begin() + i);
            i = 0;
        }
        else if (isVertexFirstInList(vertices[i][vertices[i].size() - 1], continuousLine))
        {
            vertices[i].pop_back();
            for (int j = vertices[i].size() - 1; j >= 0; j--)
            {
                continuousLine.insert(continuousLine.begin(), vertices[i][j]);
            }
            vertices.erase(vertices.begin() + i);
            i = 0;
        }
    }
    //continuousLine.push_back(continuousLine[0]);
    if (vertices.size() > 0) {
        std::vector<std::vector<Rte::Vec2<float>>> result = createContinuousLines(vertices);
        if (continuousLine.size() > 4) {
            result.push_back(continuousLine);
        }
        return result;
    } else {
        if (continuousLine.size() > 4) {
            return {continuousLine};
        } else {
            return {};
        }
    }
}

float perpendicularDistance(Rte::Vec2<float> point, Rte::Vec2<float> lineStart, Rte::Vec2<float> lineEnd) {
    float dx = lineEnd.x - lineStart.x;
    float dy = lineEnd.y - lineStart.y;
    float mag = std::sqrt(dx * dx + dy * dy);
    if (mag > 0.0) {
        dx /= mag;
        dy /= mag;
    }
    float pvx = point.x - lineStart.x;
    float pvy = point.y - lineStart.y;
    float pvdot = dx * pvx + dy * pvy;
    float ax = pvx - pvdot * dx;
    float ay = pvy - pvdot * dy;
    return std::sqrt(ax * ax + ay * ay);
}

void douglasPeuckerRecursive(std::vector<Rte::Vec2<float>>& line, float epsilon, std::vector<Rte::Vec2<float>>& result) {
    if (line.size() < 2) {
        return;
    }

    float maxDistance = 0.0;
    size_t index = 0;
    for (size_t i = 1; i < line.size() - 1; ++i) {
        float distance = perpendicularDistance(line[i], line[0], line[line.size() - 1]);
        if (distance > maxDistance) {
            index = i;
            maxDistance = distance;
        }
    }

    if (maxDistance > epsilon) {
        std::vector<Rte::Vec2<float>> recResults1;
        std::vector<Rte::Vec2<float>> recResults2;
        std::vector<Rte::Vec2<float>> firstLine(line.begin(), line.begin() + index + 1);
        std::vector<Rte::Vec2<float>> lastLine(line.begin() + index, line.end());
        douglasPeuckerRecursive(firstLine, epsilon, recResults1);
        douglasPeuckerRecursive(lastLine, epsilon, recResults2);
        result.assign(recResults1.begin(), recResults1.end() - 1);
        result.insert(result.end(), recResults2.begin(), recResults2.end());
        if (result.size() < 2) {
            result.push_back(line[0]);
            result.push_back(line[line.size() - 1]);
        }
    } else {
        result.push_back(line[0]);
        result.push_back(line[line.size() - 1]);
    }
}

std::vector<Rte::Vec2<float>> douglasPeucker(std::vector<Rte::Vec2<float>> line, float epsilon) {
    std::vector<Rte::Vec2<float>> result;
    douglasPeuckerRecursive(line, epsilon, result);
    return result;
}

std::vector<Tri> polygoneToTriangles(std::vector<Rte::Vec2<float>> polygone) {
    std::vector<p2t::Point*> points;
    for (size_t i = 0; i < polygone.size(); i++) {
        points.push_back(new p2t::Point(polygone[i].x, polygone[i].y));
    }
    p2t::CDT cdt(points);
    cdt.Triangulate();
    std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();
    std::vector<Tri> convertedTriangles;
    for (size_t i = 0; i < triangles.size(); i++) {
        Tri tri;
        tri.a.x = triangles[i]->GetPoint(0)->x;
        tri.a.y = triangles[i]->GetPoint(0)->y;
        tri.b.x = triangles[i]->GetPoint(1)->x;
        tri.b.y = triangles[i]->GetPoint(1)->y;
        tri.c.x = triangles[i]->GetPoint(2)->x;
        tri.c.y = triangles[i]->GetPoint(2)->y;
        convertedTriangles.push_back(tri);
    }
    return convertedTriangles;
}


bool isPointInPolygon(const Rte::Vec2<float>& point, const std::vector<Rte::Vec2<float>>& polygon) {
    bool inside = false;
    size_t n = polygon.size();
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        if (((polygon[i].y > point.y) != (polygon[j].y > point.y)) &&
            (point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

bool isPolygonInsidePolygon(const std::vector<Rte::Vec2<float>>& polygon1, const std::vector<Rte::Vec2<float>>& polygon2) {
    for (const auto& point : polygon2) {
        if (!isPointInPolygon(point, polygon1)) {
            return false;
        }
    }
    return true;
}

std::vector<std::vector<Rte::Vec2<float>>> findHolesInPolygons(std::vector<std::vector<Rte::Vec2<float>>>& polygons) {
    for (size_t i = 0; i < polygons.size(); i++) {
        for (size_t j = 0; j < polygons.size(); j++) {
            if (i != j && isPolygonInsidePolygon(polygons[i], polygons[j])) {
                polygons.erase(polygons.begin() + j);
                i = 0;
            }
        }
    }
    return polygons;
}

Rte::Vec2<float> rotate(Rte::Vec2<float> point, Rte::Vec2<float> center, float angle) {
    float s = std::sin(angle);
    float c = std::cos(angle);
    point.x -= center.x;
    point.y -= center.y;
    float xnew = point.x * c - point.y * s;
    float ynew = point.x * s + point.y * c;
    point.x = xnew + center.x;
    point.y = ynew + center.y;
    return point;
}

//where m_pixel is an array of uint8 in rgba format and pixel is an array of struct containing r,g,b and a and the pos of each pixel
std::vector<std::vector<pixel>> RigidBodyImpl::getRotatedPixels() const {
    Rte::Vec2<float> center = {static_cast<float>(m_size.x) / 2.0F, static_cast<float>(m_size.y) / 2.0F};
    float angle = -getRotation();
    std::vector<std::vector<pixel>> rotatedPixels(m_size.y, std::vector<pixel>(m_size.x));
    
    for (int y = 0; y < m_size.y; y++) {
        for (int x = 0; x < m_size.x; x++) {
            Rte::Vec2<float> rotatedPos = rotate({static_cast<float>(x), static_cast<float>(y)}, center, angle);
            rotatedPixels[y][x] = {m_pixels[(y * m_size.x + x) * 4], m_pixels[(y * m_size.x + x) * 4 + 1], m_pixels[(y * m_size.x + x) * 4 + 2], m_pixels[(y * m_size.x + x) * 4 + 3], (rotatedPos.x - center.x), (rotatedPos.y - center.y)};
        }
    }
    return rotatedPixels;
}

float RigidBodyImpl::getRotation() const {
    b2Rot rotation = b2Body_GetRotation(m_bodyId);
    return b2Rot_GetAngle(rotation);
}

Rte::Vec2<float> RigidBodyImpl::getPosition() const {
    b2Vec2 position = b2Body_GetPosition(m_bodyId);
    return {position.x, position.y};
}

std::pair<materials_t *, material_def> createMaterialMap(Rte::Vec2<Rte::u16> size , const Rte::u8* pixels) {
    materials_t *materialMap = new materials_t[size.x * size.y];
    material_def averageProperties;
    averageProperties.is_dynamic = true;
    for (int i = 0; i < size.x * size.y; i++) {
        int pixelColor = pixels[i * 4] * 1 + pixels[i * 4 + 1] * 2 + pixels[i * 4 + 2] * 4 + pixels[i * 4 + 3] * 8;
        switch (matColors.at(pixelColor)) {
            case air:
                materialMap[i] = air;
                break;
            case s_wood:
                materialMap[i] = s_wood;
                break;
            case d_wood:
                materialMap[i] = d_wood;
                break;
            materialMap[i] = air;
        }
        if (materialMap[i] != air) {
            averageProperties.density += materials.at(materialMap[i]).density;
            averageProperties.friction += materials.at(materialMap[i]).friction;
        }
        if (!materials.at(materialMap[i]).is_dynamic) {
            averageProperties.is_dynamic = false;
        }
    }
    averageProperties.density /= size.x * size.y;
    averageProperties.friction /= size.x * size.y;
    return {materialMap, averageProperties};
}

RigidBodyImpl::RigidBodyImpl(const u8* pixels, Vec2<u16> size, b2WorldId worldId, Vec2<float> pos, Vec2<float> scale, float rotation) : m_pixels(pixels), m_size(size), m_worldId(worldId) {
    std::pair<materials_t *, material_def> result = createMaterialMap(size, pixels);
    m_materials = result.first;
    material_def properties = result.second;
    // Create a body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();

    if (properties.is_dynamic) {
        bodyDef.type = b2BodyType::b2_dynamicBody;
    } else {
        bodyDef.type = b2BodyType::b2_staticBody;
    }

    bodyDef.position = {(pos.x - 1920 / 2.F) / 8.F / PPM, -(pos.y - 1080 / 2.F) / 8.F / PPM};
    bodyDef.rotation = b2MakeRot(rotation * b2_pi / 180.F);
    
    // Create the polygons from the image
    std::vector<int> binaryImage = convertToBinary(pixels, size);
    std::vector<std::vector<Rte::Vec2<float>>> vertices = marchingSquares(binaryImage, size);
    std::vector<std::vector<Rte::Vec2<float>>> continuousLines = createContinuousLines(vertices);
    continuousLines = findHolesInPolygons(continuousLines); 
    for (size_t i = 0; i < continuousLines.size(); i++)
    {
        continuousLines[i] = douglasPeucker(continuousLines[i], 0.75);
    }
    std::vector<Tri> triangles = polygoneToTriangles(continuousLines[0]);

    m_bodyId = b2CreateBody(worldId, &bodyDef);

    // Create attach the triangles to the body
    for (const auto& tri : triangles) {
        b2Vec2 vertices[3];
        vertices[0] = {(tri.a.x - size.x / 2) / PPM, -(tri.a.y - size.y / 2) / PPM};
        vertices[1] = {(tri.b.x - size.x / 2) / PPM, -(tri.b.y - size.y / 2) / PPM};
        vertices[2] = {(tri.c.x - size.x / 2) / PPM, -(tri.c.y - size.y / 2) / PPM};
        b2Hull hull = b2ComputeHull(vertices, 3);
        b2Polygon triangle = b2MakePolygon(&hull, 0);

        // Create a shape definition
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = properties.density;
        shapeDef.friction = properties.friction;

        // Create the shape
        b2CreatePolygonShape(m_bodyId, &shapeDef, &triangle);
    }
}

RigidBodyImpl::RigidBodyImpl(std::shared_ptr<RigidBodyImpl> rigidBody, const u8* pixels, Rte::Vec2<u16> size, b2WorldId worldId) : m_pixels(pixels), m_size(size), m_worldId(worldId) {
    // Copy the body definition from the existing rigid body
    std::pair<materials_t *, material_def> result = createMaterialMap(size, pixels);
    m_materials = result.first;
    material_def properties = result.second;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    if (properties.is_dynamic) {
        bodyDef.type = b2BodyType::b2_dynamicBody;
    } else {
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
    std::vector<int> binaryImage = convertToBinary(m_pixels, size);
    std::vector<std::vector<Rte::Vec2<float>>> vertices = marchingSquares(binaryImage, size);
    std::vector<std::vector<Rte::Vec2<float>>> continuousLines = createContinuousLines(vertices);
    continuousLines = findHolesInPolygons(continuousLines); 
    for (size_t i = 0; i < continuousLines.size(); i++)
    {
        continuousLines[i] = douglasPeucker(continuousLines[i], 0.75);
    }
    std::vector<Tri> triangles = polygoneToTriangles(continuousLines[0]);

    m_bodyId = b2CreateBody(m_worldId, &bodyDef);

    // Create attach the triangles to the body
    for (const auto& tri : triangles) {
        b2Vec2 vertices[3];
        vertices[0] = {(tri.a.x - size.x / 2) / PPM, -(tri.a.y - size.y / 2) / PPM};
        vertices[1] = {(tri.b.x - size.x / 2) / PPM, -(tri.b.y - size.y / 2) / PPM};
        vertices[2] = {(tri.c.x - size.x / 2) / PPM, -(tri.c.y - size.y / 2) / PPM};
        b2Hull hull = b2ComputeHull(vertices, 3);
        b2Polygon triangle = b2MakePolygon(&hull, 0);

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
