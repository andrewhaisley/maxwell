cmake_minimum_required(VERSION 3.5)

project(MAXWELL VERSION 0.1 DESCRIPTION "Maxwell Word Processor" LANGUAGES CXX)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

add_compile_options(-g -Wall -Ofast -std=c++20)

find_package(X11 REQUIRED)

include_directories(
        "/usr/include/X11"
        "/usr/include/freetype2"
        "."
        ${CMAKE_CURRENT_SOURCE_DIR}/xpm
        ${CMAKE_CURRENT_SOURCE_DIR}/font
        ${CMAKE_CURRENT_SOURCE_DIR}/collection
        ${CMAKE_CURRENT_SOURCE_DIR}/db
        ${CMAKE_CURRENT_SOURCE_DIR}/dgedit
        ${CMAKE_CURRENT_SOURCE_DIR}/dgui
        ${CMAKE_CURRENT_SOURCE_DIR}/document
        ${CMAKE_CURRENT_SOURCE_DIR}/edit
        ${CMAKE_CURRENT_SOURCE_DIR}/geometry
        ${CMAKE_CURRENT_SOURCE_DIR}/geomstyle
        ${CMAKE_CURRENT_SOURCE_DIR}/raster
        ${CMAKE_CURRENT_SOURCE_DIR}/rtf
        ${CMAKE_CURRENT_SOURCE_DIR}/shared
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/view)

link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/xpm
        ${CMAKE_CURRENT_SOURCE_DIR}/font
        ${CMAKE_CURRENT_SOURCE_DIR}/collection
        ${CMAKE_CURRENT_SOURCE_DIR}/db
        ${CMAKE_CURRENT_SOURCE_DIR}/dgedit
        ${CMAKE_CURRENT_SOURCE_DIR}/dgui
        ${CMAKE_CURRENT_SOURCE_DIR}/document
        ${CMAKE_CURRENT_SOURCE_DIR}/edit
        ${CMAKE_CURRENT_SOURCE_DIR}/geometry
        ${CMAKE_CURRENT_SOURCE_DIR}/geomstyle
        ${CMAKE_CURRENT_SOURCE_DIR}/raster
        ${CMAKE_CURRENT_SOURCE_DIR}/rtf
        ${CMAKE_CURRENT_SOURCE_DIR}/shared
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/view)

add_subdirectory(xpm)
add_subdirectory(font)
add_subdirectory(collection)
add_subdirectory(db)
add_subdirectory(dgedit)
add_subdirectory(dgui)
add_subdirectory(document)
add_subdirectory(edit)
add_subdirectory(geometry)
add_subdirectory(geomstyle)
add_subdirectory(raster)
add_subdirectory(rtf)
add_subdirectory(shared)
add_subdirectory(ui)
add_subdirectory(view)

add_executable(maxwell maxwell.C)
target_link_libraries(maxwell collection db dgedit font dgui document edit geometry geomstyle raster rtf font shared ui view collection db dgedit dgui document edit geometry geomstyle raster rtf shared ui view xpm -lX11 -lXm -lXt -lfontconfig -lfreetype -lXft -ljpeg -lz -ltiff -lXpm)
