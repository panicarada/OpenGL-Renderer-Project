cmake_minimum_required(VERSION 3.17)
project(CG_project)

set(CMAKE_CXX_STANDARD 17)



set(HEADERS lib/include)
include_directories(${HEADERS})


set(GLFW_LINK /usr/local/Cellar/glfw/3.3.2/lib/libglfw.3.3.dylib)
set(GLEW_LINK /usr/local/Cellar/glew/2.2.0/lib/libGLEW.2.2.0.dylib)

set(ASSIMP_LINK /usr/local/Cellar/libassimp.3.1.1.dylib)


set(FRAMEWORKS_1 /System/Library/Frameworks/Cocoa.framework)
set(FRAMEWORKS_2 /System/Library/Frameworks/OpenGL.framework)
set(FRAMEWORKS_3 /System/Library/Frameworks/GLUT.framework)
set(FRAMEWORKS_4 /System/Library/Frameworks/CoreVideo.framework)
set(FRAMEWORKS_5 /System/Library/Frameworks/IOKit.framework)


set(HEADERS src)
include_directories(${HEADERS})
set(HEADERS src/Render)
include_directories(${HEADERS})
set(HEADERS src/Tests)
include_directories(${HEADERS})
set(HEADERS src/Camera)
include_directories(${HEADERS})
set(HEADERS src/Geometry)
include_directories(${HEADERS})
set(HEADERS src/Scene)
include_directories(${HEADERS})
set(HEADERS lib)
include_directories(${HEADERS})
set(HEADERS lib/imgui)
include_directories(${HEADERS})




link_libraries(${GLFW_LINK} ${FRAMEWORKS_1} ${FRAMEWORKS_2} ${FRAMEWORKS_3} ${FRAMEWORKS_4} ${FRAMEWORKS_5})
link_libraries(${GLEW_LINK} ${FRAMEWORKS_1} ${FRAMEWORKS_2} ${FRAMEWORKS_3} ${FRAMEWORKS_4} ${FRAMEWORKS_5})


find_package(OpenMP REQUIRED)
add_executable(CG_project main.cpp src/Camera/Camera.cpp src/Camera/Camera.h src/Basic.h src/Render/Renderer.h src/Render/Shader.cpp src/Render/Shader.h src/Render/VertexBuffer.h src/Render/VertexBufferLayout.h src/Render/VertexArray.h src/Render/IndexBuffer.h src/Render/TextureArray.h src/Tests/Test.h src/Tests/TestBatchColor.cpp src/Tests/TestBatchColor.h src/Geometry/Geometry.h src/Geometry/Sphere.cpp src/Geometry/Sphere.h src/Tests/TestSphere.cpp src/Tests/TestSphere.h
        lib/imgui/examples/imgui_impl_glfw.cpp lib/imgui/examples/imgui_impl_glfw.h
        lib/imgui/imgui.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_widgets.cpp
        lib/imgui/examples/imgui_impl_opengl3.cpp src/Geometry/Cube.cpp src/Geometry/Cube.h src/Tests/TestCube.cpp src/Tests/TestCube.h src/Geometry/Cylinder.cpp src/Geometry/Cylinder.h src/Tests/TestCylinder.cpp src/Tests/TestCylinder.h src/Tests/TestGeometry.cpp src/Tests/TestGeometry.h src/Scene/Light.h src/Scene/Scene.cpp src/Scene/Scene.h src/Tests/TestDepth.cpp src/Tests/TestDepth.h src/Render/FrameBuffer.h src/Scene/Shadow.cpp src/Scene/Shadow.h
        src/Tests/TestShadow.h src/Tests/TestTexture.cpp src/Tests/TestTexture.h
        lib/include/stb_image.h lib/stb_image/stb_image.cpp src/Render/TextureArray.cpp src/Scene/Light.cpp
        lib/include/stb_image_resize.h lib/stb_image/stb_image_resize.cpp src/Tests/TestShadow.cpp
        src/Tests/TestShadowPCSS.cpp src/Tests/TestShadowPCSS.h
        src/Scene/PoissonMap.cpp src/Scene/PoissonMap.h src/Scene/SoftShadow.cpp
        src/Scene/SoftShadow.h src/Tests/TestShadowVSM.cpp
        src/Tests/TestShadowVSM.h
#        src/Geometry/Model.cpp src/Geometry/Model.h
        src/Geometry/ObjLoader.cpp src/Geometry/ObjLoader.h src/Basic.cpp)
target_link_libraries(CG_project PRIVATE OpenMP::OpenMP_CXX ${ASSIMP_LINK})