/*****************************************************************************
 * Plus42 -- an enhanced HP-42S calculator simulator
 * Copyright (C) 2004-2025  Thomas Okken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#ifndef SHELL_MAIN_H
#define SHELL_MAIN_H 1

#include <string>
#include "stdafx.h"
#include "resource.h"

#include "shell_skin.h"

struct ci_char_traits : public std::char_traits<wchar_t> {
    static bool eq(wchar_t c1, wchar_t c2) { return towupper(c1) == towupper(c2); }
    static bool ne(wchar_t c1, wchar_t c2) { return towupper(c1) != towupper(c2); }
    static bool lt(wchar_t c1, wchar_t c2) { return towupper(c1) < towupper(c2); }
    static int compare(const wchar_t *s1, const wchar_t *s2, size_t n) {
        while (n-- != 0) {
            if (towupper(*s1) < towupper(*s2)) return -1;
            if (towupper(*s1) > towupper(*s2)) return 1;
            ++s1; ++s2;
        }
        return 0;
    }
    static const wchar_t *find(const wchar_t *s, int n, wchar_t a) {
        while (n-- > 0 && towupper(*s) != towupper(a)) {
            ++s;
        }
        return s;
    }
};

typedef std::basic_string<wchar_t, ci_char_traits> ci_string;

#define FILENAMELEN 256

struct state_type {
    BOOL extras;
    WINDOWPLACEMENT mainPlacement;
    int mainPlacementValid;
    WINDOWPLACEMENT printOutPlacement;
    int printOutPlacementValid;
    int printOutOpen;
    int printerToTxtFile;
    int printerToGifFile;
    wchar_t printerTxtFileName[FILENAMELEN];
    wchar_t printerGifFileName[FILENAMELEN];
    int printerGifMaxLength;
    wchar_t skinName[FILENAMELEN];
    BOOL alwaysOnTop;
    BOOL singleInstance;
    // formerly 'calculator key'; removed because of permissions
    BOOL dummy1;
    wchar_t coreName[FILENAMELEN];
    bool matrix_singularmatrix;
    bool matrix_outofrange;
    bool auto_repeat;
    bool localized_copy_paste;
    int mainWindowWidth, mainWindowHeight;
};

extern state_type state;
extern int skin_mode;
extern int disp_rows, disp_cols;

extern HINSTANCE hInst;                                    // current instance
extern wchar_t free42dirname[FILENAMELEN];

void update_skin(int rows, int cols);
ci_string GetDlgItemTextLong(HWND hWnd, int item);
int browse_file(HWND owner, wchar_t *title, int save, wchar_t *filter, wchar_t *defExt, char *buf, int buflen);
int browse_file_w(HWND owner, wchar_t *title, int save, wchar_t *filter, wchar_t *defExt, wchar_t *buf, int buflen);
char *wide2utf(const wchar_t *w);
ci_string to_ci_string(int i);

extern int ckey;
extern int skey;

extern int ann_updown;
extern int ann_shift;
extern int ann_print;
extern int ann_run;
extern int ann_battery;
extern int ann_g;
extern int ann_rad;

void get_keymap(keymap_entry **map, int *length);

#endif
