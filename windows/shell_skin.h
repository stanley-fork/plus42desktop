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

#ifndef SHELL_SKIN_H
#define SHELL_SKIN_H 1

#include <windows.h>

void skin_load(wchar_t *skinname, const wchar_t *basedir, long *width, long *height, int *rows, int *cols, int *flags);

struct SkinColor {
    unsigned char r, g, b, pad;
};

#define IMGTYPE_MONO 1
#define IMGTYPE_GRAY 2
#define IMGTYPE_COLORMAPPED 3
#define IMGTYPE_TRUECOLOR 4

#define KEYMAP_MAX_MACRO_LENGTH 31
struct keymap_entry {
    bool ctrl;
    bool alt;
    bool extended;
    bool shift; 
    bool cshift; 
    int keycode;
    unsigned char macro[KEYMAP_MAX_MACRO_LENGTH + 1];
};

keymap_entry *parse_keymap_entry(char *line, int lineno);

int skin_getchar();
void skin_rewind();
bool skin_init_image(int type, int ncolors, const SkinColor *colors,
                     int width, int height);
void skin_put_pixels(unsigned const char *data);
void skin_finish_image();

void skin_repaint(bool shortcuts);
void skin_invalidate_annunciator(int which);
void skin_find_key(int x, int y, bool cshift, int *skey, int *ckey);
int skin_find_skey(int ckey);
unsigned char *skin_find_macro(int ckey, int *type);
unsigned char *skin_keymap_lookup(int keycode, bool ctrl, bool alt, bool extended, bool shift, bool cshift, bool *exact);
void skin_invalidate_key(int key);
void skin_display_blitter(const char *bits, int bytesperline, int x, int y, int width, int height);
void skin_get_size(int *width, int *height);
void skin_set_window(HWND window);
void skin_set_window_size(int width, int height);
void skin_get_window_size(int *width, int *height);

#endif
