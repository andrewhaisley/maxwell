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

add_library(shared
    mx_config.C
    mx_error.C
    mx_error_stack.C
    mx_file_type.C
    mx_language.C
    mx_stdio.C
    mx_string.C
    mx_unit.C)
