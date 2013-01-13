/** Copyright (C) 2011 Josh Ventura
 ** This file is part of WinGNOME.
 ** WinGNOME is free software; you are free to use, modify, and redistribute it in accordance
 ** with the terms of the GNU General Public Licensed, as published by the Free Software
 ** Foundation, either version 3 of the license, or (at your option) any later version.
**/

#define _WIN32_IE 9001
#include <iostream>
#include <windows.h>
#include <commctrl.h>
#include <cstdlib>
#include "workspace_switcher.h"
#define szClassName "workspaceswitcher"
using namespace std;

HWND get_taskbar(), get_windowpicker(HWND), get_systemtray(HWND);
LRESULT CALLBACK workspaceProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static HWND hwnd_workswitch;

typedef struct {
    DWORD dwMask;
    POINTL ptMinSize;
    POINTL ptMaxSize;
    POINTL ptIntegral;
    POINTL ptActual;
    WCHAR wszTitle[256];
    DWORD dwModeFlags;
    COLORREF crBkgnd;
} DESKBANDINFO;

#define DBIM_MINSIZE   0x01L
#define DBIM_MAXSIZE   0x02L
#define DBIM_INTEGRAL  0x04L
#define DBIM_ACTUAL    0x08L
#define DBIM_TITLE     0x10L
#define DBIM_MODEFLAGS 0x20L
#define DBIM_BKCOLOR   0x40L

#define DBIMF_NORMAL         0x0000L
#define DBIMF_FIXED          0x0001L
#define DBIMF_FIXEDBMP       0x0004L
#define DBIMF_VARIABLEHEIGHT 0x0008L
#define DBIMF_UNDELETEABLE   0x0010L
#define DBIMF_DEBOSSED       0x0020L
#define DBIMF_BKCOLOR        0x0040L
#define DBIMF_USECHEVRON     0x0080L
#define DBIMF_BREAK          0x0100L
#define DBIMF_ADDTOFRONT     0x0200L
#define DBIMF_TOPALIGN       0x0400L
#define DBIMF_NOGRIPPER      0x0800L
#define DBIMF_ALWAYSGRIPPER  0x1000L
#define DBIMF_NOMARGINS      0x2000L

#define DBIF_VIEWMODE_NORMAL      0
#define DBIF_VIEWMODE_VERTICAL    1
#define DBIF_VIEWMODE_FLOATING    2
#define DBIF_VIEWMODE_TRANSPARENT 4

#define DBPC_SELECTFIRST 0xFFFFFFFFL
#define DBPC_SELECTLAST  0xFFFFFFFEL

