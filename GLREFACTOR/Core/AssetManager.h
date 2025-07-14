#pragma once
#include "ShaderManager.h"
#include <iostream>
#include <vector>
#include <string>
#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

class Mesh;
struct RenderBuffers;
struct Light;
struct Point;
struct AABB;

namespace AssetManager {
	extern std::unordered_map<std::string, RenderBuffers> g_VAOs;
	extern std::unordered_map<std::string, Mesh> g_MeshCollection;
	extern std::unordered_map<std::string, unsigned int> g_TextureCollection;
	extern std::vector<Light> lights;

	void Init();
	void genTexture(std::string texName);
	void genVAO(std::string name);
	void genVAO(RenderBuffers& preMadeVAO);
	void VAOFillBuffer(std::vector<Point>& arr);
	void VAOsubBuffer(std::vector<Point>& arr);
	void VAOSetAttrib();
	void updateLightBuffer();
	void CleanUp();
}

struct AABB {
    glm::vec3 min = glm::vec3(0);
    glm::vec3 max = glm::vec3(0);
};

struct OBB {
    glm::vec3 position;
    glm::vec3 axes[3];
    glm::vec3 halfExtents;
};

class Mesh {
public:
    std::string meshName;
    Mesh(std::string fileName);
    //Mesh(Cube& generatedCube);
    Mesh(std::string meshName, std::vector<float> meshData, std::vector<unsigned int> indices);

    AABB meshBoundingBox;
    std::vector<glm::vec3> meshVertices;

    void useVAO();
    size_t getIndexCount();
private:
    unsigned int VAO, VBO, EBO;
    size_t indexCount;
};

struct Light {
    glm::vec4 position;
    glm::vec4 color;
    float intensity;
    float range;
    float padding1;
    float padding2;
};

struct RenderBuffers {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
};

struct Point {
    Point(glm::vec3 pos) : position(pos) {};
    Point(glm::vec3 pos, glm::vec3 col) : position(pos), color(col) {};

    glm::vec3 position;
    glm::vec3 color = glm::vec3(0);
};

struct Line {
    RenderBuffers vao;
    std::vector<Point> linePoints;

    void LineSetup() {
        AssetManager::genVAO(vao);
        AssetManager::VAOFillBuffer(linePoints);
        AssetManager::VAOSetAttrib();
    };

    void AddPointFromVec3(glm::vec3 newPoint) {
        linePoints.emplace_back(newPoint);
    };

    void AddPointFromVec3(glm::vec3 newPoint, glm::vec3 newPointColor) {
        linePoints.emplace_back(newPoint, newPointColor);
    };

    void SetLineColor(glm::vec3 newColor) {
        for (Point& nextPoint : linePoints) {
            nextPoint.color = newColor;
        }
    }

    glm::vec3 GetPointFromIndex(int index) {
        return linePoints[index].position;
    }
};

struct Square {
    Line squareOutline;
    glm::mat4 model = glm::mat4(1);
    Square(glm::vec3 pos, float sze, glm::vec3 rot) :
        position(pos),
        size(sze),
        rotationDeg(rot) {

        // TL, TR, BR, BL  
        glm::vec3 TL = glm::vec3(-size / 2, size / 2, 0);
        glm::vec3 TR = glm::vec3(size / 2, size / 2, 0);
        glm::vec3 BR = glm::vec3(size / 2, -size / 2, 0);
        glm::vec3 BL = glm::vec3(-size / 2, -size / 2, 0);

        model = glm::translate(glm::mat4(1.0f), position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotationDeg.x), glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotationDeg.y), glm::vec3(0, 1, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotationDeg.z), glm::vec3(0, 0, 1));

        // TL, TR, BR, BL
        squareOutline.AddPointFromVec3(TL);
        squareOutline.AddPointFromVec3(TR);
        squareOutline.AddPointFromVec3(BR);
        squareOutline.AddPointFromVec3(BL);
    }

    void SetWholeColor(glm::vec3 newColor) {
        for (Point& nextPoint : squareOutline.linePoints) {
            nextPoint.color = newColor;
        }
    }

    void SetCornerColor(glm::vec3 newColor, std::string corner) {
        if (corner == "TL") {
            squareOutline.linePoints[0].color = newColor;
        }
        else if (corner == "TR") {
            squareOutline.linePoints[1].color = newColor;
        }
        else if (corner == "BR") {
            squareOutline.linePoints[2].color = newColor;
        }
        else if (corner == "BL") {
            squareOutline.linePoints[3].color = newColor;
        }
    }

    void Setup() {
        squareOutline.LineSetup();
    };

    glm::vec3 position;
    float size;
    glm::vec3 rotationDeg;

};

