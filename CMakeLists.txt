cmake_minimum_required(VERSION 3.21.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_SDK_VERSION 10.0.26100.0)
add_definitions(-D_WIN32_WINNT=0x0B00)

project(websocket_client)

set(CMAKE_TOOLCHAIN_FILE "D:/VCPKG/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_PREFIX_PATH "D:/VCPKG/vcpkg/installed/x64-windows" CACHE STRING "")

find_package(Boost COMPONENTS asio beast REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB SRC "src/*.cpp")

include_directories(
  "./include"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_executable(websocket_client ${SRC})
target_link_libraries(websocket_client PRIVATE Boost::asio Boost::beast OpenSSL::SSL OpenSSL::Crypto nlohmann_json::nlohmann_json)

if (MSVC) 
  target_compile_options(websocket_client PRIVATE "/bigobj")
endif()
