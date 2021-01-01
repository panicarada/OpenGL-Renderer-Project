//
// Created by 邱泽鸿 on 2020/12/31.
//

#include "Model.h"

#include "stb_image.h"


Model::Model(const std::shared_ptr<Camera> &Camera, const std::shared_ptr<Shader> &Shader, const glm::vec3 &Position,
             const Material &material, const Rotation &rotation, const Scale &Scale) : Geometry(Camera, Shader,
                                                                                                Position, material,
                                                                                                rotation, Scale)
{
    m_Layout->Push<float>(3); // 点坐标
    m_Layout->Push<float>(3); // 法向量
    m_Layout->Push<float>(4); // 颜色

    m_Layout->Push<float>(2); // 纹理坐标
}

void Model::load(const std::string &Path)
{
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(Path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << Importer.GetErrorString() << std::endl;
        return ;
    }
    m_Directory = Path.substr(0, Path.find_last_of('/'));


}

void Model::processNode(aiNode *Node, const aiScene *Scene)
{
    // 处理节点所有的网格
    for (unsigned int i = 0;i < Node->mNumMeshes; ++i)
    {
        aiMesh *Mesh = Scene->mMeshes[Node->mMeshes[i]];

    }

    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0;i < Node->mNumChildren; ++i)
    {
        processNode(Node->mChildren[i], Scene);
    }

}


void Model::processMesh(aiMesh *Mesh, const aiScene *Scene)
{
    std::vector<Vertex> Vertices;
    // Vertices
    for (unsigned int i = 0;i < Mesh->mNumVertices; ++i)
    {
        Vertex Temp;
        Temp.Position = {Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z};
        Temp.Normal = {Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z};
        Temp.Color = glm::vec4(0.0f); // 暂时不考虑使用
        if (Mesh->mTextureCoords[0])
        { // 是否有纹理坐标
            Temp.TexCoord = {Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y};
        }
        else
        {
            Temp.TexCoord = glm::vec2(0.0f);
        }
    }

    std::vector<unsigned int> Indices;
    // Indices
    for (unsigned int i = 0;i < Mesh->mNumFaces; ++i)
    {
        aiFace Face = Mesh->mFaces[i];
        for (unsigned int j = 0;j < Face.mNumIndices; ++j)
        {
            Indices.push_back(Face.mIndices[j]);
        }
    }

    // 材质
    if (Mesh->mMaterialIndex >= 0)
    {
        aiMaterial *Material = Scene->mMaterials[Mesh->mMaterialIndex];
//        std::vector<Texture> diffuseMaps = load
    }
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *Material, aiTextureType Type, std::string TypeName)
{
    std::vector<Texture> Textures;
    for (unsigned int i = 0;i < Material->GetTextureCount(Type); ++i)
    {
        aiString str;
        Material->GetTexture(Type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < loadedTextures.size(); j++)
        {
            if(std::strcmp(loadedTextures[j].Path.data(), str.C_Str()) == 0)
            {
                Textures.push_back(loadedTextures[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.ID = TextureFromFile(str.C_Str(), this->m_Directory);
            texture.Type = TypeName;
            texture.Path = str.C_Str();
            Textures.push_back(texture);
            loadedTextures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
}



//
unsigned int Model::TextureFromFile(const char *Path, const std::string &Directory, bool Gamma)
{
    std::string filename = Path;
    filename = Directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << Path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
