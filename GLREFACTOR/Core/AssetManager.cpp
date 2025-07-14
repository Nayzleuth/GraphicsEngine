#include "BackEnd.h"
#include "AssetManager.h"
#include "../Common/Util.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STBI/stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Why isn't ShaderManager combined with AssetManager?

Mesh::Mesh(std::string fileName) : meshName(fileName) {
    std::cout << "Loading OBJ named " << fileName << "\n";
    std::ifstream fileReader;

    std::vector<unsigned int> indices = {};

    std::vector<std::vector<unsigned int>> vertIndices; // Holds the indices of vertices in order
    std::vector<std::vector<unsigned int>> texIndices;  // Holds the indices of tex coords that are located in newTexCoords
    std::vector<std::vector<unsigned int>> normIndices; // Holds the indices of normals that are located in newNormals
    std::vector<float> newVertices;                     // Holds all needed vertices in order
    std::vector<float> newTexCoords;                    // Holds all needed tex coords in order
    std::vector<float> newNormals;                      // Holds all needed normals in order
    std::string newLine;
    fileReader.open("res/Objs/" + fileName);

    if (fileReader.is_open()) {
        std::cout << "Successful Open.\n";
    }
    else {
        std::cerr << "Failed Open.\n";
    }

    int inc = 0;
    int incTest = 0;


    //FORMATTING:
    // VERTEX INDEX / TEXCOORD INDEX / NORMAL INDEX

    while (std::getline(fileReader, newLine)) {
        //Turn "v" lines into vertices.
        if (newLine.find("v ") == 0) {
            std::istringstream lineReader(newLine);
            std::string prefix;
            float x, y, z;

            lineReader >> prefix >> x >> y >> z;
            newVertices.emplace_back(x);
            newVertices.emplace_back(y);
            newVertices.emplace_back(z);
        }

        //Turn "vn" lines into normals.
        if (newLine.find("vn ") == 0) {
            std::istringstream lineReader(newLine);
            std::string prefix;
            float x, y, z;

            lineReader >> prefix >> x >> y >> z;
            newNormals.emplace_back(x);
            newNormals.emplace_back(y);
            newNormals.emplace_back(z);
        }

        //Turn "vt" lines into tex coords.
        if (newLine.find("vt ") == 0) {
            std::istringstream lineReader(newLine);
            std::string prefix;
            float x, y, z;

            lineReader >> prefix >> x >> y;
            newTexCoords.emplace_back(x);
            newTexCoords.emplace_back(y);
        }

        //Split up each F line into its appropriate index storage
        if (newLine.find("f ") == 0) {
            std::vector<unsigned int> sideVertIndices;
            std::vector<unsigned int> sideTexIndices;
            std::vector<unsigned int> sideNormIndices;

            std::string lineData = newLine;
            lineData.erase(0, 2);
            std::istringstream dataReader(lineData);
            std::string dataSplice;

            int numSides = 0;
            while (!dataReader.eof()) {
                numSides++;
                dataReader >> dataSplice;
                sideVertIndices.push_back(std::stoi(dataSplice.substr(0, dataSplice.find('/'))) - 1); // Stores the first indice of each triplet.
                sideTexIndices.push_back(std::stoi(dataSplice.substr(dataSplice.find('/') + 1, dataSplice.find_last_of('/') - 2)) - 1); // Stores the second indice of each triplet.
                sideNormIndices.push_back(std::stoi(dataSplice.substr(dataSplice.find_last_of('/') + 1, dataSplice.length())) - 1); // Guess.
            }

            vertIndices.push_back(sideVertIndices);
            texIndices.push_back(sideTexIndices);
            normIndices.push_back(sideNormIndices);

            inc++;
        }
    }

    // Triangulates vertIndices
    std::vector<std::vector<unsigned int>> tempVec;
    for (int outer = 0; outer < vertIndices.size(); outer++) {
        for (int inner = 0; inner < vertIndices[outer].size() - 2; inner++) {
            std::vector<unsigned int> triangulatedIndices;
            triangulatedIndices.push_back(vertIndices[outer][0]);
            triangulatedIndices.push_back(vertIndices[outer][inner + 1]);
            triangulatedIndices.push_back(vertIndices[outer][inner + 2]);
            tempVec.push_back(triangulatedIndices);
        }
    }
    vertIndices = tempVec;

    // Triangulates texIndices
    std::vector<std::vector<unsigned int>> temp2Vec;
    for (int outer = 0; outer < texIndices.size(); outer++) {
        for (int inner = 0; inner < texIndices[outer].size() - 2; inner++) {
            std::vector<unsigned int> triangulatedIndices;
            triangulatedIndices.push_back(texIndices[outer][0]);
            triangulatedIndices.push_back(texIndices[outer][inner + 1]);
            triangulatedIndices.push_back(texIndices[outer][inner + 2]);
            temp2Vec.push_back(triangulatedIndices);
        }
    }
    texIndices = temp2Vec;

    // Triangulates normIndices
    std::vector<std::vector<unsigned int>> temp3Vec;
    for (int outer = 0; outer < normIndices.size(); outer++) {
        for (int inner = 0; inner < normIndices[outer].size() - 2; inner++) {
            std::vector<unsigned int> triangulatedIndices;
            triangulatedIndices.push_back(normIndices[outer][0]);
            triangulatedIndices.push_back(normIndices[outer][inner + 1]);
            triangulatedIndices.push_back(normIndices[outer][inner + 2]);
            temp3Vec.push_back(triangulatedIndices);
        }
    }
    normIndices = temp3Vec;

    int ctr = 0;
    for (int outer = 0; outer < vertIndices.size(); outer++) {
        for (int inner = 0; inner < vertIndices[outer].size(); inner++) {
            ctr++;
        }
    }

    for (int i = 0; i < ctr; i++) {
        indices.push_back(i);
    }

    std::vector<float> vertTexNorm; // Stores the data in order.
    for (int trInc = 0; trInc < vertIndices.size(); trInc++) {
        for (int vertIndInc = 0; vertIndInc < vertIndices[trInc].size(); vertIndInc++) {
            for (int offset = 0; offset < 3; offset++) {
                vertTexNorm.push_back(newVertices[(vertIndices[trInc][vertIndInc] * 3) + offset]);
            }
            for (int offset = 0; offset < 2; offset++) {
                vertTexNorm.push_back(newTexCoords[(texIndices[trInc][vertIndInc] * 2) + offset]);
            }
            for (int offset = 0; offset < 3; offset++) {
                vertTexNorm.push_back(newNormals[(normIndices[trInc][vertIndInc] * 3) + offset]);
            }
        }
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Indices
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertTexNorm.size() * sizeof(float), vertTexNorm.data(), GL_STATIC_DRAW);

    // Pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
    glEnableVertexAttribArray(0);
    
    // Tex
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    
    // Norm
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 5));
    glEnableVertexAttribArray(2);

    indexCount = indices.size();

    // AABB Corner init
    glm::vec3 min = glm::vec3(newVertices[0], newVertices[1], newVertices[2]);
    glm::vec3 max = glm::vec3(newVertices[0], newVertices[1], newVertices[2]);

    for (int i = 0; i < newVertices.size(); i += 3) {
        //x
        if (min.x > newVertices[i]) min.x = newVertices[i];
        if (max.x < newVertices[i]) max.x = newVertices[i];
        //y
        if (min.y > newVertices[i + 1]) min.y = newVertices[i + 1];
        if (max.y < newVertices[i + 1]) max.y = newVertices[i + 1];
        //z
        if (min.z > newVertices[i + 2]) min.z = newVertices[i + 2];
        if (max.z < newVertices[i + 2]) max.z = newVertices[i + 2];

        meshVertices.emplace_back(glm::vec3(newVertices[i], newVertices[i + 1], newVertices[i + 2]));
    }

    meshBoundingBox.min = min;
    meshBoundingBox.max = max;

    AssetManager::g_MeshCollection.emplace(fileName, *this);
}  

