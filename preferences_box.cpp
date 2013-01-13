/** Copyright (C) 2011 Josh Ventura
 ** This file is part of WinGNOME.
 ** WinGNOME is free software; you are free to use, modify, and redistribute it in accordance
 ** with the terms of the GNU General Public Licensed, as published by the Free Software
 ** Foundation, either version 3 of the license, or (at your option) any later version.
**/

#include <windows.h>
#include "preferences_box.h"

#define szClassName "gwprefclass"

HWND hwnd_preferences;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int create_preferences_box(HINSTANCE hThisInstance)
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
  wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
  wincl.lpszMenuName = NULL;         /* No menu */
  wincl.cbClsExtra = 0;            /* No extra bytes after the window class */
  wincl.cbWndExtra = 0;            /* structure or the window instance */
  /* Use Windows's default colour as the background of the window */
  wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

  /* Register the window class, and if it fails quit the program */
  if (!RegisterClassEx (&wincl))
    return 1;

  /* The class is registered, let's create the program*/
  hwnd_preferences = CreateWindowEx(0, szClassName, "WinGNOME Preferences", WS_MAXIMIZE,  CW_USEDEFAULT, CW_USEDEFAULT, 544, 375, HWND_DESKTOP, NULL, hThisInstance, NULL);
  if (!hwnd_preferences)
    return 2;
  return 0;
}
