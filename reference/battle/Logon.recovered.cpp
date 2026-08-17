/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 52
 */


// ==============================================================================
/*
 * EA: 0x1900FE70
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: unknown_libname_4
 * CodeView module: 16
 * CV address: segment 1 : 0xEE70
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_4()
{
  sub_1900FE7F();
  return sub_1900FE8E();
}


// ==============================================================================
/*
 * EA: 0x1900FE7F
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: sub_1900FE7F
 * CodeView module: 16
 * CV address: segment 1 : 0xEE7F
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1900FE7F()
{
  return TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>(a1: &sgProfileList);
}


// ==============================================================================
/*
 * EA: 0x1900FE8E
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: sub_1900FE8E
 * CodeView module: 16
 * CV address: segment 1 : 0xEE8E
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1900FE8E()
{
  return atexit(a1: sub_1900FEA0);
}


// ==============================================================================
/*
 * EA: 0x1900FEA0
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: sub_1900FEA0
 * CodeView module: 16
 * CV address: segment 1 : 0xEEA0
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_1900FEA0()
{
  TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::~TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>(a1: &sgProfileList);
}


// ==============================================================================
/*
 * EA: 0x1900FEAF
 * CodeView symbol: LogonDialogProc
 * Demangled: LogonDialogProc
 * IDA name: ?LogonDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 16
 * CV address: segment 1 : 0xEEAF
 * Code length: 0x4D3
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall LogonDialogProc(HWND window, UINT message, HDC wparam, HWND lparam)
{
  HWND v5; // eax
  HWND DlgItem; // eax
  HWND FrameWindow; // eax
  HWND v8; // eax
  int v9; // eax
  LRESULT v10; // [esp-4h] [ebp-1Ch]
  HWND passedit; // [esp+8h] [ebp-10h]
  HWND nameedit; // [esp+Ch] [ebp-Ch]
  HWND parent; // [esp+14h] [ebp-4h]

  if ( message > 0x112 )
  {
    switch ( message )
    {
      case 0x113u:
        if ( wparam == (HDC)1 )
        {
          DlgItem = GetDlgItem(hDlg: window, nIDDlgItem: 1040);
          LogoAnimate(a1: window, a2: DlgItem);
        }
        else if ( wparam == (HDC)123 && gbConnectionLost != 0 )
        {
          SDlgKillTimer(a1: window, a2: 275);
          gbConnectionLost = 0;
          UiConnectionLost(a1: *((struct _SNETUIDATA **)dword_19042C10 + 3), a2: window);
        }
        break;
      case 0x138u:
        if ( GetWindowLongA(hWnd: lparam, nIndex: -12) == 1030 )
        {
          SetTextColor(hdc: wparam, color: 0xFFFFu);
          return (int)GetStockObject(i: 5);
        }
        break;
      case 0x469u:
        UiNotification();
        return 1;
      default:
        break;
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message == 274 )
  {
    if ( wparam == (HDC)61536 )
    {
      parent = GetParent(hWnd: window);
      SDlgEndDialog(a1: window, a2: 0);
      PostMessageA(hWnd: parent, Msg: message, wParam: 0xF060u, lParam: (LPARAM)lparam);
      return 0;
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message > 0x110 )
  {
    if ( (unsigned __int16)wparam > 0x3F2u )
    {
      if ( (unsigned __int16)wparam == 1054 )
      {
        OnNewAccount(a1: window, a2: lparam);
        return 0;
      }
      if ( (unsigned __int16)wparam == 1055 )
      {
        EnableWindow(hWnd: lparam, bEnable: false);
        sub_19010AC5(a1: window);
        EnableWindow(hWnd: lparam, bEnable: true);
        return 0;
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    switch ( (unsigned __int16)wparam )
    {
      case 0x3F2u:
        if ( HIWORD(wparam) == 768 )
        {
          v10 = SendMessageA(hWnd: lparam, Msg: 0xEu, wParam: 0, lParam: 0);
          v5 = GetDlgItem(hDlg: window, nIDDlgItem: 1);
          EnableWindow(hWnd: v5, bEnable: v10);
          SetProfileBmp(a1: window);
          return 0;
        }
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
      case 1u:
        if ( *(_DWORD *)(*((_DWORD *)dword_19042C10 + 3) + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)dword_19042C10 + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
        LogonAttempt(a1: window, a2: lparam);
        return 0;
      case 2u:
        if ( *(_DWORD *)(*((_DWORD *)dword_19042C10 + 3) + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)dword_19042C10 + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
        SrvCancel();
        SErrSetLastError(a1: 1223);
        SDlgEndDialog(a1: window, a2: 0);
        SetCursor(hCursor: nullptr);
        return 0;
      default:
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
  }
  else
  {
    if ( message != 272 )
    {
      if ( message == 2 )
      {
        ghWndUiMainParent = nullptr;
        SDlgKillTimer(a1: window, a2: 1);
        SDlgKillTimer(a1: window, a2: 123);
        UiVidFadeOut(a1: 10);
        sub_19010382(a1: window);
      }
      else if ( message > 0x103 && message <= 0x105 )
      {
        FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
        SendMessageA(hWnd: FrameWindow, Msg: message, wParam: (WPARAM)wparam, lParam: (LPARAM)lparam);
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    ghWndUiMainParent = window;
    if ( *(_DWORD *)(*((_DWORD *)dword_19042C10 + 3) + 12) != 0 )
      sub_19010442(a1: window, a2: *(_DWORD *)(*((_DWORD *)dword_19042C10 + 3) + 12));
    v8 = GetDlgItem(hDlg: window, nIDDlgItem: 1);
    EnableWindow(hWnd: v8, bEnable: false);
    nameedit = GetDlgItem(hDlg: window, nIDDlgItem: 1010);
    passedit = GetDlgItem(hDlg: window, nIDDlgItem: 1011);
    SendMessageA(hWnd: nameedit, Msg: 0xC5u, wParam: 0xFu, lParam: 0);
    SendMessageA(hWnd: passedit, Msg: 0xC5u, wParam: 0xCu, lParam: 0);
    if ( *(_BYTE *)lparam != 0 )
    {
      SendMessageA(hWnd: nameedit, Msg: 0xCu, wParam: 0, lParam: (LPARAM)lparam);
    }
    else if ( TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::IsEmpty(a1: &sgProfileList) != 0 )
    {
      SendMessageA(hWnd: window, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042CA8);
    }
    else
    {
      v9 = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: &sgProfileList);
      SendMessageA(hWnd: window, Msg: 0xCu, wParam: 0, lParam: v9 + 8);
    }
    SetProfileBmp(a1: window);
    UiLoadCursors(a1: window, a2: *((struct _SNETUIDATA **)dword_19042C10 + 3));
    UiVidFade(a1: 1, a2: 1);
    SDlgSetTimer(a1: window, a2: 123, a3: 200, a4: 0);
    LogoSetTimer(
      a1: window,
      a2: 1,
      getdatacallback: *(int (__stdcall **)(unsigned int, unsigned int, void *, unsigned int, unsigned int *))(*((_DWORD *)dword_19042C10
                                                                                              + 3)
                                                                                            + 44));
    if ( TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::IsEmpty(a1: &sgProfileList) == 0 || *(_BYTE *)lparam != 0 )
      SetFocus(hWnd: passedit);
    else
      SetFocus(hWnd: nameedit);
    PostMessageA(hWnd: window, Msg: 0x469u, wParam: 0, lParam: 0);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19010382
 * CodeView symbol: DestroyArtwork
 * Demangled: DestroyArtwork
 * IDA name: sub_19010382
 * CodeView module: 16
 * CV address: segment 1 : 0xF382
 * Code length: 0xC0
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall sub_19010382(HWND this)
{
  HWND DlgItem; // eax

  DlgItem = GetDlgItem(hDlg: this, nIDDlgItem: 1040);
  LogoFramesDestroy(a1: DlgItem);
  if ( sgBkgBmp != nullptr )
  {
    SMemFree(a1: sgBkgBmp, a2: aLogonCpp, a3: 73, a4: 0);
    sgBkgBmp = nullptr;
  }
  if ( dword_19042CA4 != nullptr )
  {
    SMemFree(a1: dword_19042CA4, a2: aLogonCpp_0, a3: 76, a4: 0);
    dword_19042CA4 = nullptr;
  }
  if ( dword_19042C0C != nullptr )
  {
    SMemFree(a1: dword_19042C0C, a2: aLogonCpp_1, a3: 79, a4: 0);
    dword_19042C0C = nullptr;
  }
  if ( sgpBlankProfile != nullptr )
  {
    SMemFree(a1: sgpBlankProfile, a2: aLogonCpp_2, a3: 82, a4: 0);
    sgpBlankProfile = nullptr;
  }
}


// ==============================================================================
/*
 * EA: 0x19010442
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: sub_19010442
 * CodeView module: 16
 * CV address: segment 1 : 0xF442
 * Code length: 0x1AA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_19010442(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  HWND DlgItem; // eax
  unsigned __int8 *v4; // [esp-Ch] [ebp-58h]
  struct tagSIZE medBtnSize; // [esp+8h] [ebp-44h] BYREF
  _DWORD big_btn_ids[3]; // [esp+10h] [ebp-3Ch] BYREF
  HWND profile; // [esp+1Ch] [ebp-30h]
  struct tagSIZE bgSize; // [esp+20h] [ebp-2Ch] BYREF
  _DWORD med_btn_ids[3]; // [esp+28h] [ebp-24h] BYREF
  struct tagRECT rect; // [esp+34h] [ebp-18h] BYREF
  int static_txt; // [esp+44h] [ebp-8h]
  int v14; // [esp+48h] [ebp-4h]

  med_btn_ids[0] = 1;
  med_btn_ids[1] = 2;
  med_btn_ids[2] = 0;
  static_txt = 1030;
  v14 = 0;
  big_btn_ids[0] = 1054;
  big_btn_ids[1] = 1055;
  big_btn_ids[2] = 0;
  UiLoadArtwork(
    a1: a2,
    a2: a1,
    hWndParent: nullptr,
    artid: 0x80000007,
    controltype: byte_19042CAC,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 1,
    prepfadein: 0,
    a10: &sgBkgBmp,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 5u,
    controltype: aButton_3,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19042CA4,
    a11: &sgButtonSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 7u,
    controltype: aButton_4,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19042C0C,
    a11: &medBtnSize);
  SDlgSetControlBitmaps(a1, a2: big_btn_ids, a3: 0, a4: dword_19042C0C, a5: &medBtnSize, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: med_btn_ids, a3: 0, a4: dword_19042CA4, a5: &sgButtonSize, a6: 1, a7: -1);
  profile = GetDlgItem(hDlg: a1, nIDDlgItem: 1066);
  GetClientRect(hWnd: profile, lpRect: &rect);
  sgpBlankProfile = (unsigned __int8 *)SMemAlloc(a1: 54000, a2: aLogonCpp_3, a3: 140, a4: 8);
  if ( ProfileDraw(a1: dword_19042C10, a2: 0, usertbl: nullptr, bitmaptbl: &sgpBlankProfile, a5: 270, a6: 200) != 0 )
    SDlgSetBitmapI(
      a1: profile,
      a2: a1,
      a3: &unk_19042CB0,
      a4: -1,
      a5: 1,
      a6: sgpBlankProfile,
      a7: &rect,
      a8: 270,
      a9: 200,
      a10: -1);
  v4 = sgBkgBmp;
  DlgItem = GetDlgItem(hDlg: a1, nIDDlgItem: 1040);
  LogoInit(a1, a2: DlgItem, bitmap: v4, bitmapsize: &bgSize, artcallback: a2);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x190105EC
 * CodeView symbol: LogonAttempt
 * Demangled: LogonAttempt
 * IDA name: LogonAttempt
 * CodeView module: 16
 * CV address: segment 1 : 0xF5EC
 * Code length: 0x1FC
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall LogonAttempt(HWND a1, HWND a2)
{
  HWND DlgItem; // eax
  HWND v3; // eax
  HWND v4; // eax
  HWND v5; // eax
  HWND v6; // eax
  HWND v8; // eax
  HWND v9; // eax
  HWND v10; // eax
  char szPassErr[256]; // [esp+8h] [ebp-124h] BYREF
  char szPass[16]; // [esp+108h] [ebp-24h] BYREF
  unsigned int errorcode; // [esp+118h] [ebp-14h] BYREF
  char szName[16]; // [esp+11Ch] [ebp-10h] BYREF

  EnableWindow(hWnd: a2, bEnable: false);
  DlgItem = GetDlgItem(hDlg: a1, nIDDlgItem: 1010);
  EnableWindow(hWnd: DlgItem, bEnable: false);
  v3 = GetDlgItem(hDlg: a1, nIDDlgItem: 1011);
  EnableWindow(hWnd: v3, bEnable: false);
  SendDlgItemMessageA(hDlg: a1, nIDDlgItem: 1010, Msg: 0xDu, wParam: 0x10u, lParam: (LPARAM)szName);
  SendDlgItemMessageA(hDlg: a1, nIDDlgItem: 1011, Msg: 0xDu, wParam: 0xDu, lParam: (LPARAM)szPass);
  if ( ChangeName(a1, a2: szName) != 0 )
  {
    if ( SrvLogon(a1: szName, a2: &sgszDescription, a3: szPass, a4: &errorcode) != 0 )
    {
      LogonRememberName(a1: szName);
      SDlgEndDialog(a1, a2: 1);
      return 1;
    }
    else
    {
      if ( errorcode == 1 )
      {
        LocaleLoadString(a1: 0x417u, a2: szPassErr, buffersize: 256);
        UiMessageBox(
          a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*((_DWORD *)dword_19042C10 + 3) + 32),
          a2: a1,
          lpText: szPassErr,
          a4: byte_19042CB4,
          a5: 0x30u);
        SendDlgItemMessageA(hDlg: a1, nIDDlgItem: 1011, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042CB8);
      }
      EnableWindow(hWnd: a2, bEnable: true);
      v8 = GetDlgItem(hDlg: a1, nIDDlgItem: 1010);
      EnableWindow(hWnd: v8, bEnable: true);
      v9 = GetDlgItem(hDlg: a1, nIDDlgItem: 1011);
      EnableWindow(hWnd: v9, bEnable: true);
      v10 = GetDlgItem(hDlg: a1, nIDDlgItem: 1011);
      SetFocus(hWnd: v10);
      return 0;
    }
  }
  else
  {
    EnableWindow(hWnd: a2, bEnable: true);
    v4 = GetDlgItem(hDlg: a1, nIDDlgItem: 1010);
    EnableWindow(hWnd: v4, bEnable: true);
    v5 = GetDlgItem(hDlg: a1, nIDDlgItem: 1011);
    EnableWindow(hWnd: v5, bEnable: true);
    v6 = GetDlgItem(hDlg: a1, nIDDlgItem: 1010);
    SetFocus(hWnd: v6);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x190107E8
 * CodeView symbol: LogonRememberName
 * Demangled: LogonRememberName
 * IDA name: LogonRememberName
 * CodeView module: 16
 * CV address: segment 1 : 0xF7E8
 * Code length: 0xDF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall LogonRememberName(const char *this)
{
  _PROFILEINFO *v1; // eax
  int namecount; // [esp+Ch] [ebp-8h]
  int node; // [esp+10h] [ebp-4h]

  namecount = 1;
  for ( node = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: &sgProfileList);
        node > 0;
        node = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::RawNext(ptr: node) )
  {
    ++namecount;
    if ( _stricmp(String1: this, String2: (const char *)(node + 8)) == 0 )
    {
      TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::UnlinkNode(ptr: node);
      TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::LinkNode(ptr: node, linktype: 1, existingptr: 0);
      return;
    }
  }
  v1 = (_PROFILEINFO *)TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::NewNode(location: 1, extrabytes: 0, flags: 0);
  _PROFILEINFO::Make(this: v1, a2: this);
}


// ==============================================================================
/*
 * EA: 0x190108C7
 * CodeView symbol: ChangeName
 * Demangled: ChangeName
 * IDA name: ChangeName
 * CodeView module: 16
 * CV address: segment 1 : 0xF8C7
 * Code length: 0xCC
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ChangeName(HWND a1, const char *a2)
{
  char message[512]; // [esp+8h] [ebp-200h] BYREF

  if ( sgszCurrentName == 0 )
    return 1;
  if ( _stricmp(String1: a2, String2: &sgszCurrentName) == 0 )
    return 1;
  LocaleLoadString(a1: 0x440u, a2: message, buffersize: 512);
  if ( UiMessageBox(
         a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*((_DWORD *)dword_19042C10 + 3) + 32),
         a2: a1,
         lpText: message,
         a4: byte_19042CBC,
         a5: 0x31u) != 1 )
    return 0;
  if ( DoRenameAccount(a1, a2: dword_19042C10, currname: &sgszCurrentName, newname: a2) == 0 )
    return 0;
  SStrCopy(a1: &sgszCurrentName, a2, a3: 16);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19010993
 * CodeView symbol: SetProfileBmp
 * Demangled: SetProfileBmp
 * IDA name: SetProfileBmp
 * CodeView module: 16
 * CV address: segment 1 : 0xF993
 * Code length: 0x132
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __thiscall SetProfileBmp(HWND this)
{
  unsigned __int8 *v2; // [esp+0h] [ebp-40h]
  HWND profile; // [esp+10h] [ebp-30h]
  int test; // [esp+14h] [ebp-2Ch]
  char szName[16]; // [esp+18h] [ebp-28h] BYREF
  unsigned __int8 *bitmap; // [esp+28h] [ebp-18h]
  struct tagRECT rect; // [esp+2Ch] [ebp-14h] BYREF
  int node; // [esp+3Ch] [ebp-4h]

  SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1010, Msg: 0xDu, wParam: 0x10u, lParam: (LPARAM)szName);
  node = 0;
  for ( test = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: &sgProfileList);
        test > 0;
        test = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::RawNext(ptr: test) )
  {
    if ( _stricmp(String1: (const char *)(test + 8), String2: szName) == 0 )
      node = test;
  }
  profile = GetDlgItem(hDlg: this, nIDDlgItem: 1066);
  GetClientRect(hWnd: profile, lpRect: &rect);
  if ( node != 0 && *(_DWORD *)(node + 24) != 0 )
    v2 = *(unsigned __int8 **)(node + 24);
  else
    v2 = sgpBlankProfile;
  bitmap = v2;
  SDlgSetBitmapI(
    a1: profile,
    a2: this,
    a3: &unk_19042CC0,
    a4: -1,
    a5: 1,
    a6: v2,
    a7: &rect,
    a8: rect.right,
    a9: rect.bottom,
    a10: -1);
  return InvalidateRect(hWnd: profile, lpRect: nullptr, bErase: true);
}


// ==============================================================================
/*
 * EA: 0x19010AC5
 * CodeView symbol: ChangePassword
 * Demangled: ChangePassword
 * IDA name: sub_19010AC5
 * CodeView module: 16
 * CV address: segment 1 : 0xFAC5
 * Code length: 0x6F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND __thiscall sub_19010AC5(HWND this)
{
  HWND DlgItem; // eax
  char name[16]; // [esp+4h] [ebp-10h] BYREF

  if ( *(_DWORD *)(*((_DWORD *)dword_19042C10 + 3) + 36) != 0 )
    (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)dword_19042C10 + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
  SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1010, Msg: 0xDu, wParam: 0x10u, lParam: (LPARAM)name);
  DoNewPassword(a1: this, a2: dword_19042C10, a3: name);
  DlgItem = GetDlgItem(hDlg: this, nIDDlgItem: 1011);
  return SetFocus(hWnd: DlgItem);
}


// ==============================================================================
/*
 * EA: 0x19010B34
 * CodeView symbol: OnNewAccount
 * Demangled: OnNewAccount
 * IDA name: OnNewAccount
 * CodeView module: 16
 * CV address: segment 1 : 0xFB34
 * Code length: 0xC7
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND __fastcall OnNewAccount(HWND a1, HWND a2)
{
  HWND DlgItem; // eax
  char szName; // [esp+8h] [ebp-10h] BYREF
  int v7; // [esp+9h] [ebp-Fh]
  int v8; // [esp+Dh] [ebp-Bh]
  int v9; // [esp+11h] [ebp-7h]
  __int16 v10; // [esp+15h] [ebp-3h]
  char v11; // [esp+17h] [ebp-1h]

  if ( *(_DWORD *)(*((_DWORD *)dword_19042C10 + 3) + 36) != 0 )
    (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)dword_19042C10 + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
  EnableWindow(hWnd: a2, bEnable: false);
  szName = byte_19042CC4;
  v7 = 0;
  v8 = 0;
  v9 = 0;
  v10 = 0;
  v11 = 0;
  if ( sgbNewName != 0 )
    SStrCopy(a1: &szName, a2: &sgszCurrentName, a3: 16);
  if ( DoNewAccount(a1, a2: dword_19042C10, name: &szName, namesize: 16, description: &sgszDescription) != 0 )
  {
    LogonRememberName(this: &szName);
    return (HWND)SDlgEndDialog(a1, a2: 1);
  }
  else
  {
    EnableWindow(hWnd: a2, bEnable: true);
    DlgItem = GetDlgItem(hDlg: a1, nIDDlgItem: 1011);
    return SetFocus(hWnd: DlgItem);
  }
}


// ==============================================================================
/*
 * EA: 0x19010BFB
 * CodeView symbol: UiLogon
 * Demangled: UiLogon
 * IDA name: ?UiLogon@@YIHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@@Z
 * CodeView module: 16
 * CV address: segment 1 : 0xFBFB
 * Code length: 0x14A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall UiLogon(
        struct _SNETPROGRAMDATA *a1,
        struct _SNETPLAYERDATA *a2,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata)
{
  HMODULE ModuleHandleA; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-48h]
  int FrameWindow; // [esp+0h] [ebp-38h]
  char logonname; // [esp+Ch] [ebp-2Ch] BYREF
  int v9; // [esp+Dh] [ebp-2Bh]
  int v10; // [esp+11h] [ebp-27h]
  int v11; // [esp+15h] [ebp-23h]
  __int16 v12; // [esp+19h] [ebp-1Fh]
  char v13; // [esp+1Bh] [ebp-1Dh]
  _DWORD uiparams[6]; // [esp+1Ch] [ebp-1Ch] BYREF
  int v15; // [esp+34h] [ebp-4h]

  uiparams[0] = 0;
  uiparams[1] = a1;
  uiparams[2] = a2;
  uiparams[3] = interfacedata;
  uiparams[4] = versiondata;
  uiparams[5] = 0;
  dword_19042C10 = (struct _UIPARAMS *)uiparams;
  do
  {
    sgbNewName = 0;
    sgszCurrentName = 0;
    sgszDescription = 0;
    logonname = byte_19042CC8;
    v9 = 0;
    v10 = 0;
    v11 = 0;
    v12 = 0;
    v13 = 0;
    if ( *((_DWORD *)interfacedata + 18) != 0 )
    {
      UiEndConnect(a1: 1);
      if ( SelectName(newname: (int)&sgbNewName) == 0 )
        return 0;
      SStrCopy(a1: &sgszCurrentName, a2: &logonname, a3: 16);
    }
    else
    {
      LogonGetNames(a1: &logonname, a2: 16);
      if ( DrawProfiles() == 0 )
        return 0;
      UiEndConnect(a1: 1);
    }
    if ( interfacedata != nullptr )
      FrameWindow = *((_DWORD *)interfacedata + 2);
    else
      FrameWindow = SDrawGetFrameWindow(a1: 0);
    Dialog = LocaleGetDialog(a1: 0x67u);
    ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
    v15 = SDlgDialogBoxIndirectParam(
            a1: ModuleHandleA,
            a2: Dialog,
            a3: FrameWindow,
            a4: LogonDialogProc,
            a5: &logonname);
    LogonWriteRegNames();
    FreeProfiles();
  }
  while ( *((_DWORD *)interfacedata + 18) != 0 && v15 == 0 && SErrGetLastError() != 1232 );
  return v15;
}


// ==============================================================================
/*
 * EA: 0x19010D45
 * CodeView symbol: LogonWriteRegNames
 * Demangled: LogonWriteRegNames
 * IDA name: LogonWriteRegNames
 * CodeView module: 16
 * CV address: segment 1 : 0xFD45
 * Code length: 0x2CF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int LogonWriteRegNames()
{
  int result; // eax
  _PROFILEINFO *v1; // [esp+14h] [ebp-230h]
  int ptr; // [esp+1Ch] [ebp-228h]
  int node; // [esp+24h] [ebp-220h]
  HKEY keyhandle; // [esp+28h] [ebp-21Ch] BYREF
  _DWORD valuename[65]; // [esp+2Ch] [ebp-218h] BYREF
  CHAR key[260]; // [esp+130h] [ebp-114h] BYREF
  int namecount; // [esp+234h] [ebp-10h]
  const BYTE *curr; // [esp+238h] [ebp-Ch]
  const BYTE *regstring; // [esp+23Ch] [ebp-8h]
  DWORD disposition; // [esp+240h] [ebp-4h] BYREF

  namecount = 0;
  result = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: &sgProfileList);
  for ( node = result; node > 0; node = result )
  {
    ++namecount;
    result = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::RawNext(ptr: node);
  }
  if ( namecount != 0 )
  {
    while ( namecount - 5 > 0 )
    {
      --namecount;
      v1 = (_PROFILEINFO *)TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Tail(a1: &sgProfileList);
      if ( v1 != nullptr )
        _PROFILEINFO::`scalar deleting destructor'(this: v1, a2: 1u);
    }
    regstring = (const BYTE *)SMemAlloc(a1: 16 * namecount + 1, a2: aLogonCpp_4, a3: 189, a4: 0);
    curr = regstring;
    for ( ptr = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: &sgProfileList);
          ptr > 0;
          ptr = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::RawNext(ptr) )
    {
      curr += SStrCopy(a1: curr, a2: ptr + 8, a3: 16) + 1;
    }
    *curr = 0;
    strcpy((char *)valuename, "Names");
    memset((char *)&valuename[1] + 2, 0, 0xFCu);
    HIWORD(valuename[64]) = 0;
    SRegGetBaseKey(a1: 2, a2: key, a3: 260);
    SStrPack(a1: key, a2: aCharacters, a3: 260);
    if ( RegCreateKeyExA(
           hKey: HKEY_LOCAL_MACHINE,
           lpSubKey: key,
           Reserved: 0,
           lpClass: nullptr,
           dwOptions: 0,
           samDesired: 0x20006u,
           lpSecurityAttributes: nullptr,
           phkResult: &keyhandle,
           lpdwDisposition: &disposition) == 0 )
    {
      RegSetValueExA(
        hKey: keyhandle,
        lpValueName: (LPCSTR)valuename,
        Reserved: 0,
        dwType: 7u,
        lpData: regstring,
        cbData: curr - regstring);
      RegCloseKey(hKey: keyhandle);
    }
    return SMemFree(a1: regstring, a2: aLogonCpp_5, a3: 223, a4: 0);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19011014
 * CodeView symbol: LogonGetNames
 * Demangled: LogonGetNames
 * IDA name: LogonGetNames
 * CodeView module: 16
 * CV address: segment 1 : 0x10014
 * Code length: 0xE6
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall LogonGetNames(int a1, int a2)
{
  int result; // eax
  _PROFILEINFO *v3; // eax
  int v4; // eax
  int length; // [esp+8h] [ebp-18h]
  int users; // [esp+Ch] [ebp-14h]
  int buffer; // [esp+10h] [ebp-10h]
  int offset; // [esp+14h] [ebp-Ch]
  char *pszName; // [esp+18h] [ebp-8h]
  int bytesread; // [esp+1Ch] [ebp-4h] BYREF

  result = LogonReadRegNames(a1: &bytesread);
  buffer = result;
  if ( result != 0 )
  {
    *(_BYTE *)(bytesread + result - 1) = 0;
    pszName = (char *)result;
    offset = 0;
    for ( users = 0; users < 5 && offset < bytesread && *pszName != 0; ++users )
    {
      FixInvalidString(a1: pszName, a2: asc_1903CE60);
      v3 = (_PROFILEINFO *)TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::NewNode(location: 2, extrabytes: 0, flags: 0);
      _PROFILEINFO::Make(this: v3, a2: pszName);
      length = SStrLen(a1: pszName) + 1;
      offset += length;
      pszName += length;
    }
    SMemFree(a1: buffer, a2: aLogonCpp_6, a3: 301, a4: 0);
    v4 = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: &sgProfileList);
    return SStrCopy(a1, a2: v4 + 8, a3: a2);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190110FA
 * CodeView symbol: LogonReadRegNames
 * Demangled: LogonReadRegNames
 * IDA name: LogonReadRegNames
 * CodeView module: 16
 * CV address: segment 1 : 0x100FA
 * Code length: 0x14F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LPBYTE __thiscall LogonReadRegNames(_DWORD *this)
{
  HKEY keyhandle; // [esp+8h] [ebp-21Ch] BYREF
  LPBYTE buffer; // [esp+Ch] [ebp-218h]
  int iter; // [esp+10h] [ebp-214h]
  _DWORD valuename[65]; // [esp+14h] [ebp-210h] BYREF
  DWORD datatype; // [esp+118h] [ebp-10Ch] BYREF
  char key[260]; // [esp+11Ch] [ebp-108h] BYREF
  DWORD bytesread; // [esp+220h] [ebp-4h] BYREF

  strcpy((char *)valuename, "Names");
  memset((char *)&valuename[1] + 2, 0, 0xFCu);
  HIWORD(valuename[64]) = 0;
  buffer = nullptr;
  bytesread = 0;
  datatype = 7;
  SRegGetBaseKey(a1: 2, a2: key, a3: 260);
  strcat(Destination: key, Source: aCharacters_0);
  for ( iter = 0;
        iter < 2
     && RegOpenKeyExA(
          hKey: HKEY_LOCAL_MACHINE,
          lpSubKey: key,
          ulOptions: 0,
          samDesired: 0x20019u,
          phkResult: &keyhandle) == 0;
        ++iter )
  {
    RegQueryValueExA(
      hKey: keyhandle,
      lpValueName: (LPCSTR)valuename,
      lpReserved: nullptr,
      lpType: &datatype,
      lpData: buffer,
      lpcbData: &bytesread);
    RegCloseKey(hKey: keyhandle);
    if ( buffer == nullptr && bytesread != 0 )
      buffer = (LPBYTE)SMemAlloc(a1: bytesread, a2: aLogonCpp_7, a3: 251, a4: 0);
  }
  if ( this != nullptr )
    *this = bytesread;
  return buffer;
}


// ==============================================================================
/*
 * EA: 0x19011249
 * CodeView symbol: FreeProfiles
 * Demangled: FreeProfiles
 * IDA name: FreeProfiles
 * CodeView module: 16
 * CV address: segment 1 : 0x10249
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int FreeProfiles()
{
  return TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Clear(a1: &sgProfileList);
}


// ==============================================================================
/*
 * EA: 0x19011258
 * CodeView symbol: DrawProfiles
 * Demangled: DrawProfiles
 * IDA name: DrawProfiles
 * CodeView module: 16
 * CV address: segment 1 : 0x10258
 * Code length: 0x2FB
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int DrawProfiles()
{
  int currprofile; // [esp+10h] [ebp-30h]
  int j; // [esp+14h] [ebp-2Ch]
  int k; // [esp+18h] [ebp-28h]
  int v4; // [esp+20h] [ebp-20h]
  int ptr; // [esp+24h] [ebp-1Ch]
  int node; // [esp+2Ch] [ebp-14h]
  int i; // [esp+30h] [ebp-10h]
  int namecount; // [esp+34h] [ebp-Ch]
  unsigned __int8 **bitmaptbl; // [esp+38h] [ebp-8h]
  const char **nametbl; // [esp+3Ch] [ebp-4h]

  namecount = 0;
  for ( node = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: &sgProfileList);
        node > 0;
        node = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::RawNext(ptr: node) )
  {
    ++namecount;
  }
  if ( namecount == 0 )
    return 1;
  nametbl = (const char **)SMemAlloc(a1: 4 * namecount, a2: aLogonCpp_8, a3: 323, a4: 0);
  v4 = 0;
  for ( ptr = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: &sgProfileList);
        ptr > 0;
        ptr = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::RawNext(ptr) )
  {
    nametbl[v4++] = (const char *)(ptr + 8);
  }
  bitmaptbl = (unsigned __int8 **)SMemAlloc(a1: 4 * namecount, a2: aLogonCpp_9, a3: 330, a4: 8);
  for ( i = 0; i < namecount; ++i )
    bitmaptbl[i] = (unsigned __int8 *)SMemAlloc(a1: 54000, a2: aLogonCpp_10, a3: 332, a4: 0);
  if ( ProfileDraw(a1: dword_19042C10, a2: namecount, usertbl: nametbl, bitmaptbl, a5: 270, a6: 200) != 0 )
  {
    currprofile = 0;
    for ( j = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: &sgProfileList);
          j > 0;
          j = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::RawNext(ptr: j) )
    {
      *(_DWORD *)(j + 24) = bitmaptbl[currprofile++];
    }
  }
  else
  {
    FreeProfiles();
    for ( k = 0; k < namecount; ++k )
    {
      if ( bitmaptbl[k] != nullptr )
        SMemFree(a1: bitmaptbl[k], a2: aLogonCpp_11, a3: 342, a4: 0);
    }
    if ( SErrGetLastError() == 1223 )
    {
      SMemFree(a1: nametbl, a2: aLogonCpp_12, a3: 344, a4: 0);
      SMemFree(a1: bitmaptbl, a2: aLogonCpp_13, a3: 345, a4: 0);
      return 0;
    }
  }
  SMemFree(a1: nametbl, a2: aLogonCpp_14, a3: 355, a4: 0);
  SMemFree(a1: bitmaptbl, a2: aLogonCpp_15, a3: 356, a4: 0);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19011553
 * CodeView symbol: SelectName
 * Demangled: SelectName
 * IDA name: SelectName
 * CodeView module: 16
 * CV address: segment 1 : 0x10553
 * Code length: 0x76
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SelectName(int a1, int a2, int newname)
{
  int v6; // [esp+8h] [ebp-4h]

  ShowCursor(bShow: false);
  v6 = (*(int (__stdcall **)(_DWORD, _DWORD, _DWORD, _DWORD, unsigned int, int, int, char *, int, int))(*((_DWORD *)dword_19042C10 + 3) + 72))(
         a1: *((_DWORD *)dword_19042C10 + 1),
         a2: *((_DWORD *)dword_19042C10 + 2),
         a3: *((_DWORD *)dword_19042C10 + 3),
         a4: *((_DWORD *)dword_19042C10 + 4),
         a5: bn_id,
         a6: a1,
         a7: a2,
         a8: &sgszDescription,
         a9: 128,
         a10: newname);
  ShowCursor(bShow: true);
  return v6;
}


// ==============================================================================
/*
 * EA: 0x190115D0
 * CodeView symbol: _PROFILEINFO::Make
 * Demangled: _PROFILEINFO::Make
 * IDA name: ?Make@_PROFILEINFO@@QAEXPBD@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x105D0
 * Code length: 0x29
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall _PROFILEINFO::Make(_PROFILEINFO *this, const char *pszName)
{
  SStrCopy(a1: (char *)this + 8, a2: pszName, a3: 16);
  *((_DWORD *)this + 6) = 0;
}


// ==============================================================================
/*
 * EA: 0x19011600
 * CodeView symbol: _PROFILEINFO::`scalar deleting destructor'
 * Demangled: _PROFILEINFO::`scalar deleting destructor'
 * IDA name: ??_G_PROFILEINFO@@QAEPAXI@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x10600
 * Code length: 0x2E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_PROFILEINFO *__thiscall _PROFILEINFO::`scalar deleting destructor'(_PROFILEINFO *this, char __flags)
{
  _PROFILEINFO::~_PROFILEINFO(this);
  if ( (__flags & 1) != 0 )
    operator delete(ptr: this);
  return this;
}


// ==============================================================================
/*
 * EA: 0x19011630
 * CodeView symbol: _PROFILEINFO::~_PROFILEINFO
 * Demangled: _PROFILEINFO::~_PROFILEINFO
 * IDA name: ??1_PROFILEINFO@@QAE@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10630
 * Code length: 0x31
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall _PROFILEINFO::~_PROFILEINFO(_PROFILEINFO *this)
{
  if ( *((_DWORD *)this + 6) != 0 )
    SMemFree(a1: *((_DWORD *)this + 6), a2: `string', a3: 40, a4: 0);
  TSLinkedNode<_PROFILEINFO>::~TSLinkedNode<_PROFILEINFO>(a1: this);
}


// ==============================================================================
/*
 * EA: 0x19011670
 * CodeView symbol: TSLinkedNode<_PROFILEINFO>::~TSLinkedNode<_PROFILEINFO>
 * Demangled: TSLinkedNode<_PROFILEINFO>::~TSLinkedNode<_PROFILEINFO>
 * IDA name: ??1?$TSLinkedNode@U_PROFILEINFO@@@@QAE@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10670
 * Code length: 0x1B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLinkedNode<_PROFILEINFO>::~TSLinkedNode<_PROFILEINFO>(void *ecx0)
{
  TSLinkedNode<_PROFILEINFO>::Unlink(a1: ecx0);
  return TSLink<_PROFILEINFO>::~TSLink<_PROFILEINFO>(a1: ecx0);
}


// ==============================================================================
/*
 * EA: 0x19011690
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >
 * IDA name: ??0?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QAE@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10690
 * Code length: 0x21
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
char *__thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>(char *ecx0)
{
  TSLink<_PROFILEINFO>::TSLink<_PROFILEINFO>(a1: ecx0 + 4);
  TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Constructor(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x190116C0
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::~TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::~TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >
 * IDA name: ??1?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QAE@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x106C0
 * Code length: 0x1E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::~TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>(char *ecx0)
{
  TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::UnlinkAll(a1: ecx0);
  return TSLink<_PROFILEINFO>::~TSLink<_PROFILEINFO>(a1: ecx0 + 4);
}


// ==============================================================================
/*
 * EA: 0x190116E0
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Clear
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Clear
 * IDA name: ?Clear@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QAEXXZ
 * CodeView module: 16
 * CV address: segment 1 : 0x106E0
 * Code length: 0x48
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_PROFILEINFO *__thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Clear(void *ecx0)
{
  _PROFILEINFO *result; // eax

  while ( 1 )
  {
    result = (_PROFILEINFO *)TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(a1: ecx0);
    if ( result == nullptr )
      break;
    _PROFILEINFO::`scalar deleting destructor'(this: result, __flags: 1);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19011730
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::DeleteNode
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::DeleteNode
 * IDA name: ?DeleteNode@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QAEPAU_PROFILEINFO@@PAU2@@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x10730
 * Code length: 0x49
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::DeleteNode(_PROFILEINFO *ptr)
{
  int nextptr; // [esp+10h] [ebp-4h]

  nextptr = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Next((int)ptr);
  if ( ptr != nullptr )
    _PROFILEINFO::`scalar deleting destructor'(this: ptr, __flags: 1);
  return nextptr;
}


// ==============================================================================
/*
 * EA: 0x19011780
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Head
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Head
 * IDA name: ?Head@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QBEPAU_PROFILEINFO@@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10780
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(char *this)
{
  return TSLink<_PROFILEINFO>::Next(a1: this + 4, a2: this);
}


// ==============================================================================
/*
 * EA: 0x190117A0
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::IsEmpty
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::IsEmpty
 * IDA name: ?IsEmpty@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QBEHXZ
 * CodeView module: 16
 * CV address: segment 1 : 0x107A0
 * Code length: 0x1B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::IsEmpty(char *this)
{
  return TSLink<_PROFILEINFO>::Next(a1: this + 4, a2: this) == 0;
}


// ==============================================================================
/*
 * EA: 0x190117C0
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::LinkNode
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::LinkNode
 * IDA name: ?LinkNode@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QAEXPAU_PROFILEINFO@@K0@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x107C0
 * Code length: 0xC1
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::LinkNode(
        int *ecx0,
        int ptr,
        int linktype,
        int existingptr)
{
  int *result; // eax
  int prevlink; // [esp+8h] [ebp-Ch]
  int *existinglink; // [esp+Ch] [ebp-8h]
  _DWORD *link; // [esp+10h] [ebp-4h]

  link = (_DWORD *)TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Link(ptr);
  if ( *link != 0 )
    TSLink<_PROFILEINFO>::Unlink(a1: link);
  if ( existingptr != 0 )
  {
    result = (int *)TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Link(ptr: existingptr);
    existinglink = result;
  }
  else
  {
    result = ecx0 + 1;
    existinglink = ecx0 + 1;
  }
  if ( linktype == 1 )
  {
    *link = existinglink;
    link[1] = existinglink[1];
    *(_DWORD *)TSLink<_PROFILEINFO>::NextLink(linkoffset: *ecx0) = link;
    existinglink[1] = ptr;
    return (int *)ptr;
  }
  else if ( linktype == 2 )
  {
    prevlink = *existinglink;
    *link = *existinglink;
    link[1] = *(_DWORD *)(prevlink + 4);
    *(_DWORD *)(prevlink + 4) = ptr;
    *existinglink = (int)link;
    return (int *)ptr;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19011890
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::NewNode
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::NewNode
 * IDA name: ?NewNode@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QAEPAU_PROFILEINFO@@KKK@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x10890
 * Code length: 0x77
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_PROFILEINFO *__thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::NewNode(
        int *ecx0,
        int location,
        int extrabytes,
        int flags)
{
  int v4; // eax
  const char *v5; // eax
  void *v6; // eax
  int v8; // [esp-4h] [ebp-14h]
  _PROFILEINFO *v9; // [esp+0h] [ebp-10h]
  _PROFILEINFO *v11; // [esp+8h] [ebp-8h]

  v4 = flags;
  LOBYTE(v4) = flags | 8;
  v8 = v4;
  v5 = type_info::internal_raw_name(this: (type_info *)&_PROFILEINFO `RTTI Type Descriptor');
  v6 = (void *)SMemAlloc(a1: extrabytes + 28, a2: v5, a3: -2, a4: v8);
  v11 = (_PROFILEINFO *)operator new(__formal: 28, ptr: v6);
  if ( v11 != nullptr )
    v9 = _PROFILEINFO::_PROFILEINFO(this: v11);
  else
    v9 = nullptr;
  if ( location != 0 )
    TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::LinkNode(ecx0, ptr: (int)v9, linktype: location, existingptr: 0);
  return v9;
}


// ==============================================================================
/*
 * EA: 0x19011910
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::RawNext
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::RawNext
 * IDA name: ?RawNext@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QBEPAU_PROFILEINFO@@PBU2@@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x10910
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::RawNext(void *ecx0, int ptr)
{
  int v2; // eax

  v2 = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Link(ptr);
  return TSLink<_PROFILEINFO>::RawNext(a1: v2, a2: ecx0);
}


// ==============================================================================
/*
 * EA: 0x19011930
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Tail
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Tail
 * IDA name: ?Tail@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QBEPAU_PROFILEINFO@@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10930
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Tail(char *this)
{
  return TSLink<_PROFILEINFO>::Prev(a1: this + 4, a2: this);
}


// ==============================================================================
/*
 * EA: 0x19011950
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::UnlinkNode
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::UnlinkNode
 * IDA name: ?UnlinkNode@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QAEXPAU_PROFILEINFO@@@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x10950
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::UnlinkNode(int ptr)
{
  int v1; // eax

  v1 = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Link(ptr);
  return TSLink<_PROFILEINFO>::Unlink(a1: v1);
}


// ==============================================================================
/*
 * EA: 0x19011970
 * CodeView symbol: _PROFILEINFO::_PROFILEINFO
 * Demangled: _PROFILEINFO::_PROFILEINFO
 * IDA name: ??0_PROFILEINFO@@QAE@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10970
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_PROFILEINFO *__thiscall _PROFILEINFO::_PROFILEINFO(_PROFILEINFO *this)
{
  TSLinkedNode<_PROFILEINFO>::TSLinkedNode<_PROFILEINFO>(a1: this);
  return this;
}


// ==============================================================================
/*
 * EA: 0x19011990
 * CodeView symbol: TSLinkedNode<_PROFILEINFO>::TSLinkedNode<_PROFILEINFO>
 * Demangled: TSLinkedNode<_PROFILEINFO>::TSLinkedNode<_PROFILEINFO>
 * IDA name: ??0?$TSLinkedNode@U_PROFILEINFO@@@@QAE@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10990
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSLinkedNode<_PROFILEINFO>::TSLinkedNode<_PROFILEINFO>(void *ecx0)
{
  TSLink<_PROFILEINFO>::TSLink<_PROFILEINFO>(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x190119B0
 * CodeView symbol: TSLinkedNode<_PROFILEINFO>::Unlink
 * Demangled: TSLinkedNode<_PROFILEINFO>::Unlink
 * IDA name: ?Unlink@?$TSLinkedNode@U_PROFILEINFO@@@@QAEXXZ
 * CodeView module: 16
 * CV address: segment 1 : 0x109B0
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLinkedNode<_PROFILEINFO>::Unlink(void *this)
{
  return TSLink<_PROFILEINFO>::Unlink(a1: this);
}


// ==============================================================================
/*
 * EA: 0x190119D0
 * CodeView symbol: TSLink<_PROFILEINFO>::NextLink
 * Demangled: TSLink<_PROFILEINFO>::NextLink
 * IDA name: ?NextLink@?$TSLink@U_PROFILEINFO@@@@ABEPAV1@H@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x109D0
 * Code length: 0x3D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_PROFILEINFO>::NextLink(_DWORD *this, int linkoffset)
{
  if ( (int)*(this + 1) < 0 )
    return ~*(this + 1);
  if ( linkoffset < 0 )
    linkoffset = (int)this - *(_DWORD *)(*this + 4);
  return *(this + 1) + linkoffset;
}


// ==============================================================================
/*
 * EA: 0x19011A10
 * CodeView symbol: TSLink<_PROFILEINFO>::TSLink<_PROFILEINFO>
 * Demangled: TSLink<_PROFILEINFO>::TSLink<_PROFILEINFO>
 * IDA name: ??0?$TSLink@U_PROFILEINFO@@@@QAE@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10A10
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSLink<_PROFILEINFO>::TSLink<_PROFILEINFO>(void *ecx0)
{
  TSLink<_PROFILEINFO>::Constructor(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x19011A30
 * CodeView symbol: TSLink<_PROFILEINFO>::~TSLink<_PROFILEINFO>
 * Demangled: TSLink<_PROFILEINFO>::~TSLink<_PROFILEINFO>
 * IDA name: ??1?$TSLink@U_PROFILEINFO@@@@QAE@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10A30
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_PROFILEINFO>::~TSLink<_PROFILEINFO>(void *this)
{
  return TSLink<_PROFILEINFO>::Unlink(a1: this);
}


// ==============================================================================
/*
 * EA: 0x19011A50
 * CodeView symbol: TSLink<_PROFILEINFO>::Next
 * Demangled: TSLink<_PROFILEINFO>::Next
 * IDA name: ?Next@?$TSLink@U_PROFILEINFO@@@@QBEPAU_PROFILEINFO@@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10A50
 * Code length: 0x2B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_PROFILEINFO>::Next(_DWORD *this)
{
  if ( (int)*(this + 1) <= 0 )
    return 0;
  else
    return *(this + 1);
}


// ==============================================================================
/*
 * EA: 0x19011A80
 * CodeView symbol: TSLink<_PROFILEINFO>::Prev
 * Demangled: TSLink<_PROFILEINFO>::Prev
 * IDA name: ?Prev@?$TSLink@U_PROFILEINFO@@@@QBEPAU_PROFILEINFO@@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10A80
 * Code length: 0x17
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_PROFILEINFO>::Prev(_DWORD ***this)
{
  return TSLink<_PROFILEINFO>::Next(this: **this);
}


// ==============================================================================
/*
 * EA: 0x19011AA0
 * CodeView symbol: TSLink<_PROFILEINFO>::RawNext
 * Demangled: TSLink<_PROFILEINFO>::RawNext
 * IDA name: ?RawNext@?$TSLink@U_PROFILEINFO@@@@QBEPAU_PROFILEINFO@@XZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10AA0
 * Code length: 0x11
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_PROFILEINFO>::RawNext(_DWORD *this)
{
  return *(this + 1);
}


// ==============================================================================
/*
 * EA: 0x19011AC0
 * CodeView symbol: TSLink<_PROFILEINFO>::Unlink
 * Demangled: TSLink<_PROFILEINFO>::Unlink
 * IDA name: ?Unlink@?$TSLink@U_PROFILEINFO@@@@QAEXXZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10AC0
 * Code length: 0x47
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSLink<_PROFILEINFO>::Unlink(_DWORD *ecx0)
{
  _DWORD *result; // eax

  result = ecx0;
  if ( *ecx0 != 0 )
  {
    *(_DWORD *)TSLink<_PROFILEINFO>::NextLink(this: ecx0, linkoffset: -1) = *ecx0;
    result = (_DWORD *)*(ecx0 + 1);
    *(_DWORD *)(*ecx0 + 4) = result;
    *ecx0 = 0;
    *(ecx0 + 1) = 0;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19011B10
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Constructor
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Constructor
 * IDA name: ?Constructor@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@AAEXXZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10B10
 * Code length: 0x1C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Constructor(_DWORD *this)
{
  *this = 0;
  return TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::InitializeTerminator(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x19011B30
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Link
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Link
 * IDA name: ?Link@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@ABEPAV?$TSLink@U_PROFILEINFO@@@@PBU_PROFILEINFO@@@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x10B30
 * Code length: 0x1A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Link(_DWORD *this, int ptr)
{
  return TSGetLink<_PROFILEINFO>::Link(a1: ptr, a2: *this, a3: this);
}


// ==============================================================================
/*
 * EA: 0x19011B50
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Next
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::Next
 * IDA name: ?Next@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QBEPAU_PROFILEINFO@@PBU2@@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x10B50
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Next(_DWORD *this, int ptr)
{
  _DWORD *v2; // eax

  v2 = (_DWORD *)TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Link(this, ptr);
  return TSLink<_PROFILEINFO>::Next(this: v2);
}


// ==============================================================================
/*
 * EA: 0x19011B70
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::UnlinkAll
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::UnlinkAll
 * IDA name: ?UnlinkAll@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@QAEXXZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10B70
 * Code length: 0x2C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::UnlinkAll(char *ecx0)
{
  int result; // eax

  while ( 1 )
  {
    result = TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::Head(this: ecx0);
    if ( result == 0 )
      break;
    TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::UnlinkNode(ptr: result);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19011BA0
 * CodeView symbol: TSLink<_PROFILEINFO>::Constructor
 * Demangled: TSLink<_PROFILEINFO>::Constructor
 * IDA name: ?Constructor@?$TSLink@U_PROFILEINFO@@@@AAEXXZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10BA0
 * Code length: 0x1E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSLink<_PROFILEINFO>::Constructor(_DWORD *this)
{
  *this = 0;
  *(this + 1) = 0;
  return this;
}


// ==============================================================================
/*
 * EA: 0x19011BC0
 * CodeView symbol: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::InitializeTerminator
 * Demangled: TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO> >::InitializeTerminator
 * IDA name: ?InitializeTerminator@?$TSList@U_PROFILEINFO@@V?$TSGetLink@U_PROFILEINFO@@@@@@AAEXXZ
 * CodeView module: 16
 * CV address: segment 1 : 0x10BC0
 * Code length: 0x25
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSList<_PROFILEINFO,TSGetLink<_PROFILEINFO>>::InitializeTerminator(_DWORD *this)
{
  *(this + 1) = this + 1;
  *(this + 2) = ~(unsigned int)(this + 1);
  return this;
}


// ==============================================================================
/*
 * EA: 0x19011BF0
 * CodeView symbol: TSGetLink<_PROFILEINFO>::Link
 * Demangled: TSGetLink<_PROFILEINFO>::Link
 * IDA name: ?Link@?$TSGetLink@U_PROFILEINFO@@@@SIPAV?$TSLink@U_PROFILEINFO@@@@PBV?$TSLinkedNode@U_PROFILEINFO@@@@H@Z
 * CodeView module: 16
 * CV address: segment 1 : 0x10BF0
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Logon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Logon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSGetLink<_PROFILEINFO>::Link(void *this)
{
  return this;
}

