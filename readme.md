# WinGNOME
*A window manager program to simulate Metacity/Marco/XFWM features in Windows*

Implements the following features and hotkeys:
* WinKey + Mouse Left: Drag windows
* WinKey + Mouse Right: Resize windows
* WinKey + Mouse Middle: Show window context menu
** Offers "Stay on Top" option to pin window above others
** Offers "Stay in active workspace" option to not hide the window
* Control-Alt-<Arrow key> switches to one of four workspaces
* Control-Alt-Shift-<Arrow key> switches workspaces, keeping the foreground window
* Implements custom minimize/maximize key combos WinKey+Up/Down

Still to be done:
* No start menu widget at present due to how big a bitch it is to do anything on Windows
* Minimizing windows is a bit buggy at times; it's difficult to work around this due to lack of features in Win32 and the way this mechanism works
* Bug testing in general needs done