void create_workspace_switcher(HINSTANCE hThisInstance)
{
  WNDCLASSEX wincl;    /* Data structure for the windowclass */

  /* The Window structure */
  wincl.hInstance = hThisInstance;
  wincl.lpszClassName = szClassName;
  wincl.lpfnWndProc = workspaceProc;    /* This function is called by windows */
  wincl.style = CS_DBLCLKS;         /* Catch double-clicks */
  wincl.cbSize = sizeof (WNDCLASSEX);

  /* Use default icon and mouse-pointer */
  wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
  wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
  wincl.lpszMenuName = NULL;         /* No menu */
  wincl.cbClsExtra = 0;            /* No extra bytes after the window class */
  wincl.cbWndExtra = 0;            /* structure or the window instance */
  /* Use Windows's default colour as the background of the window */
  wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

  /* Register the window class, and if it fails quit the program */
  if (!RegisterClassEx (&wincl))
    return;

  /* The class is registered, let's create the program*/
  HWND taskbar = get_taskbar();
  HWND windpicker = get_windowpicker(taskbar);//, tray = get_systemtray(taskbar);
  if (!taskbar or !windpicker) { // or !tray) {
    cout << "No idea where to put this workspace switcher.";
    return;
  }
  hwnd_workswitch = CreateWindowEx(0, szClassName, "", WS_CHILD,  0, 0, 32, 24, windpicker, NULL, hThisInstance, NULL);
  if (!hwnd_workswitch) {
    cout << "No workspace switcher for you!" << endl;
    return;
  }
  /*
  RECT taskrect, trayrect, wprect;
  if (!GetWindowRect(taskbar,&taskrect))
    return (cout << "Failed to get rectangle...",void());
  if (!GetWindowRect(tray,&trayrect))
    return (cout << "Failed to get rectangle...",void());
  if (!GetWindowRect(windpicker,&wprect))
    return (cout << "Failed to get rectangle...",void());
  if (wprect.top < trayrect.top) trayrect.top = wprect.top;
  if (wprect.bottom > trayrect.bottom) trayrect.bottom = wprect.bottom;
  
  trayrect.left   -= taskrect.left, wprect.left   -= taskrect.left;
  trayrect.right  -= taskrect.left, wprect.right  -= taskrect.left;
  trayrect.top    -= taskrect.top,  wprect.top    -= taskrect.top;
  trayrect.bottom -= taskrect.top,  wprect.bottom -= taskrect.top;
  
  const int h = trayrect.bottom-trayrect.top, w = h*16/9;
  cout << "SetWindowPos(hwnd_workswitch,NULL," << (trayrect.left - 32) << "," << trayrect.top << "," << w << "," << h << "," << "SWP_NOZORDER);" << endl;
  SetWindowPos(hwnd_workswitch,NULL,trayrect.left - w, trayrect.top, w, h, SWP_NOZORDER);
  SetWindowPos(windpicker, NULL,wprect.left, wprect.top, wprect.right-w-wprect.left, wprect.bottom-wprect.top, SWP_NOZORDER);
  ShowWindow(hwnd_workswitch, SW_SHOW);*/
  
  HWND window    = CreateWindow(szClassName,"Test...",WS_VISIBLE|WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,526,96,HWND_DESKTOP,NULL,hThisInstance,0);
  HWND rebartest = CreateWindow(REBARCLASSNAME,"",WS_VISIBLE|WS_CHILD,CW_USEDEFAULT,CW_USEDEFAULT,500,64,window,NULL,hThisInstance,0);
  
  REBARBANDINFO rbBand;
  ZeroMemory(&rbBand,sizeof(rbBand));
  const int w = 96, h = 32;
  
  // Initialize REBARBANDINFO for all coolbar bands.
  rbBand.cbSize = sizeof(REBARBANDINFO);
  rbBand.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID;
  rbBand.fStyle = 0;
  rbBand.wID = 1337;
  rbBand.hwndChild = CreateWindowEx (0,"COMBOBOX","Testing", WS_VISIBLE | WS_CHILD | CBS_SIMPLE,900,0,96,32,rebartest,(HMENU)1337,hThisInstance,0);
  rbBand.cxMinChild = w;
  rbBand.cyMinChild = h;
  
  DESKBANDINFO dbi; ZeroMemory(&dbi,sizeof(dbi));
  dbi.dwMask = DBIMF_NORMAL | DBIM_MINSIZE | DBIM_MAXSIZE | DBIM_ACTUAL | DBIM_MODEFLAGS | DBIM_TITLE;
  dbi.dwModeFlags = DBIMF_DEBOSSED | DBIMF_NOGRIPPER;
  dbi.ptMaxSize.x = dbi.ptMinSize.x = dbi.ptActual.x = 96;
  dbi.ptMaxSize.y = dbi.ptMinSize.y = dbi.ptActual.y = 32;
  int i = 0;
  for (const char* c = "Workspace Band"; *c; c++)
    dbi.wszTitle[i++] = *c;
  
  // Insert band into coolbar.
  if (!SendMessage(rebartest, RB_INSERTBAND, (UINT)-1, (LPARAM)(LPREBARBANDINFO)&rbBand))
    cout << "Fucking windows. " << GetLastError() << endl;
  rbBand.hwndChild = CreateWindowEx (0,"COMBOBOX","Testing", WS_VISIBLE | WS_CHILD | CBS_SIMPLE,900,0,96,32,rebartest,(HMENU)1337,hThisInstance,0);
  if (!SendMessage(rebartest, RB_INSERTBAND, (UINT)-1, (LPARAM)(LPREBARBANDINFO)&rbBand))
    cout << "Fucking windows. " << GetLastError() << endl;
    
  if (!SendMessage(windpicker, RB_INSERTBAND, (WPARAM)&dbi, (LPARAM)&dbi))
    cout << "Fucking windows. " << GetLastError() << endl;
  
  REBARINFO rbi; ZeroMemory(&rbi,sizeof(REBARINFO)); rbi.cbSize = sizeof(REBARINFO);
  cout << SendMessage(rebartest, RB_GETBANDCOUNT, (WPARAM)0, (LPARAM)0) << endl;
  
  ShowWindow(hwnd_workswitch, SW_SHOW);
}

//Shell_TrayWnd
//->ReBarWindow32
//->TrayNotifyWnd
struct searchdata { string find; HWND result; };
static BOOL find_by_class(HWND handle,LPARAM found)
{
  char writehere[1025];
  GetClassName(handle,writehere,1024);
  
  string castr=writehere;
  if (castr.length() != ((searchdata*)found)->find.length()) return true;
  for (unsigned int i=0;i<castr.length();i++) if (castr[i]>='A' && castr[i]<='Z') castr[i]-='A'-'a';
  if (castr == ((searchdata*)found)->find)
    return (((searchdata*)found)->result = handle, false);
  return true;
}

HWND get_taskbar()
{
  searchdata found = { "shell_traywnd", NULL };
  EnumWindows((WNDENUMPROC)find_by_class,(LPARAM)&found); // Search for "Shell_TrayWnd"
  return found.result;
}
HWND get_windowpicker(HWND tb)
{
  searchdata found = { "rebarwindow32", NULL };
  EnumChildWindows(tb, (WNDENUMPROC)find_by_class,(LPARAM)&found); // Search for "Shell_TrayWnd"
  return found.result;
}/*
HWND get_systemtray(HWND tb)
{
  searchdata found = { "traynotifywnd", NULL };
  EnumChildWindows(tb, (WNDENUMPROC)find_by_class,(LPARAM)&found); // Search for "Shell_TrayWnd"
  return found.result;
}*/

LRESULT CALLBACK workspaceProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)          /* handle the messages */
  {
    default: /* for messages that we don't deal with */
      return DefWindowProc (hwnd, message, wParam, lParam);
  }
  return 0;
}

