/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COnnect.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COnnect.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 8
 */


// ==============================================================================
/*
 * EA: 0x1900AB10
 * CodeView symbol: LogoSetTimer
 * Demangled: LogoSetTimer
 * IDA name: ?LogoSetTimer@@YIHPAUHWND__@@HP6GHKKPAXKPAK@Z@Z
 * CodeView module: 22
 * CV address: segment 1 : 0x9B10
 * Code length: 0x6E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COnnect.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COnnect.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall LogoSetTimer(
        HWND a1,
        int a2,
        int (__stdcall *getdatacallback)(unsigned int, unsigned int, void *, unsigned int, unsigned int *))
{
  int nResult; // [esp+8h] [ebp-10h]
  unsigned int dwSize; // [esp+10h] [ebp-8h] BYREF
  unsigned __int8 *pData; // [esp+14h] [ebp-4h] BYREF

  if ( UiGetData(a1: getdatacallback, a2: 0x80000000, a3: &pData, a4: &dwSize) == 0 || dwSize != 4 )
    return false;
  nResult = SDlgSetTimer(a1, a2, a3: *(_DWORD *)pData, a4: 0);
  SMemFree(a1: pData, a2: aConnectCpp, a3: 54, a4: 0);
  return nResult != 0;
}


// ==============================================================================
/*
 * EA: 0x1900AB7E
 * CodeView symbol: LogoAnimate
 * Demangled: LogoAnimate
 * IDA name: ?LogoAnimate@@YIXPAUHWND__@@0@Z
 * CodeView module: 22
 * CV address: segment 1 : 0x9B7E
 * Code length: 0x121
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COnnect.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COnnect.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall LogoAnimate(HWND a1, HWND a2)
{
  _DWORD *tpBmp; // [esp+8h] [ebp-14h]
  struct tagRECT rect; // [esp+Ch] [ebp-10h] BYREF

  tpBmp = (_DWORD *)GetWindowLongA(hWnd: a2, nIndex: -21);
  if ( a2 != nullptr && tpBmp != nullptr && sgLogoBmp != 0 && sgTransHandles[0] != 0 )
  {
    if ( sgTransHandles[++sgFrame] == 0 || sgFrame >= 32 )
      sgFrame = 0;
    GetWindowRect(hWnd: a2, lpRect: &rect);
    ScreenToClient(hWnd: a1, lpPoint: (LPPOINT)&rect);
    ScreenToClient(hWnd: a1, lpPoint: (LPPOINT)&rect.right);
    SBltROP3(
      a1: sgLogoBmp,
      a2: rect.left + tpBmp[1] * rect.top + *tpBmp,
      a3: sgLogoBmpSize,
      a4: dword_190421A4,
      a5: sgLogoBmpSize,
      a6: tpBmp[1],
      a7: 0,
      a8: 13369376);
    STransBlt(a1: sgLogoBmp, a2: 0, a3: 0, a4: sgLogoBmpSize, a5: sgTransHandles[sgFrame]);
    InvalidateRect(hWnd: a2, lpRect: nullptr, bErase: false);
  }
}


// ==============================================================================
/*
 * EA: 0x1900AC9F
 * CodeView symbol: LogoFramesDestroy
 * Demangled: LogoFramesDestroy
 * IDA name: ?LogoFramesDestroy@@YIXPAUHWND__@@@Z
 * CodeView module: 22
 * CV address: segment 1 : 0x9C9F
 * Code length: 0xA1
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COnnect.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COnnect.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall LogoFramesDestroy(HWND a1)
{
  int index; // [esp+4h] [ebp-8h]
  LONG tpBmp; // [esp+8h] [ebp-4h]

  tpBmp = GetWindowLongA(hWnd: a1, nIndex: -21);
  if ( tpBmp != 0 )
    SMemFree(a1: tpBmp, a2: aConnectCpp_0, a3: 105, a4: 0);
  for ( index = 0; index < 32; ++index )
  {
    if ( sgTransHandles[index] != 0 )
    {
      STransDelete(a1: sgTransHandles[index]);
      sgTransHandles[index] = 0;
    }
  }
  if ( sgLogoBmp != 0 )
  {
    SMemFree(a1: sgLogoBmp, a2: aConnectCpp_1, a3: 115, a4: 0);
    sgLogoBmp = 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1900AD40
 * CodeView symbol: LogoInit
 * Demangled: LogoInit
 * IDA name: ?LogoInit@@YIHPAUHWND__@@0PAEQAUtagSIZE@@P6GHKKPAUtagPALETTEENTRY@@1KPAH44@Z@Z
 * CodeView module: 22
 * CV address: segment 1 : 0x9D40
 * Code length: 0x243
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COnnect.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COnnect.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall LogoInit(
        HWND a1,
        HWND a2,
        unsigned __int8 *bitmap,
        struct tagSIZE *const bitmapsize,
        int (__stdcall *artcallback)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  LONG cy; // eax
  _DWORD *v7; // ecx
  LONG bottom; // [esp+0h] [ebp-48h]
  LONG right; // [esp+4h] [ebp-44h]
  _DWORD srcrect[4]; // [esp+10h] [ebp-38h] BYREF
  int index; // [esp+20h] [ebp-28h]
  int frames; // [esp+24h] [ebp-24h]
  struct tagSIZE srcsize; // [esp+28h] [ebp-20h] BYREF
  _DWORD *tpBmp; // [esp+30h] [ebp-18h]
  unsigned __int8 *srcframes; // [esp+34h] [ebp-14h] BYREF
  struct tagRECT rect; // [esp+38h] [ebp-10h] BYREF

  if ( a1 == nullptr )
    return 0;
  if ( a2 == nullptr )
    return 0;
  if ( artcallback == nullptr )
    return 0;
  if ( UiLoadArtwork(
         a1: artcallback,
         a2: nullptr,
         hWndParent: nullptr,
         artid: 9u,
         controltype: nullptr,
         controlstyle: 0,
         usageflags: 0,
         loadpalette: 0,
         prepfadein: 0,
         a10: &srcframes,
         a11: &srcsize) != 0 )
  {
    GetClientRect(hWnd: a2, lpRect: &rect);
    if ( srcsize.cx >= rect.right )
      right = rect.right;
    else
      right = srcsize.cx;
    sgLogoBmpSize = right;
    if ( srcsize.cy >= rect.bottom )
      bottom = rect.bottom;
    else
      bottom = srcsize.cy;
    dword_190421A4 = bottom;
    sgLogoBmp = SMemAlloc(a1: rect.bottom * rect.right, a2: aConnectCpp_2, a3: 157, a4: 0);
    if ( sgLogoBmp != 0 )
    {
      SDlgSetBitmapI(
        a1: a2,
        a2: 0,
        a3: 0,
        a4: -1,
        a5: 1,
        a6: sgLogoBmp,
        a7: 0,
        a8: rect.right,
        a9: rect.bottom,
        a10: -1);
      memset(a1: sgTransHandles, Val: 0, Size: sizeof(sgTransHandles));
      if ( srcframes != nullptr && dword_190421A4 != 0 )
      {
        frames = srcsize.cy / dword_190421A4;
        if ( srcsize.cy / dword_190421A4 > 32 )
          frames = 32;
        for ( index = 0; index < frames; ++index )
        {
          srcrect[0] = 0;
          srcrect[2] = sgLogoBmpSize - 1;
          srcrect[1] = dword_190421A4 * index;
          srcrect[3] = dword_190421A4 * index + dword_190421A4 - 1;
          STransCreateI(
            a1: srcframes,
            a2: sgLogoBmpSize,
            a3: dword_190421A4,
            a4: 8,
            a5: srcrect,
            a6: 16777466,
            a7: &sgTransHandles[index]);
        }
        SMemFree(a1: srcframes, a2: aConnectCpp_3, a3: 198, a4: 0);
      }
      sgFrame = 0;
      tpBmp = (_DWORD *)SMemAlloc(a1: 12, a2: aConnectCpp_4, a3: 202, a4: 0);
      *tpBmp = bitmap;
      cy = bitmapsize->cy;
      v7 = tpBmp;
      tpBmp[1] = bitmapsize->cx;
      v7[2] = cy;
      SetWindowLongA(hWnd: a2, nIndex: -21, dwNewLong: (LONG)tpBmp);
      LogoAnimate(a1, a2);
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    ShowWindow(hWnd: a2, nCmdShow: 0);
    sgLogoBmp = 0;
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1900AF83
 * CodeView symbol: ProtectMinimize
 * Demangled: ProtectMinimize
 * IDA name: ?ProtectMinimize@@YIXH@Z
 * CodeView module: 22
 * CV address: segment 1 : 0x9F83
 * Code length: 0x1E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COnnect.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COnnect.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ProtectMinimize(int a1)
{
  sgbPreventMinimize = a1;
  if ( a1 == 0 )
    UiRestoreApp();
}


// ==============================================================================
/*
 * EA: 0x1900AFA1
 * CodeView symbol: ConnectDialogProc
 * Demangled: ConnectDialogProc
 * IDA name: ?ConnectDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 22
 * CV address: segment 1 : 0x9FA1
 * Code length: 0x1C2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COnnect.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COnnect.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ConnectDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
  HWND DlgItem; // eax
  HWND FrameWindow; // eax

  if ( message > 0x110 )
  {
    if ( message != 275 )
    {
      if ( message == 1129 )
      {
        UiNotification();
        return 1;
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( wparam == 1 )
    {
      DlgItem = GetDlgItem(hDlg: window, nIDDlgItem: 1040);
      LogoAnimate(a1: window, a2: DlgItem);
      if ( gbConnectionLost != 0 )
      {
        SDlgKillTimer(a1: window, a2: 275);
        gbConnectionLost = 0;
        UiConnectionLost(a1: *(struct _SNETUIDATA **)(dword_190421A8 + 12), a2: window);
      }
    }
    return 1;
  }
  else
  {
    if ( message != 272 )
    {
      if ( message == 2 )
      {
        if ( ghWndUiMainParent == window )
          ghWndUiMainParent = nullptr;
        UiVidFadeOut(a1: 10);
        SDlgKillTimer(a1: window, a2: 1);
        sub_1900B163(a1: window);
      }
      else if ( message > 0x103 && message <= 0x105 )
      {
        FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
        SendMessageA(hWnd: FrameWindow, Msg: message, wParam: wparam, lParam: lparam);
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    dword_190421A8 = lparam;
    if ( lparam != 0 && *(_DWORD *)(dword_190421A8 + 12) != 0 )
    {
      sub_1900B1DA(a1: window, a2: *(_DWORD *)(*(_DWORD *)(dword_190421A8 + 12) + 12));
      UiLoadCursors(a1: window, a2: *(struct _SNETUIDATA **)(dword_190421A8 + 12));
      LogoSetTimer(
        a1: window,
        a2: 1,
        getdatacallback: *(int (__stdcall **)(unsigned int, unsigned int, void *, unsigned int, unsigned int *))(*(_DWORD *)(dword_190421A8 + 12)
                                                                                              + 44));
      ghWndUiMainParent = window;
      UiVidFade(a1: 1, a2: 1);
      return 1;
    }
    else
    {
      return 0;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1900B163
 * CodeView symbol: DestroyArtwork
 * Demangled: DestroyArtwork
 * IDA name: sub_1900B163
 * CodeView module: 22
 * CV address: segment 1 : 0xA163
 * Code length: 0x77
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COnnect.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COnnect.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall sub_1900B163(HWND this)
{
  HWND DlgItem; // eax

  DlgItem = GetDlgItem(hDlg: this, nIDDlgItem: 1040);
  LogoFramesDestroy(a1: DlgItem);
  if ( dword_190421B4 != nullptr )
  {
    SMemFree(a1: dword_190421B4, a2: aConnectCpp_5, a3: 216, a4: 0);
    dword_190421B4 = nullptr;
  }
  if ( dword_190421B8 != nullptr )
  {
    SMemFree(a1: dword_190421B8, a2: aConnectCpp_6, a3: 221, a4: 0);
    dword_190421B8 = nullptr;
  }
}


// ==============================================================================
/*
 * EA: 0x1900B1DA
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: sub_1900B1DA
 * CodeView module: 22
 * CV address: segment 1 : 0xA1DA
 * Code length: 0xDF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COnnect.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COnnect.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_1900B1DA(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  HWND DlgItem; // eax
  unsigned __int8 *v4; // [esp-Ch] [ebp-34h]
  _DWORD btn_ids[2]; // [esp+8h] [ebp-20h] BYREF
  _DWORD btn_static[2]; // [esp+10h] [ebp-18h] BYREF
  struct tagSIZE bgSize; // [esp+18h] [ebp-10h] BYREF
  struct tagSIZE sizeBtns; // [esp+20h] [ebp-8h] BYREF

  btn_ids[0] = 2;
  btn_ids[1] = 0;
  btn_static[0] = 1041;
  btn_static[1] = 0;
  UiLoadArtwork(
    a1: a2,
    a2: a1,
    hWndParent: nullptr,
    artid: 0x80000005,
    controltype: byte_190421CC,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 1,
    prepfadein: 1,
    a10: &dword_190421B4,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 6u,
    controltype: aButton_10,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_190421B8,
    a11: &sizeBtns);
  SDlgSetControlBitmaps(a1, a2: btn_ids, a3: 0, a4: dword_190421B8, a5: &sizeBtns, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: btn_static, a3: 0, a4: dword_190421B4, a5: &bgSize, a6: 2, a7: -1);
  v4 = dword_190421B4;
  DlgItem = GetDlgItem(hDlg: a1, nIDDlgItem: 1040);
  LogoInit(a1, a2: DlgItem, bitmap: v4, bitmapsize: &bgSize, artcallback: a2);
  return 1;
}

