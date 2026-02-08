#include <stdio.h>
#include <filesystem>

#include <mx_xpm.h>

#include <mx_about_maxwell.xpm>
#include <mx_background.xpm>
#include <mx_back_ins.xpm>
#include <mx_back.xpm>
#include <mx_black.xpm>
#include <mx_blue.xpm>
#include <mx_bold_ins.xpm>
#include <mx_bold_lg_ins.xpm>
#include <mx_bold_lg.xpm>
#include <mx_bold.xpm>
#include <mx_centre_justify_lg.xpm>
#include <mx_centre_justify.xpm>
#include <mx_copy_ins.xpm>
#include <mx_copy.xpm>
#include <mx_create.xpm>
#include <mx_cut_ins.xpm>
#include <mx_cut.xpm>
#include <mx_dashed.xpm>
#include <mx_dd_black.xpm>
#include <mx_dd_blue.xpm>
#include <mx_dd_green.xpm>
#include <mx_dd_indigo.xpm>
#include <mx_dd_ltblue.xpm>
#include <mx_dd_ltgreen.xpm>
#include <mx_dd_ltindigo.xpm>
#include <mx_dd_ltorange.xpm>
#include <mx_dd_ltred.xpm>
#include <mx_dd_ltviolet.xpm>
#include <mx_dd_ltyellow.xpm>
#include <mx_dd_orange.xpm>
#include <mx_dd_red.xpm>
#include <mx_dd_transparent.xpm>
#include <mx_dd_violet.xpm>
#include <mx_dd_white.xpm>
#include <mx_dd_yellow.xpm>
#include <mx_dotted.xpm>
#include <mx_envelope.xpm>
#include <mx_error.xpm>
#include <mx_find_dummy.xpm>
#include <mx_find.xpm>
#include <mx_footer.xpm>
#include <mx_format_character.xpm>
#include <mx_format_columns_ins.xpm>
#include <mx_format_columns.xpm>
#include <mx_format_paragraph.xpm>
#include <mx_format_table_ins.xpm>
#include <mx_format_table.xpm>
#include <mx_forward_ins.xpm>
#include <mx_forward.xpm>
#include <mx_goto_ins.xpm>
#include <mx_goto.xpm>
#include <mx_green.xpm>
#include <mx_header.xpm>
#include <mx_help_icon.xpm>
#include <mx_help.xpm>
#include <mx_icon.xpm>
#include <mx_indigo.xpm>
#include <mx_inform.xpm>
#include <mx_insert_bullets.xpm>
#include <mx_insert_diagram.xpm>
#include <mx_insert_image.xpm>
#include <mx_insert_page_break_ins.xpm>
#include <mx_insert_page_break.xpm>
#include <mx_insert_symbol.xpm>
#include <mx_insert_table_ins.xpm>
#include <mx_insert_table.xpm>
#include <mx_italic_ins.xpm>
#include <mx_italic_lg_ins.xpm>
#include <mx_italic_lg.xpm>
#include <mx_italic.xpm>
#include <mx_justify_lg.xpm>
#include <mx_justify.xpm>
#include <mx_left_justify_lg.xpm>
#include <mx_left_justify.xpm>
#include <mx_ltblue.xpm>
#include <mx_ltgreen.xpm>
#include <mx_ltindigo.xpm>
#include <mx_ltorange.xpm>
#include <mx_ltred.xpm>
#include <mx_ltviolet.xpm>
#include <mx_ltyellow.xpm>
#include <mx_maxwell_bg.xpm>
#include <mx_maxwell.xpm>
#include <mx_open.xpm>
#include <mx_orange.xpm>
#include <mx_paste_ins.xpm>
#include <mx_paste.xpm>
#include <mx_print.xpm>
#include <mx_question.xpm>
#include <mx_quit.xpm>
#include <mx_red.xpm>
#include <mx_replace.xpm>
#include <mx_right_justify_lg.xpm>
#include <mx_right_justify.xpm>
#include <mx_save_ins.xpm>
#include <mx_save.xpm>
#include <mx_solid.xpm>
#include <mx_spelling.xpm>
#include <mx_subscript.xpm>
#include <mx_superscript.xpm>
#include <mx_transparant.xpm>
#include <mx_underline_ins.xpm>
#include <mx_underline_lg.xpm>
#include <mx_underline.xpm>
#include <mx_violet.xpm>
#include <mx_warn.xpm>
#include <mx_white.xpm>
#include <mx_yellow.xpm>

void mx_xpm_write_bitmap(const std::string &dir_name, const std::string &xpm_name, const char *xpm_data[])
{
    std::string file_name = dir_name + "/" + xpm_name;
    file_name[file_name.size() - 4] = '.';

    if (!std::filesystem::exists(file_name)) {
        FILE *f = fopen(file_name.c_str(), "w");
        fprintf(f, "/* XPM */\n");    
        fprintf(f, "static const char *%s[] = {\n", xpm_name.c_str());

        int w, h, num_colours;
        sscanf(xpm_data[0], "%d%d%d", &w, &h, &num_colours);

        for (int i = 0; i < h + num_colours + 1; i++)
        {
            fprintf(f, "\"%s\",\n", xpm_data[i]);
        }

        fprintf(f, "};\n");    
        fclose(f);
    }
}

