cmake_minimum_required(VERSION 3.16)
project(CppWeatherForecast LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CURL REQUIRED)

add_executable(weather_app main.cpp)
target_link_libraries(weather_app PRIVATE CURL::libcurl)
