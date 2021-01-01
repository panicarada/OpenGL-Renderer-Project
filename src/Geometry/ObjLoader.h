//
// Created by 邱泽鸿 on 2021/1/1.
//

#pragma once

#include "Basic.h"
#include "Geometry.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>



class ObjLoader : public Geometry
{
public:
    ObjLoader(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<Shader>& Shader, const glm::vec3 &Position = glm::vec3(0.0f, 0.0f, 0.0f),
           const Material& material = {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 1.0f},
           const Rotation& rotation = {0.0f, 0.0f, 0.0f},
           const Scale& Scale = {1.0f, 1.0f, 1.0f, 1.0f});

    // hasTexture：有些obj文件没有材质坐标
    // isTriangle：表明每一行的face是不是一个triangle（或者是quad）
    void loadOBJ(const std::string& FileName, bool isTriangle = true);
    void updateDrawData() override {} // 更新用于绘制的数据
    inline std::string getClassName() override
    {
        return "Geometry::ObjLoader";
    }
};
