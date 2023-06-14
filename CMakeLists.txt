cmake_minimum_required (VERSION 3.8)

project (RayTracing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Disable test, examples and documentation for GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


# Add the external library as a subdirectory
add_subdirectory(ext/glm)
add_subdirectory(ext/glfw)
# Set the include directory
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/ext/include)
# Add source files to the project
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")
file(GLOB EXT_SRC "ext/src/*.c*")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${EXT_SRC} ${HEADERS})

# Set compiler warnings to level 4
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# Set optimization to level 2
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT RayTracing)

# Link the executable with the GLM library
target_link_libraries(${PROJECT_NAME} glm glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)