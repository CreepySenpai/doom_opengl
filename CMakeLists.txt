cmake_minimum_required(VERSION 3.30.2)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)

project(Doom VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Doom main.cpp)

file(GLOB GLAD PUBLIC src/glad/*.c)
file(GLOB CREEPY PUBLIC src/Creepy/*.cpp)

target_sources(${PROJECT_NAME} PUBLIC ${CREEPY} ${GLAD})

target_include_directories(${PROJECT_NAME} PUBLIC 
                    ${CMAKE_SOURCE_DIR}/include
                )
                
target_link_directories(${PROJECT_NAME} PUBLIC 
                    ${CMAKE_SOURCE_DIR}/lib
                )

target_link_libraries(${PROJECT_NAME} PUBLIC glfw3dll stdc++exp opengl32)

add_compile_definitions(GLFW_INCLUDE_NONE)

add_compile_options(-fuse-ld=lld)