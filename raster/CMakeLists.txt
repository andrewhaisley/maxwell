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

add_library(raster
    mxraster.C
    rascache.C
    rasdib.C
    rasjpeg.C
    raspng.C
    raster.C
    rasterComp.C
    rasterData.C
    rastiff.C
    rasxbm.C
    traster.C)
