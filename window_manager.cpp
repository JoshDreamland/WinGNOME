/** Copyright (C) 2011 Josh Ventura
 ** This file is part of WinGNOME.
 ** WinGNOME is free software; you are free to use, modify, and redistribute it in accordance
 ** with the terms of the GNU General Public Licensed, as published by the Free Software
 ** Foundation, either version 3 of the license, or (at your option) any later version.
**/

#define _WIN32_WINNT 9001
#include <windows.h>
#include <iostream>
#include <cmath>
using namespace std;

#include "workspace.h"
#include "help_win32.h"
#define szClassName "WinGNOME Control"
#include "window_manager.h"

static HWND hwnd_supervisor;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void create_window_manager(HINSTANCE hThisInstance)
{
  WNDCLASSEX wincl;    /* Data structure for the windowclass */

  /* The Window structure */
  wincl.hInstance = hThisInstance;
  wincl.lpszClassName = szClassName;
  wincl.lpfnWndProc = WindowProcedure;    /* This function is called by windows */
  wincl.style = CS_DBLCLKS;         /* Catch double-clicks */
  wincl.cbSize = sizeof (WNDCLASSEX);

  /* Use default icon and mouse-pointer */
  wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
  wincl.hCursor = NULL;
  wincl.lpszMenuName = NULL;         /* No menu */
  wincl.cbClsExtra = 0;            /* No extra bytes after the window class */
  wincl.cbWndExtra = 0;            /* structure or the window instance */
  /* Use Windows's default colour as the background of the window */
  wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

  /* Register the window class, and if it fails quit the program */
  if (!RegisterClassEx (&wincl))
    return;

  /* The class is registered, let's create the program*/
  hwnd_supervisor = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED,szClassName, "", WS_MAXIMIZE | WS_POPUP,  CW_USEDEFAULT, CW_USEDEFAULT, 544, 375, HWND_DESKTOP, NULL, hThisInstance, NULL);
  if (!hwnd_supervisor)
    cout << "Catastrophe." << endl;

  /* Make the window visible on the screen */
  SetWindowPos(hwnd_supervisor,HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
  SetLayeredWindowAttributes(hwnd_supervisor, 0, 1, LWA_ALPHA);
  SetCursor(LoadCursor(NULL, IDC_ARROW));

  #ifndef LWA_ALPHA
  #define LWA_ALPHA 0x02
  #endif
  if (!RegisterHotKey(hwnd_supervisor,1,MOD_WIN,VK_LWIN))
    cout << "Failed to register left Mod4 hotkey." << endl;
  if (!RegisterHotKey(hwnd_supervisor,2,MOD_WIN,VK_RWIN))
    cout << "Failed to register right Mod4 hotkey." << endl;
  if (!RegisterHotKey(hwnd_supervisor,3,MOD_CONTROL | MOD_ALT, VK_UP))
    cout << "Failed to register workspace up hotkey." << endl;
  if (!RegisterHotKey(hwnd_supervisor,4,MOD_CONTROL | MOD_ALT, VK_DOWN))
    cout << "Failed to register workspace down hotkey." << endl;
  if (!RegisterHotKey(hwnd_supervisor,5,MOD_CONTROL | MOD_ALT, VK_LEFT))
    cout << "Failed to register workspace left hotkey." << endl;
  if (!RegisterHotKey(hwnd_supervisor,6,MOD_CONTROL | MOD_ALT, VK_RIGHT))
    cout << "Failed to register workspace right hotkey." << endl;
  if (!RegisterHotKey(hwnd_supervisor,7,MOD_CONTROL | MOD_ALT | MOD_SHIFT, VK_UP))
    cout << "Failed to register workspace move up hotkey." << endl;
  if (!RegisterHotKey(hwnd_supervisor,8,MOD_CONTROL | MOD_ALT | MOD_SHIFT, VK_DOWN))
    cout << "Failed to register workspace move down hotkey." << endl;
  if (!RegisterHotKey(hwnd_supervisor,9,MOD_CONTROL | MOD_ALT | MOD_SHIFT, VK_LEFT))
    cout << "Failed to register workspace move left hotkey." << endl;
  if (!RegisterHotKey(hwnd_supervisor,10,MOD_CONTROL | MOD_ALT | MOD_SHIFT, VK_RIGHT))
    cout << "Failed to register workspace move right hotkey." << endl;
}

