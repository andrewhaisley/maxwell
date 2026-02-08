cmake_minimum_required(VERSION 3.5)

include_directories(
    ..
    ../collection  
    ../db  
    ../dgedit  
    ../dgui  
    ../document  
    ../edit  
    ../geometry  
    ../geomstyle  
    ../raster  
    ../rtf  
    ../shared  
    ../ui  
    ../view)

add_library(dgui mx_dg_menubar.C mx_dg_toolbar.C)