struct DynamicRecPrism {
    DynamicRecPrism(glm::vec3 pos, glm::vec3 halfExtents) : worldPos(pos), halfWidth(halfExtents.x), halfHeight(halfExtents.y), halfDepth(halfExtents.z) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 8, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        unsigned int indices[] = {
            0,1, 1,3, 3,2, 2,0, 
            4,5, 5,7, 7,6, 6,4, 
            0,4, 1,5, 2,6, 3,7  
        };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        std::vector<glm::vec3> corners(8);

        glm::vec3 right = rotation * glm::vec3(1, 0, 0);
        glm::vec3 up = rotation * glm::vec3(0, 1, 0);
        glm::vec3 forward = rotation * glm::vec3(0, 0, 1);

        glm::vec3 hx = right * (float)halfWidth;
        glm::vec3 hy = up * (float)halfHeight;
        glm::vec3 hz = forward * (float)halfDepth;

        int i = 0;
        for (int x = -1; x <= 1; x += 2) {
            for (int y = -1; y <= 1; y += 2) {
                for (int z = -1; z <= 1; z += 2) {
                    corners[i++] = float(x) * hx + float(y) * hy + float(z) * hz;
                }
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, corners.size() * sizeof(glm::vec3), corners.data());
        
        updateModel();
    };
    
    void updateModel() {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), worldPos);
        glm::mat4 R = glm::mat4_cast(rotation);
        model = T * R;
    }

    void updateWorldPos(glm::vec3 newPos) {
        worldPos = newPos;
    };
    
    void updateRotation(glm::quat rot) {
        rotation = rot;
    }

    void updateFromAABB(const glm::vec3& min, const glm::vec3& max) {        
        std::vector<glm::vec3> corners(8);

        corners[0] = glm::vec3(min.x, min.y, min.z);
        corners[1] = glm::vec3(max.x, min.y, min.z);
        corners[2] = glm::vec3(min.x, max.y, min.z);
        corners[3] = glm::vec3(max.x, max.y, min.z);
        corners[4] = glm::vec3(min.x, min.y, max.z);
        corners[5] = glm::vec3(max.x, min.y, max.z);
        corners[6] = glm::vec3(min.x, max.y, max.z);
        corners[7] = glm::vec3(max.x, max.y, max.z);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, corners.size() * sizeof(glm::vec3), corners.data());
        model = glm::mat4(1.0f);
    }

    void updateFromOBB(const OBB& obb) {
        std::vector<glm::vec3> corners(8);

        for (int i = 0; i < 8; i++) {
            corners[i] = obb.position;
            corners[i] += obb.axes[0] * ((i & 1) ? obb.halfExtents.x : -obb.halfExtents.x);
            corners[i] += obb.axes[1] * ((i & 2) ? obb.halfExtents.y : -obb.halfExtents.y);
            corners[i] += obb.axes[2] * ((i & 4) ? obb.halfExtents.z : -obb.halfExtents.z);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, corners.size() * sizeof(glm::vec3), corners.data());

        model = glm::mat4(1.0f);
    }

    void RenderWireframe() {
        glBindVertexArray(vao);
        ShaderManager::g_Shaders.PointsOnly.use();
        ShaderManager::g_Shaders.PointsOnly.updateMat4("model", model);

        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    }

    GLuint vao, vbo, ebo;

    glm::vec3 worldPos = glm::vec3(0);
    glm::quat rotation = glm::quat(glm::vec3(0));
    double halfWidth;
    double halfHeight;
    double halfDepth;

    glm::mat4 model;
};

