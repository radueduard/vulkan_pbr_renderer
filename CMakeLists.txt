cmake_minimum_required(VERSION 3.5)
project(VulkanEngine)

# Find and include the required packages
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(ms-gltf REQUIRED)

# Add the source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")

# Add the executable
add_executable(VulkanEngine ${SOURCES} ${HEADERS})

# Link the required libraries
target_link_libraries(VulkanEngine PRIVATE Vulkan::Vulkan)
target_link_libraries(VulkanEngine PRIVATE glfw)
target_link_libraries(VulkanEngine PRIVATE glm)
target_link_libraries(VulkanEngine PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(VulkanEngine PRIVATE ms-gltf::ms-gltf)
