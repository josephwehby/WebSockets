cmake_minimum_required(VERSION 3.21.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(websocket_client)

set(CMAKE_TOOLCHAIN_FILE "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_PREFIX_PATH "/opt/vcpkg/installed/x64-linux" CACHE STRING "")

find_package(Boost COMPONENTS asio beast REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(SRC main.cpp WebSocket.cpp)
set(HEADERS WebSocket.hpp root_certificates.hpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
add_executable(websocket_client ${SRC} ${HEADERS})

target_link_libraries(websocket_client PRIVATE Boost::asio Boost::beast OpenSSL::SSL OpenSSL::Crypto nlohmann_json::nlohmann_json)
