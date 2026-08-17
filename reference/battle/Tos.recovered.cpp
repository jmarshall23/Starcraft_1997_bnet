/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 14
 */


// ==============================================================================
/*
 * EA: 0x1901D090
 * CodeView symbol: TOSInitialize
 * Demangled: TOSInitialize
 * IDA name: ?TOSInitialize@@YIXXZ
 * CodeView module: 5
 * CV address: segment 1 : 0x1C090
 * Code length: 0x81
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl TOSInitialize()
{
  LCID Locale; // eax
  CHAR code[64]; // [esp+0h] [ebp-C0h] BYREF
  char filename[128]; // [esp+40h] [ebp-80h] BYREF

  sgpTOSFile = 0;
  Locale = LocaleGetLocale();
  if ( GetLocaleInfoA(Locale, LCType: 7u, lpLCData: code, cchData: 64) == 0 )
    code[0] = 0;
  SStrCopy(a1: filename, a2: aTos, a3: 128);
  SStrPack(a1: filename, a2: code, a3: 128);
  SStrPack(a1: filename, a2: aTxt, a3: 128);
  SrvRequestFile(a1: filename, a2: 0x1Au);
}


// ==============================================================================
/*
 * EA: 0x1901D111
 * CodeView symbol: TOSUpdate
 * Demangled: TOSUpdate
 * IDA name: ?TOSUpdate@@YIXPAE@Z
 * CodeView module: 5
 * CV address: segment 1 : 0x1C111
 * Code length: 0x82
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall TOSUpdate(unsigned __int8 *a1)
{
  int length; // [esp+4h] [ebp-4h]

  if ( sgpTOSFile != 0 )
  {
    SMemFree(a1: sgpTOSFile, a2: aTosCpp, a3: 298, a4: 0);
    sgpTOSFile = 0;
  }
  length = SStrLen(a1);
  sgpTOSFile = SMemAlloc(a1: length + 1, a2: aTosCpp_0, a3: 302, a4: 0);
  SStrCopy(a1: sgpTOSFile, a2: a1, a3: length + 1);
  ShowTOS(a1: sghTOSWindow);
}


// ==============================================================================
/*
 * EA: 0x1901D193
 * CodeView symbol: ShowTOS
 * Demangled: ShowTOS
 * IDA name: ShowTOS
 * CodeView module: 5
 * CV address: segment 1 : 0x1C193
 * Code length: 0xBC
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall ShowTOS(HWND this)
{
  HWND DlgItem; // eax
  HWND v3; // eax
  HWND edit; // [esp+4h] [ebp-4h]

  if ( this == nullptr || sgpTOSFile == 0 )
    return 0;
  edit = GetDlgItem(hDlg: this, nIDDlgItem: 1089);
  SendMessageA(hWnd: edit, Msg: 0xCu, wParam: 0, lParam: sgpTOSFile);
  DlgItem = GetDlgItem(hDlg: this, nIDDlgItem: 1089);
  EditUpdateScrollbar(a1: DlgItem);
  if ( sghTOSCancelDlg != 0 )
    SDlgEndDialog(a1: sghTOSCancelDlg, a2: 1);
  v3 = GetDlgItem(hDlg: this, nIDDlgItem: 1);
  EnableWindow(hWnd: v3, bEnable: true);
  SetFocus(hWnd: edit);
  SendMessageA(hWnd: edit, Msg: 0xB1u, wParam: 0xFFFFFFFF, lParam: 0);
  InvalidateRect(hWnd: this, lpRect: nullptr, bErase: false);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901D24F
 * CodeView symbol: TOSDestroy
 * Demangled: TOSDestroy
 * IDA name: ?TOSDestroy@@YIXXZ
 * CodeView module: 5
 * CV address: segment 1 : 0x1C24F
 * Code length: 0x2F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl TOSDestroy()
{
  if ( sgpTOSFile != 0 )
  {
    SMemFree(a1: sgpTOSFile, a2: aTosCpp_1, a3: 311, a4: 0);
    sgpTOSFile = 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901D27E
 * CodeView symbol: TOSShow
 * Demangled: TOSShow
 * IDA name: ?TOSShow@@YIHPAUHWND__@@PAU_UIPARAMS@@@Z
 * CodeView module: 5
 * CV address: segment 1 : 0x1C27E
 * Code length: 0x4C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TOSShow(HWND a1, struct _UIPARAMS *a2)
{
  HMODULE ModuleHandleA; // eax
  int result; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-1Ch]

  sgpUiParams = (int)a2;
  Dialog = LocaleGetDialog(a1: 0x73u);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  result = SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: a1, a4: DialogProc, a5: 0);
  sgpUiParams = 0;
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901D2CA
 * CodeView symbol: DialogProc
 * Demangled: DialogProc
 * IDA name: DialogProc
 * CodeView module: 5
 * CV address: segment 1 : 0x1C2CA
 * Code length: 0x1CD
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HGDIOBJ __stdcall DialogProc(HWND window, UINT message, HDC wparam, HWND lparam)
{
  HWND parent; // [esp+8h] [ebp-4h]

  if ( message > 0x112 )
  {
    if ( message == 312 )
    {
      if ( GetWindowLongA(hWnd: lparam, nIndex: -12) == 1030 )
      {
        SetTextColor(hdc: wparam, color: 0xFFFFu);
        return GetStockObject(i: 5);
      }
    }
    else if ( message == 1524 )
    {
      ShowTOSCancel(a1: window);
    }
    return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  switch ( message )
  {
    case 0x112u:
      if ( wparam == (HDC)61536 )
      {
        parent = GetParent(hWnd: window);
        SDlgEndDialog(a1: window, a2: 0);
        PostMessageA(hWnd: parent, Msg: message, wParam: 0xF060u, lParam: (LPARAM)lparam);
        return nullptr;
      }
      return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    case 2u:
      Destroy(a1: window);
      return nullptr;
    case 0x110u:
      Init(a1: window);
      return nullptr;
    default:
      break;
  }
  if ( message != 273 )
    return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  if ( (unsigned __int16)wparam == 1 )
  {
    if ( *(_DWORD *)(*(_DWORD *)(sgpUiParams + 12) + 36) != 0 )
      (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(sgpUiParams + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
    SDlgEndDialog(a1: window, a2: 1);
    return nullptr;
  }
  else
  {
    if ( (unsigned __int16)wparam != 2 )
    {
      if ( (unsigned __int16)wparam == 1089 && HIWORD(wparam) == 1538 )
      {
        EditUpdateScrollbar(a1: lparam);
        return nullptr;
      }
      return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( *(_DWORD *)(*(_DWORD *)(sgpUiParams + 12) + 36) != 0 )
      (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(sgpUiParams + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
    SDlgEndDialog(a1: window, a2: 0);
    return nullptr;
  }
}


// ==============================================================================
/*
 * EA: 0x1901D497
 * CodeView symbol: ShowTOSCancel
 * Demangled: ShowTOSCancel
 * IDA name: ShowTOSCancel
 * CodeView module: 5
 * CV address: segment 1 : 0x1C497
 * Code length: 0x45
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall ShowTOSCancel(void *this)
{
  HMODULE ModuleHandleA; // eax
  int result; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-18h]

  Dialog = LocaleGetDialog(a1: 0x74u);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  result = SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: this, a4: TOSCancelProc, a5: 0);
  if ( result == 0 )
    return SDlgEndDialog(a1: this, a2: 0);
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901D4DC
 * CodeView symbol: TOSCancelProc
 * Demangled: TOSCancelProc
 * IDA name: TOSCancelProc
 * CodeView module: 5
 * CV address: segment 1 : 0x1C4DC
 * Code length: 0x94
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall TOSCancelProc(int window, int message, int wparam, int lparam)
{
  if ( message == 2 )
  {
    TOSCancelDestroy(a1: window);
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message == 272 )
  {
    TOSCancelInit(a1: window);
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message != 273 || (unsigned __int16)wparam != 2 )
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  if ( *(_DWORD *)(*(_DWORD *)(sgpUiParams + 12) + 36) != 0 )
    (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(sgpUiParams + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
  SDlgEndDialog(a1: window, a2: 0);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1901D570
 * CodeView symbol: TOSCancelInit
 * Demangled: TOSCancelInit
 * IDA name: TOSCancelInit
 * CodeView module: 5
 * CV address: segment 1 : 0x1C570
 * Code length: 0xC5
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TOSCancelInit(HWND this)
{
  _DWORD btn_ids[2]; // [esp+4h] [ebp-20h] BYREF
  _DWORD btn_static[2]; // [esp+Ch] [ebp-18h] BYREF
  struct tagSIZE bgSize; // [esp+14h] [ebp-10h] BYREF
  struct tagSIZE sizeBtns; // [esp+1Ch] [ebp-8h] BYREF

  sghTOSCancelDlg = (int)this;
  btn_ids[0] = 2;
  btn_ids[1] = 0;
  btn_static[0] = 1030;
  btn_static[1] = 0;
  UiLoadArtwork(
    a1: *(int (__stdcall **)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))(*(_DWORD *)(sgpUiParams + 12) + 12),
    a2: this,
    hWndParent: nullptr,
    artid: 0xCu,
    controltype: byte_19043C30,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgCancelBmp,
    a11: &bgSize);
  UiLoadArtwork(
    a1: *(int (__stdcall **)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))(*(_DWORD *)(sgpUiParams + 12) + 12),
    a2: nullptr,
    hWndParent: nullptr,
    artid: 6u,
    controltype: aButton,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgButtonBmp,
    a11: &sizeBtns);
  SDlgSetControlBitmaps(a1: this, a2: btn_ids, a3: 0, a4: sgButtonBmp, a5: &sizeBtns, a6: 1, a7: -1);
  return SDlgSetControlBitmaps(a1: this, a2: btn_static, a3: 0, a4: sgCancelBmp, a5: &bgSize, a6: 2, a7: -1);
}


// ==============================================================================
/*
 * EA: 0x1901D635
 * CodeView symbol: TOSCancelDestroy
 * Demangled: TOSCancelDestroy
 * IDA name: TOSCancelDestroy
 * CodeView module: 5
 * CV address: segment 1 : 0x1C635
 * Code length: 0x64
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int TOSCancelDestroy()
{
  int result; // eax

  sghTOSCancelDlg = 0;
  if ( sgCancelBmp != nullptr )
  {
    result = SMemFree(a1: sgCancelBmp, a2: aTosCpp_2, a3: 78, a4: 0);
    sgCancelBmp = nullptr;
  }
  if ( sgButtonBmp != nullptr )
  {
    result = SMemFree(a1: sgButtonBmp, a2: aTosCpp_3, a3: 83, a4: 0);
    sgButtonBmp = nullptr;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901D699
 * CodeView symbol: Destroy
 * Demangled: Destroy
 * IDA name: Destroy
 * CodeView module: 5
 * CV address: segment 1 : 0x1C699
 * Code length: 0x1D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall Destroy(void *this)
{
  sghTOSWindow = 0;
  return DestroyArtwork(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x1901D6B6
 * CodeView symbol: DestroyArtwork
 * Demangled: DestroyArtwork
 * IDA name: DestroyArtwork
 * CodeView module: 5
 * CV address: segment 1 : 0x1C6B6
 * Code length: 0x60
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int DestroyArtwork()
{
  int result; // eax

  if ( sgBackgroundBmp != nullptr )
  {
    result = SMemFree(a1: sgBackgroundBmp, a2: aTosCpp_4, a3: 153, a4: 0);
    sgBackgroundBmp = nullptr;
  }
  if ( sgMedBtnBmp != nullptr )
  {
    result = SMemFree(a1: sgMedBtnBmp, a2: aTosCpp_5, a3: 157, a4: 0);
    sgMedBtnBmp = nullptr;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901D716
 * CodeView symbol: Init
 * Demangled: Init
 * IDA name: Init
 * CodeView module: 5
 * CV address: segment 1 : 0x1C716
 * Code length: 0x9F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall Init(HWND this)
{
  HWND DlgItem; // eax
  HWND v2; // eax
  int result; // eax
  HWND edit; // [esp+4h] [ebp-4h]

  LoadArtwork(a1: this, a2: *(_DWORD *)(*(_DWORD *)(sgpUiParams + 12) + 12));
  UiSetFont(a1: this, a2: 0, a3: &smalltbl);
  DlgItem = GetDlgItem(hDlg: this, nIDDlgItem: 1);
  EnableWindow(hWnd: DlgItem, bEnable: false);
  edit = GetDlgItem(hDlg: this, nIDDlgItem: 1089);
  v2 = GetDlgItem(hDlg: this, nIDDlgItem: 1052);
  ScrollbarLink(a1: edit, a2: v2);
  EditUpdateScrollbar(a1: edit);
  sghTOSWindow = (int)this;
  result = ShowTOS(this);
  if ( result == 0 )
    return PostMessageA(hWnd: this, Msg: 0x5F4u, wParam: 0, lParam: 0);
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901D7B5
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: LoadArtwork
 * CodeView module: 5
 * CV address: segment 1 : 0x1C7B5
 * Code length: 0x8E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Tos.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Tos.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall LoadArtwork(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  struct tagSIZE medBtnSize; // [esp+8h] [ebp-1Ch] BYREF
  struct tagSIZE bgSize; // [esp+10h] [ebp-14h] BYREF
  _DWORD med_btn_ids[3]; // [esp+18h] [ebp-Ch] BYREF

  med_btn_ids[0] = 1;
  med_btn_ids[1] = 2;
  med_btn_ids[2] = 0;
  UiLoadArtwork(
    a1: a2,
    a2: a1,
    hWndParent: nullptr,
    artid: 0x8000000E,
    controltype: byte_19043C34,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgBackgroundBmp,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 5u,
    controltype: aButton_0,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgMedBtnBmp,
    a11: &medBtnSize);
  return SDlgSetControlBitmaps(a1, a2: med_btn_ids, a3: 0, a4: sgMedBtnBmp, a5: &medBtnSize, a6: 1, a7: -1);
}

