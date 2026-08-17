/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Password.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Password.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 7
 */


// ==============================================================================
/*
 * EA: 0x19011C70
 * CodeView symbol: DoNewPassword
 * Demangled: DoNewPassword
 * IDA name: ?DoNewPassword@@YIHPAUHWND__@@PAU_UIPARAMS@@PBD@Z
 * CodeView module: 14
 * CV address: segment 1 : 0x10C70
 * Code length: 0x50
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Password.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Password.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall DoNewPassword(HWND a1, struct _UIPARAMS *a2, const char *name)
{
  HMODULE ModuleHandleA; // eax
  int result; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-1Ch]

  dword_19042CD4 = (int)a2;
  Dialog = LocaleGetDialog(a1: 0x71u);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  result = SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: a1, a4: sub_19011CC0, a5: name);
  dword_19042CD4 = 0;
  return result;
}


// ==============================================================================
/*
 * EA: 0x19011CC0
 * CodeView symbol: DialogProc
 * Demangled: DialogProc
 * IDA name: sub_19011CC0
 * CodeView module: 14
 * CV address: segment 1 : 0x10CC0
 * Code length: 0x25B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Password.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Password.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HGDIOBJ __stdcall sub_19011CC0(HWND window, UINT message, HDC wparam, HWND lparam)
{
  HWND parent; // [esp+8h] [ebp-4h]

  if ( message > 0x111 )
  {
    if ( message == 274 )
    {
      if ( wparam == (HDC)61536 )
      {
        parent = GetParent(hWnd: window);
        SDlgEndDialog(a1: window, a2: 0);
        PostMessageA(hWnd: parent, Msg: message, wParam: 0xF060u, lParam: (LPARAM)lparam);
        return nullptr;
      }
    }
    else if ( message == 312 && GetWindowLongA(hWnd: lparam, nIndex: -12) == 1030 )
    {
      SetTextColor(hdc: wparam, color: 0xFFFFu);
      return GetStockObject(i: 5);
    }
    return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message == 273 )
  {
    if ( (unsigned __int16)wparam > 0x401u )
    {
      if ( (unsigned __int16)wparam != 1064 && ((unsigned __int16)wparam <= 0x43Eu || (unsigned __int16)wparam > 0x440u) )
        return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    else if ( (unsigned __int16)wparam != 1025 )
    {
      if ( (unsigned __int16)wparam == 1 )
      {
        EnableWindow(hWnd: lparam, bEnable: false);
        if ( ChangePassword(a1: window) != 0 )
          SDlgEndDialog(a1: window, a2: 1);
        else
          EnableWindow(hWnd: lparam, bEnable: true);
        return nullptr;
      }
      if ( (unsigned __int16)wparam == 2 )
      {
        EnableWindow(hWnd: lparam, bEnable: false);
        SrvCancel();
        SErrSetLastError(a1: 1223);
        if ( *(_DWORD *)(*(_DWORD *)(dword_19042CD4 + 12) + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(dword_19042CD4 + 12) + 36))(
            a1: 1112425812,
            a2: 1,
            a3: 0);
        SDlgEndDialog(a1: window, a2: 0);
        return nullptr;
      }
      return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( HIWORD(wparam) == 768 )
      CheckOk(a1: window);
    return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message != 2 )
  {
    if ( message == 272 )
    {
      InitPassword(a1: window, a2: lparam);
      return nullptr;
    }
    return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( sgNewBkgBmp != nullptr )
  {
    SMemFree(a1: sgNewBkgBmp, a2: aPasswordCpp, a3: 235, a4: 0);
    sgNewBkgBmp = nullptr;
  }
  if ( dword_19042CD0 != nullptr )
  {
    SMemFree(a1: dword_19042CD0, a2: aPasswordCpp_0, a3: 239, a4: 0);
    dword_19042CD0 = nullptr;
  }
  return nullptr;
}


// ==============================================================================
/*
 * EA: 0x19011F1B
 * CodeView symbol: ChangePassword
 * Demangled: ChangePassword
 * IDA name: ChangePassword
 * CodeView module: 14
 * CV address: segment 1 : 0x10F1B
 * Code length: 0x236
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Password.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Password.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall ChangePassword(HWND this)
{
  char szPassErr[256]; // [esp+4h] [ebp-554h] BYREF
  unsigned __int8 szNewPassword2[16]; // [esp+104h] [ebp-454h] BYREF
  char szOldPassword[16]; // [esp+114h] [ebp-444h] BYREF
  unsigned int errorcode; // [esp+124h] [ebp-434h] BYREF
  HWND hNewPass2; // [esp+128h] [ebp-430h]
  HWND hOldPass; // [esp+12Ch] [ebp-42Ch]
  HWND hNewPass1; // [esp+130h] [ebp-428h]
  char szAccountErr[1024]; // [esp+134h] [ebp-424h] BYREF
  unsigned __int8 szNewPassword[16]; // [esp+534h] [ebp-24h] BYREF
  char szName[16]; // [esp+544h] [ebp-14h] BYREF
  HWND hName; // [esp+554h] [ebp-4h]

  if ( *(_DWORD *)(*(_DWORD *)(dword_19042CD4 + 12) + 36) != 0 )
    (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(dword_19042CD4 + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
  hName = GetDlgItem(hDlg: this, nIDDlgItem: 1064);
  hOldPass = GetDlgItem(hDlg: this, nIDDlgItem: 1087);
  hNewPass1 = GetDlgItem(hDlg: this, nIDDlgItem: 1088);
  hNewPass2 = GetDlgItem(hDlg: this, nIDDlgItem: 1025);
  SendMessageA(hWnd: hName, Msg: 0xDu, wParam: 0x10u, lParam: (LPARAM)szName);
  SendMessageA(hWnd: hOldPass, Msg: 0xDu, wParam: 0xDu, lParam: (LPARAM)szOldPassword);
  SendMessageA(hWnd: hNewPass1, Msg: 0xDu, wParam: 0xDu, lParam: (LPARAM)szNewPassword);
  SendMessageA(hWnd: hNewPass2, Msg: 0xDu, wParam: 0xDu, lParam: (LPARAM)szNewPassword2);
  if ( _tcscmp(a1: szNewPassword, a2: szNewPassword2) != 0 )
  {
    LocaleLoadString(a1: 0x429u, a2: szPassErr, buffersize: 256);
    UiMessageBox(
      a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*(_DWORD *)(dword_19042CD4 + 12) + 32),
      a2: this,
      lpText: szPassErr,
      lpCaption: byte_19042CD8,
      uType: 0x30u);
    SendMessageA(hWnd: hNewPass1, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042CDC);
    SendMessageA(hWnd: hNewPass2, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042CE0);
    SetFocus(hWnd: hNewPass1);
    return 0;
  }
  else if ( SrvChangePass(a1: szName, a2: szOldPassword, a3: (const char *)szNewPassword, a4: &errorcode) != 0 )
  {
    return 1;
  }
  else
  {
    LocaleLoadString(a1: 0x43Bu, a2: szAccountErr, buffersize: 1024);
    UiMessageBox(
      a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*(_DWORD *)(dword_19042CD4 + 12) + 32),
      a2: this,
      lpText: szAccountErr,
      lpCaption: byte_19042CE4,
      uType: 0x30u);
    SendMessageA(hWnd: hName, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042CE8);
    SendMessageA(hWnd: hOldPass, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042CEC);
    SendMessageA(hWnd: hNewPass1, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042CF0);
    SendMessageA(hWnd: hNewPass2, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042CF4);
    SetFocus(hWnd: hName);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19012151
 * CodeView symbol: CheckOk
 * Demangled: CheckOk
 * IDA name: CheckOk
 * CodeView module: 14
 * CV address: segment 1 : 0x11151
 * Code length: 0x85
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Password.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Password.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __thiscall CheckOk(HWND this)
{
  HWND DlgItem; // eax
  int v3; // [esp-4h] [ebp-Ch]
  LRESULT len; // [esp+4h] [ebp-4h]
  int lena; // [esp+4h] [ebp-4h]

  len = SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1064, Msg: 0xEu, wParam: 0, lParam: 0);
  lena = SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1088, Msg: 0xEu, wParam: 0, lParam: 0) * len;
  v3 = SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1025, Msg: 0xEu, wParam: 0, lParam: 0) * lena;
  DlgItem = GetDlgItem(hDlg: this, nIDDlgItem: 1);
  return EnableWindow(hWnd: DlgItem, bEnable: v3);
}


// ==============================================================================
/*
 * EA: 0x190121D6
 * CodeView symbol: InitPassword
 * Demangled: InitPassword
 * IDA name: InitPassword
 * CodeView module: 14
 * CV address: segment 1 : 0x111D6
 * Code length: 0xFD
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Password.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Password.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND __fastcall InitPassword(HWND a1, LPARAM a2)
{
  HWND DlgItem; // eax
  void *v3; // eax
  HWND v4; // eax
  HWND nameedit; // [esp+8h] [ebp-4h]

  sub_190122D3(a1, a2: *(_DWORD *)(*(_DWORD *)(dword_19042CD4 + 12) + 12));
  UiSetFont(a1, a2: 0, a3: &unk_1903CF1C);
  DlgItem = GetDlgItem(hDlg: a1, nIDDlgItem: 1);
  EnableWindow(hWnd: DlgItem, bEnable: false);
  nameedit = GetDlgItem(hDlg: a1, nIDDlgItem: 1064);
  SendMessageA(hWnd: nameedit, Msg: 0xC5u, wParam: 0xFu, lParam: 0);
  SendMessageA(hWnd: nameedit, Msg: 0xCu, wParam: 0, lParam: a2);
  SendDlgItemMessageA(hDlg: a1, nIDDlgItem: 1087, Msg: 0xC5u, wParam: 0xCu, lParam: 0);
  SendDlgItemMessageA(hDlg: a1, nIDDlgItem: 1088, Msg: 0xC5u, wParam: 0xCu, lParam: 0);
  SendDlgItemMessageA(hDlg: a1, nIDDlgItem: 1025, Msg: 0xC5u, wParam: 0xCu, lParam: 0);
  v3 = (void *)SetWindowLongA(hWnd: nameedit, nIndex: -4, dwNewLong: (LONG)EditFilterProc);
  SetPropA(hWnd: nameedit, lpString: "OldEditProc", hData: v3);
  v4 = GetDlgItem(hDlg: a1, nIDDlgItem: 1087);
  return SetFocus(hWnd: v4);
}


// ==============================================================================
/*
 * EA: 0x190122D3
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: sub_190122D3
 * CodeView module: 14
 * CV address: segment 1 : 0x112D3
 * Code length: 0x8E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Password.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Password.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_190122D3(
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
    artid: 0x80000009,
    controltype: byte_19042CF8,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgNewBkgBmp,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 5u,
    controltype: aButton_2,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19042CD0,
    a11: &medBtnSize);
  return SDlgSetControlBitmaps(a1, a2: med_btn_ids, a3: 0, a4: dword_19042CD0, a5: &medBtnSize, a6: 1, a7: -1);
}


// ==============================================================================
/*
 * EA: 0x19012361
 * CodeView symbol: EditFilterProc
 * Demangled: EditFilterProc
 * IDA name: EditFilterProc
 * CodeView module: 14
 * CV address: segment 1 : 0x11361
 * Code length: 0x62
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Password.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Password.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __stdcall EditFilterProc(HWND hEdit, UINT message, int wparam, LPARAM lparam)
{
  LRESULT (__stdcall *oldProc)(HWND, UINT, WPARAM, LPARAM); // [esp+4h] [ebp-4h]

  oldProc = (LRESULT (__stdcall *)(HWND, UINT, WPARAM, LPARAM))GetPropA(hWnd: hEdit, lpString: "OldEditProc");
  if ( message == 258 && (IsInvalid(a1: wparam) != 0 || (char)wparam == 32) )
    return 0;
  else
    return CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hEdit, Msg: message, wParam: wparam, lParam: lparam);
}

