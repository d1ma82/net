cmake_minimum_required(VERSION 3.22)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target x86_64-w64-gnu")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target x86_64-w64-gnu")

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL") #"MultiThreadedDLL$<$<CONFIG:Debug>:Debug>"
cmake_path(CONVERT $ENV{COMPILER} TO_CMAKE_PATH_LIST COMPILER)
message(STATUS "Compiler: ${COMPILER}")
set(CMAKE_CXX_COMPILER "${COMPILER}/clang++.exe")
set(CMAKE_C_COMPILER "${COMPILER}/clang.exe")
set(CMAKE_RC_COMPILER "${COMPILER}/llvm-rc.exe")
set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH "../")

cmake_path(CONVERT $ENV{OpenCV} TO_CMAKE_PATH_LIST OpenCV_DIR)
set(OpenCV_DIR "${OpenCV_DIR}") # OpenCV_DIR "${OpenCV_DIR}
message(STATUS "OpenCV: ${OpenCV_DIR}")

project (Net)

add_definitions(-DLOGGING)
add_definitions(-DTEST)
add_definitions(-DWRITEIMG)
#add_definitions(-std=c++17)
add_definitions(-std=c++2a)

file(GLOB SRC "./src/*.cpp")
add_executable (${PROJECT_NAME} ${SRC})

find_package(OpenCV REQUIRED core imgcodecs highgui)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")
message(STATUS "OpenCV include: ${OpenCV_INCLUDE_DIRS}")
target_include_directories(${PROJECT_NAME} PRIVATE 
"./include"
"${OpenCV_INCLUDE_DIRS}")
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})