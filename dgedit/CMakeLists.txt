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

add_library(dgedit
    mx_dg_arc.C
    mx_dg_editor.C
    mx_dg_farc.C
    mx_dg_fcirc.C
    mx_dg_fell.C
    mx_dg_grid.C
    mx_dg_parc.C
    mx_dg_pcirc.C
    mx_dg_pell.C
    mx_dg_pline.C
    mx_dg_point.C
    mx_dg_rect.C
    mx_dg_select.C
    mx_dg_snap.C
    mx_dg_span.C
    mx_dg_state.C
    mx_dg_top.C
    mx_dg_undo.C)
