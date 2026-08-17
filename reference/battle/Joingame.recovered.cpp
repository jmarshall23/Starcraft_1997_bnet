/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 12
 */


// ==============================================================================
/*
 * EA: 0x1900C3D0
 * CodeView symbol: JoinGameDialogProc
 * Demangled: JoinGameDialogProc
 * IDA name: ?JoinGameDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 19
 * CV address: segment 1 : 0xB3D0
 * Code length: 0x8C3
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall JoinGameDialogProc(HWND window, UINT message, HDC wparam, HWND lparam)
{
  HWND v5; // eax
  HWND v6; // eax
  HWND v7; // eax
  HWND FrameWindow; // eax
  HWND DlgItem; // eax
  WPARAM v10; // eax
  HWND v11; // eax
  HWND v12; // eax
  LRESULT v13; // [esp-4h] [ebp-D8h]
  int v14; // [esp+4h] [ebp-D0h]
  HWND v15; // [esp+10h] [ebp-C4h]
  struct tagRECT rect; // [esp+14h] [ebp-C0h] BYREF
  LRESULT v17; // [esp+24h] [ebp-B0h]
  LRESULT nItems; // [esp+28h] [ebp-ACh]
  HWND parent; // [esp+2Ch] [ebp-A8h]
  unsigned int *data; // [esp+30h] [ebp-A4h]
  LRESULT nPos; // [esp+34h] [ebp-A0h]
  LPARAM nLen; // [esp+38h] [ebp-9Ch]
  char *p; // [esp+3Ch] [ebp-98h]
  WPARAM wParam; // [esp+40h] [ebp-94h]
  HWND v25; // [esp+44h] [ebp-90h]
  char szText[128]; // [esp+48h] [ebp-8Ch] BYREF
  LRESULT nIndex; // [esp+C8h] [ebp-Ch]
  HWND hWnd; // [esp+CCh] [ebp-8h]
  HWND hWndGameList; // [esp+D0h] [ebp-4h]

  if ( message > 0x111 )
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
    else if ( message == 275 )
    {
      v14 = iter++ & 3;
      if ( v14 != 0 )
      {
        if ( v14 == 3 )
        {
          GetDlgItem(hDlg: window, nIDDlgItem: 1027);
          UpdateGameList(bSelGame: 0);
        }
      }
      else
      {
        DlgItem = GetDlgItem(hDlg: window, nIDDlgItem: 1027);
        UpdateGameLags(a1: DlgItem);
      }
    }
    else if ( message == 312 && GetWindowLongA(hWnd: lparam, nIndex: -12) == 1030 )
    {
      SetTextColor(hdc: wparam, color: 0xFFFFu);
      return (int)GetStockObject(i: 5);
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message == 273 )
  {
    if ( (unsigned __int16)wparam > 0x3F2u )
    {
      if ( (unsigned __int16)wparam == 1027 )
      {
        if ( HIWORD(wparam) == 2 )
        {
          hWnd = lparam;
          nIndex = SendMessageA(hWnd: lparam, Msg: 0x188u, wParam: 0, lParam: 0);
          if ( nIndex != -1 )
          {
            SendMessageA(hWnd: window, Msg: 0x111u, wParam: 0x10403u, lParam: (LPARAM)hWnd);
            v6 = GetDlgItem(hDlg: window, nIDDlgItem: 1);
            SendMessageA(hWnd: window, Msg: 0x111u, wParam: 1u, lParam: (LPARAM)v6);
          }
        }
        else if ( HIWORD(wparam) == 1 )
        {
          v25 = lparam;
          wParam = SendMessageA(hWnd: lparam, Msg: 0x188u, wParam: 0, lParam: 0);
          if ( wParam != -1 )
          {
            SendMessageA(hWnd: v25, Msg: 0x189u, wParam, lParam: (LPARAM)szText);
            p = strchr(Str: szText, Val: 9);
            if ( p != nullptr )
              *p = 0;
            nLen = strlen(Str: szText);
            SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1010, Msg: 0xCu, wParam: 0, lParam: (LPARAM)szText);
            SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1010, Msg: 0xB1u, wParam: 0, lParam: nLen);
            SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1011, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042B2C);
          }
          v7 = GetDlgItem(hDlg: window, nIDDlgItem: 1031);
          InvalidateRect(hWnd: v7, lpRect: nullptr, bErase: true);
          ListUpdateScrollbar(a1: v25);
        }
      }
      else if ( (unsigned __int16)wparam == 1063 && HIWORD(wparam) == 1 )
      {
        EnableWindow(hWnd: lparam, bEnable: false);
        EnableWindow(hWnd: window, bEnable: false);
        nPos = SendMessageA(hWnd: lparam, Msg: 0x147u, wParam: 0, lParam: 0);
        if ( nPos != -1 )
        {
          data = (unsigned int *)SendMessageA(hWnd: lparam, Msg: 0x150u, wParam: nPos, lParam: 0);
          if ( data != nullptr )
          {
            sgdwCategoryBits = *data;
            sgdwCategoryMask = data[1];
            GetDlgItem(hDlg: window, nIDDlgItem: 1027);
            UpdateGameList(bSelGame: 1);
          }
        }
        EnableWindow(hWnd: window, bEnable: true);
        EnableWindow(hWnd: lparam, bEnable: true);
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    switch ( (unsigned __int16)wparam )
    {
      case 0x3F2u:
        if ( HIWORD(wparam) == 768 && lparam == GetFocus() )
        {
          hWndGameList = GetDlgItem(hDlg: window, nIDDlgItem: 1027);
          SendMessageA(hWnd: hWndGameList, Msg: 0x186u, wParam: 0xFFFFFFFF, lParam: 0);
          SendMessageA(hWnd: window, Msg: 0x111u, wParam: 0x10403u, lParam: (LPARAM)hWndGameList);
        }
        v13 = SendMessageA(hWnd: lparam, Msg: 0xEu, wParam: 0, lParam: 0);
        v5 = GetDlgItem(hDlg: window, nIDDlgItem: 1);
        EnableWindow(hWnd: v5, bEnable: v13);
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
      case 1u:
        if ( IsWindowEnabled(hWnd: window) )
        {
          EnableWindow(hWnd: window, bEnable: false);
          if ( DoJoin(a1: window, a2: uiparams) == 0 )
            EnableWindow(hWnd: window, bEnable: true);
          return 0;
        }
        else
        {
          return 0;
        }
      case 2u:
        if ( *(_DWORD *)(*(_DWORD *)(uiparams + 12) + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(uiparams + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
        SDlgEndDialog(a1: window, a2: 0);
        return 0;
      default:
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
  }
  else
  {
    if ( message <= 0x105 )
    {
      if ( message >= 0x104 )
      {
        FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
        SendMessageA(hWnd: FrameWindow, Msg: message, wParam: (WPARAM)wparam, lParam: (LPARAM)lparam);
      }
      else if ( message == 2 )
      {
        sub_1900CC93(a1: window);
        if ( sghFilterCombo != nullptr )
        {
          RememberFilterSetting(a1: sghFilterCombo);
          nItems = SendMessageA(hWnd: sghFilterCombo, Msg: 0x146u, wParam: 0, lParam: 0);
          while ( 1 )
          {
            v10 = nItems--;
            if ( v10 == 0 )
              break;
            v17 = SendMessageA(hWnd: sghFilterCombo, Msg: 0x150u, wParam: nItems, lParam: 0);
            if ( v17 != 0 )
              SMemFree(a1: v17, a2: aJoingameCpp, a3: 797, a4: 0);
          }
        }
        if ( sbSecondTimerActive != 0 )
          SDlgKillTimer(a1: window, a2: 2);
        if ( mediumfont != nullptr )
          DeleteObject(ho: mediumfont);
        if ( smallfont != nullptr )
          DeleteObject(ho: smallfont);
      }
      else if ( message == 43 )
      {
        return JoinDrawItem((int)window, (int)wparam, (int)lparam);
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( message != 272 )
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    uiparams = (int)lparam;
    sbSecondTimerActive = 1;
    sub_1900CD15(a1: window, a2: *(_DWORD *)(*((_DWORD *)lparam + 3) + 12));
    v11 = GetDlgItem(hDlg: window, nIDDlgItem: 1);
    EnableWindow(hWnd: v11, bEnable: false);
    smallfont = (HGDIOBJ)UiSetFont(a1: window, a2: 0, a3: &unk_1903CB2C);
    v15 = GetDlgItem(hDlg: window, nIDDlgItem: 1027);
    v12 = GetDlgItem(hDlg: window, nIDDlgItem: 1052);
    ScrollbarLink(a1: v15, a2: v12);
    if ( *(_DWORD *)(*(_DWORD *)(uiparams + 12) + 48) != 0
      && (*(int (__stdcall **)(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, unsigned int *, unsigned int *))(*(_DWORD *)(uiparams + 12) + 48))(
           a1: 0,
           a2: *(_DWORD *)(uiparams + 4),
           a3: *(_DWORD *)(uiparams + 8),
           a4: *(_DWORD *)(uiparams + 12),
           a5: *(_DWORD *)(uiparams + 16),
           a6: &sgdwCategoryBits,
           a7: &sgdwCategoryMask) == 0 )
    {
      SDlgEndDialog(a1: window, a2: 0);
    }
    if ( *(_DWORD *)(*(_DWORD *)(uiparams + 12) + 52) != 0
      && (sghFilterCombo = GetDlgItem(hDlg: window, nIDDlgItem: 1063), sghFilterCombo != nullptr) )
    {
      mediumfont = (HGDIOBJ)UiSetFont(a1: window, a2: 1, a3: &mediumtbl);
      SendMessageA(hWnd: sghFilterCombo, Msg: 0x141u, wParam: 0x80u, lParam: 0);
      (*(void (__stdcall **)(_DWORD, int (__stdcall *)(int, int, int)))(*(_DWORD *)(uiparams + 12) + 52))(
        a1: *(_DWORD *)(uiparams + 8),
        a2: JoinAddCategory);
      SendMessageA(hWnd: sghFilterCombo, Msg: 0x14Eu, wParam: 0, lParam: 0);
      GetFilterSetting(a1: sghFilterCombo);
      SendMessageA(hWnd: window, Msg: 0x111u, wParam: 0x10427u, lParam: (LPARAM)sghFilterCombo);
    }
    else
    {
      mediumfont = nullptr;
      sghFilterCombo = nullptr;
      UpdateGameList(bSelGame: 1);
    }
    SDlgSetTimer(a1: window, a2: 2, a3: 3000, a4: 0);
    GetClientRect(hWnd: v15, lpRect: &rect);
    SendMessageA(hWnd: v15, Msg: 0x192u, wParam: 1u, lParam: (LPARAM)&rect.right);
    SendMessageA(hWnd: v15, Msg: 0x1A0u, wParam: 0, lParam: 20);
    return 1;
  }
}


// ==============================================================================
/*
 * EA: 0x1900CC93
 * CodeView symbol: DestroyArtwork
 * Demangled: DestroyArtwork
 * IDA name: sub_1900CC93
 * CodeView module: 19
 * CV address: segment 1 : 0xBC93
 * Code length: 0x82
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1900CC93()
{
  int result; // eax

  if ( dword_19042A14 != nullptr )
  {
    result = SMemFree(a1: dword_19042A14, a2: aJoingameCpp_0, a3: 64, a4: 0);
    dword_19042A14 = nullptr;
  }
  if ( dword_19042A18 != nullptr )
  {
    result = SMemFree(a1: dword_19042A18, a2: aJoingameCpp_1, a3: 69, a4: 0);
    dword_19042A18 = nullptr;
  }
  if ( sgListboxBmp != 0 )
  {
    result = SMemFree(a1: sgListboxBmp, a2: aJoingameCpp_2, a3: 75, a4: 0);
    sgListboxBmp = 0;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900CD15
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: sub_1900CD15
 * CodeView module: 19
 * CV address: segment 1 : 0xBD15
 * Code length: 0xBC
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_1900CD15(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  _DWORD btn_ids[3]; // [esp+8h] [ebp-24h] BYREF
  _DWORD btn_desc[2]; // [esp+14h] [ebp-18h] BYREF
  struct tagSIZE bgSize; // [esp+1Ch] [ebp-10h] BYREF
  struct tagSIZE sizeBtns; // [esp+24h] [ebp-8h] BYREF

  btn_ids[0] = 1;
  btn_ids[1] = 2;
  btn_ids[2] = 0;
  btn_desc[0] = 1031;
  btn_desc[1] = 0;
  UiLoadArtwork(
    a1: a2,
    a2: a1,
    hWndParent: nullptr,
    artid: 2u,
    controltype: byte_19042B30,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19042A14,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 5u,
    controltype: aButton_9,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19042A18,
    a11: &sizeBtns);
  SDlgSetControlBitmaps(a1, a2: btn_ids, a3: 0, a4: dword_19042A18, a5: &sizeBtns, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: btn_desc, a3: 0, a4: dword_19042A14, a5: &bgSize, a6: 2, a7: -1);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900CDD1
 * CodeView symbol: UpdateGameList
 * Demangled: UpdateGameList
 * IDA name: UpdateGameList
 * CodeView module: 19
 * CV address: segment 1 : 0xBDD1
 * Code length: 0x39C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall UpdateGameList(HWND a1, HWND a2, int bSelGame)
{
  unsigned int v3; // eax
  LRESULT v4; // eax
  int v5; // [esp-8h] [ebp-32Ch]
  const char *v6; // [esp-4h] [ebp-328h]
  char *p; // [esp+8h] [ebp-31Ch]
  char liststring[256]; // [esp+Ch] [ebp-318h] BYREF
  WPARAM index; // [esp+10Ch] [ebp-218h]
  LRESULT nSelect; // [esp+110h] [ebp-214h]
  char szOldString[256]; // [esp+114h] [ebp-210h] BYREF
  char szString[256]; // [esp+214h] [ebp-110h] BYREF
  WPARAM nIndex; // [esp+314h] [ebp-10h]
  LRESULT nTop; // [esp+318h] [ebp-Ch]
  int gamelist; // [esp+31Ch] [ebp-8h] BYREF
  int curr; // [esp+320h] [ebp-4h]

  if ( LockGameList(a1: &gamelist) != 0 )
  {
    curr = gamelist;
    SendMessageA(hWnd: a2, Msg: 0xBu, wParam: 0, lParam: 0);
    nTop = SendMessageA(hWnd: a2, Msg: 0x18Eu, wParam: 0, lParam: 0);
    while ( curr != 0 )
    {
      if ( (*(_DWORD *)(curr + 4) & 1) != 0 )
      {
        curr = *(_DWORD *)(curr + 296);
      }
      else
      {
        nIndex = ListFindName(a1: a2, a2: (const char *)(curr + 40));
        v6 = (const char *)(curr + 168);
        v5 = *(_DWORD *)(curr + 8);
        v3 = NormalizeNetLag(a1: *(_DWORD *)(curr + 28));
        sprintf(Buffer: szString, Format: "%s\t%x %x %x\t%s", (const char *)(curr + 40), *(_DWORD *)curr, v3, v5, v6);
        if ( nIndex == -1 )
        {
          SendMessageA(hWnd: a2, Msg: 0x180u, wParam: 0, lParam: (LPARAM)szString);
          if ( bSelGame != 0
            && SendMessageA(hWnd: a2, Msg: 0x188u, wParam: 0, lParam: 0) == -1
            && SendDlgItemMessageA(hDlg: a1, nIDDlgItem: 1010, Msg: 0xEu, wParam: 0, lParam: 0) == 0 )
          {
            SendMessageA(hWnd: a2, Msg: 0x186u, wParam: 0, lParam: 0);
            SendMessageA(hWnd: a1, Msg: 0x111u, wParam: 0x10403u, lParam: (LPARAM)a2);
          }
        }
        else
        {
          SendMessageA(hWnd: a2, Msg: 0x189u, wParam: nIndex, lParam: (LPARAM)szOldString);
          if ( strcmp(Str1: szString, Str2: szOldString) != 0 )
          {
            nSelect = SendMessageA(hWnd: a2, Msg: 0x188u, wParam: 0, lParam: 0);
            SendMessageA(hWnd: a2, Msg: 0x182u, wParam: nIndex, lParam: 0);
            SendMessageA(hWnd: a2, Msg: 0x181u, wParam: nIndex, lParam: (LPARAM)szString);
            if ( nSelect == nIndex )
              SendMessageA(hWnd: a2, Msg: 0x186u, wParam: nIndex, lParam: 0);
          }
        }
        curr = *(_DWORD *)(curr + 296);
      }
    }
    if ( bSelGame != 0 )
    {
      index = 0;
      while ( SendMessageA(hWnd: a2, Msg: 0x189u, wParam: index, lParam: (LPARAM)liststring) != -1 )
      {
        curr = gamelist;
        p = strchr(Str: liststring, Val: 9);
        if ( p != nullptr )
          *p = 0;
        while ( curr != 0 && strcmp(Str1: (const char *)(curr + 40), Str2: liststring) != 0 )
          curr = *(_DWORD *)(curr + 296);
        if ( curr != 0 )
        {
          ++index;
        }
        else
        {
          v4 = SendMessageA(hWnd: a2, Msg: 0x188u, wParam: 0, lParam: 0);
          if ( v4 == index )
          {
            SendMessageA(hWnd: a2, Msg: 0x186u, wParam: index - 1, lParam: 0);
            SendMessageA(hWnd: a1, Msg: 0x111u, wParam: 0x10403u, lParam: (LPARAM)a2);
          }
          SendMessageA(hWnd: a2, Msg: 0x182u, wParam: index, lParam: 0);
        }
      }
    }
    UnlockGamelist(a1: gamelist);
    SrvMaintainLatencies();
    SendMessageA(hWnd: a2, Msg: 0x197u, wParam: nTop, lParam: 0);
    SendMessageA(hWnd: a2, Msg: 0xBu, wParam: 1u, lParam: 0);
    ListUpdateScrollbar(a1: a2);
  }
}


// ==============================================================================
/*
 * EA: 0x1900D16D
 * CodeView symbol: LockGameList
 * Demangled: LockGameList
 * IDA name: LockGameList
 * CodeView module: 19
 * CV address: segment 1 : 0xC16D
 * Code length: 0x46
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __thiscall LockGameList(struct _SNETSPI_GAMELIST **this)
{
  if ( sgbListLocked != 0 )
    return false;
  sgbListLocked = 1;
  return SpiLockGameList(categorybits: sgdwCategoryBits, categorymask: sgdwCategoryMask, gamelist: this) != 0;
}


// ==============================================================================
/*
 * EA: 0x1900D1B3
 * CodeView symbol: UnlockGamelist
 * Demangled: UnlockGamelist
 * IDA name: UnlockGamelist
 * CodeView module: 19
 * CV address: segment 1 : 0xC1B3
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall UnlockGamelist(struct _SNETSPI_GAMELIST *this)
{
  int result; // eax

  result = SpiUnlockGameList(gamelist: this, hintnextcall: nullptr);
  sgbListLocked = 0;
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900D1D3
 * CodeView symbol: UpdateGameLags
 * Demangled: UpdateGameLags
 * IDA name: UpdateGameLags
 * CodeView module: 19
 * CV address: segment 1 : 0xC1D3
 * Code length: 0x237
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __thiscall UpdateGameLags(HWND this)
{
  LRESULT result; // eax
  unsigned int v2; // eax
  int v3; // [esp-8h] [ebp-26Ch]
  LRESULT nSelect; // [esp+4h] [ebp-260h]
  LRESULT nTop; // [esp+8h] [ebp-25Ch]
  LRESULT nCount; // [esp+Ch] [ebp-258h]
  signed int i; // [esp+10h] [ebp-254h]
  char szString[256]; // [esp+14h] [ebp-250h] BYREF
  unsigned int nGameId; // [esp+114h] [ebp-150h] BYREF
  char nLag; // [esp+118h] [ebp-14Ch] BYREF
  _DWORD curr[3]; // [esp+11Ch] [ebp-148h] BYREF
  _BYTE v13[28]; // [esp+128h] [ebp-13Ch] BYREF
  char v14[128]; // [esp+144h] [ebp-120h] BYREF
  char v15[148]; // [esp+1C4h] [ebp-A0h] BYREF
  unsigned int dwLatency; // [esp+258h] [ebp-Ch] BYREF
  char nTime; // [esp+25Ch] [ebp-8h] BYREF
  char *p; // [esp+260h] [ebp-4h]

  SrvMaintainLatencies();
  SendMessageA(hWnd: this, Msg: 0xBu, wParam: 0, lParam: 0);
  nTop = SendMessageA(hWnd: this, Msg: 0x18Eu, wParam: 0, lParam: 0);
  result = SendMessageA(hWnd: this, Msg: 0x18Bu, wParam: 0, lParam: 0);
  nCount = result;
  if ( result != -1 )
  {
    for ( i = 0; i < nCount; ++i )
    {
      if ( SendMessageA(hWnd: this, Msg: 0x189u, wParam: i, lParam: (LPARAM)szString) != -1 )
      {
        p = strchr(Str: szString, Val: 9);
        if ( p != nullptr
          && sscanf(Buffer: ++p, Format: "%x %x %x", &nGameId, &nLag, &nTime) == 3
          && SpiGetGameInfo(
               gameid: nGameId,
               gamename: gamename,
               gamepassword: gamepassword,
               gameinfo: (struct _SNETSPI_GAMELIST *)curr) != 0 )
        {
          SrvGetLatency(a1: (struct _SNETADDR *)v13, a2: &dwLatency);
          v3 = curr[2];
          v2 = NormalizeNetLag(a1: dwLatency);
          sprintf(Buffer: szString, Format: "%s\t%x %x %x\t%s", v14, curr[0], v2, v3, v15);
          nSelect = SendMessageA(hWnd: this, Msg: 0x188u, wParam: 0, lParam: 0);
          SendMessageA(hWnd: this, Msg: 0x182u, wParam: i, lParam: 0);
          SendMessageA(hWnd: this, Msg: 0x181u, wParam: i, lParam: (LPARAM)szString);
          if ( nSelect == i )
            SendMessageA(hWnd: this, Msg: 0x186u, wParam: i, lParam: 0);
        }
      }
    }
    SendMessageA(hWnd: this, Msg: 0x197u, wParam: nTop, lParam: 0);
    return SendMessageA(hWnd: this, Msg: 0xBu, wParam: 1u, lParam: 0);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900D40A
 * CodeView symbol: JoinDrawItem
 * Demangled: JoinDrawItem
 * IDA name: JoinDrawItem
 * CodeView module: 19
 * CV address: segment 1 : 0xC40A
 * Code length: 0x4C7
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall JoinDrawItem(int a1, int a2, HWND window, int wparam, struct tagDRAWITEMSTRUCT *lparam)
{
  COLORREF v6; // eax
  UINT v7; // eax
  DWORD color; // [esp+4h] [ebp-360h]
  char v11; // [esp+18h] [ebp-34Ch] BYREF
  char *Buffer; // [esp+1Ch] [ebp-348h]
  COLORREF oldBkColor; // [esp+20h] [ebp-344h]
  char szString[256]; // [esp+24h] [ebp-340h] BYREF
  char *v15; // [esp+124h] [ebp-240h]
  int v16; // [esp+128h] [ebp-23Ch] BYREF
  COLORREF oldTextColor; // [esp+12Ch] [ebp-238h]
  struct tagDRAWITEMSTRUCT *lpdis; // [esp+130h] [ebp-234h]
  int v19; // [esp+134h] [ebp-230h] BYREF
  int bSelected; // [esp+138h] [ebp-22Ch]
  char *v21; // [esp+13Ch] [ebp-228h]
  int dwDrawFlags; // [esp+140h] [ebp-224h]
  int dwItemFlags; // [esp+144h] [ebp-220h]
  CHAR buffer[255]; // [esp+148h] [ebp-21Ch] BYREF
  char v25; // [esp+247h] [ebp-11Dh]
  char nGameId; // [esp+248h] [ebp-11Ch] BYREF
  LRESULT sel; // [esp+24Ch] [ebp-118h]
  char name[256]; // [esp+250h] [ebp-114h] BYREF
  char *szLag; // [esp+350h] [ebp-14h]
  char *description; // [esp+354h] [ebp-10h]
  int nLag; // [esp+358h] [ebp-Ch] BYREF
  int nTime; // [esp+35Ch] [ebp-8h] BYREF
  char *p; // [esp+360h] [ebp-4h]

  if ( wparam == 1031 )
  {
    name[0] = byte_19042B3C;
    memset(&name[1], 0, 255);
    szLag = (char *)&unk_19042B40;
    description = (char *)&unk_19042B44;
    nLag = 0;
    nTime = 0;
    sel = SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1027, Msg: 0x188u, wParam: 0, lParam: 0);
    if ( sel != -1 )
      SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1027, Msg: 0x189u, wParam: sel, lParam: (LPARAM)name);
    p = strchr(Str: name, Val: 9);
    if ( p != nullptr )
    {
      *p++ = 0;
      szLag = p;
      sscanf(Buffer: p, Format: "%x %x %x", &nGameId, &nLag, &nTime);
      p = strchr(Str: szLag, Val: 9);
      if ( p != nullptr )
      {
        description = p;
        *p = 0;
        ++description;
      }
    }
    buffer[0] = byte_19042B48;
    memset(&buffer[1], 0, 254);
    v25 = 0;
    GetDlgItemTextA(hDlg: window, nIDDlgItem: 1031, lpString: buffer, cchMax: 256);
    v25 = 0;
    if ( strcmp(Str1: buffer, Str2: description) != 0 )
      SetDlgItemTextA(hDlg: window, nIDDlgItem: 1031, lpString: description);
    if ( *(_DWORD *)(a1 + 12) != 0 && *(_DWORD *)(*(_DWORD *)(a1 + 12) + 24) != 0 )
    {
      dwItemFlags = 0;
      dwDrawFlags = 2;
      return (*(int (__stdcall **)(int, int, char *, char *, _DWORD, int, int, struct tagDRAWITEMSTRUCT *))(*(_DWORD *)(a1 + 12) + 24))(
               a1: 1112425812,
               a2: 1,
               a3: name,
               a4: description,
               a5: 0,
               a6: 2,
               a7: nTime,
               a8: lparam);
    }
  }
  else if ( wparam == 1027 )
  {
    Buffer = (char *)&unk_19042B4C;
    v15 = (char *)&unk_19042B50;
    v16 = 0;
    v19 = 0;
    lpdis = lparam;
    bSelected = lparam->itemState & 1;
    if ( SendMessageA(hWnd: lparam->hwndItem, Msg: 0x189u, wParam: lparam->itemID, lParam: (LPARAM)szString) != -1
      && szString[0] != 0
      && lpdis->hDC != nullptr )
    {
      v21 = strchr(Str: szString, Val: 9);
      if ( v21 != nullptr )
      {
        *v21++ = 0;
        Buffer = v21;
        sscanf(Buffer: v21, Format: "%x %x %x", &v11, &v16, &v19);
        v21 = strchr(Str: Buffer, Val: 9);
        if ( v21 != nullptr )
        {
          v15 = v21;
          *v21 = 0;
          ++v15;
        }
      }
      if ( *(_DWORD *)(a1 + 12) != 0 && *(_DWORD *)(*(_DWORD *)(a1 + 12) + 24) != 0 )
      {
        (*(void (__stdcall **)(int, int, char *, char *, _DWORD, int, int, struct tagDRAWITEMSTRUCT *))(*(_DWORD *)(a1 + 12) + 24))(
          a1: 1112425812,
          a2: 1,
          a3: szString,
          a4: v15,
          a5: 0,
          a6: 1,
          a7: v19,
          a8: lparam);
      }
      else
      {
        oldTextColor = SetTextColor(hdc: lpdis->hDC, color: 0xFFFFFFu);
        if ( bSelected != 0 )
        {
          color = GetSysColor(nIndex: 13);
          v6 = SetBkColor(hdc: lpdis->hDC, color);
        }
        else
        {
          v6 = SetBkColor(hdc: lpdis->hDC, color: 0);
        }
        oldBkColor = v6;
        v7 = strlen(Str: szString);
        ExtTextOutA(
          hdc: lpdis->hDC,
          x: lpdis->rcItem.left,
          y: lpdis->rcItem.top,
          options: 6u,
          lprect: &lpdis->rcItem,
          lpString: szString,
          c: v7,
          lpDx: nullptr);
        SetTextColor(hdc: lpdis->hDC, color: oldTextColor);
        SetBkColor(hdc: lpdis->hDC, color: oldBkColor);
      }
      DrawNetLag(a1: v16, a2: lpdis);
      return 1;
    }
  }
  return SDlgDefDialogProc(a1: window, a2, a3: wparam, a4: lparam);
}


// ==============================================================================
/*
 * EA: 0x1900D8D1
 * CodeView symbol: RememberFilterSetting
 * Demangled: RememberFilterSetting
 * IDA name: RememberFilterSetting
 * CodeView module: 19
 * CV address: segment 1 : 0xC8D1
 * Code length: 0x67
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __thiscall RememberFilterSetting(HWND this)
{
  LRESULT result; // eax
  _BYTE szString[128]; // [esp+8h] [ebp-80h] BYREF

  result = SendMessageA(hWnd: this, Msg: 0x147u, wParam: 0, lParam: 0);
  if ( result != -1 )
  {
    SendMessageA(hWnd: this, Msg: 0xDu, wParam: 0x80u, lParam: (LPARAM)szString);
    return SRegSaveString(a1: "Preferences", a2: "Join Filter", a3: 2, a4: szString);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900D938
 * CodeView symbol: GetFilterSetting
 * Demangled: GetFilterSetting
 * IDA name: GetFilterSetting
 * CodeView module: 19
 * CV address: segment 1 : 0xC938
 * Code length: 0x4F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __thiscall GetFilterSetting(HWND this)
{
  LRESULT result; // eax
  _BYTE szString[128]; // [esp+4h] [ebp-80h] BYREF

  result = SRegLoadString(a1: "Preferences", a2: "Join Filter", a3: 2, a4: szString, a5: 128);
  if ( szString[0] != 0 )
    return SendMessageA(hWnd: this, Msg: 0x14Du, wParam: 0xFFFFFFFF, lParam: (LPARAM)szString);
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900D987
 * CodeView symbol: JoinAddCategory
 * Demangled: JoinAddCategory
 * IDA name: JoinAddCategory
 * CodeView module: 19
 * CV address: segment 1 : 0xC987
 * Code length: 0x7B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall JoinAddCategory(LPARAM pszCategoryName, int categorybits, int categorymask)
{
  LRESULT nPos; // [esp+0h] [ebp-8h]
  _DWORD *data; // [esp+4h] [ebp-4h]

  if ( sghFilterCombo != nullptr )
  {
    nPos = SendMessageA(hWnd: sghFilterCombo, Msg: 0x143u, wParam: 0, lParam: pszCategoryName);
    if ( nPos != -1 )
    {
      data = (_DWORD *)SMemAlloc(a1: 8, a2: aJoingameCpp_3, a3: 503, a4: 0);
      *data = categorybits;
      data[1] = categorymask;
      SendMessageA(hWnd: sghFilterCombo, Msg: 0x151u, wParam: nPos, lParam: (LPARAM)data);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1900DA02
 * CodeView symbol: DoJoin
 * Demangled: DoJoin
 * IDA name: DoJoin
 * CodeView module: 19
 * CV address: segment 1 : 0xCA02
 * Code length: 0x374
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Joingame.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Joingame.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall DoJoin(HWND a1, int a2)
{
  CHAR progvers[32]; // [esp+Ch] [ebp-2F0h] BYREF
  unsigned int nMsg; // [esp+2Ch] [ebp-2D0h]
  char szPlayerName[128]; // [esp+30h] [ebp-2CCh] BYREF
  HWND hWndName; // [esp+B0h] [ebp-24Ch]
  _BYTE GameInfo[168]; // [esp+B4h] [ebp-248h] BYREF
  _BYTE v10[148]; // [esp+15Ch] [ebp-1A0h] BYREF
  char szPlayerDesc[128]; // [esp+1F0h] [ebp-10Ch] BYREF
  char szText[128]; // [esp+270h] [ebp-8Ch] BYREF
  HWND focuswindow; // [esp+2F0h] [ebp-Ch]
  int nError; // [esp+2F4h] [ebp-8h]
  HWND hWndPassword; // [esp+2F8h] [ebp-4h]

  if ( *(_DWORD *)(*(_DWORD *)(a2 + 12) + 36) != 0 )
    (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(a2 + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
  hWndName = GetDlgItem(hDlg: a1, nIDDlgItem: 1010);
  hWndPassword = GetDlgItem(hDlg: a1, nIDDlgItem: 1011);
  if ( SendMessageA(hWnd: hWndName, Msg: 0xEu, wParam: 0, lParam: 0) == 0 )
  {
    LocaleLoadString(a1: 0x3E9u, a2: szText, a3: 128);
    UiMessageBox(
      a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*(_DWORD *)(a2 + 12) + 32),
      a2: a1,
      lpText: szText,
      a4: byte_19042B54,
      a5: 0x30u);
    SetFocus(hWnd: hWndName);
    return 0;
  }
  SendMessageA(hWnd: hWndName, Msg: 0xDu, wParam: 0x80u, lParam: (LPARAM)sgszGameName);
  SendMessageA(hWnd: hWndPassword, Msg: 0xDu, wParam: 0x80u, lParam: (LPARAM)sgszGamePassword);
  SrvGetLocalPlayerName(a1: 0, a2: szPlayerName, a3: 0x80u);
  SrvGetLocalPlayerDesc(a1: szPlayerDesc, a2: 0x80u);
  if ( SpiGetGameInfo(
         gameid: 0,
         gamename: sgszGameName,
         gamepassword: sgszGamePassword,
         gameinfo: (struct _SNETSPI_GAMELIST *)GameInfo) != 0 )
  {
    if ( (*(int (__stdcall **)(int, char *, char *, int, _BYTE *, char *, int))(*(_DWORD *)(a2 + 12) + 16))(
           a1: 2,
           a2: szPlayerName,
           a3: szPlayerDesc,
           a4: gnUserFlags,
           a5: v10,
           a6: szText,
           a7: 128) == 0 )
    {
      UiMessageBox(
        a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*(_DWORD *)(a2 + 12) + 32),
        a2: a1,
        lpText: szText,
        a4: nullptr,
        a5: 0x30u);
      SetFocus(hWnd: hWndName);
      return 0;
    }
    if ( SNetJoinGame(
           a1: 0,
           a2: sgszGameName,
           a3: sgszGamePassword,
           a4: szPlayerName,
           a5: szPlayerDesc,
           a6: *(_DWORD *)(a2 + 20)) != 0 )
    {
      _wsprintfA(a1: progvers, a2: "%08x%08x", global_programid, global_versionid);
      SRegSaveString(a1: aRecentGames_0, a2: progvers, a3: 2, a4: sgszGameName);
      SrvNotifyJoin(a1: sgszGameName, a2: sgszGamePassword);
      SDlgEndDialog(a1, a2: 1);
      return 1;
    }
  }
  focuswindow = sghFilterCombo;
  nError = SErrGetLastError();
  if ( nError > -2062548887 )
  {
    switch ( nError )
    {
      case -2062548862:
        nMsg = 1086;
        break;
      case 86:
        nMsg = 1077;
        focuswindow = hWndPassword;
        break;
      case 1232:
        nMsg = 1043;
        break;
      default:
LABEL_26:
        nMsg = 1008;
        break;
    }
  }
  else
  {
    switch ( nError )
    {
      case -2062548887:
        nMsg = 1080;
        break;
      case -2062548890:
        nMsg = 1078;
        break;
      case -2062548889:
        nMsg = 1044;
        break;
      case -2062548888:
        nMsg = 1079;
        focuswindow = hWndName;
        break;
      default:
        goto LABEL_26;
    }
  }
  LocaleLoadString(a1: nMsg, a2: szText, a3: 128);
  UiMessageBox(
    a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*(_DWORD *)(a2 + 12) + 32),
    a2: a1,
    lpText: szText,
    a4: byte_19042B58,
    a5: 0x30u);
  SetFocus(hWnd: focuswindow);
  return 0;
}