HWND dragging_window;
HWND resizing_window;
RECT window_rect;
RECT original_rect;
POINT dragging_point;
POINT original_point;
int resizesides;

enum {
  RS_LEFT   = 1,
  RS_RIGHT  = 2,
  RS_TOP    = 4,
  RS_BOTTOM = 8,
  RS_MASK   = 8|4|2|1
};

CHAR* cursorfor[16] = {
  IDC_SIZEALL,  //none
  IDC_SIZEWE,   //L
  IDC_SIZEWE,   // R
  IDC_SIZEWE,   //LR
  IDC_SIZENS,   //  T
  IDC_SIZENWSE, //L T
  IDC_SIZENESW, // RT
  IDC_SIZEALL,  //LRT
  IDC_SIZENS,   //   B
  IDC_SIZENESW, //L  B
  IDC_SIZENWSE, // R B
  IDC_SIZEALL,  //LR B
  IDC_SIZENS,   //  TB
  IDC_SIZEALL,  //L TB
  IDC_SIZEALL,  // RTB
  IDC_SIZEALL,  //LRTB
};

static void start_drag(int x, int y) {
  dragging_window = get_window_at_not(x, y, hwnd_supervisor);
  if (dragging_window) {
    GetWindowRect(dragging_window,&window_rect);
    GetWindowRect(dragging_window,&original_rect);
    dragging_point.x = original_point.x = x;
    dragging_point.y = original_point.y = y;
    SetCursor(LoadCursor(NULL, IDC_HAND));
  }
}
static void start_resize(int mouse_x, int mouse_y) {
  resizing_window = get_window_at_not(mouse_x, mouse_y, hwnd_supervisor);
  if (resizing_window)
  {
    GetWindowRect(resizing_window,&window_rect);
    GetWindowRect(resizing_window,&original_rect);
    dragging_point.x = mouse_x;
    dragging_point.y = mouse_y;
    const int x = window_rect.left, y = window_rect.top,
    w = window_rect.right - window_rect.left,
    h = window_rect.bottom - window_rect.top;
    resizesides =
      (dragging_point.x < x +  (w)  /3? RS_LEFT   : 0) |
      (dragging_point.x > x + (w*2) /3? RS_RIGHT  : 0) |
      (dragging_point.y < y +  (h)  /3? RS_TOP    : 0) |
      (dragging_point.y > y + (h*2) /3? RS_BOTTOM : 0);
    SetCursor(LoadCursor(NULL, cursorfor[resizesides]));
  }
}
static void stop_drag() {
  dragging_window = false;
  SetCursor(LoadCursor(NULL, IDC_ARROW));
}
static void stop_resize() {
  resizing_window = false;
  SetCursor(LoadCursor(NULL, IDC_ARROW));
}

static bool super_modifiers() {
  return (GetKeyState(VK_LWIN) >> 15) || (GetKeyState(VK_RWIN) >> 15);
}

static void show_supervisor() {
  ShowWindow(hwnd_supervisor, SW_SHOW);
  SetForegroundWindow(hwnd_supervisor);
  keybd_event(VK_SHIFT,0,0,0);
  keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);
}
static void hide_supervisor() {
  ShowWindow(hwnd_supervisor, SW_HIDE);
  if (dragging_window) stop_drag();
  if (resizing_window) stop_resize();
}

