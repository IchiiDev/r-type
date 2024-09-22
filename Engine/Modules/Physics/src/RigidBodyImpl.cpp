#include "Rte/Common.hpp"

#include "RigidBodyImpl.hpp"
#include "Rte/Physics/Tool.hpp"
#include "box2d/collision.h"
#include "box2d/math_functions.h"
#include "poly2tri/poly2tri.h"

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
    for (int i = 0; i < size.x * size.y; i++)
    {
        if (image[i * 4] == 0)
        {
            binaryImage.push_back(0);
            std::cout << " ";
        }
        else
        {
            binaryImage.push_back(1);
            std::cout << "X";
        }
        if (i % size.x == 0)
        {
            std::cout << std::endl;
        }
    }
    return binaryImage;
}

// Using the marching squares algorithm to generate outline of the binary image
std::vector<std::vector<Rte::Vec2<float>>> marchingSquares (const std::vector<int> binaryImage, Rte::Vec2<Rte::u16> size) {
    std::vector<std::vector<Rte::Vec2<float>>> vertices;
    for (float y = 0; y < size.y - 1; y++)
    {
        for (float x = 0; x < size.x - 1; x++)
        {
            int topLeft = binaryImage[y * size.x + x];
            int topRight = binaryImage[y * size.x + x + 1];
            int bottomLeft = binaryImage[(y + 1) * size.x + x];
            int bottomRight = binaryImage[(y + 1) * size.x + x + 1];
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
        // std::cout << "Number of verticies: " << vertices.size() << std::endl;

        // std::cout << "Vertice: " << std::endl;
        // for (size_t j = 0; j < vertices[i].size(); j++)
        // {
        //     std::cout << "{" << vertices[i][j].x << ", " << vertices[i][j].y << "} ";
        // }
        // std::cout << std::endl;
        // std::cout << "Continuous Line: " << std::endl;
        // for (size_t j = 0; j < continuousLine.size(); j++)
        // {
        //     std::cout << "{" << continuousLine[j].x << ", " << continuousLine[j].y << "} ";
        // }
        // std::cout << std::endl;

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
        //std::cout << vertices.size() << " vertices left" << std::endl;
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

RigidBodyImpl::RigidBodyImpl(BodyType type, const u8* pixels, Rte::Vec2<u16> size, float density, float friction, b2WorldId worldId, Vec2<float> pos, Vec2<float> scale, float rotation) {
    
    // Create a body definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    if (type == BodyType::STATIC) {
        bodyDef.type = b2BodyType::b2_staticBody;
    } else if (type == BodyType::DYNAMIC) {
        bodyDef.type = b2BodyType::b2_dynamicBody;
    } else if (type == BodyType::KINEMATIC) {
        bodyDef.type = b2BodyType::b2_kinematicBody;
    }
    

    bodyDef.position = {(pos.x - 1920 / 2.F) / 8.F / PPM, -(pos.y - 1080 / 2.F) / 8.F / PPM};
    bodyDef.rotation = b2MakeRot(rotation * b2_pi / 180.F);
    std::cout << "Position: " << bodyDef.position.x << ", " << bodyDef.position.y << std::endl;
    // Create the body
    std::vector<int> binaryImage = convertToBinary(pixels, size);
    std::vector<std::vector<Rte::Vec2<float>>> vertices = marchingSquares(binaryImage, size);
    std::vector<std::vector<Rte::Vec2<float>>> continuousLines = createContinuousLines(vertices);
    for (size_t i = 0; i < continuousLines.size(); i++)
    {
        continuousLines[i] = douglasPeucker(continuousLines[i], 0.75);
    }
    std::vector<Tri> triangles = polygoneToTriangles(continuousLines[0]);

    m_bodyId = b2CreateBody(worldId, &bodyDef);

    // Create a polygon shape
    for (const auto& tri : triangles) {
        b2Vec2 vertices[3];
        vertices[0] = {(tri.a.x - size.x / 2) / PPM, -(tri.a.y - size.y / 2) / PPM};
        vertices[1] = {(tri.b.x - size.x / 2) / PPM, -(tri.b.y - size.y / 2) / PPM};
        vertices[2] = {(tri.c.x - size.x / 2) / PPM, -(tri.c.y - size.y / 2) / PPM};
        std::cout << "Triangle: " << vertices[0].x << ", " << vertices[0].y << " " << vertices[1].x << ", " << vertices[1].y << " " << vertices[2].x << ", " << vertices[2].y << std::endl;
        b2Hull hull = b2ComputeHull(vertices, 3);
        b2Polygon triangle = b2MakePolygon(&hull, 0);

        // Create a shape definition
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = density;
        shapeDef.friction = friction;

        // Create the shape
        b2CreatePolygonShape(m_bodyId, &shapeDef, &triangle);
    }
}

b2BodyId RigidBodyImpl::getBodyId() const {
    return m_bodyId;
}