void mx_xpm_write_bitmaps_if_needed(const std::string &dir)
{
    mx_xpm_write_bitmap(dir, "mx_about_maxwell_xpm", mx_about_maxwell_xpm);
    mx_xpm_write_bitmap(dir, "mx_background_xpm", mx_background_xpm);
    mx_xpm_write_bitmap(dir, "mx_back_ins_xpm", mx_back_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_back_xpm", mx_back_xpm);
    mx_xpm_write_bitmap(dir, "mx_black_xpm", mx_black_xpm);
    mx_xpm_write_bitmap(dir, "mx_blue_xpm", mx_blue_xpm);
    mx_xpm_write_bitmap(dir, "mx_bold_ins_xpm", mx_bold_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_bold_lg_ins_xpm", mx_bold_lg_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_bold_lg_xpm", mx_bold_lg_xpm);
    mx_xpm_write_bitmap(dir, "mx_bold_xpm", mx_bold_xpm);
    mx_xpm_write_bitmap(dir, "mx_centre_justify_lg_xpm", mx_centre_justify_lg_xpm);
    mx_xpm_write_bitmap(dir, "mx_centre_justify_xpm", mx_centre_justify_xpm);
    mx_xpm_write_bitmap(dir, "mx_copy_ins_xpm", mx_copy_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_copy_xpm", mx_copy_xpm);
    mx_xpm_write_bitmap(dir, "mx_create_xpm", mx_create_xpm);
    mx_xpm_write_bitmap(dir, "mx_cut_ins_xpm", mx_cut_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_cut_xpm", mx_cut_xpm);
    mx_xpm_write_bitmap(dir, "mx_dashed_xpm", mx_dashed_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_black_xpm", mx_dd_black_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_blue_xpm", mx_dd_blue_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_green_xpm", mx_dd_green_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_indigo_xpm", mx_dd_indigo_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_ltblue_xpm", mx_dd_ltblue_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_ltgreen_xpm", mx_dd_ltgreen_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_ltindigo_xpm", mx_dd_ltindigo_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_ltorange_xpm", mx_dd_ltorange_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_ltred_xpm", mx_dd_ltred_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_ltviolet_xpm", mx_dd_ltviolet_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_ltyellow_xpm", mx_dd_ltyellow_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_orange_xpm", mx_dd_orange_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_red_xpm", mx_dd_red_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_transparent_xpm", mx_dd_transparent_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_violet_xpm", mx_dd_violet_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_white_xpm", mx_dd_white_xpm);
    mx_xpm_write_bitmap(dir, "mx_dd_yellow_xpm", mx_dd_yellow_xpm);
    mx_xpm_write_bitmap(dir, "mx_dotted_xpm", mx_dotted_xpm);
    mx_xpm_write_bitmap(dir, "mx_envelope_xpm", mx_envelope_xpm);
    mx_xpm_write_bitmap(dir, "mx_error_xpm", mx_error_xpm);
    mx_xpm_write_bitmap(dir, "mx_find_dummy_xpm", mx_find_dummy_xpm);
    mx_xpm_write_bitmap(dir, "mx_find_xpm", mx_find_xpm);
    mx_xpm_write_bitmap(dir, "mx_footer_xpm", mx_footer_xpm);
    mx_xpm_write_bitmap(dir, "mx_format_character_xpm", mx_format_character_xpm);
    mx_xpm_write_bitmap(dir, "mx_format_columns_ins_xpm", mx_format_columns_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_format_columns_xpm", mx_format_columns_xpm);
    mx_xpm_write_bitmap(dir, "mx_format_paragraph_xpm", mx_format_paragraph_xpm);
    mx_xpm_write_bitmap(dir, "mx_format_table_ins_xpm", mx_format_table_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_format_table_xpm", mx_format_table_xpm);
    mx_xpm_write_bitmap(dir, "mx_forward_ins_xpm", mx_forward_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_forward_xpm", mx_forward_xpm);
    mx_xpm_write_bitmap(dir, "mx_goto_ins_xpm", mx_goto_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_goto_xpm", mx_goto_xpm);
    mx_xpm_write_bitmap(dir, "mx_green_xpm", mx_green_xpm);
    mx_xpm_write_bitmap(dir, "mx_header_xpm", mx_header_xpm);
    mx_xpm_write_bitmap(dir, "mx_help_icon_xpm", mx_help_icon_xpm);
    mx_xpm_write_bitmap(dir, "mx_help_xpm", mx_help_xpm);
    mx_xpm_write_bitmap(dir, "mx_icon_xpm", mx_icon_xpm);
    mx_xpm_write_bitmap(dir, "mx_indigo_xpm", mx_indigo_xpm);
    mx_xpm_write_bitmap(dir, "mx_inform_xpm", mx_inform_xpm);
    mx_xpm_write_bitmap(dir, "mx_insert_bullets_xpm", mx_insert_bullets_xpm);
    mx_xpm_write_bitmap(dir, "mx_insert_diagram_xpm", mx_insert_diagram_xpm);
    mx_xpm_write_bitmap(dir, "mx_insert_image_xpm", mx_insert_image_xpm);
    mx_xpm_write_bitmap(dir, "mx_insert_page_break_ins_xpm", mx_insert_page_break_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_insert_page_break_xpm", mx_insert_page_break_xpm);
    mx_xpm_write_bitmap(dir, "mx_insert_symbol_xpm", mx_insert_symbol_xpm);
    mx_xpm_write_bitmap(dir, "mx_insert_table_ins_xpm", mx_insert_table_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_insert_table_xpm", mx_insert_table_xpm);
    mx_xpm_write_bitmap(dir, "mx_italic_ins_xpm", mx_italic_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_italic_lg_ins_xpm", mx_italic_lg_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_italic_lg_xpm", mx_italic_lg_xpm);
    mx_xpm_write_bitmap(dir, "mx_italic_xpm", mx_italic_xpm);
    mx_xpm_write_bitmap(dir, "mx_justify_lg_xpm", mx_justify_lg_xpm);
    mx_xpm_write_bitmap(dir, "mx_justify_xpm", mx_justify_xpm);
    mx_xpm_write_bitmap(dir, "mx_left_justify_lg_xpm", mx_left_justify_lg_xpm);
    mx_xpm_write_bitmap(dir, "mx_left_justify_xpm", mx_left_justify_xpm);
    mx_xpm_write_bitmap(dir, "mx_ltblue_xpm", mx_ltblue_xpm);
    mx_xpm_write_bitmap(dir, "mx_ltgreen_xpm", mx_ltgreen_xpm);
    mx_xpm_write_bitmap(dir, "mx_ltindigo_xpm", mx_ltindigo_xpm);
    mx_xpm_write_bitmap(dir, "mx_ltorange_xpm", mx_ltorange_xpm);
    mx_xpm_write_bitmap(dir, "mx_ltred_xpm", mx_ltred_xpm);
    mx_xpm_write_bitmap(dir, "mx_ltviolet_xpm", mx_ltviolet_xpm);
    mx_xpm_write_bitmap(dir, "mx_ltyellow_xpm", mx_ltyellow_xpm);
    mx_xpm_write_bitmap(dir, "mx_maxwell_bg_xpm", mx_maxwell_bg_xpm);
    mx_xpm_write_bitmap(dir, "mx_maxwell_xpm", mx_maxwell_xpm);
    mx_xpm_write_bitmap(dir, "mx_open_xpm", mx_open_xpm);
    mx_xpm_write_bitmap(dir, "mx_orange_xpm", mx_orange_xpm);
    mx_xpm_write_bitmap(dir, "mx_paste_ins_xpm", mx_paste_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_paste_xpm", mx_paste_xpm);
    mx_xpm_write_bitmap(dir, "mx_print_xpm", mx_print_xpm);
    mx_xpm_write_bitmap(dir, "mx_question_xpm", mx_question_xpm);
    mx_xpm_write_bitmap(dir, "mx_quit_xpm", mx_quit_xpm);
    mx_xpm_write_bitmap(dir, "mx_red_xpm", mx_red_xpm);
    mx_xpm_write_bitmap(dir, "mx_replace_xpm", mx_replace_xpm);
    mx_xpm_write_bitmap(dir, "mx_right_justify_lg_xpm", mx_right_justify_lg_xpm);
    mx_xpm_write_bitmap(dir, "mx_right_justify_xpm", mx_right_justify_xpm);
    mx_xpm_write_bitmap(dir, "mx_save_ins_xpm", mx_save_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_save_xpm", mx_save_xpm);
    mx_xpm_write_bitmap(dir, "mx_solid_xpm", mx_solid_xpm);
    mx_xpm_write_bitmap(dir, "mx_spelling_xpm", mx_spelling_xpm);
    mx_xpm_write_bitmap(dir, "mx_subscript_xpm", mx_subscript_xpm);
    mx_xpm_write_bitmap(dir, "mx_superscript_xpm", mx_superscript_xpm);
    mx_xpm_write_bitmap(dir, "mx_transparant_xpm", mx_transparant_xpm);
    mx_xpm_write_bitmap(dir, "mx_underline_ins_xpm", mx_underline_ins_xpm);
    mx_xpm_write_bitmap(dir, "mx_underline_lg_xpm", mx_underline_lg_xpm);
    mx_xpm_write_bitmap(dir, "mx_underline_xpm", mx_underline_xpm);
    mx_xpm_write_bitmap(dir, "mx_violet_xpm", mx_violet_xpm);
    mx_xpm_write_bitmap(dir, "mx_warn_xpm", mx_warn_xpm);
    mx_xpm_write_bitmap(dir, "mx_white_xpm", mx_white_xpm);
    mx_xpm_write_bitmap(dir, "mx_yellow_xpm", mx_yellow_xpm);
}
