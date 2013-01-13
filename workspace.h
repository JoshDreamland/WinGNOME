/** Copyright (C) 2013 Josh Ventura
 ** This file is part of WinGNOME.
 ** WinGNOME is free software; you are free to use, modify, and redistribute it in accordance
 ** with the terms of the GNU General Public Licensed, as published by the Free Software
 ** Foundation, either version 3 of the license, or (at your option) any later version.
**/

#ifndef _WORKSPACE__H
#define _WORKSPACE__H

#include <windows.h>

#include <string>
#include <vector>
#include <set>
using std::string;
using std::vector;
using std::set;

struct workspace {
  string name;
  vector<HWND> windows;
  void switch_to();
  void switch_from();
  ~workspace();
};

extern vector< vector<workspace> > workspaces;
extern size_t current_workspace_x;
extern size_t current_workspace_y;

extern set<HWND> workspace_persistance_list;
bool workspace_always_visible(HWND hwnd);
void workspace_toggle_pin(HWND hwnd);

void workspace_hop_up(HWND keep = NULL);
void workspace_hop_down(HWND keep = NULL);
void workspace_hop_left(HWND keep = NULL);
void workspace_hop_right(HWND keep = NULL);

void workspace_init();

#endif
