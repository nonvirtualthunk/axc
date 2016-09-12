cmake_policy(SET CMP0011 NEW)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire)

cmake_minimum_required(VERSION 3.5)
project(test2)

add_subdirectory(external/catch)
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})
enable_testing(true)  # Enables unit-testing.

set(Boost_USE_STATIC_LIBS   ON)
set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/../boost)

find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
include_directories(external/glfw/include ${Boost_INCLUDE_DIRS})

set(GLAD "external/glad/glad.h"
        "external/glad/glad.c")

include_directories("external")
include_directories("src")
include_directories("headers")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-all_load" )

set(BSTRLIB_FILES external/bstrlib/bstraux.c external/bstrlib/bstrlib.c external/bstrlib/bstrwrap.cpp external/bstrlib/buniutil.c external/bstrlib/utf8util.c)

set(SOURCE_FILES
        ${BSTRLIB_FILES}
        src/UnitOfMeasure.cpp src/containers/Seq.h src/application/Application.cpp src/application/Application.h src/Combinators.h src/Event.cpp src/Event.h src/ArxEnum.cpp src/ArxEnum.h src/Optional.h src/core/Predef.h src/containers/ArxMap.h src/Noto.cpp src/Noto.h src/core/Predef.cpp src/GUIEvents.cpp src/GUIEvents.h src/engine/Engine.cpp src/engine/Engine.h src/core/Reflect.h src/core/AuxData.h src/engine/World.cpp src/engine/World.h src/application/GameApplication.cpp src/application/GameApplication.h src/containers/Talea.h src/containers/TaleaGrid.h src/core/VoxelCoord.h src/graphics/VBO.h src/graphics/AxGL.h src/graphics/AxGL.cpp src/graphics/VAO.h src/graphics/VAO.cpp src/containers/ArxSet.h src/containers/DelegatingHasher.h src/graphics/Image.cpp src/graphics/Image.cpp src/graphics/Image.h
        external/rbp/MaxRectsBinPack.cpp external/rbp/Rect.cpp src/graphics/TextureBlock.cpp src/graphics/TextureBlock.h src/math/Shapes.h src/testbeds/Testbed.h src/graphics/StandardAttributeProfiles.h src/graphics/StandardAttributeProfiles.cpp src/graphics/Shader.cpp src/graphics/Shader.h src/core/ArxString.cpp src/core/ArxString.h src/application/ResourceManager.cpp src/application/ResourceManager.h src/gui/Widget.cpp src/gui/Widget.h src/gui/BackgroundImageMetrics.h src/graphics/Color.h src/gui/WindowingSystem.cpp src/gui/WindowingSystem.h
        src/testbeds/Testbed.cpp src/testbeds/WindowingSystemTestbed.cpp src/testbeds/WindowingSystemTestbed.h src/testbeds/SimpleTestbed.cpp src/testbeds/SimpleTestbed.h src/graphics/Color.cpp external/stb/stb_truetype.h src/gui/Font.cpp src/gui/Font.h src/core/Metrics.h src/gui/TextWidget.cpp src/gui/TextWidget.h src/core/Moddable.h
        external/msdfgen/core/Bitmap.cpp
        external/msdfgen/core/Contour.cpp
        external/msdfgen/core/EdgeHolder.cpp
        external/msdfgen/core/Shape.cpp
        external/msdfgen/core/SignedDistance.cpp
        external/msdfgen/core/Vector2.cpp
        external/msdfgen/core/edge-coloring.cpp
        external/msdfgen/core/edge-segments.cpp
        external/msdfgen/core/equation-solver.cpp
        external/msdfgen/core/msdfgen.cpp
        external/msdfgen/core/render-sdf.cpp
        external/msdfgen/core/save-bmp.cpp
        external/msdfgen/core/shape-description.cpp src/testbeds/MSDFTestbed.cpp src/testbeds/MSDFTestbed.h src/conf/AxmParser.cpp src/conf/AxmParser.h src/core/ArxFile.cpp src/core/ArxFile.h src/core/Parsing.h src/core/Introspection.cpp src/core/Introspection.h src/core/Format.h)

set(GENERATION_SOURCE_FILES test/core/Introspectable.h src/gui/Font.h)

set(FAKE_TARGET fakeTarget)
add_custom_target(fakeTarget /bin/bash ${CMAKE_SOURCE_DIR}/axcPreprocess.sh ${GENERATION_SOURCE_FILES})

add_library(demo_lib STATIC ${SOURCE_FILES})
add_dependencies(demo_lib catch)
cotire(demo_lib)

add_executable(demo main.cpp ${GLAD})
target_link_libraries(demo demo_lib)
target_link_libraries(demo glfw)
TARGET_LINK_LIBRARIES(demo ${Boost_LIBRARIES} )

add_dependencies(demo_lib ${FAKE_TARGET})



set(TEST_SOURCE_FILES
        test/testMain.cpp
        test/UnitOfMeasureTests.cpp test/CombinatorTests.cpp test/EventTests.cpp test/OptionalTests.cpp test/PredefTests.cpp test/EnumTests.cpp test/ArxMapTests.cpp test/AuxDataTests.cpp test/engine/GameEngineTests.cpp test/containers/TaleaTests.cpp test/graphics/VBOTests.cpp test/graphics/ImageTests.cpp test/graphics/ShaderTests.cpp test/graphics/TextureBlockTests.cpp test/gui/FontTests.cpp test/core/ModdableTests.cpp test/conf/AxmParserTest.cpp test/core/IntrospectionTests.cpp test/core/Introspectable.h test/core/Introspectable.cpp)
add_executable(testing ${TEST_SOURCE_FILES} ${GLAD})
target_link_libraries(testing demo_lib)
target_link_libraries(testing glfw)
TARGET_LINK_LIBRARIES(testing ${Boost_LIBRARIES} )

add_custom_command(TARGET demo_lib PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:demo_lib>/resources)

add_executable(axcPreprocess src/core/AxcMetaGenerator.cpp
        src/core/Introspection.cpp
        src/core/Predef.cpp
        src/core/ArxString.cpp
        src/core/ArxFile.cpp
        src/Noto.cpp
        ${BSTRLIB_FILES})
TARGET_LINK_LIBRARIES(axcPreprocess ${Boost_LIBRARIES} )

add_custom_command(TARGET axcPreprocess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE_DIR:demo_lib>/axcPreprocess ${CMAKE_SOURCE_DIR}/axcMetaGenerator)