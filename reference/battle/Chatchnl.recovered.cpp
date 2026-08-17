/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 13
 */


// ==============================================================================
/*
 * EA: 0x19004680
 * CodeView symbol: IsNastyName
 * Demangled: IsNastyName
 * IDA name: ?IsNastyName@@YIHPBD@Z
 * CodeView module: 27
 * CV address: segment 1 : 0x3680
 * Code length: 0x84
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall IsNastyName(const char *a1)
{
  const char *name; // [esp+0h] [ebp-48h]
  signed int index; // [esp+4h] [ebp-44h]
  char tempname[32]; // [esp+8h] [ebp-40h] BYREF
  char nasty[32]; // [esp+28h] [ebp-20h] BYREF

  name = a1;
  strcpy(Destination: tempname, Source: a1);
  _strlwr(String: tempname);
  for ( index = 1020; index <= 1027; ++index )
  {
    LocaleLoadString(a1: index, a2: nasty, a3: 32);
    convert_nasty(a1: nasty, a2: name);
    _strlwr(String: nasty);
    if ( strstr(Str: tempname, SubStr: nasty) != nullptr )
      return 1;
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x19004704
 * CodeView symbol: convert_nasty
 * Demangled: convert_nasty
 * IDA name: convert_nasty
 * CodeView module: 27
 * CV address: segment 1 : 0x3704
 * Code length: 0x2C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_BYTE *__thiscall convert_nasty(_BYTE *this)
{
  _BYTE *result; // eax

  while ( 1 )
  {
    result = this;
    if ( *this == 0 )
      break;
    --*this++;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19004730
 * CodeView symbol: ChatChannelFull
 * Demangled: ChatChannelFull
 * IDA name: ?ChatChannelFull@@YIXPBD@Z
 * CodeView module: 27
 * CV address: segment 1 : 0x3730
 * Code length: 0x90
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatChannelFull(const char *a1)
{
  HWND DlgItem; // eax
  char szText[256]; // [esp+4h] [ebp-200h] BYREF
  char szFmt[256]; // [esp+104h] [ebp-100h] BYREF

  if ( sghWndChannel != nullptr && dword_19041D08 != 0 )
  {
    LocaleLoadString(a1: 0x406u, a2: szFmt, a3: 256);
    sprintf(Buffer: szText, Format: szFmt, a1);
    UiMessageBox(
      a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(dword_19041D08 + 32),
      a2: sghWndChannel,
      lpText: szText,
      a4: nullptr,
      a5: 0);
    DlgItem = GetDlgItem(hDlg: sghWndChannel, nIDDlgItem: 1);
    EnableWindow(hWnd: DlgItem, bEnable: true);
  }
}


// ==============================================================================
/*
 * EA: 0x190047C0
 * CodeView symbol: ChatChannelDoesNotExist
 * Demangled: ChatChannelDoesNotExist
 * IDA name: ?ChatChannelDoesNotExist@@YIXPBD@Z
 * CodeView module: 27
 * CV address: segment 1 : 0x37C0
 * Code length: 0x4E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatChannelDoesNotExist(const char *a1)
{
  char szText[256]; // [esp+4h] [ebp-100h] BYREF

  if ( sghWndChannel != nullptr )
  {
    strcpy(Destination: szText, Source: a1);
    SendMessageA(hWnd: sghWndChannel, Msg: 0x464u, wParam: 0, lParam: (LPARAM)szText);
  }
}


// ==============================================================================
/*
 * EA: 0x1900480E
 * CodeView symbol: ChatChannelRestricted
 * Demangled: ChatChannelRestricted
 * IDA name: ?ChatChannelRestricted@@YIXPBD@Z
 * CodeView module: 27
 * CV address: segment 1 : 0x380E
 * Code length: 0xDD
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatChannelRestricted(const char *a1)
{
  _BYTE szError[256]; // [esp+8h] [ebp-204h] BYREF
  char szUserDesc[128]; // [esp+108h] [ebp-104h] BYREF
  char szUserName[128]; // [esp+188h] [ebp-84h] BYREF
  unsigned int nFlags; // [esp+208h] [ebp-4h]

  szError[0] = byte_19041D1C;
  memset(&szError[1], 0, 255);
  if ( sghWndChannel != nullptr && *(_DWORD *)(dword_19041D08 + 16) != 0 )
  {
    SrvGetLocalPlayerName(a1: 1, a2: szUserName, a3: 0x80u);
    SrvGetLocalPlayerDesc(a1: szUserDesc, a2: 0x80u);
    nFlags = ChatGetUserFlags();
    if ( (*(int (__stdcall **)(int, char *, char *, unsigned int, const char *, _BYTE *, int))(dword_19041D08 + 16))(
           a1: 1,
           a2: szUserName,
           a3: szUserDesc,
           a4: nFlags,
           a5: a1,
           a6: szError,
           a7: 256) != 0 )
      SrvJoinChannel(a1, a2: 1);
    else
      SendMessageA(hWnd: sghWndChannel, Msg: 0x466u, wParam: 0, lParam: (LPARAM)szError);
  }
}


// ==============================================================================
/*
 * EA: 0x190048EB
 * CodeView symbol: ChatChannelJoined
 * Demangled: ChatChannelJoined
 * IDA name: ?ChatChannelJoined@@YIXPBD@Z
 * CodeView module: 27
 * CV address: segment 1 : 0x38EB
 * Code length: 0x2B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void ChatChannelJoined()
{
  if ( sghWndChannel != nullptr )
    SendMessageA(hWnd: sghWndChannel, Msg: 0x465u, wParam: 0, lParam: 0);
}


// ==============================================================================
/*
 * EA: 0x19004916
 * CodeView symbol: ChannelDialogProc
 * Demangled: ChannelDialogProc
 * IDA name: ?ChannelDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 27
 * CV address: segment 1 : 0x3916
 * Code length: 0x640
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ChannelDialogProc(HWND window, UINT message, HDC wparam, HWND lparam)
{
  HWND v5; // eax
  HWND v6; // eax
  HWND DlgItem; // eax
  HWND FrameWindow; // eax
  HWND v9; // esi
  HWND v10; // eax
  HWND v11; // eax
  const CHAR *v12; // [esp-4h] [ebp-348h]
  HWND parent; // [esp+Ch] [ebp-338h]
  char Buffer[256]; // [esp+14h] [ebp-330h] BYREF
  char szFmt[256]; // [esp+114h] [ebp-230h] BYREF
  WPARAM wParam; // [esp+214h] [ebp-130h]
  LPARAM lParam[8]; // [esp+218h] [ebp-12Ch] BYREF
  HWND hWnd; // [esp+238h] [ebp-10Ch]
  LRESULT nIndex; // [esp+23Ch] [ebp-108h]
  HWND hWndChannelList; // [esp+240h] [ebp-104h]
  char szText[256]; // [esp+244h] [ebp-100h] BYREF

  if ( message > 0x112 )
  {
    if ( message > 0x465 )
    {
      if ( message == 1126 )
      {
        UiMessageBox(
          a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(dword_19041D08 + 32),
          a2: window,
          lpText: (const char *)lparam,
          a4: nullptr,
          a5: 0x30u);
        DlgItem = GetDlgItem(hDlg: window, nIDDlgItem: 1);
        EnableWindow(hWnd: DlgItem, bEnable: true);
        return 1;
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( message == 1125 )
    {
      SDlgEndDialog(a1: window, a2: 1);
      return 1;
    }
    if ( message == 312 )
    {
      if ( GetWindowLongA(hWnd: lparam, nIndex: -12) == 1030 )
      {
        SetTextColor(hdc: wparam, color: 0xFFFFu);
        return (int)GetStockObject(i: 5);
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( message != 1124 )
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    LocaleLoadString(a1: 0x405u, a2: szFmt, a3: 256);
    sprintf(Buffer, Format: szFmt, lparam);
    if ( UiMessageBox(
           a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(dword_19041D08 + 32),
           a2: window,
           lpText: Buffer,
           a4: nullptr,
           a5: 1u) == 1 )
    {
      SrvJoinChannel(a1: (const char *)lparam, a2: 1);
    }
    else
    {
      v6 = GetDlgItem(hDlg: window, nIDDlgItem: 1);
      EnableWindow(hWnd: v6, bEnable: true);
    }
    return 1;
  }
  else
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
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( message <= 0x110 )
    {
      if ( message == 272 )
      {
        dword_19041D08 = (int)lparam;
        sghWndChannel = window;
        sub_19004FB0(a1: window, a2: *((_DWORD *)lparam + 3));
        v9 = GetDlgItem(hDlg: window, nIDDlgItem: 1052);
        v10 = GetDlgItem(hDlg: window, nIDDlgItem: 1039);
        ScrollbarLink(a1: v10, a2: v9);
        v12 = sgpszLastChannel;
        v11 = GetDlgItem(hDlg: window, nIDDlgItem: 1010);
        SetWindowTextA(hWnd: v11, lpString: v12);
        SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1010, Msg: 0xC5u, wParam: 0x1Fu, lParam: 0);
        SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1039, Msg: 0x1A0u, wParam: 0, lParam: 19);
        AddChannels(a1: window);
        return 1;
      }
      if ( message == 2 )
      {
        sghWndChannel = nullptr;
        sub_19004F56(a1: window);
      }
      else if ( message > 0x103 && message <= 0x105 )
      {
        FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
        SendMessageA(hWnd: FrameWindow, Msg: message, wParam: (WPARAM)wparam, lParam: (LPARAM)lparam);
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( (unsigned __int16)wparam > 0x3F2u )
    {
      if ( (unsigned __int16)wparam == 1039 )
      {
        if ( HIWORD(wparam) == 2 )
        {
          hWndChannelList = lparam;
          nIndex = SendMessageA(hWnd: lparam, Msg: 0x188u, wParam: 0, lParam: 0);
          if ( nIndex != -1 )
          {
            SendMessageA(hWnd: window, Msg: 0x111u, wParam: 0x1040Fu, lParam: (LPARAM)hWndChannelList);
            v5 = GetDlgItem(hDlg: window, nIDDlgItem: 1);
            SendMessageA(hWnd: window, Msg: 0x111u, wParam: 1u, lParam: (LPARAM)v5);
          }
        }
        else if ( HIWORD(wparam) == 1 )
        {
          hWnd = lparam;
          wParam = SendMessageA(hWnd: lparam, Msg: 0x188u, wParam: 0, lParam: 0);
          if ( wParam != -1 )
          {
            SendMessageA(hWnd, Msg: 0x189u, wParam, (LPARAM)lParam);
            SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1010, Msg: 0xCu, wParam: 0, (LPARAM)lParam);
          }
          ListUpdateScrollbar(a1: hWnd);
        }
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    switch ( (unsigned __int16)wparam )
    {
      case 0x3F2u:
        if ( HIWORD(wparam) == 768 && GetFocus() == lparam )
          SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1039, Msg: 0x186u, wParam: 0xFFFFFFFF, lParam: 0);
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
      case 1u:
        if ( *(_DWORD *)(dword_19041D08 + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(dword_19041D08 + 36))(a1: 1112425812, a2: 1, a3: 0);
        SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1010, Msg: 0xDu, wParam: 0x20u, lParam: (LPARAM)szText);
        if ( SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1039, Msg: 0x188u, wParam: 0, lParam: 0) == -1
          && (InvalidChars(a1: szText, a2: byte_19041D20) != 0 || ProcessName(a1: szText) == 0) )
        {
          LocaleLoadString(a1: 0x3FBu, a2: szText, a3: 256);
          UiMessageBox(
            a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(dword_19041D08 + 32),
            a2: window,
            lpText: szText,
            a4: nullptr,
            a5: 0x10u);
          return 1;
        }
        else
        {
          if ( _strnicmp(String1: szText, String2: sgpszLastChannel, MaxCount: 0x100u) == 0 )
          {
            SDlgEndDialog(a1: window, a2: 0);
          }
          else
          {
            SrvJoinChannel(a1: szText, a2: 0);
            strcpy(Destination: &sgszChannel, Source: szText);
            EnableWindow(hWnd: lparam, bEnable: false);
          }
          return 1;
        }
      case 2u:
        if ( *(_DWORD *)(dword_19041D08 + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(dword_19041D08 + 36))(a1: 1112425812, a2: 1, a3: 0);
        sgszChannel = 0;
        SDlgEndDialog(a1: window, a2: 0);
        return 1;
      default:
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19004F56
 * CodeView symbol: DestroyArtwork
 * Demangled: DestroyArtwork
 * IDA name: sub_19004F56
 * CodeView module: 27
 * CV address: segment 1 : 0x3F56
 * Code length: 0x5A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_19004F56()
{
  int result; // eax

  if ( dword_19041D14 != nullptr )
  {
    result = SMemFree(a1: dword_19041D14, a2: aChatchnlCpp, a3: 31, a4: 0);
    dword_19041D14 = nullptr;
  }
  if ( dword_19041D18 != nullptr )
  {
    result = SMemFree(a1: dword_19041D18, a2: aChatchnlCpp_0, a3: 36, a4: 0);
    dword_19041D18 = nullptr;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19004FB0
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: sub_19004FB0
 * CodeView module: 27
 * CV address: segment 1 : 0x3FB0
 * Code length: 0xBF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_19004FB0(
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
    artid: 0x80000006,
    controltype: byte_19041D24,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19041D14,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 5u,
    controltype: aButton_14,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &dword_19041D18,
    a11: &sizeBtns);
  SDlgSetControlBitmaps(a1, a2: btn_ids, a3: 0, a4: dword_19041D18, a5: &sizeBtns, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: btn_desc, a3: 0, a4: dword_19041D14, a5: &bgSize, a6: 2, a7: -1);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900506F
 * CodeView symbol: ProcessName
 * Demangled: ProcessName
 * IDA name: ProcessName
 * CodeView module: 27
 * CV address: segment 1 : 0x406F
 * Code length: 0xB5
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall ProcessName(char *this)
{
  signed int nStart; // [esp+4h] [ebp-28h]
  signed int nEnd; // [esp+8h] [ebp-24h]
  char szTemp[32]; // [esp+Ch] [ebp-20h] BYREF

  strcpy(Destination: szTemp, Source: this);
  for ( nStart = 0; szTemp[nStart] == 32; ++nStart )
  {
    if ( szTemp[nStart] == 0 )
      return 0;
  }
  for ( nEnd = strlen(Str: szTemp) - 1; szTemp[nEnd] == 32; --nEnd )
  {
    if ( nEnd <= nStart )
      return 0;
  }
  szTemp[++nEnd] = 0;
  strcpy(Destination: this, Source: &szTemp[nStart]);
  return nEnd - nStart;
}


// ==============================================================================
/*
 * EA: 0x19005124
 * CodeView symbol: AddChannels
 * Demangled: AddChannels
 * IDA name: AddChannels
 * CodeView module: 27
 * CV address: segment 1 : 0x4124
 * Code length: 0x5E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall AddChannels(HWND this)
{
  HWND hWndList; // [esp+4h] [ebp-8h]
  LPARAM pCurr; // [esp+8h] [ebp-4h]

  hWndList = GetDlgItem(hDlg: this, nIDDlgItem: 1039);
  if ( hWndList != nullptr )
  {
    for ( pCurr = dword_19041D10; pCurr != 0; pCurr = *(_DWORD *)(pCurr + 32) )
      SendMessageA(hWnd: hWndList, Msg: 0x180u, wParam: 0, lParam: pCurr);
    ListUpdateScrollbar(a1: hWndList);
  }
}


// ==============================================================================
/*
 * EA: 0x19005182
 * CodeView symbol: DoChatSelectChannel
 * Demangled: DoChatSelectChannel
 * IDA name: ?DoChatSelectChannel@@YIHPAU_SNETUIDATA@@PADPAU_CHANNEL_LIST@@@Z
 * CodeView module: 27
 * CV address: segment 1 : 0x4182
 * Code length: 0x57
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall DoChatSelectChannel(HWND *a1, char *a2, struct _CHANNEL_LIST *pChannelListHead)
{
  int dwReturn; // [esp+8h] [ebp-8h]

  SetActiveWindow(hWnd: a1[2]);
  ShowWindow(hWnd: ghWndChat, nCmdShow: 0);
  dwReturn = ChatSelectChannel((int)pChannelListHead);
  ShowWindow(hWnd: ghWndChat, nCmdShow: 5);
  return dwReturn;
}


// ==============================================================================
/*
 * EA: 0x190051D9
 * CodeView symbol: ChatSelectChannel
 * Demangled: ChatSelectChannel
 * IDA name: ChatSelectChannel
 * CodeView module: 27
 * CV address: segment 1 : 0x41D9
 * Code length: 0x77
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatchnl.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatchnl.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall ChatSelectChannel(int a1, const CHAR *a2, int pChannelListHead)
{
  HMODULE ModuleHandleA; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-20h]
  int FrameWindow; // [esp+0h] [ebp-10h]

  sgszChannel = 0;
  sgpszLastChannel = a2;
  dword_19041D10 = pChannelListHead;
  if ( a1 != 0 )
    FrameWindow = *(_DWORD *)(a1 + 8);
  else
    FrameWindow = SDrawGetFrameWindow(a1: 0);
  Dialog = LocaleGetDialog(a1: 0x68u);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  return SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: FrameWindow, a4: ChannelDialogProc, a5: a1) == 1;
}