struct RectangularPrism {
    RectangularPrism(glm::vec3 pos, glm::vec3 dim, glm::vec3 rot) :
        position(pos),
        rotationDeg(rot) {

        glm::vec3 vertices[24] = {
            // Front
            glm::vec3(-dim.x,  dim.y,  dim.z),
            glm::vec3(dim.x,  dim.y,  dim.z),
            glm::vec3(-dim.x, -dim.y,  dim.z),
            glm::vec3(dim.x, -dim.y,  dim.z),
            // Back
            glm::vec3(dim.x,  dim.y, -dim.z),
            glm::vec3(-dim.x,  dim.y, -dim.z),
            glm::vec3(dim.x, -dim.y, -dim.z),
            glm::vec3(-dim.x, -dim.y, -dim.z),
            // Left
            glm::vec3(-dim.x,  dim.y, -dim.z),
            glm::vec3(-dim.x,  dim.y,  dim.z),
            glm::vec3(-dim.x, -dim.y, -dim.z),
            glm::vec3(-dim.x, -dim.y,  dim.z),
            // Right
            glm::vec3(dim.x,  dim.y,  dim.z),
            glm::vec3(dim.x,  dim.y, -dim.z),
            glm::vec3(dim.x, -dim.y,  dim.z),
            glm::vec3(dim.x, -dim.y, -dim.z),
            // Top
            glm::vec3(-dim.x,  dim.y, -dim.z),
            glm::vec3(dim.x,  dim.y, -dim.z),
            glm::vec3(-dim.x,  dim.y,  dim.z),
            glm::vec3(dim.x,  dim.y,  dim.z),
            // Bottom
            glm::vec3(-dim.x, -dim.y,  dim.z),
            glm::vec3(dim.x, -dim.y,  dim.z),
            glm::vec3(-dim.x, -dim.y, -dim.z),
            glm::vec3(dim.x, -dim.y, -dim.z)
        };

        for (int i = 0; i < 24; i++) {
            this->vertices[i] = vertices[i];
        }

        unsigned int FaceIndexes[6][4] = {
            {0, 1, 2, 3},
            {4, 0, 6, 2},
            {5, 4, 7, 6},
            {1, 5, 3, 7},
            {4, 5, 0, 1},
            {2, 3, 6, 7}
        };

        unsigned int indices[36];

        for (int i = 0; i < 6; i++) {
            unsigned int startingNum = i * 4;
            indices[(i * 6)] = startingNum;
            indices[(i * 6) + 1] = startingNum + 1;
            indices[(i * 6) + 2] = startingNum + 2;
            indices[(i * 6) + 3] = startingNum + 1;
            indices[(i * 6) + 4] = startingNum + 3;
            indices[(i * 6) + 5] = startingNum + 2;
        }

        unsigned int edgeIndices[] = {
            // Front
            0, 1, 1, 3, 3, 2, 2, 0,
            // Back
            4, 5, 5, 7, 7, 6, 6, 4,
            // Left
            8, 9, 9, 11, 11, 10, 10, 8,
            // Right
            12, 13, 13, 15, 15, 14, 14, 12,
            // Top
            16, 17, 17, 19, 19, 18, 18, 16,
            // Bottom
            20, 21, 21, 23, 23, 22, 22, 20
        };

        CreateMultiColoredEdgeVertices(dim);

        glGenVertexArrays(1, &recPrismBuffers.VAO);
        glBindVertexArray(recPrismBuffers.VAO);

        glGenBuffers(1, &recPrismBuffers.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, recPrismBuffers.VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &colorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

        glGenBuffers(1, &recPrismBuffers.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, recPrismBuffers.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &multiColoredEdgesVAO);
        glBindVertexArray(multiColoredEdgesVAO);

        glGenBuffers(1, &multiColoredEdgesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, multiColoredEdgesVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(multiColoredVertices), multiColoredVertices, GL_STATIC_DRAW);

        glGenBuffers(1, &multiColoredEdgesColorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, multiColoredEdgesColorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(multiColoredColors), multiColoredColors, GL_STATIC_DRAW);

        glBindVertexArray(recPrismBuffers.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, recPrismBuffers.VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(multiColoredEdgesVAO);
        glBindBuffer(GL_ARRAY_BUFFER, multiColoredEdgesVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, multiColoredEdgesColorVBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(1);

        recPrismModel = glm::translate(glm::mat4(1.0f), position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotationDeg.x), glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotationDeg.y), glm::vec3(0, 1, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotationDeg.z), glm::vec3(0, 0, 1));
    };

    enum Face {
        FRONT = 0,
        BACK = 1,
        LEFT = 2,
        RIGHT = 3,
        TOP = 4,
        BOTTOM = 5
    };

    RenderBuffers recPrismBuffers;
    unsigned int colorVBO;
    unsigned int multiColoredEdgesVAO;
    unsigned int multiColoredEdgesVBO;
    unsigned int multiColoredEdgesColorVBO;
    glm::vec3 vertices[24] = { glm::vec3(0) };
    glm::vec3 colors[24] = { glm::vec3(0) };

    glm::vec3 multiColoredVertices[48] = { glm::vec3(0) };
    glm::vec3 multiColoredColors[48] = { glm::vec3(0) };

    glm::vec3 position;
    glm::vec3 rotationDeg;
    glm::mat4 recPrismModel;

    void CreateMultiColoredEdgeVertices(glm::vec3 dim) {
        const float offset = 0.004f;

        int faceVertexIndices[6][4] = {
            {0, 1, 2, 3},     // FRONT
            {5, 4, 7, 6},     // BACK
            {8, 9, 10, 11},   // LEFT
            {12, 13, 14, 15}, // RIGHT
            {16, 17, 18, 19}, // TOP
            {20, 21, 22, 23}  // BOTTOM
        };

        int faceEdges[6][4][2] = {
            {{0, 1}, {1, 3}, {3, 2}, {2, 0}}, // FRONT
            {{0, 1}, {1, 3}, {3, 2}, {2, 0}}, // BACK
            {{0, 1}, {1, 3}, {3, 2}, {2, 0}}, // LEFT
            {{0, 1}, {1, 3}, {3, 2}, {2, 0}}, // RIGHT
            {{0, 1}, {1, 3}, {3, 2}, {2, 0}}, // TOP
            {{0, 1}, {1, 3}, {3, 2}, {2, 0}}  // BOTTOM
        };

        glm::vec3 offsetDirs[6] = {
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, -1),
            glm::vec3(-1, 0, 0),
            glm::vec3(1, 0, 0),
            glm::vec3(0, 1, 0),
            glm::vec3(0, -1, 0)
        };

        int vertexCount = 0;

        for (int face = 0; face < 6; face++) {
            for (int edge = 0; edge < 4; edge++) {
                int v1 = faceVertexIndices[face][faceEdges[face][edge][0]];
                int v2 = faceVertexIndices[face][faceEdges[face][edge][1]];

                multiColoredVertices[vertexCount] = vertices[v1] + offsetDirs[face] * offset;
                multiColoredVertices[vertexCount + 1] = vertices[v2] + offsetDirs[face] * offset;

                multiColoredColors[vertexCount] = glm::vec3(1.0f);
                multiColoredColors[vertexCount + 1] = glm::vec3(1.0f);

                vertexCount += 2;
            }
        }
    }

