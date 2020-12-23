// 放一些头文件，共有基本结构等

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <omp.h> // Open MP




// shader代码中材质数组的大小
#define SLOT_NUM 15

// 电脑核数
#define CORE_NUM 8


// GUI库
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"

/* 输出调试信息 */
#define ASSERT(x) if (!(x)) __builtin_debugtrap();
#define DebugCall(x) ClearError();\
    x;\
    ASSERT(LogCall(#x, __FILE_NAME__, __LINE__))

static void ClearError()
{
    while (glGetError() != GL_NO_ERROR);
}
static bool LogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" <<
                  function << "  " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

static unsigned int getSizeofType(unsigned int Type)
{ // 获取GL数据类型的字节长度
    switch (Type)
    {
        case GL_FLOAT:          return sizeof(GLfloat);
        case GL_UNSIGNED_INT:   return sizeof(GLuint);
        case GL_UNSIGNED_BYTE:  return sizeof(GL_BYTE);
    }
    ASSERT(false);
    return 0;
}

struct Scale
{
    float w; // 圆台上部分的拉伸
    // 三个方向的拉伸，范围是[0, 1.0f]
    float x;
    float y;
    float z;
};

struct Rotation
{ // 物体的旋转
    float Pitch;// 俯仰角
    float Yaw; // 偏航角
    float Roll; // 滚转角
};

// 物体的材质
struct Material
{
    glm::vec4 Ambient; // 环境光照部分
    glm::vec4 Diffuse; // 漫射光部分
    glm::vec4 Specular; // 镜面光部分
    float Highlight; // 亮度
};



/* ImGui的扩展 */
namespace ImGui
{ // 重写ImGui一些Api，方便动态处理
    static auto vector_getter = [](void* vec, int idx, const char** out_text)
    {
        auto& vector = *static_cast<std::vector<std::string>*>(vec);
        if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
        *out_text = vector.at(idx).c_str();
        return true;
    };

    static bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
    {
        if (values.empty()) { return false; }
        return Combo(label, currIndex, vector_getter,
                     static_cast<void*>(&values), values.size());
    }

    static bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
    {
        if (values.empty()) { return false; }
        return ListBox(label, currIndex, vector_getter,
                       static_cast<void*>(&values), values.size());
    }
}


namespace ImGui
{ // ImGui::InputText() with std::string

    struct InputTextCallback_UserData
    {
        std::string*            Str;
        ImGuiInputTextCallback  ChainCallback;
        void*                   ChainCallbackUserData;
    };

    static int InputTextCallback(ImGuiInputTextCallbackData* data)
    {
        InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            // Resize string callback
            // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
            std::string* str = user_data->Str;
            IM_ASSERT(data->Buf == str->c_str());
            str->resize(data->BufTextLen);
            data->Buf = (char*)str->c_str();
        }
        else if (user_data->ChainCallback)
        {
            // Forward to user callback, if any
            data->UserData = user_data->ChainCallbackUserData;
            return user_data->ChainCallback(data);
        }
        return 0;
    }

    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    static IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
    {
        IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
        flags |= ImGuiInputTextFlags_CallbackResize;

        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str = str;
        cb_user_data.ChainCallback = callback;
        cb_user_data.ChainCallbackUserData = user_data;
        return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
    }
}