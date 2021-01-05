//
// Created by 邱泽鸿 on 2021/1/1.
//

#include "Basic.h"


#include "stb_image/stb_image.h"
#include "stb_image_write.h"// 写入图片
#include <array>


#include "Geometry.h"
#include "Sphere.h"
#include "ObjLoader.h"
#include "Cylinder.h"
#include "Cube.h"
#include "Camera.h"
#include "Shader.h"



// 常量表，用来管理C++文件和shader文件公用的常量
// 常量只需要定义在shader文件中，通过init()来抓取到C++中
namespace Basic
{
    std::unordered_map<std::string, std::unordered_map<std::string, int>> ConstantMap;
    std::unordered_map<std::string, std::string> AliasMap;


    std::string getFileName(const std::string& AliasFileName)
    {
        return AliasMap[AliasFileName];
    }

    void init()
    {
        AliasMap["Scene"] = "../resource/Scene/Scene.shader";
        AliasMap["Shadow"] = "../resource/Scene/Shadow.shader";

        for (auto& Pair : AliasMap)
        { // 遍历每个文件
            std::ifstream File(Pair.second); // 打开shader文件
            std::string Line;

            while (std::getline(File, Line))
            { // 遍历每一行
                std::stringstream ss(Line);
//                std::cout << Line << std::endl;
                std::vector<std::string> Temp;
                std::string Word;
                ss >> Word;
                if (Word == "const")
                {
                    ss >> Word;
                    std::string Name;
                    ss >> Name; // 获得名字
                    ss >> Word; // 等号
                    int Value;
                    ss >> Value;
                    ConstantMap[Pair.first][Name] = Value;
                    std::cout << "In file '" << Pair.second << "', constant '" << Name << "' = " << Value << std::endl;
                }
            }
            File.close();
        }

        Geometry::ConstructorMap["Sphere"] = [](const std::shared_ptr<Camera>& camera, const std::shared_ptr<Shader>& shader)
                -> std::shared_ptr<Geometry>{
            return std::make_shared<Sphere>(camera, shader);
        };
        Geometry::ConstructorMap["ObjLoader"] = [](const std::shared_ptr<Camera>& camera, const std::shared_ptr<Shader>& shader)
                -> std::shared_ptr<Geometry>{
            return std::make_shared<ObjLoader>(camera, shader);
        };
        Geometry::ConstructorMap["Cylinder"] = [](const std::shared_ptr<Camera>& camera, const std::shared_ptr<Shader>& shader)
                -> std::shared_ptr<Geometry>{
            return std::make_shared<Cylinder>(camera, shader);
        };
        Geometry::ConstructorMap["Cube"] = [](const std::shared_ptr<Camera>& camera, const std::shared_ptr<Shader>& shader)
                -> std::shared_ptr<Geometry>{
            return std::make_shared<Cube>(camera, shader);
        };
    }
    int getConstant(const std::string& AliasFileName, const std::string&& ConstantName)
    {
        return ConstantMap[AliasFileName][ConstantName];
    }

    int exportImage(const std::string& Filename)
    {
        const int ChannelNum = 3; // RGB

        int Viewport[4];
        glGetIntegerv(GL_VIEWPORT, Viewport);

        int x = Viewport[0];
        int y = Viewport[1];
        int Width = Viewport[2];
        int Height = Viewport[3];
        std::vector<unsigned char> Data(Width * Height * ChannelNum);
        // 读取frame buffer像素值
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(x, y, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, &Data[0]);
        // 利用stbi image写入png文件
        std::string Output = "../Export/Screenshots/" + Filename;
        // image的左上角是(0, 0)，而OpenGL左下角是(0, 0)
        // 所以要上下翻转
        stbi_flip_vertically_on_write(1);
        // 写入png
        int res = stbi_write_png(Output.c_str(), Width, Height, ChannelNum, &Data[0], 0);
        // 0表示写入失败，1表示写入成功
        return res;
    }

    void setFileName(const std::string &AliasFileName, const std::string &FileName)
    {
        // 设置文件映射表
        AliasMap[AliasFileName] = FileName;
    }
}
