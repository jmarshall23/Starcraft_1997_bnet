/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Progress.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Progress.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 6
 */


// ==============================================================================
/*
 * EA: 0x190129A0
 * CodeView symbol: UiModelessProgressDialog
 * Demangled: UiModelessProgressDialog
 * IDA name: ?UiModelessProgressDialog@@YIPAUHWND__@@PAU_SNETUIDATA@@PBDHP6GHXZK@Z
 * CodeView module: 12
 * CV address: segment 1 : 0x119A0
 * Code length: 0x83
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Progress.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Progress.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall UiModelessProgressDialog(
        struct _SNETUIDATA *a1,
        const char *a2,
        int abortable,
        int (__stdcall *progressfcn)(),
        int callspersec)
{
  HMODULE ModuleHandleA; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-18h]
  int v8; // [esp-Ch] [ebp-14h]

  sgProgressFcn = (int)progressfcn;
  sgAbortable = abortable;
  sgCallsPerSec = callspersec;
  if ( strlen(Str: a2) < 0x400 )
    strcpy(Destination: sgszProgressText, Source: a2);
  if ( a1 == nullptr )
    return 0;
  v8 = *((_DWORD *)a1 + 2);
  Dialog = LocaleGetDialog(a1: 0x6Au);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  return SDlgCreateDialogIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: v8, a4: ProgressDialogProc, a5: a1);
}


// ==============================================================================
/*
 * EA: 0x19012A23
 * CodeView symbol: ProgressDialogProc
 * Demangled: ProgressDialogProc
 * IDA name: ProgressDialogProc
 * CodeView module: 12
 * CV address: segment 1 : 0x11A23
 * Code length: 0xEE
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Progress.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Progress.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ProgressDialogProc(int window, UINT message, WPARAM wparam, LPARAM lparam)
{
  HWND FrameWindow; // eax

  if ( message > 0x110 )
  {
    if ( message == 273 )
    {
      if ( (unsigned __int16)wparam == 2 )
      {
        SrvCancel();
        SDlgEndDialog(a1: window, a2: 0);
        return 0;
      }
    }
    else if ( message == 275 )
    {
      ProgressTimer(a1: window);
    }
  }
  else
  {
    if ( message == 272 )
    {
      interfacedata = lparam;
      ProgressInit(a1: window, a2: *(_DWORD *)(lparam + 12));
      return 1;
    }
    if ( message == 2 )
    {
      SDlgKillTimer(a1: window, a2: 1);
      ProgressDestroy(a1: window);
    }
    else if ( message > 0x103 && message <= 0x105 )
    {
      FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
      SendMessageA(hWnd: FrameWindow, Msg: message, wParam: wparam, lParam: lparam);
    }
  }
  return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
}


// ==============================================================================
/*
 * EA: 0x19012B11
 * CodeView symbol: ProgressDestroy
 * Demangled: ProgressDestroy
 * IDA name: ProgressDestroy
 * CodeView module: 12
 * CV address: segment 1 : 0x11B11
 * Code length: 0xD1
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Progress.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Progress.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int ProgressDestroy()
{
  int result; // eax

  if ( sgBgBmp != nullptr )
  {
    result = SMemFree(a1: sgBgBmp, a2: aProgressCpp, a3: 97, a4: 0);
    sgBgBmp = nullptr;
  }
  if ( sgBtnBmp != nullptr )
  {
    result = SMemFree(a1: sgBtnBmp, a2: aProgressCpp_0, a3: 101, a4: 0);
    sgBtnBmp = nullptr;
  }
  if ( sgProgBgBmp != nullptr )
  {
    result = SMemFree(a1: sgProgBgBmp, a2: aProgressCpp_1, a3: 105, a4: 0);
    sgProgBgBmp = nullptr;
  }
  if ( sgProgFillBmp != nullptr )
  {
    result = SMemFree(a1: sgProgFillBmp, a2: aProgressCpp_2, a3: 109, a4: 0);
    sgProgFillBmp = nullptr;
  }
  if ( sgProgBmp != 0 )
  {
    result = SMemFree(a1: sgProgBmp, a2: aProgressCpp_3, a3: 113, a4: 0);
    sgProgBmp = 0;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19012BE2
 * CodeView symbol: ProgressInit
 * Demangled: ProgressInit
 * IDA name: ProgressInit
 * CodeView module: 12
 * CV address: segment 1 : 0x11BE2
 * Code length: 0x1DA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Progress.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Progress.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall ProgressInit(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  HWND Parent; // eax
  HWND DlgItem; // eax
  HWND child; // [esp+8h] [ebp-24h]
  HWND childa; // [esp+8h] [ebp-24h]
  _DWORD BtnIDs[2]; // [esp+Ch] [ebp-20h] BYREF
  struct tagRECT rect; // [esp+14h] [ebp-18h] BYREF
  struct tagSIZE btnsize; // [esp+24h] [ebp-8h] BYREF

  BtnIDs[0] = 2;
  BtnIDs[1] = 0;
  if ( sgCallsPerSec != 0 )
    SDlgSetTimer(a1, a2: 1, a3: 0x3E8u / sgCallsPerSec, a4: 0);
  else
    SDlgSetTimer(a1, a2: 1, a3: 50, a4: 0);
  Parent = GetParent(hWnd: a1);
  UiLoadArtwork(
    a1: a2,
    a2: a1,
    hWndParent: Parent,
    artid: 0xCu,
    controltype: aPopup,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgBgBmp,
    a11: nullptr);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 6u,
    controltype: aButton_1,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgBtnBmp,
    a11: &btnsize);
  SDlgSetControlBitmaps(a1, a2: BtnIDs, a3: 0, a4: sgBtnBmp, a5: &btnsize, a6: 1, a7: -1);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 0xAu,
    controltype: nullptr,
    controlstyle: 0,
    usageflags: 0,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgProgBgBmp,
    a11: &sgProgBgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 0xBu,
    controltype: nullptr,
    controlstyle: 0,
    usageflags: 0,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgProgFillBmp,
    a11: &sgProgFillSize);
  child = GetDlgItem(hDlg: a1, nIDDlgItem: 1043);
  GetClientRect(hWnd: child, lpRect: &rect);
  sgProgBmp = SMemAlloc(a1: rect.bottom * rect.right, a2: aProgressCpp_4, a3: 196, a4: 0);
  sgProgSize = rect.right;
  dword_1904311C = rect.bottom;
  SDlgSetBitmapI(a1: child, a2: 0, a3: 0, a4: -1, a5: 1, a6: sgProgBmp, a7: 0, a8: rect.right, a9: rect.bottom, a10: -1);
  ProgressDraw(a1, a2: 0);
  DlgItem = GetDlgItem(hDlg: a1, nIDDlgItem: 1042);
  SetWindowTextA(hWnd: DlgItem, lpString: sgszProgressText);
  childa = GetDlgItem(hDlg: a1, nIDDlgItem: 2);
  ShowWindow(hWnd: childa, nCmdShow: sgAbortable);
  return EnableWindow(hWnd: childa, bEnable: sgAbortable);
}


// ==============================================================================
/*
 * EA: 0x19012DBC
 * CodeView symbol: ProgressDraw
 * Demangled: ProgressDraw
 * IDA name: ProgressDraw
 * CodeView module: 12
 * CV address: segment 1 : 0x11DBC
 * Code length: 0x100
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Progress.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Progress.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND __fastcall ProgressDraw(HWND a1, int a2)
{
  HWND result; // eax
  HWND child; // [esp+8h] [ebp-14h]
  struct tagRECT rect; // [esp+Ch] [ebp-10h] BYREF

  result = GetDlgItem(hDlg: a1, nIDDlgItem: 1043);
  child = result;
  if ( sgProgBmp != 0 && sgProgBgBmp != nullptr )
  {
    if ( a2 > 100 )
      a2 = 100;
    if ( a2 < 0 )
      a2 = 0;
    SBltROP3(
      a1: sgProgBmp,
      a2: sgProgBgBmp,
      a3: sgProgSize,
      a4: dword_1904311C,
      a5: sgProgSize,
      a6: sgProgBgSize.cx,
      a7: 0,
      a8: 13369376);
    SBltROP3(
      a1: sgProgBmp,
      a2: sgProgFillBmp,
      a3: a2 * sgProgSize / 100,
      a4: dword_1904311C,
      a5: sgProgSize,
      a6: sgProgFillSize.cx,
      a7: 0,
      a8: 13369376);
    GetWindowRect(hWnd: child, lpRect: &rect);
    ScreenToClient(hWnd: a1, lpPoint: (LPPOINT)&rect);
    ScreenToClient(hWnd: a1, lpPoint: (LPPOINT)&rect.right);
    return (HWND)InvalidateRect(hWnd: a1, lpRect: &rect, bErase: false);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19012EBC
 * CodeView symbol: ProgressTimer
 * Demangled: ProgressTimer
 * IDA name: ProgressTimer
 * CodeView module: 12
 * CV address: segment 1 : 0x11EBC
 * Code length: 0x31
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Progress.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Progress.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND __thiscall ProgressTimer(HWND this)
{
  HWND window; // [esp+0h] [ebp-8h]
  int percent; // [esp+4h] [ebp-4h]

  window = this;
  percent = 0;
  if ( sgProgressFcn != nullptr )
    percent = sgProgressFcn(a1: this, a2: 0);
  return ProgressDraw(a1: window, a2: percent);
}