static void handle_drag(int mouse_x, int mouse_y)
{
  if (isWindowMaximized(dragging_window)) {
    if (hypot(dragging_point.x - mouse_x, dragging_point.y - mouse_y) < 72)
      return;
    
    { // This block works around the fact that I can't restore the fucking window without activating it
      HWND restore = dragging_window;
      ShowWindow(dragging_window,SW_RESTORE);
      dragging_window = restore;
    }
    
    GetWindowRect(dragging_window, &window_rect);
    int offx = dragging_point.x - (dragging_point.x - original_rect.left)/double(original_rect.right - original_rect.left) * (window_rect.right - window_rect.left) - window_rect.left;
    int offy = dragging_point.y - (dragging_point.y - original_rect.top)/double(original_rect.bottom - original_rect.top) * (window_rect.bottom - window_rect.top) - window_rect.top;
    window_rect.left += offx; window_rect.right += offx;
    window_rect.top += offy; window_rect.bottom += offy;
  }
  else {
    GetWindowRect(dragging_window,&window_rect);
    window_rect.left += mouse_x - dragging_point.x;
    window_rect.top  += mouse_y - dragging_point.y;
  }
            
  SetWindowPos(dragging_window,HWND_TOP,window_rect.left,window_rect.top,0,0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
  dragging_point.x = mouse_x;
  dragging_point.y = mouse_y;
}

static void handle_resize(int mouse_x, int mouse_y)
{
  GetWindowRect(resizing_window, &window_rect);
  if (resizesides & RS_LEFT)   window_rect.left   = original_rect.left   + mouse_x - dragging_point.x; else
  if (resizesides & RS_RIGHT)  window_rect.right  = original_rect.right  + mouse_x - dragging_point.x;
  if (resizesides & RS_TOP)    window_rect.top    = original_rect.top    + mouse_y - dragging_point.y; else
  if (resizesides & RS_BOTTOM) window_rect.bottom = original_rect.bottom + mouse_y - dragging_point.y;
  SetWindowPos(resizing_window,HWND_TOP,window_rect.left,window_rect.top,window_rect.right-window_rect.left,window_rect.bottom-window_rect.top, SWP_NOACTIVATE);
}

enum menu_checktype { ct_none = 0, ct_check, ct_radio };
static bool menu_add_item(HMENU menu, int item_id, int position, string str, menu_checktype check = ct_radio, bool checked = false)
{
  MENUITEMINFO mii;
  mii.cbSize = sizeof(MENUITEMINFO);
  mii.fMask = MIIM_ID|MIIM_FTYPE|MIIM_STRING|MIIM_STATE|(check? MIIM_CHECKMARKS : 0);
  mii.fType = MFT_STRING|(check == ct_radio? MFT_RADIOCHECK : 0);
  mii.wID = item_id;
  mii.dwTypeData = (char*)str.c_str();
  mii.cch = str.length();
  mii.fState = (checked? MFS_CHECKED : MFS_UNCHECKED);
  mii.hbmpChecked = NULL;
  mii.hbmpUnchecked = NULL;

  return InsertMenuItem(menu,position,0,&mii);
}
static void show_window_menu(HWND hwnd) {
  HMENU menu = CreatePopupMenu();
  bool isalwaysactive = workspace_always_visible(hwnd);
  bool isalwaysontop = isWindowTopmost(hwnd);
  menu_add_item(menu, 1,1, "Minimize");
  menu_add_item(menu, 2,2, isWindowMaximized(hwnd)?"Restore":"Maximize");
  menu_add_item(menu, 3,3, "Always in active workspace", ct_check, isalwaysactive);
  menu_add_item(menu, 4,4, "Stay on top", ct_check, isalwaysontop);
  
  POINT mouse; GetCursorPos(&mouse);
  int rv = TrackPopupMenuEx(menu,TPM_LEFTBUTTON|TPM_RETURNCMD|TPM_LEFTALIGN|TPM_TOPALIGN,mouse.x,mouse.y,hwnd_supervisor,NULL);
  SendMessage(hwnd_supervisor, WM_NULL, 0, 0);
  switch (rv) {
    case 1: minimize(hwnd); break;
    case 2: toggle_maximization(hwnd); break;
    case 3: workspace_toggle_pin(hwnd); break;
    case 4: toggle_topmost(hwnd); break;
    default: cerr << "Menu Malfunction" << endl;
  }
}


LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)          /* handle the messages */
  {
    case WM_KILLFOCUS:
        if (!super_modifiers())
          hide_supervisor();
        else
          show_supervisor();
      break;
    case WM_DESTROY:
        PostQuitMessage(0);
      break;
    case WM_HOTKEY:
        switch (wParam) {
          case 1: case 2: show_supervisor(); break;
          case 3: workspace_hop_up(); break;
          case 4: workspace_hop_down(); break;
          case 5: workspace_hop_left(); break;
          case 6: workspace_hop_right(); break;
          case 7:  workspace_hop_up(GetForegroundWindow()); break;
          case 8:  workspace_hop_down(GetForegroundWindow()); break;
          case 9:  workspace_hop_left(GetForegroundWindow()); break;
          case 10: workspace_hop_right(GetForegroundWindow()); break;
          default: cerr << "I don't have a hotkey #" << wParam << endl;
        }
      return 0;
    case WM_LBUTTONDBLCLK:
        {
          HWND wnd = get_window_at_not(LOWORD(lParam), HIWORD(lParam), hwnd_supervisor);
          if (isWindowMaximized(wnd))
            ShowWindow(wnd,SW_RESTORE);
          else
            ShowWindow(wnd,SW_MAXIMIZE);
          SetForegroundWindow(hwnd_supervisor);
        }
      break;
    case WM_MBUTTONDBLCLK:
        {
          HWND wnd = get_window_at_not(LOWORD(lParam), HIWORD(lParam), hwnd_supervisor);
          if (wnd) SetWindowPos(wnd,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
        }
      break;
    case WM_MBUTTONUP:
        {
          HWND wnd = get_window_at_not(LOWORD(lParam), HIWORD(lParam), hwnd_supervisor);
          if (wnd) show_window_menu(wnd);
        }
      break;
    case WM_LBUTTONDOWN:
        start_drag(LOWORD(lParam), HIWORD(lParam));
      break;
    case WM_LBUTTONUP:
        stop_drag();
        if (!super_modifiers()) hide_supervisor();
      break;
    case WM_RBUTTONDOWN:
        start_resize(LOWORD(lParam), HIWORD(lParam));
      break;
    case WM_RBUTTONUP:
        stop_resize();
        if (not super_modifiers()) hide_supervisor(); 
      break;
    case WM_KEYUP:
        switch (wParam) {
          case VK_RWIN: case VK_LWIN:
              if (not(resizing_window or dragging_window))
                hide_supervisor();
            return 0;
            
          // These have to be key up events, because Windows is an unholy piece of shit
          case VK_UP:
              toggle_maximization(first_window_not(hwnd_supervisor));
            return 0;
          case VK_DOWN:
              minimize(first_window_not(hwnd_supervisor));
            return 0;
          case VK_LEFT:
            return 0;
          case VK_RIGHT:
            return 0;
          default: ;
        }
      break;
    case WM_SYSKEYDOWN:
      switch (wParam) {
          case VK_RWIN: case VK_LWIN:
              if (not(resizing_window or dragging_window))
                hide_supervisor();
            return 0;
            
          // These have to be key up events, because Windows is an unholy piece of shit
          case VK_UP:
              workspace_hop_up();
            return 0;
          case VK_DOWN:
              workspace_hop_down();
            return 0;
          case VK_LEFT:
              workspace_hop_left();
            return 0;
          case VK_RIGHT:
              workspace_hop_right();
            return 0;
          default: ;
        }
      break;
    case WM_MOUSEMOVE:
        if (dragging_window) handle_drag(LOWORD(lParam), HIWORD(lParam));
        if (resizing_window) handle_resize(LOWORD(lParam), HIWORD(lParam));
      return 0;
    
    case WM_MOUSEWHEEL: {
        POINT mouse; GetCursorPos(&mouse);
        HWND wnd = get_window_at_not(mouse.x, mouse.y, hwnd_supervisor);
        if (wnd)
          SendMessage(wnd, message, wParam, lParam);
      } return 0;
    
    default: /* for messages that we don't deal with */
      return DefWindowProc (hwnd, message, wParam, lParam);
  }
  return 0;
}

