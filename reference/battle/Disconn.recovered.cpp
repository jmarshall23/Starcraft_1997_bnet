/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Disconn.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Disconn.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 1
 */


// ==============================================================================
/*
 * EA: 0x1900B2C0
 * CodeView symbol: RasEnumCallback
 * Demangled: RasEnumCallback
 * IDA name: ?RasEnumCallback@@YGHPBDPAX1@Z
 * CodeView module: 21
 * CV address: segment 1 : 0xA2C0
 * Code length: 0xCE
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Disconn.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Disconn.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall RasEnumCallback(
        const char *szEntryName,
        void *rashandle,
        int (__stdcall **lpContext)(HWND, const char *, const char *, unsigned int))
{
  HWND ActiveWindow; // eax
  HWND v4; // eax
  int bResult; // [esp+4h] [ebp-324h]
  char szMsg[512]; // [esp+8h] [ebp-320h] BYREF
  char szFmt[256]; // [esp+208h] [ebp-120h] BYREF
  char szTitle[32]; // [esp+308h] [ebp-20h] BYREF

  LocaleLoadString(a1: 0x411u, a2: szFmt, a3: 256);
  LocaleLoadString(a1: 0x3F2u, a2: szTitle, a3: 32);
  sprintf(Buffer: szMsg, Format: szFmt, szEntryName);
  if ( lpContext[8] != nullptr && GetActiveWindow() != nullptr )
  {
    ActiveWindow = GetActiveWindow();
    bResult = UiMessageBox(a1: lpContext[8], a2: ActiveWindow, lpText: szMsg, a4: szTitle, a5: 1u);
  }
  else
  {
    v4 = GetActiveWindow();
    bResult = MessageBoxA(hWnd: v4, lpText: szMsg, lpCaption: szTitle, uType: 1u);
  }
  if ( bResult == 1 )
    HangupRASConnection(rasconn: rashandle);
  return 1;
}

