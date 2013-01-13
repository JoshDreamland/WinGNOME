/** Copyright (C) 2013 Josh Ventura
 ** This file is part of WinGNOME.
 ** WinGNOME is free software; you are free to use, modify, and redistribute it in accordance
 ** with the terms of the GNU General Public Licensed, as published by the Free Software
 ** Foundation, either version 3 of the license, or (at your option) any later version.
**/

#include "workspace.h"
#include <iostream>
using namespace std;

vector< vector<workspace> > workspaces;
size_t current_workspace_x;
size_t current_workspace_y;

static bool foreachhide(HWND handle, LPARAM lpar)
{
  // Stack space is cheap
  char writehere[1024]; string castr;
  GetClassName(handle,writehere,1024);
  castr=writehere;
  
  if (castr == "Shell_TrayWnd" || castr == "Progman")
    return 1;
  
  GetWindowText(handle,writehere,1024);
  castr = writehere;
  
  if (IsWindowVisible(handle) and !workspace_always_visible(handle)) {
    workspace *rcv = (workspace*)lpar;
    rcv->windows.push_back(handle);
    ShowWindow(handle, SW_HIDE);
  }
  return 1;
}

set<HWND> workspace_persistance_list;
bool workspace_always_visible(HWND hwnd) {
  return workspace_persistance_list.find(hwnd) != workspace_persistance_list.end();
}
void workspace_toggle_pin(HWND hwnd) {
  pair<set<HWND>::iterator, bool> ins = workspace_persistance_list.insert(hwnd);
  if (!ins.second) workspace_persistance_list.erase(hwnd);
}

void workspace::switch_to() {
  for (vector<HWND>::iterator it = windows.begin(); it != windows.end(); ++it)
    ShowWindow(*it, SW_SHOW);
  windows.clear();
}
void workspace::switch_from() {
  switch_to();
  EnumWindows((WNDENUMPROC)foreachhide, (LPARAM)this);
}
workspace::~workspace() {
  for (vector<HWND>::iterator it = windows.begin(); it != windows.end(); ++it)
    ShowWindow(*it, SW_SHOW);
}

struct Keep {
  pair<set<HWND>::iterator, bool> insert;
  Keep(HWND x): insert(workspace_persistance_list.insert(x)) {}
  ~Keep();
};
Keep::~Keep() { if (insert.second) workspace_persistance_list.erase(insert.first); }

void workspace_hop_up(HWND keep) {
  Keep handle(keep);
  cout << "Hop workspace up" << endl;
  if (current_workspace_y > 0) {
    workspaces[current_workspace_x][current_workspace_y].switch_from();
    workspaces[current_workspace_x][--current_workspace_y].switch_to();
  } else cout << "No such workspace" << endl;
}
void workspace_hop_down(HWND keep) {
  Keep handle(keep);
  cout << "Hop workspace down" << endl;
  if (current_workspace_y + 1 < workspaces[current_workspace_x].size()) {
    workspaces[current_workspace_x][current_workspace_y].switch_from();
    workspaces[current_workspace_x][++current_workspace_y].switch_to();
  } else cout << "No such workspace" << endl;
}
void workspace_hop_left(HWND keep) {
  Keep handle(keep);
  cout << "Hop workspace left" << endl;
  if (current_workspace_x > 0) {
    workspaces[current_workspace_x][current_workspace_y].switch_from();
    workspaces[--current_workspace_x][current_workspace_y].switch_to();
  } else cout << "No such workspace" << endl;
}
void workspace_hop_right(HWND keep) {
  Keep handle(keep);
  cout << "Hop workspace right" << endl;
  if (current_workspace_x + 1 < workspaces.size()) {
    workspaces[current_workspace_x][current_workspace_y].switch_from();
    workspaces[++current_workspace_x][current_workspace_y].switch_to();
  } else cout << "No such workspace" << endl;
}

void workspace_init() {
  workspaces.resize(2);
  workspaces[0].resize(2);
  workspaces[1].resize(2);
  current_workspace_x = 0;
  current_workspace_y = 0;
}
