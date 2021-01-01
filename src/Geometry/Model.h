//
// Created by 邱泽鸿 on 2020/12/14.
//

#pragma once
#include "Geometry.h"
#include "Renderer.h"

// 模型导入
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


struct Texture
{
    unsigned int ID;
    std::string Type;
    std::string Path;
};

class Model : public Geometry
{
public:
    Model(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader>& Shader, const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
           const Material& material = {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 1.0f},
           const Rotation& rotation = {0.0f, 0.0f, 0.0f},
           const Scale& Scale = {1.0f, 1.0f, 1.0f, 1.0f});
    void load(const std::string& Path); // 加载模型
    void updateDrawData() override
    {

    }
    inline std::string getClassName() override
    {
        return "Geometry::Model";
    }
private:
    void processNode(aiNode *Node, const aiScene *Scene);
    void processMesh(aiMesh *Mesh, const aiScene *Scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *Material, aiTextureType Type, std::string TypeName);
    unsigned int TextureFromFile(const char *Path, const std::string &Directory, bool Gamma);
private:
    std::string m_Directory;
    std::vector<Texture> loadedTextures;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
};
