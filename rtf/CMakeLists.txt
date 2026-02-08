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

add_library(rtf
    mx_metafile.C
    mx_mxmetafile.C
    mx_mxmetafile_wr.C
    mx_rtfe_conv.C
    mx_rtfe_lu.C
    mx_rtfe_op.C
    rtf.C
    rtfControl.C
    rtfConvert.C
    rtfOutput.C
    rtfParse.C)
