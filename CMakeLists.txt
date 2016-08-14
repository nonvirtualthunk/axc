cmake_policy(SET CMP0011 NEW)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire)

cmake_minimum_required(VERSION 3.5)
project(test2)

add_subdirectory(external/catch)
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})
enable_testing(true)  # Enables unit-testing.

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)

find_package(OpenGL REQUIRED)

set(GLAD "external/glad/glad.h"
        "external/glad/glad.c" src/testbeds/SimpleTestbed.cpp src/testbeds/SimpleTestbed.h src/testbeds/Testbed.cpp)

include_directories("external")
include_directories("src")
include_directories("headers")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        external/bstrlib/bstraux.c external/bstrlib/bstrlib.c external/bstrlib/bstrwrap.cpp external/bstrlib/buniutil.c external/bstrlib/utf8util.c
        src/UnitOfMeasure.cpp src/containers/Seq.h src/application/Application.cpp src/application/Application.h src/Combinators.h src/Event.cpp src/Event.h src/ArxEnum.cpp src/ArxEnum.h src/Optional.h src/core/Predef.h src/containers/ArxMap.h src/Noto.cpp src/Noto.h src/core/Predef.cpp src/GUIEvents.cpp src/GUIEvents.h src/engine/Engine.cpp src/engine/Engine.h src/core/Reflect.h src/core/AuxData.h src/engine/World.cpp src/engine/World.h src/application/GameApplication.cpp src/application/GameApplication.h src/containers/Talea.h src/containers/TaleaGrid.h src/core/VoxelCoord.h src/graphics/VBO.h src/graphics/AxGL.h src/graphics/AxGL.cpp src/graphics/VAO.h src/graphics/VAO.cpp src/containers/ArxSet.h src/containers/DelegatingHasher.h src/graphics/Image.cpp src/graphics/Image.cpp src/graphics/Image.h
        external/rbp/MaxRectsBinPack.cpp external/rbp/Rect.cpp src/graphics/TextureBlock.cpp src/graphics/TextureBlock.h src/math/Shapes.h src/testbeds/Testbed.h src/graphics/StandardAttributeProfiles.h src/graphics/StandardAttributeProfiles.cpp src/graphics/Shader.cpp src/graphics/Shader.h src/core/ArxString.cpp src/core/ArxString.h src/application/ResourceManager.cpp src/application/ResourceManager.h)

add_library(demo_lib STATIC ${SOURCE_FILES})
add_dependencies(demo_lib catch)
add_executable(demo main.cpp ${GLAD})
target_link_libraries(demo demo_lib)
target_link_libraries(demo glfw)

cotire(demo)

set(TEST_SOURCE_FILES
        test/testMain.cpp
        test/UnitOfMeasureTests.cpp test/CombinatorTests.cpp test/EventTests.cpp test/OptionalTests.cpp test/PredefTests.cpp test/EnumTests.cpp test/ArxMapTests.cpp test/AuxDataTests.cpp test/engine/GameEngineTests.cpp test/containers/TaleaTests.cpp test/graphics/VBOTests.cpp test/graphics/ImageTests.cpp test/graphics/ShaderTests.cpp)
add_executable(testing ${TEST_SOURCE_FILES} ${GLAD})
target_link_libraries(testing demo_lib)

add_custom_command(TARGET demo_lib POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:demo_lib>/resources)