//Mesh::Mesh(Cube& generatedCube) {
//    VAO = generatedCube.sideSquare. 
//    VBO =
//    EBO =
//    size_t indexCount;
//};

Mesh::Mesh(std::string name, std::vector<float> meshData, std::vector<unsigned int> indices) : meshName(name) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Indices
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

    // Pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(0);
    
    // Tex
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    indexCount = indices.size();

    AssetManager::g_MeshCollection.emplace(name, *this);
}

void Mesh::useVAO() {
	glBindVertexArray(VAO);
};

size_t Mesh::getIndexCount() {
    return indexCount;
}

namespace AssetManager {
    // ASSETS
    std::unordered_map<std::string, RenderBuffers> g_VAOs;
    std::unordered_map<std::string, Mesh> g_MeshCollection;
    std::unordered_map<std::string, unsigned int> g_TextureCollection;
    std::vector<std::string> meshFileNames;

    // SCENE DATA
    std::vector<Light> lights; 
    unsigned int lightDataSSBO = 0;

    // EX DATA
    std::vector<float> scrQuad = {
        // Example quad data
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Top-right
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // Top-left
    };

    std::vector<unsigned int> quadIndices = {
        0, 1, 2,
        0, 2, 3
    };

    void Init() {
        // Mesh
        meshFileNames.emplace_back("cube.obj");
        meshFileNames.emplace_back("thinfish.obj");
        
        for (std::string fileName : meshFileNames) {
            Mesh newMesh(fileName);
        }

        Mesh screenQuad("ScreenQuad", scrQuad, quadIndices);

        // Textures
        genTexture("MissingTexture.png");

        // Lights
        Light newLight;

        newLight.position = glm::vec4(-2, 2, -2, 1);
        newLight.color = glm::vec4(glm::normalize(glm::vec3(0, 0, 255)), 1);
        newLight.intensity = 1;
        newLight.range = 8;

        lights.emplace_back(newLight);

        Light lightTwo;

        lightTwo.position = glm::vec4(1, -1, -1, 1);
        lightTwo.color = glm::vec4(0, 0.5f, 0, 1);
        lightTwo.intensity = 1;
        lightTwo.range = 8;

        lights.emplace_back(lightTwo);
        
        Light lightThree;

        lightThree.position = glm::vec4(2, 1.5f, -4, 1);
        lightThree.color = glm::vec4(glm::normalize(glm::vec3(76, 0, 153)), 1);
        lightThree.intensity = 1;
        lightThree.range = 8;

        lights.emplace_back(lightThree);


        /*
        for (int i = 0; i < 300; i++) {
            Light newLight;

            newLight.position = glm::vec4(Util::randomFloat(-15, 15), Util::randomFloat(-15, 75), Util::randomFloat(-15, 15), 1);
            newLight.color = glm::vec4(glm::normalize(Util::randomVec3(0, 1)), 1);
            newLight.intensity = 1;
            newLight.range = 8;

            lights.emplace_back(newLight);
        }
        */
        updateLightBuffer();
    };

