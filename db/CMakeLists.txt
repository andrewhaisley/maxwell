cmake_minimum_required(VERSION 3.5)

include_directories(
    ../collection  
    ../db  
    ../dgedit  
    ../dgui  
    ../document  
    ../edit  
    ../font
    ../geometry  
    ../geomstyle  
    ../raster  
    ../rtf  
    ../shared  
    ../ui  
    ../view)

add_library(db
    mx_attribute.C
    mx_db_cc.C
    mx_db_client.C
    mx_db_object.C
    mx_doc_file.C
    mx_file.C
    mx_sl_obj.C)
