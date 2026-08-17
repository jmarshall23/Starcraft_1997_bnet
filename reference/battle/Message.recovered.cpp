/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Message.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Message.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 1
 */


// ==============================================================================
/*
 * EA: 0x19011C10
 * CodeView symbol: UiMessageBox
 * Demangled: UiMessageBox
 * IDA name: ?UiMessageBox@@YIHP6GHPAUHWND__@@PBD1I@Z011I@Z
 * CodeView module: 15
 * CV address: segment 1 : 0x10C10
 * Code length: 0x60
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Message.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Message.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall UiMessageBox(
        int (__stdcall *a1)(HWND, const char *, const char *, unsigned int),
        HWND a2,
        const char *lpText,
        const char *lpCaption,
        unsigned int uType)
{
  HWND FrameWindow; // eax
  int bResult; // [esp+8h] [ebp-4h]

  ShowCursor(bShow: true);
  if ( a1 != nullptr )
  {
    bResult = a1(a1: a2, a2: lpText, a3: lpCaption, a4: uType);
  }
  else
  {
    FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
    bResult = MessageBoxA(hWnd: FrameWindow, lpText, lpCaption, uType);
  }
  ShowCursor(bShow: false);
  return bResult;
}

