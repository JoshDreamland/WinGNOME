/** Copyright (C) 2011 Josh Ventura
 ** This file is part of WinGNOME.
 ** WinGNOME is free software; you are free to use, modify, and redistribute it in accordance
 ** with the terms of the GNU General Public Licensed, as published by the Free Software
 ** Foundation, either version 3 of the license, or (at your option) any later version.
**/

#define _WIN32_IE 9001
#include <windows.h>
#include <commctrl.h>
#include <iostream>
using namespace std;

#include "workspace_switcher.h"
#include "window_manager.h"
#include "disable_windows_key.h"
#include "preferences_box.h"
#include "workspace.h"

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE, LPSTR, int)
{
  InitCommonControls();
  INITCOMMONCONTROLSEX icce;
  icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icce.dwICC = ICC_COOL_CLASSES;
  
  if (!InitCommonControlsEx(&icce))
    return (cout << "Failed to init common controls." << endl, 0);
  if (create_preferences_box(hThisInstance))
    return 0;
  
  workspace_init();
  //create_workspace_switcher(hThisInstance);
  create_window_manager(hThisInstance);
  disable_windows_key();

  MSG msg = {0, 0, 0, 0, 0, {0,0}};
  while( GetMessage( &msg, NULL, 0, 0 ) )
  {
    DispatchMessage( &msg );
  }
  return 0;
}
