/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 11
 */


// ==============================================================================
/*
 * EA: 0x19001000
 * CodeView symbol: operator delete
 * Demangled: operator delete
 * IDA name: ??3@YAXPAX@Z
 * CodeView module: 33
 * CV address: segment 1 : 0x0
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl operator delete(void *ptr)
{
  if ( ptr != nullptr )
    SMemFree(a1: ptr, a2: `string', a3: 717, a4: 0);
}


// ==============================================================================
/*
 * EA: 0x19001020
 * CodeView symbol: operator new
 * Demangled: operator new
 * IDA name: ??2@YAPAXI@Z
 * CodeView module: 33
 * CV address: segment 1 : 0x20
 * Code length: 0x1A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__cdecl operator new(unsigned int bytes)
{
  return (void *)SMemAlloc(a1: bytes, a2: `string', a3: 721, a4: 0);
}


// ==============================================================================
/*
 * EA: 0x19001040
 * CodeView symbol: DoRenameAccount
 * Demangled: DoRenameAccount
 * IDA name: ?DoRenameAccount@@YIHPAUHWND__@@PBU_UIPARAMS@@PBD2@Z
 * CodeView module: 33
 * CV address: segment 1 : 0x40
 * Code length: 0x1A2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall DoRenameAccount(HWND a1, const struct _UIPARAMS *a2, const char *currname, const char *newname)
{
  char errorformat[1024]; // [esp+14h] [ebp-800h] BYREF
  char errormessage[1024]; // [esp+414h] [ebp-400h] BYREF

  if ( a2 == nullptr )
    SErrDisplayError(a1: -2062548992, a2: aAccountCpp, a3: 347, a4: aUiparamsNull, a5: 0, a6: 1);
  if ( currname == nullptr )
    SErrDisplayError(a1: -2062548992, a2: aAccountCpp_0, a3: 348, a4: aCurrnameNull, a5: 0, a6: 1);
  if ( newname == nullptr )
    SErrDisplayError(a1: -2062548992, a2: aAccountCpp_1, a3: 349, a4: aNewnameNull, a5: 0, a6: 1);
  if ( _stricmp(String1: currname, String2: newname) == 0 )
    return 1;
  if ( *(_DWORD *)(*((_DWORD *)a2 + 3) + 76) == 0 )
    return 1;
  if ( (*(int (__stdcall **)(_DWORD, _DWORD, _DWORD, _DWORD, const char *, const char *))(*((_DWORD *)a2 + 3) + 76))(
         a1: *((_DWORD *)a2 + 1),
         a2: *((_DWORD *)a2 + 2),
         a3: *((_DWORD *)a2 + 3),
         a4: *((_DWORD *)a2 + 4),
         a5: currname,
         a6: newname) != 0 )
    return 1;
  LocaleLoadString(a1: 0x43Fu, a2: errorformat, a3: 1024);
  _snprintf(Buffer: errormessage, BufferCount: 0x400u, Format: errorformat, newname);
  UiMessageBox(
    a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*((_DWORD *)a2 + 3) + 32),
    a2: a1,
    lpText: errormessage,
    a4: byte_19041200,
    a5: 0x30u);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x190011E2
 * CodeView symbol: DoNewAccount
 * Demangled: DoNewAccount
 * IDA name: ?DoNewAccount@@YIHPAUHWND__@@PAU_UIPARAMS@@PADH2@Z
 * CodeView module: 33
 * CV address: segment 1 : 0x1E2
 * Code length: 0x95
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall DoNewAccount(HWND a1, struct _UIPARAMS *a2, char *name, int namesize, char *description)
{
  HMODULE ModuleHandleA; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-1Ch]
  int v9; // [esp+8h] [ebp-4h]

  dword_190411F8 = a2;
  dword_190411FC = description;
  if ( TOSShow(a1, a2) == 0 )
    return 0;
  SStrCopy(a1: &sgszCurrName, a2: name, a3: 16);
  Dialog = LocaleGetDialog(a1: 0x6Fu);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  v9 = SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: a1, a4: NewDialogProc, a5: 0);
  dword_190411F8 = nullptr;
  if ( v9 != 0 )
    SStrCopy(a1: name, a2: &sgszNewName, a3: namesize);
  return v9;
}


// ==============================================================================
/*
 * EA: 0x19001277
 * CodeView symbol: NewDialogProc
 * Demangled: NewDialogProc
 * IDA name: NewDialogProc
 * CodeView module: 33
 * CV address: segment 1 : 0x277
 * Code length: 0x3AB
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HGDIOBJ __stdcall NewDialogProc(HWND window, UINT message, HDC wparam, HWND lparam)
{
  HWND DlgItem; // eax
  HWND v6; // eax
  HWND v7; // eax
  void *v8; // eax
  HWND v9; // eax
  HWND text; // [esp+8h] [ebp-Ch]
  HWND name; // [esp+Ch] [ebp-8h]
  HWND parent; // [esp+10h] [ebp-4h]

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
  switch ( message )
  {
    case 0x111u:
      if ( (unsigned __int16)wparam > 0x3FEu )
      {
        if ( (unsigned __int16)wparam >= 0x3FFu
          && ((unsigned __int16)wparam <= 0x400u || (unsigned __int16)wparam == 1064)
          && HIWORD(wparam) == 768 )
        {
          sub_19001885(a1: window);
        }
      }
      else
      {
        switch ( (unsigned __int16)wparam )
        {
          case 0x3FEu:
            if ( HIWORD(wparam) == 1538 )
            {
              EditUpdateScrollbar(a1: lparam);
              return nullptr;
            }
            break;
          case 1u:
            EnableWindow(hWnd: lparam, bEnable: false);
            NewAccount(a1: window);
            EnableWindow(hWnd: lparam, bEnable: true);
            sub_19001885(a1: window);
            break;
          case 2u:
            EnableWindow(hWnd: lparam, bEnable: false);
            SrvCancel();
            SErrSetLastError(a1: 1223);
            if ( *(_DWORD *)(*((_DWORD *)dword_190411F8 + 3) + 36) != 0 )
              (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)dword_190411F8 + 3) + 36))(
                a1: 1112425812,
                a2: 1,
                a3: 0);
            SDlgEndDialog(a1: window, a2: 0);
            break;
          default:
            break;
        }
      }
      return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    case 2u:
      if ( dword_190411D0 != nullptr )
      {
        SMemFree(a1: dword_190411D0, a2: aAccountCpp_2, a3: 283, a4: 0);
        dword_190411D0 = nullptr;
      }
      if ( dword_190411F4 != nullptr )
      {
        SMemFree(a1: dword_190411F4, a2: aAccountCpp_3, a3: 287, a4: 0);
        dword_190411F4 = nullptr;
      }
      return nullptr;
    case 0x110u:
      NewLoadArtwork(a1: window, a2: *(_DWORD *)(*((_DWORD *)dword_190411F8 + 3) + 12));
      UiSetFont(a1: window, a2: 0, a3: &unk_1903C070);
      DlgItem = GetDlgItem(hDlg: window, nIDDlgItem: 1);
      EnableWindow(hWnd: DlgItem, bEnable: false);
      name = GetDlgItem(hDlg: window, nIDDlgItem: 1064);
      SendMessageA(hWnd: name, Msg: 0xC5u, wParam: 0xFu, lParam: 0);
      SendMessageA(hWnd: name, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&sgszCurrName);
      v6 = GetDlgItem(hDlg: window, nIDDlgItem: 1023);
      SendMessageA(hWnd: v6, Msg: 0xC5u, wParam: 0xCu, lParam: 0);
      v7 = GetDlgItem(hDlg: window, nIDDlgItem: 1024);
      SendMessageA(hWnd: v7, Msg: 0xC5u, wParam: 0xCu, lParam: 0);
      if ( sgszCurrName == 0 )
      {
        text = GetDlgItem(hDlg: window, nIDDlgItem: 1089);
        SetWindowPos(hWnd: text, hWndInsertAfter: nullptr, X: 0, Y: 0, cx: 0, cy: 0, uFlags: 0x87u);
      }
      v8 = (void *)SetWindowLongA(hWnd: name, nIndex: -4, dwNewLong: (LONG)sub_190016B0);
      SetPropA(hWnd: name, lpString: "OldEditProc", hData: v8);
      SetupHelp(a1: window);
      if ( sgszCurrName != 0 )
      {
        v9 = GetDlgItem(hDlg: window, nIDDlgItem: 1023);
        SetFocus(hWnd: v9);
      }
      else
      {
        SetFocus(hWnd: name);
      }
      return nullptr;
    default:
      return (HGDIOBJ)SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
}


// ==============================================================================
/*
 * EA: 0x19001622
 * CodeView symbol: NewLoadArtwork
 * Demangled: NewLoadArtwork
 * IDA name: NewLoadArtwork
 * CodeView module: 33
 * CV address: segment 1 : 0x622
 * Code length: 0x8E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall NewLoadArtwork(
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
    artid: 0x80000008,
    controltype: controltype,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_190411D0,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 5u,
    controltype: aButton_16,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_190411F4,
    a11: &medBtnSize);
  return SDlgSetControlBitmaps(a1, a2: med_btn_ids, a3: 0, a4: dword_190411F4, a5: &medBtnSize, a6: 1, a7: -1);
}


// ==============================================================================
/*
 * EA: 0x190016B0
 * CodeView symbol: EditFilterProc
 * Demangled: EditFilterProc
 * IDA name: sub_190016B0
 * CodeView module: 33
 * CV address: segment 1 : 0x6B0
 * Code length: 0x62
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __stdcall sub_190016B0(HWND hEdit, UINT message, int wparam, LPARAM lparam)
{
  LRESULT (__stdcall *oldProc)(HWND, UINT, WPARAM, LPARAM); // [esp+4h] [ebp-4h]

  oldProc = (LRESULT (__stdcall *)(HWND, UINT, WPARAM, LPARAM))GetPropA(hWnd: hEdit, lpString: "OldEditProc");
  if ( message == 258 && (IsInvalid(a1: wparam) != 0 || (char)wparam == 32) )
    return 0;
  else
    return CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hEdit, Msg: message, wParam: wparam, lParam: lparam);
}


// ==============================================================================
/*
 * EA: 0x19001712
 * CodeView symbol: SetupHelp
 * Demangled: SetupHelp
 * IDA name: SetupHelp
 * CodeView module: 33
 * CV address: segment 1 : 0x712
 * Code length: 0x173
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall SetupHelp(HWND this)
{
  void *pHelpText; // [esp+4h] [ebp-224h]
  HWND hWndEdit; // [esp+8h] [ebp-220h]
  char szHelpRes[512]; // [esp+Ch] [ebp-21Ch] BYREF
  const void *pText; // [esp+20Ch] [ebp-1Ch]
  signed int i; // [esp+210h] [ebp-18h]
  HGLOBAL hResource; // [esp+214h] [ebp-14h]
  HRSRC hRsrc; // [esp+218h] [ebp-10h]
  HWND hWndScroll; // [esp+21Ch] [ebp-Ch]
  _BYTE *pSearch; // [esp+220h] [ebp-8h]
  signed int nSize; // [esp+224h] [ebp-4h]

  LocaleLoadString(a1: 0x43Au, a2: szHelpRes, a3: 512);
  hRsrc = LocaleFindResource(a1: aText_0, a2: szHelpRes);
  hResource = LoadResource(hModule: global_hinstance, hResInfo: hRsrc);
  pText = LockResource(hResData: hResource);
  nSize = SizeofResource(hModule: global_hinstance, hResInfo: hRsrc);
  pHelpText = (void *)SMemAlloc(a1: nSize + 1, a2: aAccountCpp_4, a3: 113, a4: 0);
  if ( pHelpText != nullptr )
  {
    memcpy(a1: pHelpText, Src: pText, Size: nSize);
    *((_BYTE *)pHelpText + nSize) = 0;
    i = 0;
    pSearch = pHelpText;
    while ( i < nSize )
    {
      if ( *pSearch == 92 )
      {
        *pSearch = 0;
        break;
      }
      ++i;
      ++pSearch;
    }
    hWndScroll = GetDlgItem(hDlg: this, nIDDlgItem: 1052);
    hWndEdit = GetDlgItem(hDlg: this, nIDDlgItem: 1022);
    SendMessageA(hWnd: hWndEdit, Msg: 0xCu, wParam: 0, lParam: (LPARAM)pHelpText);
    SMemFree(a1: pHelpText, a2: aAccountCpp_5, a3: 130, a4: 0);
    ScrollbarLink(a1: hWndEdit, a2: hWndScroll);
    EditUpdateScrollbar(a1: hWndEdit);
  }
}


// ==============================================================================
/*
 * EA: 0x19001885
 * CodeView symbol: CheckOk
 * Demangled: CheckOk
 * IDA name: sub_19001885
 * CodeView module: 33
 * CV address: segment 1 : 0x885
 * Code length: 0x85
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __thiscall sub_19001885(HWND this)
{
  HWND DlgItem; // eax
  int v3; // [esp-4h] [ebp-Ch]
  LRESULT len; // [esp+4h] [ebp-4h]
  int lena; // [esp+4h] [ebp-4h]

  len = SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1064, Msg: 0xEu, wParam: 0, lParam: 0);
  lena = SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1023, Msg: 0xEu, wParam: 0, lParam: 0) * len;
  v3 = SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1024, Msg: 0xEu, wParam: 0, lParam: 0) * lena;
  DlgItem = GetDlgItem(hDlg: this, nIDDlgItem: 1);
  return EnableWindow(hWnd: DlgItem, bEnable: v3);
}


// ==============================================================================
/*
 * EA: 0x1900190A
 * CodeView symbol: NewAccount
 * Demangled: NewAccount
 * IDA name: NewAccount
 * CodeView module: 33
 * CV address: segment 1 : 0x90A
 * Code length: 0x2AC
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall NewAccount(HWND this)
{
  char szPassErr[256]; // [esp+4h] [ebp-540h] BYREF
  _BYTE szPassword2[16]; // [esp+104h] [ebp-440h] BYREF
  char szPassword[16]; // [esp+114h] [ebp-430h] BYREF
  char szNewName[16]; // [esp+124h] [ebp-420h] BYREF
  unsigned int errorcode; // [esp+134h] [ebp-410h] BYREF
  char errormessage[1024]; // [esp+138h] [ebp-40Ch] BYREF
  HWND hPass2; // [esp+538h] [ebp-Ch]
  HWND hPass1; // [esp+53Ch] [ebp-8h]
  HWND hName; // [esp+540h] [ebp-4h]

  hName = GetDlgItem(hDlg: this, nIDDlgItem: 1064);
  hPass1 = GetDlgItem(hDlg: this, nIDDlgItem: 1023);
  hPass2 = GetDlgItem(hDlg: this, nIDDlgItem: 1024);
  if ( *(_DWORD *)(*((_DWORD *)dword_190411F8 + 3) + 36) != 0 )
    (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)dword_190411F8 + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
  sgszNewName = 0;
  SendMessageA(hWnd: hName, Msg: 0xDu, wParam: 0x10u, lParam: (LPARAM)szNewName);
  SendMessageA(hWnd: hPass1, Msg: 0xDu, wParam: 0xDu, lParam: (LPARAM)szPassword);
  SendMessageA(hWnd: hPass2, Msg: 0xDu, wParam: 0xDu, lParam: (LPARAM)szPassword2);
  if ( _tcscmp(a1: szPassword, a2: szPassword2) != 0 )
  {
    LocaleLoadString(a1: 0x429u, a2: szPassErr, a3: 256);
    UiMessageBox(
      a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*((_DWORD *)dword_190411F8 + 3) + 32),
      a2: this,
      lpText: szPassErr,
      a4: byte_19041208,
      a5: 0x30u);
    SendMessageA(hWnd: hPass1, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_1904120C);
    SendMessageA(hWnd: hPass2, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19041210);
    SetFocus(hWnd: hPass1);
    return 0;
  }
  if ( DoRenameAccount(a1: this, a2: dword_190411F8, currname: &sgszCurrName, newname: szNewName) == 0 )
    return 0;
  errorcode = 0;
  if ( SrvCreateAccount(a1: szNewName, a2: szPassword) != 0 )
  {
    SStrCopy(a1: &sgszNewName, a2: szNewName, a3: 16);
    if ( SrvLogon(a1: szNewName, a2: dword_190411FC, a3: szPassword, a4: &errorcode) != 0 )
    {
      SDlgEndDialog(a1: this, a2: 1);
      return 1;
    }
  }
  else if ( DoRenameAccount(a1: this, a2: dword_190411F8, currname: szNewName, newname: &sgszCurrName) == 0 )
  {
    return 0;
  }
  if ( errorcode != 0 )
  {
    LocaleLoadString(a1: 0x43Cu, a2: errormessage, a3: 1024);
    SDlgEndDialog(a1: this, a2: 0);
  }
  else
  {
    LocaleLoadString(a1: 0x42Bu, a2: errormessage, a3: 1024);
  }
  UiMessageBox(
    a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*((_DWORD *)dword_190411F8 + 3) + 32),
    a2: this,
    lpText: errormessage,
    a4: byte_19041214,
    a5: 0x30u);
  SendMessageA(hWnd: hName, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19041218);
  SendMessageA(hWnd: hPass1, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_1904121C);
  SendMessageA(hWnd: hPass2, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19041220);
  SetFocus(hWnd: hName);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x19001BC0
 * CodeView symbol: _tcscmp
 * Demangled: _tcscmp
 * IDA name: @_tcscmp@8
 * CodeView module: 33
 * CV address: segment 1 : 0xBC0
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Account.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Account.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall _tcscmp(const unsigned __int8 *a1, const unsigned __int8 *a2)
{
  return _mbscmp(Str1: a1, Str2: a2);
}

