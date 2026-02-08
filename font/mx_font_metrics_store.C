/*
 *  This file is part of the Maxwell Word Processor application.
 *  Copyright (C) 1996, 1997, 1998 Andrew Haisley, David Miller, Tom Newton
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <X11/Xft/Xft.h>

#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include <filesystem>

#include <mx_font_metrics.h>
#include <mx_font_metrics_store.h>
#include <mx.h>

using namespace std;

// global variable
extern char* global_maxhome;

std::map<std::string, std::shared_ptr<mx_font_family>> mx_font_metrics_store::m_font_families;
std::string mx_font_metrics_store::m_default_roman_font_family_name;
std::string mx_font_metrics_store::m_default_symbol_font_family_name;

FT_Library mx_font_metrics_store::m_ft;

mx_font_metrics_store::mx_font_metrics_store()
{
    int err = MX_ERROR_OK;

    init_xft_fm(err);
    MX_ERROR_CHECK(err);

    return;
abort:
    global_error_trace->print();
    MX_ERROR_CLEAR(err);
    return;
}

mx_font_style_t mx_font_metrics_store::translate_style(int weight, int slant)
{
    int is_bold = (weight >= FC_WEIGHT_BOLD);
    int is_italic = (slant != FC_SLANT_ROMAN);

    if (is_bold && is_italic) return mx_bold_italic;
    else if (is_bold) return mx_bold;
    else if (is_italic) return mx_italic;
    return mx_normal;
}


void mx_font_metrics_store::init_font_config(int& err)
{
    FcInit();

    if (FT_Init_FreeType(&m_ft)) {
        MX_ERROR_THROW(err, MX_FREETYPE_INIT_FAILED);
    }

abort:;
}

void mx_font_metrics_store::load_everything(int &err)
{
    FcCharSet *latinCharset = FcCharSetCreate();
    FcCharSetAddChar(latinCharset, 'A');
    FcCharSetAddChar(latinCharset, 'Z');
    FcCharSetAddChar(latinCharset, 'a');
    FcCharSetAddChar(latinCharset, 'z');

    FcPattern *pat = FcPatternCreate();
    FcPatternAddCharSet(pat, FC_CHARSET, latinCharset);

    FcObjectSet *os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FILE, FC_SCALABLE, FC_SLANT, FC_WEIGHT, FC_CHARSET, NULL);
    FcFontSet *fs = FcFontList(NULL, pat, os);

    for (int i = 0; i < fs->nfont; i++) {
        FcPattern *font = fs->fonts[i];
        FcChar8 *family = nullptr, *style = nullptr, *file = nullptr; 
        int slant = 0, weight = 0;

        if (FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
            FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch &&
            FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
            FcPatternGetInteger(font, FC_SLANT, 0, &slant) == FcResultMatch &&
            FcPatternGetInteger(font, FC_WEIGHT, 0, &weight) == FcResultMatch)
        {

            mx_font_metrics *fm = new mx_font_metrics;

            fm->set_family_name(reinterpret_cast<char *>(family));
            fm->set_file_name(reinterpret_cast<char *>(file));
            fm->set_style(translate_style(weight, slant));

            FT_Face face;

            if (file && FT_New_Face(m_ft, (const char *)file, 0, &face) == 0) {

                std::filesystem::path p((const char *)file);
                std::string metrics_file_name = std::string(global_maxhome) + "/metrics/" + string(p.filename());

                FT_Pos ascender = face->ascender;
                FT_Pos descender = face->descender;
                FT_Pos units_per_em = face->units_per_EM;

                double a = ascender;
                a /= double(units_per_em);
                a *= 1000.0;
                fm->set_em_ascender(a);

                double d = descender;
                d /= double(units_per_em);
                d *= 1000.0;
                fm->set_em_descender(d);

                if (std::filesystem::exists(metrics_file_name)) {
                    fm->read_from_file(metrics_file_name);
                }
                else {
                    FT_ULong charcode;
                    FT_UInt gindex;

                    charcode = FT_Get_First_Char(face, &gindex);

                    while (gindex != 0) {
                        if (FT_Load_Char(face, charcode, FT_LOAD_NO_SCALE) == 0) {
                            FT_Pos width = face->glyph->advance.x;
                            double w = width;
                            w /= units_per_em;
                            w *= 1000.0;
                            fm->set_em_width(charcode, w);
                        }
                        charcode = FT_Get_Next_Char(face, charcode, &gindex);
                    }
                    fm->write_to_file(metrics_file_name);
                }
                FT_Done_Face(face);
            }
            add_fm(fm, reinterpret_cast<char *>(family));
        }
    }

    FcFontSetDestroy(fs);
    FcPatternDestroy(pat);
    FcObjectSetDestroy(os);
}

std::string mx_font_metrics_store::get_default_from_name(int& err, const std::string &name)
{
	FcConfig* config = FcInitLoadConfigAndFonts();

	FcPattern* pat = FcNameParse((const FcChar8*)name.c_str());

	FcConfigSubstitute(config, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);

	FcResult result;

	FcPattern* font = FcFontMatch(config, pat, &result);

	if (font)
	{
		FcChar8* family = NULL; 

        if (FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch)
        {
            std::string res(reinterpret_cast<const char *>(family));
            FcPatternDestroy(font);
            FcPatternDestroy(pat);
            FcConfigDestroy(config);
            return res;
        }
        else
        {   
            MX_ERROR_THROW(err, MX_FREETYPE_INIT_FAILED);
        }
	}
    else
    {
        MX_ERROR_THROW(err, MX_FREETYPE_INIT_FAILED);
    }

	FcPatternDestroy(font);
	FcPatternDestroy(pat);
	FcConfigDestroy(config);

abort:;
    return "invalid";
}

void mx_font_metrics_store::set_default(int& err)
{
    m_default_roman_font_family_name = get_default_from_name(err, "Nimbus Roman");
    MX_ERROR_CHECK(err);

    m_default_symbol_font_family_name = get_default_from_name(err, "Standard Symbols PS");
    MX_ERROR_CHECK(err);
abort:;
}

void mx_font_metrics_store::init_xft_fm(int& err)
{
    init_font_config(err);
    MX_ERROR_CHECK(err);

    load_everything(err);
    MX_ERROR_CHECK(err);

    set_default(err);
    MX_ERROR_CHECK(err);

abort:;
}

mx_font_metrics_store::~mx_font_metrics_store()
{
}

std::shared_ptr<mx_font_family> mx_font_metrics_store::get_font_family(const std::string &font_family_name)
{
    if (m_font_families.find(font_family_name) == m_font_families.end()) {
        return nullptr;
    } else {
        return m_font_families[font_family_name];
    }
}

std::shared_ptr<mx_font_family> mx_font_metrics_store::get_nearest_font_family(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    for (auto i : m_font_families)
    {
        auto s = i.first;

        std::transform(s.begin(), s.end(), s.begin(), ::tolower);

        if (s.find(name) != std::string::npos) {
            return i.second;
        }
    }

    return get_font_family(m_default_roman_font_family_name.c_str());
}

mx_font_metrics* mx_font_metrics_store::get_font_metrics(int& err, const std::string &font_family_name, mx_font_style_t style)
{
    auto family = get_font_family(font_family_name);

    if (family) {
        return family->get_font_metrics(style);
    } else {
        return nullptr;
    }
}

uint32 mx_font_metrics_store::get_num_font_families()
{
    return m_font_families.size();
}

std::vector<std::string> mx_font_metrics_store::get_font_family_names()
{
    std::vector<std::string> res;

    for (auto i : m_font_families) {
        res.push_back(i.first);
    }

    return res;
}

void mx_font_metrics_store::add_fm(mx_font_metrics* fm, const std::string &family_name)
{
    auto ff = get_font_family(family_name);
    
    if (ff) {
        ff->set_font_metrics(fm);
    } else {
        ff = std::make_shared<mx_font_family>();
        ff->set_font_metrics(fm);

        m_font_families[fm->get_family_name()] = ff;
    }
}
