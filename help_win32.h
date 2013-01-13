/** Copyright (C) 2013 Josh Ventura
 ** This file is part of WinGNOME.
 ** WinGNOME is free software; you are free to use, modify, and redistribute it in accordance
 ** with the terms of the GNU General Public Licensed, as published by the Free Software
 ** Foundation, either version 3 of the license, or (at your option) any later version.
**/

#ifndef _HELP_WIN32__H
#define _HELP_WIN32__H

#include <windows.h>

#include <string>
using std::string;

string get_caption(HWND hwnd);
string get_classname(HWND hwnd);
bool isWindowMaximized(HWND hwnd);
bool isWindowTopmost(HWND hwnd);
void toggle_maximization(HWND hwnd);
void toggle_topmost(HWND hwnd);
void minimize(HWND hwnd);
HWND first_window_not(HWND notme);
HWND get_window_at_not(int x, int y, HWND notme);

#endif
