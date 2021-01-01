//
// Created by 邱泽鸿 on 2021/1/1.
//

#include "Basic.h"





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

        for (auto Pair : AliasMap)
        { // 遍历每个文件
            std::ifstream File(Pair.second); // 打开shader文件
            std::string Line = "";

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
    }
    int getConstant(const std::string& AliasFileName, const std::string&& ConstantName)
    {
        return ConstantMap[AliasFileName][ConstantName];
    }
}
