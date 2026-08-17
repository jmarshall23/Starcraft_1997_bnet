/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAncel.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAncel.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 3
 */


// ==============================================================================
/*
 * EA: 0x19004480
 * CodeView symbol: ConnectCancelDialogProc
 * Demangled: ConnectCancelDialogProc
 * IDA name: ?ConnectCancelDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 28
 * CV address: segment 1 : 0x3480
 * Code length: 0xEA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAncel.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAncel.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ConnectCancelDialogProc(HWND window, UINT message, WPARAM wparam, struct _SNETUIDATA *lparam)
{
  HWND FrameWindow; // eax

  if ( message > 0x110 )
  {
    if ( message == 273 && (_WORD)wparam != 0 && (unsigned __int16)wparam <= 2u )
    {
      SrvCancel();
      return 0;
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message != 272 )
  {
    if ( message == 2 )
    {
      sub_1900456A(a1: window);
    }
    else if ( message > 0x103 && message <= 0x105 )
    {
      FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
      SendMessageA(hWnd: FrameWindow, Msg: message, wParam: wparam, lParam: (LPARAM)lparam);
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  sInterfacedata = lparam;
  UiLoadCursors(a1: window, a2: lparam);
  if ( sInterfacedata != nullptr )
    sub_190045C4(a1: window, a2: *((_DWORD *)sInterfacedata + 3));
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900456A
 * CodeView symbol: DestroyArtwork
 * Demangled: DestroyArtwork
 * IDA name: sub_1900456A
 * CodeView module: 28
 * CV address: segment 1 : 0x356A
 * Code length: 0x5A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAncel.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAncel.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1900456A()
{
  int result; // eax

  if ( dword_19041CD8 != nullptr )
  {
    result = SMemFree(a1: dword_19041CD8, a2: aCancelCpp, a3: 24, a4: 0);
    dword_19041CD8 = nullptr;
  }
  if ( dword_19041CDC != nullptr )
  {
    result = SMemFree(a1: dword_19041CDC, a2: aCancelCpp_0, a3: 29, a4: 0);
    dword_19041CDC = nullptr;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190045C4
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: sub_190045C4
 * CodeView module: 28
 * CV address: segment 1 : 0x35C4
 * Code length: 0xB5
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAncel.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAncel.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_190045C4(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  _DWORD btn_ids[2]; // [esp+8h] [ebp-20h] BYREF
  _DWORD btn_static[2]; // [esp+10h] [ebp-18h] BYREF
  struct tagSIZE bgSize; // [esp+18h] [ebp-10h] BYREF
  struct tagSIZE sizeBtns; // [esp+20h] [ebp-8h] BYREF

  btn_ids[0] = 2;
  btn_ids[1] = 0;
  btn_static[0] = 1030;
  btn_static[1] = 0;
  UiLoadArtwork(
    a1: a2,
    a2: a1,
    hWndParent: nullptr,
    artid: 0xCu,
    controltype: byte_19041CE0,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19041CD8,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 6u,
    controltype: aButton_15,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19041CDC,
    a11: &sizeBtns);
  SDlgSetControlBitmaps(a1, a2: btn_ids, a3: 0, a4: dword_19041CDC, a5: &sizeBtns, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: btn_static, a3: 0, a4: dword_19041CD8, a5: &bgSize, a6: 2, a7: -1);
  return 1;
}

