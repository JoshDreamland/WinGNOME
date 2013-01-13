/** Copyright (C) 2011 Josh Ventura
 ** This file is part of WinGNOME.
 ** WinGNOME is free software; you are free to use, modify, and redistribute it in accordance
 ** with the terms of the GNU General Public Licensed, as published by the Free Software
 ** Foundation, either version 3 of the license, or (at your option) any later version.
**/

#include <iostream>
#include <windows.h>
#include "disable_windows_key.h"
using namespace std;

const char *explorer_keypath = "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";

void disable_windows_key()
{
  /*HKEY key = NULL;
  long err;
  if ((err = RegCreateKeyEx(HKEY_CURRENT_USER,explorer_keypath,0,NULL,REG_OPTION_NON_VOLATILE,
                            KEY_SET_VALUE,NULL,&key,NULL)) != ERROR_SUCCESS)
    return (cout << "Failed to open HKEY_CURRENT_USER for key_set; error " << err << endl, void());
  DWORD data = 1;
  if ((err = RegSetValueEx(key,"NoWinKeys",0,REG_DWORD,(BYTE*)&data,sizeof(data))) != ERROR_SUCCESS)
    return (cout << "Failed to set key, error " << err << endl, void());
  cout << "Successfully set reg key." << endl;*/
}

void enable_windows_key()
{
  HKEY key = NULL;
  if (RegCreateKeyEx(HKEY_CURRENT_USER,explorer_keypath,0,NULL,REG_OPTION_NON_VOLATILE,
                     KEY_SET_VALUE,NULL,&key,NULL) != ERROR_SUCCESS)
    return (cout << "Failed pretty hard", void());
  DWORD data = 1;
  if (RegSetValueEx(key,"NoWinKeys",0,REG_DWORD,(BYTE*)&data,sizeof(data)) != ERROR_SUCCESS)
    return (cout << "Failed to get key", void());
  cout << "Successfully set reg key." << endl;
}
