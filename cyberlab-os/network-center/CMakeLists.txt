cmake_minimum_required(VERSION 3.16)
project(cyberlab-network-center LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets DBus Network)

set(SOURCES
    main.cpp
    network_manager.cpp
    wifi_scanner.cpp
    wifi_connector.cpp
    network_monitor.cpp
    network_ui.cpp
    network_notifications.cpp
)

set(HEADERS
    network_manager.h
    wifi_scanner.h
    wifi_connector.h
    network_monitor.h
    network_ui.h
    network_notifications.h
)

add_executable(cyberlab-network-center ${SOURCES} ${HEADERS})
target_link_libraries(cyberlab-network-center PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::DBus Qt6::Network)

install(TARGETS cyberlab-network-center DESTINATION /usr/local/bin)
install(FILES cyberlab-network-center.desktop DESTINATION /usr/share/applications)