    void SetWholeCubeColor(glm::vec3 color) {
        for (glm::vec3& col : colors) {
            col = color;
        }

        for (glm::vec3& col : multiColoredColors) {
            col = color;
        }

        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), &colors[0]);

        glBindBuffer(GL_ARRAY_BUFFER, multiColoredEdgesColorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(multiColoredColors), &multiColoredColors[0]);

        std::cout << "Colors:\n";
        for (glm::vec3 col : colors) {
            std::cout << col.x << ", " << col.y << ", " << col.z << "\n";
        }
    }

    void SetFaceColor(unsigned int faceIndex, glm::vec3 color) {
        int startIndex = faceIndex * 4;

        for (int i = 0; i < 4; i++) {
            colors[startIndex + i] = color;
        }

        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, startIndex * sizeof(glm::vec3), 4 * sizeof(glm::vec3), &colors[startIndex]);

        int edgeStartIndex = faceIndex * 8;
        for (int i = 0; i < 8; i++) {
            multiColoredColors[edgeStartIndex + i] = color;
        }

        glBindBuffer(GL_ARRAY_BUFFER, multiColoredEdgesColorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, edgeStartIndex * sizeof(glm::vec3), 8 * sizeof(glm::vec3), &multiColoredColors[edgeStartIndex]);

        std::cout << "Colors:\n";
        for (glm::vec3 col : colors) {
            std::cout << col.x << ", " << col.y << ", " << col.z << "\n";
        }
    }

    void RenderSolid() {
        glBindVertexArray(recPrismBuffers.VAO);
        ShaderManager::g_Shaders.DebugPoints.updateMat4("model", recPrismModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, recPrismBuffers.EBO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void RenderWireframe() {
        glBindVertexArray(multiColoredEdgesVAO);
        ShaderManager::g_Shaders.DebugPoints.updateMat4("model", recPrismModel);

        glDrawArrays(GL_LINES, 0, 48);
    }
};