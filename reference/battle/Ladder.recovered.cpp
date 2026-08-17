/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 42
 */


// ==============================================================================
/*
 * EA: 0x1900DD80
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: unknown_libname_3
 * CodeView module: 18
 * CV address: segment 1 : 0xCD80
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_3()
{
  sub_1900DD8F();
  return sub_1900DD9E();
}


// ==============================================================================
/*
 * EA: 0x1900DD8F
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: sub_1900DD8F
 * CodeView module: 18
 * CV address: segment 1 : 0xCD8F
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1900DD8F()
{
  return TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>(a1: &sgPlayerList);
}


// ==============================================================================
/*
 * EA: 0x1900DD9E
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: sub_1900DD9E
 * CodeView module: 18
 * CV address: segment 1 : 0xCD9E
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1900DD9E()
{
  return atexit(a1: sub_1900DDB0);
}


// ==============================================================================
/*
 * EA: 0x1900DDB0
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: sub_1900DDB0
 * CodeView module: 18
 * CV address: segment 1 : 0xCDB0
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_1900DDB0()
{
  TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::~TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>(a1: &sgPlayerList);
}


// ==============================================================================
/*
 * EA: 0x1900DDBF
 * CodeView symbol: LadderDialogProc
 * Demangled: LadderDialogProc
 * IDA name: ?LadderDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xCDBF
 * Code length: 0x758
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall LadderDialogProc(HWND window, UINT message, HDC wparam, HWND lparam)
{
  HWND v5; // eax
  HWND v6; // eax
  HWND FrameWindow; // eax
  HWND DlgItem; // eax
  WPARAM v9; // [esp+0h] [ebp-82Ch]
  WPARAM v10; // [esp+4h] [ebp-828h]
  HWND parent; // [esp+10h] [ebp-81Ch]
  _DWORD check_btn_ids[2]; // [esp+14h] [ebp-818h] BYREF
  BOOL check; // [esp+1Ch] [ebp-810h]
  LRESULT nPos; // [esp+24h] [ebp-808h]
  int node; // [esp+28h] [ebp-804h] BYREF
  char szText[2048]; // [esp+2Ch] [ebp-800h] BYREF

  if ( message <= 0x110 )
  {
    if ( message == 272 )
    {
      sgpUiparams = (struct _UIPARAMS *)lparam;
      sub_1900E517(a1: window, a2: *(_DWORD *)(*((_DWORD *)lparam + 3) + 12));
      sghTypeWnd = (int)GetDlgItem(hDlg: window, nIDDlgItem: 1067);
      sFont = UiSetFont(a1: window, a2: 0, a3: &nControlTbl);
      sgnFirstEntry = 0;
      FillTypes(a1: sghTypeWnd);
      SetupPlayerList(a1: window);
      ScanList(a1: window, a2: sgnFirstEntry);
      DlgItem = GetDlgItem(hDlg: window, nIDDlgItem: 1010);
      SetFocus(hWnd: DlgItem);
      return 0;
    }
    if ( message > 0x2B )
    {
      if ( message >= 0x104 && message <= 0x105 )
      {
        FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
        SendMessageA(hWnd: FrameWindow, Msg: message, wParam: (WPARAM)wparam, lParam: (LPARAM)lparam);
      }
    }
    else if ( message == 43 )
    {
      if ( wparam == (HDC)1065 )
      {
        DrawPlayer((int)lparam);
        return 1;
      }
    }
    else if ( message == 2 )
    {
      TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Clear(a1: &sgPlayerList);
      FreeArtwork();
      return 0;
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message != 273 )
  {
    if ( message == 274 )
    {
      if ( wparam == (HDC)61536 )
      {
        parent = GetParent(hWnd: window);
        SDlgEndDialog(a1: window, a2: 0);
        PostMessageA(hWnd: parent, Msg: message, wParam: 0xF060u, lParam: (LPARAM)lparam);
        return 0;
      }
    }
    else if ( message == 312 && GetWindowLongA(hWnd: lparam, nIndex: -12) == 1030 )
    {
      SetTextColor(hdc: wparam, color: 0xFFFFu);
      return (int)GetStockObject(i: 5);
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( (unsigned __int16)wparam > 0x42Bu )
  {
    switch ( (unsigned __int16)wparam )
    {
      case 0x42Eu:
        EnableWindow(hWnd: lparam, bEnable: false);
        if ( *(_DWORD *)(*((_DWORD *)sgpUiparams + 3) + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)sgpUiparams + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
        if ( sgnFirstEntry != 0 )
        {
          if ( (int)(sgnFirstEntry - sgnVisiblePlayers) >= 0 )
            v10 = sgnFirstEntry - sgnVisiblePlayers;
          else
            v10 = 0;
          sgnFirstEntry = v10;
        }
        else
        {
          sgnFirstEntry = 1000 - sgnVisiblePlayers;
        }
        break;
      case 0x42Fu:
        EnableWindow(hWnd: lparam, bEnable: false);
        if ( *(_DWORD *)(*((_DWORD *)sgpUiparams + 3) + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)sgpUiparams + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
        if ( sgnFirstEntry == 1000 - sgnVisiblePlayers )
        {
          sgnFirstEntry = 0;
        }
        else
        {
          if ( 1000 - sgnVisiblePlayers >= (int)(sgnVisiblePlayers + sgnFirstEntry) )
            v9 = sgnVisiblePlayers + sgnFirstEntry;
          else
            v9 = 1000 - sgnVisiblePlayers;
          sgnFirstEntry = v9;
        }
        break;
      case 0x43Eu:
        EnableWindow(hWnd: lparam, bEnable: false);
        if ( *(_DWORD *)(*((_DWORD *)sgpUiparams + 3) + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)sgpUiparams + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
        sgbCurrentListing = sgbCurrentListing == 0;
        ScanList(a1: window, a2: sgnFirstEntry);
        check_btn_ids[0] = 1086;
        check_btn_ids[1] = 0;
        check = sgbCurrentListing != 0;
        SDlgSetControlBitmaps(
          a1: window,
          a2: check_btn_ids,
          a3: 0,
          a4: (&sgCheckBtnBmp)[check],
          a5: &sgCheckBtnSize,
          a6: 1,
          a7: -1);
        InvalidateRect(hWnd: lparam, lpRect: nullptr, bErase: false);
        EnableWindow(hWnd: lparam, bEnable: true);
        SetFocus(hWnd: lparam);
        return 0;
      default:
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    ScanList(a1: window, a2: sgnFirstEntry);
    EnableWindow(hWnd: lparam, bEnable: true);
    SetFocus(hWnd: lparam);
    return 0;
  }
  switch ( (unsigned __int16)wparam )
  {
    case 0x42Bu:
      if ( HIWORD(wparam) == 1 )
      {
        sgnFirstEntry = 0;
        ScanList(a1: window, a2: 0);
        SetFocus(hWnd: lparam);
        return 0;
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    case 1u:
      EnableWindow(hWnd: lparam, bEnable: false);
      if ( *(_DWORD *)(*((_DWORD *)sgpUiparams + 3) + 36) != 0 )
        (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)sgpUiparams + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
      v5 = GetDlgItem(hDlg: window, nIDDlgItem: 1010);
      if ( SendMessageA(hWnd: v5, Msg: 0xEu, wParam: 0, lParam: 0) != 0 )
      {
        PlayerSearch(a1: window);
      }
      else
      {
        LocaleLoadString(a1: 0x439u, a2: szText, a3: 2048);
        UiMessageBox(
          a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*((_DWORD *)sgpUiparams + 3) + 32),
          a2: window,
          lpText: szText,
          a4: nullptr,
          a5: 0x30u);
      }
      EnableWindow(hWnd: lparam, bEnable: true);
      v6 = GetDlgItem(hDlg: window, nIDDlgItem: 1010);
      SetFocus(hWnd: v6);
      return 0;
    case 2u:
      EnableWindow(hWnd: lparam, bEnable: false);
      if ( *(_DWORD *)(*((_DWORD *)sgpUiparams + 3) + 36) != 0 )
        (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)sgpUiparams + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
      SDlgEndDialog(a1: window, a2: 1);
      return 0;
    case 0x429u:
      if ( HIWORD(wparam) == 2 )
      {
        EnableWindow(hWnd: lparam, bEnable: false);
        if ( *(_DWORD *)(*((_DWORD *)sgpUiparams + 3) + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(*((_DWORD *)sgpUiparams + 3) + 36))(a1: 1112425812, a2: 1, a3: 0);
        nPos = SendMessageA(hWnd: lparam, Msg: 0x188u, wParam: 0, lParam: 0);
        if ( nPos == -1 )
          return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
        node = 0;
        SendMessageA(hWnd: lparam, Msg: 0x189u, wParam: nPos, lParam: (LPARAM)&node);
        if ( node != 0 )
          ProfileCall(a1: window, a2: sgpUiparams, a3: (const char *)(node + 20));
        EnableWindow(hWnd: lparam, bEnable: true);
        SetFocus(hWnd: lparam);
        return 0;
      }
      else
      {
        if ( HIWORD(wparam) != 4 )
          return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
        if ( SendMessageA(hWnd: lparam, Msg: 0x188u, wParam: 0, lParam: 0) == -1 )
          SendMessageA(hWnd: lparam, Msg: 0x186u, wParam: sgnFirstEntry, lParam: 0);
        return 0;
      }
    default:
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
}


// ==============================================================================
/*
 * EA: 0x1900E517
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: sub_1900E517
 * CodeView module: 18
 * CV address: segment 1 : 0xD517
 * Code length: 0x1B4
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_1900E517(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  int tile; // [esp+8h] [ebp-4Ch]
  _DWORD btn_ids[3]; // [esp+Ch] [ebp-48h] BYREF
  _DWORD square_btn_ids[3]; // [esp+18h] [ebp-3Ch] BYREF
  _DWORD check_btn_ids[2]; // [esp+24h] [ebp-30h] BYREF
  struct tagSIZE smallBtn; // [esp+2Ch] [ebp-28h] BYREF
  struct tagSIZE bgSize; // [esp+34h] [ebp-20h] BYREF
  struct tagSIZE sizeBtns; // [esp+3Ch] [ebp-18h] BYREF
  _DWORD med_btn_ids[2]; // [esp+44h] [ebp-10h] BYREF
  struct tagSIZE squareBtn; // [esp+4Ch] [ebp-8h] BYREF

  btn_ids[0] = 1;
  btn_ids[1] = 2;
  btn_ids[2] = 0;
  med_btn_ids[0] = 1;
  med_btn_ids[1] = 0;
  square_btn_ids[0] = 1070;
  square_btn_ids[1] = 1071;
  square_btn_ids[2] = 0;
  check_btn_ids[0] = 1086;
  check_btn_ids[1] = 0;
  UiLoadArtwork(
    a1: a2,
    a2: a1,
    hWndParent: nullptr,
    artid: 0x8000000A,
    controltype: byte_19042BC8,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19042B64,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 5u,
    controltype: aButton_5,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19042BC4,
    a11: &sizeBtns);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 6u,
    controltype: aButton_6,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgSmallBtnBmp,
    a11: &smallBtn);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 0x1Fu,
    controltype: aButton_7,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &sgSquareBtnBmp,
    a11: &squareBtn);
  for ( tile = 0; tile < 2; ++tile )
    UiLoadArtwork(
      a1: a2,
      a2: nullptr,
      hWndParent: nullptr,
      artid: *(&CHECKBOX_IDS + tile),
      controltype: aButton_8,
      controlstyle: 0xFFFFFFFF,
      usageflags: 1,
      loadpalette: 0,
      prepfadein: 0,
      a10: &(&sgCheckBtnBmp)[tile],
      a11: &sgCheckBtnSize);
  SDlgSetControlBitmaps(a1, a2: btn_ids, a3: 0, a4: dword_19042BC4, a5: &sizeBtns, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: med_btn_ids, a3: 0, a4: sgSmallBtnBmp, a5: &smallBtn, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: square_btn_ids, a3: 0, a4: sgSquareBtnBmp, a5: &squareBtn, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: check_btn_ids, a3: 0, a4: sgCheckBtnBmp, a5: &sgCheckBtnSize, a6: 1, a7: -1);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900E6CB
 * CodeView symbol: FreeArtwork
 * Demangled: FreeArtwork
 * IDA name: FreeArtwork
 * CodeView module: 18
 * CV address: segment 1 : 0xD6CB
 * Code length: 0x103
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int FreeArtwork()
{
  int result; // eax
  int tile; // [esp+0h] [ebp-4h]

  if ( dword_19042BC4 != nullptr )
  {
    result = SMemFree(a1: dword_19042BC4, a2: aLadderCpp, a3: 215, a4: 0);
    dword_19042BC4 = nullptr;
  }
  if ( dword_19042B64 != nullptr )
  {
    result = SMemFree(a1: dword_19042B64, a2: aLadderCpp_0, a3: 219, a4: 0);
    dword_19042B64 = nullptr;
  }
  if ( sgSmallBtnBmp != nullptr )
  {
    result = SMemFree(a1: sgSmallBtnBmp, a2: aLadderCpp_1, a3: 223, a4: 0);
    sgSmallBtnBmp = nullptr;
  }
  if ( sgSquareBtnBmp != nullptr )
  {
    result = SMemFree(a1: sgSquareBtnBmp, a2: aLadderCpp_2, a3: 227, a4: 0);
    sgSquareBtnBmp = nullptr;
  }
  for ( tile = 0; tile < 2; ++tile )
  {
    if ( (&sgCheckBtnBmp)[tile] != nullptr )
    {
      result = SMemFree(a1: (&sgCheckBtnBmp)[tile], a2: aLadderCpp_3, a3: 232, a4: 0);
      (&sgCheckBtnBmp)[tile] = nullptr;
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900E7CE
 * CodeView symbol: FillTypes
 * Demangled: FillTypes
 * IDA name: FillTypes
 * CodeView module: 18
 * CV address: segment 1 : 0xD7CE
 * Code length: 0x9B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __thiscall FillTypes(HWND this)
{
  HWND Parent; // eax
  int type; // [esp+4h] [ebp-4h]

  for ( type = 0; type < sgnLadderTblLen; ++type )
  {
    LocaleLoadString(a1: dword_1903CC0C[6 * type], a2: (char *)&unk_1903CBFC + 24 * type, a3: 16);
    SendMessageA(hWnd: this, Msg: 0x143u, wParam: 0, lParam: (LPARAM)&unk_1903CBFC + 24 * type);
  }
  SendMessageA(hWnd: this, Msg: 0x14Eu, wParam: 0, lParam: 0);
  Parent = GetParent(hWnd: this);
  return SendMessageA(hWnd: Parent, Msg: 0x111u, wParam: 0x1042Bu, lParam: (LPARAM)this);
}


// ==============================================================================
/*
 * EA: 0x1900E869
 * CodeView symbol: SetupPlayerList
 * Demangled: SetupPlayerList
 * IDA name: SetupPlayerList
 * CodeView module: 18
 * CV address: segment 1 : 0xD869
 * Code length: 0x9E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __thiscall SetupPlayerList(HWND this)
{
  LRESULT result; // eax
  int height; // [esp+8h] [ebp-1Ch]
  struct tagRECT clientrect; // [esp+Ch] [ebp-18h] BYREF
  LRESULT font; // [esp+1Ch] [ebp-8h]
  HWND lbox; // [esp+20h] [ebp-4h]

  lbox = GetDlgItem(hDlg: this, nIDDlgItem: 1065);
  GetClientRect(hWnd: lbox, lpRect: &clientrect);
  sgnVisiblePlayers = 10;
  font = SendMessageA(hWnd: lbox, Msg: 0x31u, wParam: 0, lParam: 0);
  result = GetFontHeight(a1: font, a2: this);
  LOWORD(height) = result;
  if ( result != -1 )
  {
    if ( result <= clientrect.bottom / sgnVisiblePlayers )
      height = clientrect.bottom / sgnVisiblePlayers;
    else
      sgnVisiblePlayers = clientrect.bottom / result;
    return SendMessageA(hWnd: lbox, Msg: 0x1A0u, wParam: 0, lParam: (unsigned __int16)height);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900E907
 * CodeView symbol: GetFontHeight
 * Demangled: GetFontHeight
 * IDA name: GetFontHeight
 * CodeView module: 18
 * CV address: segment 1 : 0xD907
 * Code length: 0x86
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LONG __thiscall GetFontHeight(void *this)
{
  HWND DesktopWindow; // eax
  HWND v2; // eax
  LONG height; // [esp+4h] [ebp-48h]
  HGDIOBJ oldfont; // [esp+8h] [ebp-44h]
  HDC screendc; // [esp+Ch] [ebp-40h]
  HDC memdc; // [esp+10h] [ebp-3Ch]
  struct tagTEXTMETRICA tm; // [esp+14h] [ebp-38h] BYREF

  DesktopWindow = GetDesktopWindow();
  screendc = GetDC(hWnd: DesktopWindow);
  memdc = CreateCompatibleDC(hdc: screendc);
  oldfont = SelectObject(hdc: memdc, h: this);
  height = -1;
  if ( GetTextMetricsA(hdc: memdc, lptm: &tm) )
    height = tm.tmHeight;
  SelectObject(hdc: memdc, h: oldfont);
  DeleteDC(hdc: memdc);
  v2 = GetDesktopWindow();
  ReleaseDC(hWnd: v2, hDC: screendc);
  return height;
}


// ==============================================================================
/*
 * EA: 0x1900E98D
 * CodeView symbol: ScanList
 * Demangled: ScanList
 * IDA name: ScanList
 * CodeView module: 18
 * CV address: segment 1 : 0xD98D
 * Code length: 0x201
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall ScanList(HWND a1, unsigned int a2)
{
  HWND range; // [esp+Ch] [ebp-30h]
  struct tagRECT invalidrect; // [esp+10h] [ebp-2Ch] BYREF
  HWND combo; // [esp+20h] [ebp-1Ch]
  int iterate_delete; // [esp+24h] [ebp-18h]
  HWND lbox; // [esp+28h] [ebp-14h]
  LRESULT type; // [esp+2Ch] [ebp-10h]
  int visplayers; // [esp+30h] [ebp-Ch]
  struct _LADDERREC **pInfoTbl; // [esp+34h] [ebp-8h]
  LPARAM node; // [esp+38h] [ebp-4h]

  combo = GetDlgItem(hDlg: a1, nIDDlgItem: 1067);
  lbox = GetDlgItem(hDlg: a1, nIDDlgItem: 1065);
  type = SendDlgItemMessageA(hDlg: a1, nIDDlgItem: 1067, Msg: 0x147u, wParam: 0, lParam: 0);
  visplayers = sgnVisiblePlayers;
  pInfoTbl = (struct _LADDERREC **)SMemAlloc(a1: 4 * sgnVisiblePlayers, a2: aLadderCpp_4, a3: 401, a4: 8);
  SrvReadLadderData(
    a1: *(_DWORD *)(*((_DWORD *)sgpUiparams + 1) + 12),
    a2: 1u,
    sortcriterion: sgLadderTbl[6 * type],
    firstentry: a2,
    a5: visplayers,
    a6: pInfoTbl);
  FormatList(nNum: visplayers, usecurrent: sgbCurrentListing);
  SMemFree(a1: pInfoTbl, a2: aLadderCpp_5, a3: 410, a4: 0);
  SendMessageA(hWnd: lbox, Msg: 0xBu, wParam: 0, lParam: 0);
  SendMessageA(hWnd: lbox, Msg: 0x184u, wParam: 0, lParam: 0);
  node = TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Head(a1: &sgPlayerList);
  iterate_delete = 0;
  while ( node > 0 )
  {
    SendMessageA(hWnd: lbox, Msg: 0x180u, wParam: 0, lParam: node);
    node = TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::RawNext(ptr: node);
  }
  SendMessageA(hWnd: lbox, Msg: 0xBu, wParam: 1u, lParam: 0);
  sprintf(Buffer: szBuf, Format: "%u-%u", a2 + 1, sgnVisiblePlayers + a2);
  range = GetDlgItem(hDlg: a1, nIDDlgItem: 1072);
  SendMessageA(hWnd: range, Msg: 0xCu, wParam: 0, lParam: (LPARAM)szBuf);
  GetWindowRect(hWnd: range, lpRect: &invalidrect);
  ScreenToClient(hWnd: a1, lpPoint: (LPPOINT)&invalidrect);
  ScreenToClient(hWnd: a1, lpPoint: (LPPOINT)&invalidrect.right);
  return InvalidateRect(hWnd: a1, lpRect: &invalidrect, bErase: true);
}


// ==============================================================================
/*
 * EA: 0x1900EB8E
 * CodeView symbol: FormatList
 * Demangled: FormatList
 * IDA name: FormatList
 * CodeView module: 18
 * CV address: segment 1 : 0xDB8E
 * Code length: 0x37E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall FormatList(int a1, int a2, int nNum, int usecurrent)
{
  int result; // eax
  int v5; // [esp+0h] [ebp-58h]
  DWORD dwHighDateTime; // [esp+Ch] [ebp-4Ch]
  DWORD dwLowDateTime; // [esp+10h] [ebp-48h]
  unsigned int currgamesplayed; // [esp+24h] [ebp-34h]
  unsigned int currpct; // [esp+28h] [ebp-30h]
  int showinfo; // [esp+2Ch] [ebp-2Ch]
  const FILETIME *info; // [esp+30h] [ebp-28h]
  struct _FILETIME localtime; // [esp+34h] [ebp-24h] BYREF
  unsigned int delaypct; // [esp+3Ch] [ebp-1Ch]
  struct _SYSTEMTIME systime; // [esp+40h] [ebp-18h] BYREF
  unsigned int delaygamesplayed; // [esp+50h] [ebp-8h]
  int player; // [esp+54h] [ebp-4h]

  result = TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Clear(a1: &sgPlayerList);
  for ( player = 0; player < nNum; ++player )
  {
    result = a1;
    info = *(const FILETIME **)(a1 + 4 * player);
    if ( info == nullptr )
      break;
    showinfo = TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::NewNode(location: 2, extrabytes: 0, flags: 0);
    *(_DWORD *)(showinfo + 8) = a2 + player;
    sprintf(Buffer: (char *const)(showinfo + 12), Format: "%u", *(_DWORD *)(showinfo + 8) + 1);
    SStrCopy(a1: showinfo + 20, a2: &info[10], a3: 16);
    if ( usecurrent != 0 )
      FileTimeToLocalFileTime(lpFileTime: info + 8, lpLocalFileTime: &localtime);
    else
      FileTimeToLocalFileTime(lpFileTime: info + 9, lpLocalFileTime: &localtime);
    FileTimeToSystemTime(lpFileTime: &localtime, lpSystemTime: &systime);
    GetDateFormatA(
      Locale: 0x400u,
      dwFlags: 1u,
      lpDate: &systime,
      lpFormat: nullptr,
      lpDateStr: (LPSTR)(showinfo + 36),
      cchDate: 64);
    currgamesplayed = info->dwHighDateTime + info->dwLowDateTime;
    delaygamesplayed = info[3].dwLowDateTime + info[2].dwHighDateTime;
    if ( currgamesplayed != 0 )
      currpct = 100 * info->dwLowDateTime / currgamesplayed;
    else
      currpct = 0;
    if ( delaygamesplayed != 0 )
      delaypct = 100 * info[2].dwHighDateTime / delaygamesplayed;
    else
      delaypct = 0;
    if ( usecurrent != 0 )
      sprintf(Buffer: (char *const)(showinfo + 100), Format: "%u%%", currpct);
    else
      sprintf(Buffer: (char *const)(showinfo + 100), Format: "%u%%", delaypct);
    if ( usecurrent != 0 )
      sprintf(Buffer: (char *const)(showinfo + 108), Format: "%u", info[1].dwHighDateTime);
    else
      sprintf(Buffer: (char *const)(showinfo + 108), Format: "%u", info[4].dwLowDateTime);
    if ( usecurrent != 0 )
      dwLowDateTime = info[1].dwLowDateTime;
    else
      dwLowDateTime = info[3].dwHighDateTime;
    if ( usecurrent != 0 )
      dwHighDateTime = info->dwHighDateTime;
    else
      dwHighDateTime = info[3].dwLowDateTime;
    if ( usecurrent != 0 )
      sprintf(
        Buffer: (char *const)(showinfo + 124),
        Format: "%u-%u-%u",
        info->dwLowDateTime,
        dwHighDateTime,
        dwLowDateTime);
    else
      sprintf(
        Buffer: (char *const)(showinfo + 124),
        Format: "%u-%u-%u",
        info[2].dwHighDateTime,
        dwHighDateTime,
        dwLowDateTime);
    if ( usecurrent != 0 )
      sprintf(
        Buffer: (char *const)(showinfo + 188),
        Format: "%u",
        info[1].dwLowDateTime + info->dwHighDateTime + info->dwLowDateTime);
    else
      sprintf(
        Buffer: (char *const)(showinfo + 188),
        Format: "%u",
        info[3].dwHighDateTime + info[3].dwLowDateTime + info[2].dwHighDateTime);
    if ( usecurrent != 0 )
    {
      *(_DWORD *)(showinfo + 220) |= info[4].dwLowDateTime != info[1].dwHighDateTime ? 2 : 0;
      if ( info->dwLowDateTime == info[2].dwHighDateTime
        && info->dwHighDateTime == info[3].dwLowDateTime
        && info[1].dwLowDateTime == info[3].dwHighDateTime )
      {
        v5 = 0;
      }
      else
      {
        v5 = 4;
      }
      *(_DWORD *)(showinfo + 220) |= v5;
      *(_DWORD *)(showinfo + 220) |= info[3].dwHighDateTime + info[3].dwLowDateTime + info[2].dwHighDateTime != info[1].dwLowDateTime + info->dwHighDateTime + info->dwLowDateTime
                                   ? 8
                                   : 0;
      *(_DWORD *)(showinfo + 220) |= delaypct != currpct ? 0x10 : 0;
      *(_DWORD *)(showinfo + 220) |= CompareFileTime(lpFileTime1: info + 8, lpFileTime2: info + 9) != 0 ? 0x20 : 0;
    }
    result = player + 1;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900EF0C
 * CodeView symbol: PlayerSearch
 * Demangled: PlayerSearch
 * IDA name: PlayerSearch
 * CodeView module: 18
 * CV address: segment 1 : 0xDF0C
 * Code length: 0xD5
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall PlayerSearch(HWND this)
{
  HWND DlgItem; // eax
  WPARAM v2; // [esp-8h] [ebp-24h]
  unsigned int index; // [esp+4h] [ebp-18h] BYREF
  LRESULT type; // [esp+8h] [ebp-14h]
  char szName[16]; // [esp+Ch] [ebp-10h] BYREF

  SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1010, Msg: 0xDu, wParam: 0x10u, lParam: (LPARAM)szName);
  type = SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1067, Msg: 0x147u, wParam: 0, lParam: 0);
  index = 0;
  SrvFindLadderUser(
    a1: *(_DWORD *)(*((_DWORD *)sgpUiparams + 1) + 12),
    a2: 1u,
    sortcriterion: sgLadderTbl[6 * type],
    a4: szName,
    a5: &index);
  if ( index != -1 && sgnVisiblePlayers != 0 )
  {
    sgnFirstEntry = index / sgnVisiblePlayers * sgnVisiblePlayers;
    ScanList(a1: this, a2: index / sgnVisiblePlayers * sgnVisiblePlayers);
    v2 = index % sgnVisiblePlayers;
    DlgItem = GetDlgItem(hDlg: this, nIDDlgItem: 1065);
    SendMessageA(hWnd: DlgItem, Msg: 0x186u, wParam: v2, lParam: 0);
  }
}


// ==============================================================================
/*
 * EA: 0x1900EFE1
 * CodeView symbol: DrawPlayer
 * Demangled: DrawPlayer
 * IDA name: DrawPlayer
 * CodeView module: 18
 * CV address: segment 1 : 0xDFE1
 * Code length: 0x4BA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
COLORREF __stdcall DrawPlayer(COLORREF lparam)
{
  COLORREF result; // eax
  COLORREF v2; // eax
  UINT v3; // eax
  UINT v4; // eax
  UINT v5; // eax
  UINT v6; // eax
  UINT v7; // eax
  UINT v8; // eax
  UINT v9; // eax
  DWORD color; // [esp+1Ch] [ebp-38h]
  BOOL v11; // [esp+20h] [ebp-34h]
  COLORREF oldtxt; // [esp+2Ch] [ebp-28h]
  RECT rect; // [esp+3Ch] [ebp-18h] BYREF
  int node; // [esp+4Ch] [ebp-8h]
  COLORREF oldbkg; // [esp+50h] [ebp-4h]

  result = lparam;
  if ( *(_DWORD *)(lparam + 24) != 0 )
  {
    v11 = (*(_DWORD *)(lparam + 16) & 1) != 0 && GetFocus() == *(HWND *)(lparam + 20);
    result = lparam;
    node = *(_DWORD *)(lparam + 44);
    if ( node != 0 )
    {
      oldtxt = SetTextColor(hdc: *(HDC *)(lparam + 24), color: 0xFFFFFFu);
      if ( v11 )
      {
        color = GetSysColor(nIndex: 13);
        v2 = SetBkColor(hdc: *(HDC *)(lparam + 24), color);
      }
      else
      {
        v2 = SetBkColor(hdc: *(HDC *)(lparam + 24), color: 0);
      }
      oldbkg = v2;
      rect = *(RECT *)(lparam + 28);
      SetTextColor(hdc: *(HDC *)(lparam + 24), color: 0xFFFFFFu);
      rect.left = snTabTbl + *(_DWORD *)(lparam + 28);
      v3 = strlen(Str: (const char *)(node + 12));
      ExtTextOutA(
        hdc: *(HDC *)(lparam + 24),
        x: snTabTbl + *(_DWORD *)(lparam + 28),
        y: *(_DWORD *)(lparam + 32),
        options: 6u,
        lprect: &rect,
        lpString: (LPCSTR)(node + 12),
        c: v3,
        lpDx: nullptr);
      SetTextColor(hdc: *(HDC *)(lparam + 24), color: 0xFFFFFFu);
      rect.left = unk_1903CC48 + *(_DWORD *)(lparam + 28);
      v4 = strlen(Str: (const char *)(node + 20));
      ExtTextOutA(
        hdc: *(HDC *)(lparam + 24),
        x: *(_DWORD *)(lparam + 28) + unk_1903CC48 + 2,
        y: *(_DWORD *)(lparam + 32),
        options: 6u,
        lprect: &rect,
        lpString: (LPCSTR)(node + 20),
        c: v4,
        lpDx: nullptr);
      SetTextColor(hdc: *(HDC *)(lparam + 24), color: (*(_DWORD *)(node + 220) & 2) != 0 ? 65280 : 0xFFFFFF);
      rect.left = unk_1903CC4C + *(_DWORD *)(lparam + 28);
      v5 = strlen(Str: (const char *)(node + 108));
      ExtTextOutA(
        hdc: *(HDC *)(lparam + 24),
        x: unk_1903CC4C + *(_DWORD *)(lparam + 28),
        y: *(_DWORD *)(lparam + 32),
        options: 6u,
        lprect: &rect,
        lpString: (LPCSTR)(node + 108),
        c: v5,
        lpDx: nullptr);
      SetTextColor(hdc: *(HDC *)(lparam + 24), color: (*(_DWORD *)(node + 220) & 4) != 0 ? 65280 : 0xFFFFFF);
      rect.left = unk_1903CC50 + *(_DWORD *)(lparam + 28);
      v6 = strlen(Str: (const char *)(node + 124));
      ExtTextOutA(
        hdc: *(HDC *)(lparam + 24),
        x: unk_1903CC50 + *(_DWORD *)(lparam + 28),
        y: *(_DWORD *)(lparam + 32),
        options: 6u,
        lprect: &rect,
        lpString: (LPCSTR)(node + 124),
        c: v6,
        lpDx: nullptr);
      SetTextColor(hdc: *(HDC *)(lparam + 24), color: (*(_DWORD *)(node + 220) & 8) != 0 ? 65280 : 0xFFFFFF);
      rect.left = unk_1903CC54 + *(_DWORD *)(lparam + 28);
      v7 = strlen(Str: (const char *)(node + 188));
      ExtTextOutA(
        hdc: *(HDC *)(lparam + 24),
        x: unk_1903CC54 + *(_DWORD *)(lparam + 28),
        y: *(_DWORD *)(lparam + 32),
        options: 6u,
        lprect: &rect,
        lpString: (LPCSTR)(node + 188),
        c: v7,
        lpDx: nullptr);
      SetTextColor(hdc: *(HDC *)(lparam + 24), color: (*(_DWORD *)(node + 220) & 0x10) != 0 ? 65280 : 0xFFFFFF);
      rect.left = unk_1903CC58 + *(_DWORD *)(lparam + 28);
      v8 = strlen(Str: (const char *)(node + 100));
      ExtTextOutA(
        hdc: *(HDC *)(lparam + 24),
        x: unk_1903CC58 + *(_DWORD *)(lparam + 28),
        y: *(_DWORD *)(lparam + 32),
        options: 6u,
        lprect: &rect,
        lpString: (LPCSTR)(node + 100),
        c: v8,
        lpDx: nullptr);
      SetTextColor(hdc: *(HDC *)(lparam + 24), color: (*(_DWORD *)(node + 220) & 0x20) != 0 ? 65280 : 0xFFFFFF);
      rect.left = unk_1903CC5C + *(_DWORD *)(lparam + 28);
      v9 = strlen(Str: (const char *)(node + 36));
      ExtTextOutA(
        hdc: *(HDC *)(lparam + 24),
        x: unk_1903CC5C + *(_DWORD *)(lparam + 28),
        y: *(_DWORD *)(lparam + 32),
        options: 6u,
        lprect: &rect,
        lpString: (LPCSTR)(node + 36),
        c: v9,
        lpDx: nullptr);
      SetTextColor(hdc: *(HDC *)(lparam + 24), color: oldtxt);
      return SetBkColor(hdc: *(HDC *)(lparam + 24), color: oldbkg);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900F4A0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >
 * IDA name: ??0?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QAE@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE4A0
 * Code length: 0x21
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
char *__thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>(char *ecx0)
{
  TSLink<_PLAYERINFO>::TSLink<_PLAYERINFO>(a1: ecx0 + 4);
  TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Constructor(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x1900F4D0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::~TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::~TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >
 * IDA name: ??1?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QAE@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE4D0
 * Code length: 0x1E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::~TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>(char *ecx0)
{
  TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::UnlinkAll(a1: ecx0);
  return TSLink<_PLAYERINFO>::~TSLink<_PLAYERINFO>(a1: ecx0 + 4);
}


// ==============================================================================
/*
 * EA: 0x1900F4F0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Clear
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Clear
 * IDA name: ?Clear@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QAEXXZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE4F0
 * Code length: 0x48
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_PLAYERINFO *__thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Clear(void *ecx0)
{
  _PLAYERINFO *result; // eax

  while ( 1 )
  {
    result = (_PLAYERINFO *)TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Head(a1: ecx0);
    if ( result == nullptr )
      break;
    _PLAYERINFO::`scalar deleting destructor'(this: result, a2: 1u);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900F540
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::DeleteNode
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::DeleteNode
 * IDA name: ?DeleteNode@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QAEPAU_PLAYERINFO@@PAU2@@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xE540
 * Code length: 0x49
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::DeleteNode(_PLAYERINFO *ptr)
{
  int nextptr; // [esp+10h] [ebp-4h]

  nextptr = TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Next((int)ptr);
  if ( ptr != nullptr )
    _PLAYERINFO::`scalar deleting destructor'(this: ptr, a2: 1u);
  return nextptr;
}


// ==============================================================================
/*
 * EA: 0x1900F590
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Head
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Head
 * IDA name: ?Head@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QBEPAU_PLAYERINFO@@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE590
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Head(char *this)
{
  return TSLink<_PLAYERINFO>::Next(a1: this + 4, a2: this);
}


// ==============================================================================
/*
 * EA: 0x1900F5B0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::NewNode
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::NewNode
 * IDA name: ?NewNode@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QAEPAU_PLAYERINFO@@KKK@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xE5B0
 * Code length: 0x7D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_PLAYERINFO *__stdcall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::NewNode(int location, int extrabytes, int flags)
{
  int v3; // eax
  const char *v4; // eax
  void *v5; // eax
  int v7; // [esp-4h] [ebp-14h]
  _PLAYERINFO *v8; // [esp+0h] [ebp-10h]
  _PLAYERINFO *v9; // [esp+8h] [ebp-8h]

  v3 = flags;
  LOBYTE(v3) = flags | 8;
  v7 = v3;
  v4 = type_info::internal_raw_name(this: (type_info *)&_PLAYERINFO `RTTI Type Descriptor');
  v5 = (void *)SMemAlloc(a1: extrabytes + 224, a2: v4, a3: -2, a4: v7);
  v9 = (_PLAYERINFO *)operator new(__formal: 224, ptr: v5);
  if ( v9 != nullptr )
    v8 = _PLAYERINFO::_PLAYERINFO(this: v9);
  else
    v8 = nullptr;
  if ( location != 0 )
    TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::LinkNode(ptr: (int)v8, linktype: location, existingptr: 0);
  return v8;
}


// ==============================================================================
/*
 * EA: 0x1900F630
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::RawNext
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::RawNext
 * IDA name: ?RawNext@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QBEPAU_PLAYERINFO@@PBU2@@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xE630
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::RawNext(void *ecx0, int ptr)
{
  int v2; // eax

  v2 = TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Link(ptr);
  return TSLink<_PLAYERINFO>::RawNext(a1: v2, a2: ecx0);
}


// ==============================================================================
/*
 * EA: 0x1900F650
 * CodeView symbol: _PLAYERINFO::`scalar deleting destructor'
 * Demangled: _PLAYERINFO::`scalar deleting destructor'
 * IDA name: ??_G_PLAYERINFO@@QAEPAXI@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xE650
 * Code length: 0x2E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_PLAYERINFO *__thiscall _PLAYERINFO::`scalar deleting destructor'(_PLAYERINFO *this, char __flags)
{
  _PLAYERINFO::~_PLAYERINFO(this);
  if ( (__flags & 1) != 0 )
    operator delete(ptr: this);
  return this;
}


// ==============================================================================
/*
 * EA: 0x1900F680
 * CodeView symbol: _PLAYERINFO::_PLAYERINFO
 * Demangled: _PLAYERINFO::_PLAYERINFO
 * IDA name: ??0_PLAYERINFO@@QAE@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE680
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_PLAYERINFO *__thiscall _PLAYERINFO::_PLAYERINFO(_PLAYERINFO *this)
{
  TSLinkedNode<_PLAYERINFO>::TSLinkedNode<_PLAYERINFO>(a1: this);
  return this;
}


// ==============================================================================
/*
 * EA: 0x1900F6A0
 * CodeView symbol: _PLAYERINFO::~_PLAYERINFO
 * Demangled: _PLAYERINFO::~_PLAYERINFO
 * IDA name: ??1_PLAYERINFO@@QAE@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE6A0
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall _PLAYERINFO::~_PLAYERINFO(_PLAYERINFO *this)
{
  TSLinkedNode<_PLAYERINFO>::~TSLinkedNode<_PLAYERINFO>(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x1900F6C0
 * CodeView symbol: TSLinkedNode<_PLAYERINFO>::TSLinkedNode<_PLAYERINFO>
 * Demangled: TSLinkedNode<_PLAYERINFO>::TSLinkedNode<_PLAYERINFO>
 * IDA name: ??0?$TSLinkedNode@U_PLAYERINFO@@@@QAE@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE6C0
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSLinkedNode<_PLAYERINFO>::TSLinkedNode<_PLAYERINFO>(void *ecx0)
{
  TSLink<_PLAYERINFO>::TSLink<_PLAYERINFO>(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x1900F6E0
 * CodeView symbol: TSLinkedNode<_PLAYERINFO>::~TSLinkedNode<_PLAYERINFO>
 * Demangled: TSLinkedNode<_PLAYERINFO>::~TSLinkedNode<_PLAYERINFO>
 * IDA name: ??1?$TSLinkedNode@U_PLAYERINFO@@@@QAE@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE6E0
 * Code length: 0x1B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLinkedNode<_PLAYERINFO>::~TSLinkedNode<_PLAYERINFO>(void *ecx0)
{
  TSLinkedNode<_PLAYERINFO>::Unlink(a1: ecx0);
  return TSLink<_PLAYERINFO>::~TSLink<_PLAYERINFO>(a1: ecx0);
}


// ==============================================================================
/*
 * EA: 0x1900F700
 * CodeView symbol: TSLink<_PLAYERINFO>::TSLink<_PLAYERINFO>
 * Demangled: TSLink<_PLAYERINFO>::TSLink<_PLAYERINFO>
 * IDA name: ??0?$TSLink@U_PLAYERINFO@@@@QAE@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE700
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSLink<_PLAYERINFO>::TSLink<_PLAYERINFO>(void *ecx0)
{
  TSLink<_PLAYERINFO>::Constructor(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x1900F720
 * CodeView symbol: TSLink<_PLAYERINFO>::~TSLink<_PLAYERINFO>
 * Demangled: TSLink<_PLAYERINFO>::~TSLink<_PLAYERINFO>
 * IDA name: ??1?$TSLink@U_PLAYERINFO@@@@QAE@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE720
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_PLAYERINFO>::~TSLink<_PLAYERINFO>(void *this)
{
  return TSLink<_PLAYERINFO>::Unlink(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x1900F740
 * CodeView symbol: TSLink<_PLAYERINFO>::Next
 * Demangled: TSLink<_PLAYERINFO>::Next
 * IDA name: ?Next@?$TSLink@U_PLAYERINFO@@@@QBEPAU_PLAYERINFO@@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE740
 * Code length: 0x2B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_PLAYERINFO>::Next(_DWORD *this)
{
  if ( (int)*(this + 1) <= 0 )
    return 0;
  else
    return *(this + 1);
}


// ==============================================================================
/*
 * EA: 0x1900F770
 * CodeView symbol: TSLink<_PLAYERINFO>::RawNext
 * Demangled: TSLink<_PLAYERINFO>::RawNext
 * IDA name: ?RawNext@?$TSLink@U_PLAYERINFO@@@@QBEPAU_PLAYERINFO@@XZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE770
 * Code length: 0x11
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_PLAYERINFO>::RawNext(_DWORD *this)
{
  return *(this + 1);
}


// ==============================================================================
/*
 * EA: 0x1900F790
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Constructor
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Constructor
 * IDA name: ?Constructor@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@AAEXXZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE790
 * Code length: 0x1C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Constructor(_DWORD *this)
{
  *this = 0;
  return TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::InitializeTerminator(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x1900F7B0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Link
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Link
 * IDA name: ?Link@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@ABEPAV?$TSLink@U_PLAYERINFO@@@@PBU_PLAYERINFO@@@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xE7B0
 * Code length: 0x1A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Link(_DWORD *this, int ptr)
{
  return TSGetLink<_PLAYERINFO>::Link(a1: ptr, a2: *this, a3: this);
}


// ==============================================================================
/*
 * EA: 0x1900F7D0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::LinkNode
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::LinkNode
 * IDA name: ?LinkNode@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QAEXPAU_PLAYERINFO@@K0@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xE7D0
 * Code length: 0xC1
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::LinkNode(int *ecx0, int ptr, int linktype, int existingptr)
{
  int *result; // eax
  int prevlink; // [esp+8h] [ebp-Ch]
  int *existinglink; // [esp+Ch] [ebp-8h]
  _DWORD *link; // [esp+10h] [ebp-4h]

  link = (_DWORD *)TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Link(this: ecx0, ptr);
  if ( *link != 0 )
    TSLink<_PLAYERINFO>::Unlink(a1: link);
  if ( existingptr != 0 )
  {
    result = (int *)TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Link(this: ecx0, ptr: existingptr);
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
    *(_DWORD *)TSLink<_PLAYERINFO>::NextLink(linkoffset: *ecx0) = link;
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
 * EA: 0x1900F8A0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Next
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::Next
 * IDA name: ?Next@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QBEPAU_PLAYERINFO@@PBU2@@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xE8A0
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Next(_DWORD *this, int ptr)
{
  _DWORD *v2; // eax

  v2 = (_DWORD *)TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Link(this, ptr);
  return TSLink<_PLAYERINFO>::Next(this: v2);
}


// ==============================================================================
/*
 * EA: 0x1900F8C0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::UnlinkAll
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::UnlinkAll
 * IDA name: ?UnlinkAll@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QAEXXZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE8C0
 * Code length: 0x2C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::UnlinkAll(char *ecx0)
{
  int result; // eax

  while ( 1 )
  {
    result = TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Head(this: ecx0);
    if ( result == 0 )
      break;
    TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::UnlinkNode(ptr: result);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900F8F0
 * CodeView symbol: TSLinkedNode<_PLAYERINFO>::Unlink
 * Demangled: TSLinkedNode<_PLAYERINFO>::Unlink
 * IDA name: ?Unlink@?$TSLinkedNode@U_PLAYERINFO@@@@QAEXXZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE8F0
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLinkedNode<_PLAYERINFO>::Unlink(void *this)
{
  return TSLink<_PLAYERINFO>::Unlink(a1: this);
}


// ==============================================================================
/*
 * EA: 0x1900F910
 * CodeView symbol: TSLink<_PLAYERINFO>::Constructor
 * Demangled: TSLink<_PLAYERINFO>::Constructor
 * IDA name: ?Constructor@?$TSLink@U_PLAYERINFO@@@@AAEXXZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE910
 * Code length: 0x1E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSLink<_PLAYERINFO>::Constructor(_DWORD *this)
{
  *this = 0;
  *(this + 1) = 0;
  return this;
}


// ==============================================================================
/*
 * EA: 0x1900F930
 * CodeView symbol: TSLink<_PLAYERINFO>::NextLink
 * Demangled: TSLink<_PLAYERINFO>::NextLink
 * IDA name: ?NextLink@?$TSLink@U_PLAYERINFO@@@@ABEPAV1@H@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xE930
 * Code length: 0x3D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_PLAYERINFO>::NextLink(_DWORD *this, int linkoffset)
{
  if ( (int)*(this + 1) < 0 )
    return ~*(this + 1);
  if ( linkoffset < 0 )
    linkoffset = (int)this - *(_DWORD *)(*this + 4);
  return *(this + 1) + linkoffset;
}


// ==============================================================================
/*
 * EA: 0x1900F970
 * CodeView symbol: TSLink<_PLAYERINFO>::Unlink
 * Demangled: TSLink<_PLAYERINFO>::Unlink
 * IDA name: ?Unlink@?$TSLink@U_PLAYERINFO@@@@QAEXXZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE970
 * Code length: 0x47
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSLink<_PLAYERINFO>::Unlink(_DWORD *ecx0)
{
  _DWORD *result; // eax

  result = ecx0;
  if ( *ecx0 != 0 )
  {
    *(_DWORD *)TSLink<_PLAYERINFO>::NextLink(this: ecx0, linkoffset: -1) = *ecx0;
    result = (_DWORD *)*(ecx0 + 1);
    *(_DWORD *)(*ecx0 + 4) = result;
    *ecx0 = 0;
    *(ecx0 + 1) = 0;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900F9C0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::InitializeTerminator
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::InitializeTerminator
 * IDA name: ?InitializeTerminator@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@AAEXXZ
 * CodeView module: 18
 * CV address: segment 1 : 0xE9C0
 * Code length: 0x25
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::InitializeTerminator(_DWORD *this)
{
  *(this + 1) = this + 1;
  *(this + 2) = ~(unsigned int)(this + 1);
  return this;
}


// ==============================================================================
/*
 * EA: 0x1900F9F0
 * CodeView symbol: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::UnlinkNode
 * Demangled: TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO> >::UnlinkNode
 * IDA name: ?UnlinkNode@?$TSList@U_PLAYERINFO@@V?$TSGetLink@U_PLAYERINFO@@@@@@QAEXPAU_PLAYERINFO@@@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xE9F0
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::UnlinkNode(_DWORD *this, int ptr)
{
  _DWORD *v2; // eax

  v2 = (_DWORD *)TSList<_PLAYERINFO,TSGetLink<_PLAYERINFO>>::Link(this, ptr);
  return TSLink<_PLAYERINFO>::Unlink(ecx0: v2);
}


// ==============================================================================
/*
 * EA: 0x1900FA10
 * CodeView symbol: TSGetLink<_PLAYERINFO>::Link
 * Demangled: TSGetLink<_PLAYERINFO>::Link
 * IDA name: ?Link@?$TSGetLink@U_PLAYERINFO@@@@SIPAV?$TSLink@U_PLAYERINFO@@@@PBV?$TSLinkedNode@U_PLAYERINFO@@@@H@Z
 * CodeView module: 18
 * CV address: segment 1 : 0xEA10
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ladder.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ladder.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSGetLink<_PLAYERINFO>::Link(void *this)
{
  return this;
}