    void genTexture(std::string texName) {
        unsigned int newTex;
        glGenTextures(1, &newTex);
        glBindTexture(GL_TEXTURE_2D, newTex);

        int width, height, nrChannels;

        stbi_set_flip_vertically_on_load(true);
        std::string path = "res/Textures/" + texName;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        std::cout << "Loaded texture '" << texName << "' with size: "
            << width << "x" << height << " and channels: " << nrChannels << std::endl;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        g_TextureCollection[texName] = newTex;
    };

    void updateLightBuffer() {
        if (lightDataSSBO == 0) {
            glGenBuffers(1, &lightDataSSBO);
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightDataSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, lights.size() * sizeof(Light), lights.data(), GL_DYNAMIC_DRAW);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightDataSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void genVAO(std::string name) {
        RenderBuffers newVAO;
        glGenVertexArrays(1, &newVAO.VAO);
        glBindVertexArray(newVAO.VAO);

        glGenBuffers(1, &newVAO.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, newVAO.VBO);
        
        AssetManager::g_VAOs.emplace(name, newVAO);
    };

    void genVAO(RenderBuffers& preMadeVAO) {
        glGenVertexArrays(1, &preMadeVAO.VAO);
        glBindVertexArray(preMadeVAO.VAO);

        glGenBuffers(1, &preMadeVAO.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, preMadeVAO.VBO);
    };

    void VAOFillBuffer(std::vector<Point>& arr) {
        std::vector<float> floats;

        for (Point nextPoint : arr) {
            floats.emplace_back(nextPoint.position.x);
            floats.emplace_back(nextPoint.position.y);
            floats.emplace_back(nextPoint.position.z); 
            floats.emplace_back(nextPoint.color.r);
            floats.emplace_back(nextPoint.color.g);
            floats.emplace_back(nextPoint.color.b);
        }

        glBufferData(GL_ARRAY_BUFFER, floats.size() * sizeof(float), floats.data(), GL_STATIC_DRAW);
    } 
    
    void VAOsubBuffer(std::vector<Point>& arr) {
        std::vector<float> colorData;

        for (Point nextPoint : arr) { 
            colorData.emplace_back(nextPoint.color.r);
            colorData.emplace_back(nextPoint.color.g);
            colorData.emplace_back(nextPoint.color.b);
        }

        for (int i = 0; i < arr.size(); i++) {
            glBufferSubData(GL_ARRAY_BUFFER, (i * 6 + 3) * sizeof(float), 3 * sizeof(float), &colorData[i * 3]);
        }
    }

    void VAOSetAttrib() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));        
        glEnableVertexAttribArray(1);
    }

    void CleanUp() {
        glDeleteBuffers(1, &lightDataSSBO);
    };
}
