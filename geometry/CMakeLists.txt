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

add_library(geometry
    gaffine.C
    garc.C
    gbezier.C
    geometry.C
    gpline.C
    gpoint.C
    grectspan.C
    gsnap.C
    gspline.C
    gutil.C)
