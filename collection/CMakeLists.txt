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

add_library(collection
    mx_list.C
    mx_list_iter.C
    mx_nlist.C
    mx_nlist_iter.C)
