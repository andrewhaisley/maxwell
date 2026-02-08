cmake_minimum_required(VERSION 3.5)

include_directories(
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

add_library(geomstyle
    mx_astyle.C
    mx_geoms.C
    mx_sdarc.C
    mx_sdpoly.C
    mx_sdrsp.C
    mx_sstyle.C
    mx_stydev.C
    mx_styles.C)
