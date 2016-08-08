cmake_policy(SET CMP0011 NEW)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire)

cmake_minimum_required(VERSION 3.5)
project(test2)

add_subdirectory(external/catch)
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})
enable_testing(true)  # Enables unit-testing.

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

set(GLAD "external/glad/glad.h"
        "external/glad/glad.c")

include_directories("external")
include_directories("src")
include_directories("headers")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/UnitOfMeasure.cpp src/containers/Seq.h src/application/Application.cpp src/application/Application.h src/Combinators.h src/Event.cpp src/Event.h src/ArxEnum.cpp src/ArxEnum.h src/Optional.h src/core/Predef.h src/containers/ArxMap.h src/Noto.cpp src/Noto.h src/core/Predef.cpp src/GUIEvents.cpp src/GUIEvents.h src/engine/Engine.cpp src/engine/Engine.h src/core/Reflect.h src/core/AuxData.h src/engine/World.cpp src/engine/World.h src/application/GameApplication.cpp src/application/GameApplication.h)

add_library(demo_lib STATIC ${SOURCE_FILES})
add_executable(demo main.cpp ${GLAD})
target_link_libraries(demo demo_lib)
target_link_libraries(demo glfw)

cotire(demo)

set(TEST_SOURCE_FILES
        test/testMain.cpp
        test/UnitOfMeasureTests.cpp test/CombinatorTests.cpp test/EventTests.cpp test/OptionalTests.cpp test/PredefTests.cpp test/EnumTests.cpp test/ArxMapTests.cpp test/AuxDataTests.cpp test/engine/GameEngineTests.cpp)
add_executable(testing ${TEST_SOURCE_FILES})
target_link_libraries(testing demo_lib)
