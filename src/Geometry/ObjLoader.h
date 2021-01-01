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
    void loadOBJ(const std::string& FileName);
    void updateDrawData() override {} // 更新用于绘制的数据
    inline std::string getClassName() override
    {
        return "Geometry::ObjLoader";
    }

private:
    inline static void triangulate(std::vector<unsigned int>& Points, std::vector<unsigned int>& OutputIndices)
    { // 把Points依次连线围成的多边形三角化，索引加到OutputIndices
        if (Points.size() == 3)
        { // Basic Case:
            OutputIndices.push_back(Points[0]);
            OutputIndices.push_back(Points[1]);
            OutputIndices.push_back(Points[2]);
        }
        else
        { // 取出一个三角形
            OutputIndices.push_back(Points[0]);
            OutputIndices.push_back(Points[1]);
            OutputIndices.push_back(Points[2]);

            // 第二个点所关联的三角形被载入
            Points.erase(Points.begin() + 1);

            // 递归调用
            triangulate(Points, OutputIndices);
        }
    }
};
