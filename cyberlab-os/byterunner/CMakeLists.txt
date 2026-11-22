cmake_minimum_required(VERSION 3.16)
project(byterunner LANGUAGES C)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)

add_executable(byterunner main.c)
target_include_directories(byterunner PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(byterunner PRIVATE ${SDL2_LIBRARIES} m)

install(TARGETS byterunner DESTINATION /usr/local/bin)
install(FILES byterunner.desktop DESTINATION /usr/share/applications)
