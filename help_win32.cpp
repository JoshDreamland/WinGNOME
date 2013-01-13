/** Copyright (C) 2013 Josh Ventura
 ** This file is part of WinGNOME.
 ** WinGNOME is free software; you are free to use, modify, and redistribute it in accordance
 ** with the terms of the GNU General Public Licensed, as published by the Free Software
 ** Foundation, either version 3 of the license, or (at your option) any later version.
**/

#include "help_win32.h"
#include <iostream>
using namespace std;

string get_caption(HWND hwnd) {
  char writehere[1025];
  GetWindowText(hwnd,writehere,1024);
  return writehere;
}
string get_classname(HWND hwnd) {
  char writehere[1025];
  GetClassName(hwnd,writehere,1024);
  return writehere;
}

bool isWindowMaximized(HWND hwnd)
{
  WINDOWPLACEMENT wp;
  wp.length = sizeof(WINDOWPLACEMENT);
  GetWindowPlacement(hwnd, &wp);
  return (wp.showCmd == SW_MAXIMIZE);
}
bool isWindowTopmost(HWND hwnd) {
  return GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;
}

static bool hasTaskbarButton(HWND hwnd) {
  if (!IsWindowVisible(hwnd))
    return false;
  LONG Style = GetWindowLong(hwnd, GWL_STYLE);
  LONG ExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
  if (Style & WS_CHILD) return false;
  if ((ExStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW) return false;
  if ((ExStyle & WS_EX_APPWINDOW) == WS_EX_APPWINDOW) return true;
  if ((Style & WS_POPUP) and GetParent(hwnd)) return false;
  if (Style & WS_OVERLAPPED) return true;
  if ((ExStyle & WS_EX_OVERLAPPEDWINDOW) == WS_EX_OVERLAPPEDWINDOW) return true;
  if (ExStyle & WS_EX_CLIENTEDGE) return false;
  if (ExStyle & WS_EX_DLGMODALFRAME) return false;
  return true;
}

HWND first_window_not(HWND notme) {
  HWND foundhwnd = GetForegroundWindow();
  if (foundhwnd == notme)
    ShowWindow(notme, SW_HIDE),
    foundhwnd = GetForegroundWindow(),
    ShowWindow(notme, SW_SHOW);
  while (foundhwnd) {
    if (foundhwnd != notme and hasTaskbarButton(foundhwnd))
      return foundhwnd;
    foundhwnd = GetNextWindow(foundhwnd, GW_HWNDNEXT);
  }
  return NULL;
}

void minimize(HWND hwnd) {
  ShowWindow(hwnd, SW_MINIMIZE);
}
void toggle_maximization(HWND hwnd) {
  if (isWindowMaximized(hwnd))
    ShowWindow(hwnd, SW_RESTORE);
  else
    ShowWindow(hwnd, SW_MAXIMIZE);
}
void toggle_topmost(HWND hwnd) {
  SetWindowPos(hwnd, isWindowTopmost(hwnd)? HWND_NOTOPMOST:HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOSIZE);
}

HWND get_window_at_not(int x, int y, HWND notme)
{
  HWND foundhwnd = GetTopWindow(NULL);
  while (foundhwnd)
  {
    if (foundhwnd != notme and IsWindowVisible(foundhwnd))
    {
      RECT win;
      GetWindowRect(foundhwnd, &win);
      if (x > win.left and x < win.right)
      if (y > win.top  and y < win.bottom)
        return foundhwnd;
    }
    foundhwnd = GetNextWindow(foundhwnd, GW_HWNDNEXT);
  }
  return NULL;
}
