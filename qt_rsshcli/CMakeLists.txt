cmake_minimum_required(VERSION 3.14)

project(qt_rsshcli LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(qt_rsshcli
../main.cpp
../perulink.cpp
../picojson.cpp
../shsession.cpp
../sshrev.cpp
../sysinfo.cpp
../main.h
../per_id.h
../perulink.h
../picojson.h
../shsession.h
../sshrev.h
../sysinfo.h
)
target_link_libraries(qt_rsshcli Qt${QT_VERSION_MAJOR}::Core  ${CMAKE_SOURCE_DIR}/../lib/lib_secret-Linux-x86_64.a pthread curl ssh2)
