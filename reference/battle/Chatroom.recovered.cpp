/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 43
 */


// ==============================================================================
/*
 * EA: 0x19005600
 * CodeView symbol: ListFindName
 * Demangled: ListFindName
 * IDA name: ?ListFindName@@YIHPAUHWND__@@PBD@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x4600
 * Code length: 0x11D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ListFindName(HWND a1, const char *a2)
{
  unsigned int nLen; // [esp+8h] [ebp-114h]
  char szText[256]; // [esp+Ch] [ebp-110h] BYREF
  LRESULT nItems; // [esp+10Ch] [ebp-10h]
  signed int i; // [esp+110h] [ebp-Ch]
  int bFound; // [esp+114h] [ebp-8h]
  char *p; // [esp+118h] [ebp-4h]

  nItems = SendMessageA(hWnd: a1, Msg: 0x18Bu, wParam: 0, lParam: 0);
  if ( nItems == -1 )
    return -1;
  bFound = 0;
  for ( i = 0; i < nItems; ++i )
  {
    nLen = SendMessageA(hWnd: a1, Msg: 0x18Au, wParam: i, lParam: 0);
    if ( nLen < 0x100 && nLen != 0 )
    {
      SendMessageA(hWnd: a1, Msg: 0x189u, wParam: i, lParam: (LPARAM)szText);
      p = strchr(Str: szText, Val: 9);
      if ( p != nullptr )
      {
        *p = 0;
        if ( strcmp(Str1: szText, Str2: a2) == 0 )
        {
          bFound = 1;
          break;
        }
      }
    }
  }
  if ( bFound == 0 )
    return -1;
  return i;
}


// ==============================================================================
/*
 * EA: 0x1900571D
 * CodeView symbol: ChannelWindowUpdate
 * Demangled: ChannelWindowUpdate
 * IDA name: ?ChannelWindowUpdate@@YIXXZ
 * CodeView module: 25
 * CV address: segment 1 : 0x471D
 * Code length: 0x82
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ChannelWindowUpdate()
{
  HWND DlgItem; // eax
  HWND v1; // eax
  LRESULT nUsers; // [esp+0h] [ebp-144h]
  char szText[256]; // [esp+4h] [ebp-140h] BYREF
  char szFmt[64]; // [esp+104h] [ebp-40h] BYREF

  DlgItem = GetDlgItem(hDlg: ghWndChat, nIDDlgItem: 1003);
  nUsers = SendMessageA(hWnd: DlgItem, Msg: 0x18Bu, wParam: 0, lParam: 0);
  LocaleLoadString(a1: 0x40Au, a2: szFmt, a3: 64);
  sprintf(Buffer: szText, Format: szFmt, gszCurrentChannel, nUsers);
  v1 = GetDlgItem(hDlg: ghWndChat, nIDDlgItem: 1046);
  SetWindowTextA(hWnd: v1, lpString: szText);
}


// ==============================================================================
/*
 * EA: 0x1900579F
 * CodeView symbol: ChatGetUserFlags
 * Demangled: ChatGetUserFlags
 * IDA name: ?ChatGetUserFlags@@YIIXZ
 * CodeView module: 25
 * CV address: segment 1 : 0x479F
 * Code length: 0xA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl ChatGetUserFlags()
{
  return gnUserFlags;
}


// ==============================================================================
/*
 * EA: 0x190057A9
 * CodeView symbol: NormalizeNetLag
 * Demangled: NormalizeNetLag
 * IDA name: ?NormalizeNetLag@@YIHK@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x47A9
 * Code length: 0x41
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
unsigned int __fastcall NormalizeNetLag(unsigned int a1)
{
  if ( a1 < 0xA )
    return 0;
  if ( a1 / 0x64 >= 6 )
    return 6;
  if ( a1 / 0x64 != 0 )
    return a1 / 0x64;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x190057EA
 * CodeView symbol: DrawNetLag
 * Demangled: DrawNetLag
 * IDA name: ?DrawNetLag@@YIXHPAUtagDRAWITEMSTRUCT@@@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x47EA
 * Code length: 0xEF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall DrawNetLag(int a1, struct tagDRAWITEMSTRUCT *a2)
{
  unsigned __int8 *bmp; // [esp+Ch] [ebp-1Ch]
  _DWORD srcRect[4]; // [esp+10h] [ebp-18h] BYREF
  struct tagSIZE size; // [esp+20h] [ebp-8h] BYREF

  if ( a1 != 0 )
  {
    if ( a1 < 5 )
    {
      if ( a1 >= 3 )
      {
        bmp = yellowlagbitmap;
        size = yellowSize;
      }
      else
      {
        bmp = greenlagbitmap;
        size = greenSize;
      }
    }
    else
    {
      bmp = redlagbitmap;
      size = redSize;
    }
    srcRect[0] = 0;
    srcRect[2] = 3 * a1 - 1;
    srcRect[1] = 0;
    srcRect[3] = size.cy - 1;
    SDlgBltToWindowI(
      a1: a2->hwndItem,
      a2: 0,
      a3: a2->rcItem.right - 21,
      a4: (a2->rcItem.bottom - a2->rcItem.top + 1 - size.cy) / 2 + a2->rcItem.top,
      a5: bmp,
      a6: srcRect,
      a7: &size,
      a8: -1,
      a9: 0,
      a10: 13369376);
  }
}


// ==============================================================================
/*
 * EA: 0x190058D9
 * CodeView symbol: ChatAddUser
 * Demangled: ChatAddUser
 * IDA name: ?ChatAddUser@@YIXPAU_SNADDUSERREC@@@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x48D9
 * Code length: 0x1A2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatAddUser(const char **a1)
{
  unsigned int v1; // eax
  const char *v2; // [esp-4h] [ebp-1A4h]
  HWND hWndList; // [esp+4h] [ebp-19Ch]
  char szText[280]; // [esp+8h] [ebp-198h] BYREF
  char szFmt[128]; // [esp+120h] [ebp-80h] BYREF

  if ( ghWndChat != nullptr )
  {
    hWndList = GetDlgItem(hDlg: ghWndChat, nIDDlgItem: 1003);
    if ( ListFindName(a1: hWndList, a2: *a1) == -1 )
    {
      if ( IsUserSquelched(a1: *a1) != 0 )
        a1[2] = (const char *)((unsigned int)a1[2] | 0x20);
      v2 = a1[1];
      v1 = NormalizeNetLag(a1: (unsigned int)a1[3]);
      sprintf(Buffer: szText, Format: "%s\t%d %d\t%s", *a1, a1[2], v1, v2);
      SendMessageA(hWnd: hWndList, Msg: (((unsigned int)a1[2] & 0xF) != 0) + 384, wParam: 0, lParam: (LPARAM)szText);
      if ( sgbModeratedChannel == 0 && a1[4] != nullptr && sgdwVerboseMode != 0 )
      {
        LocaleLoadString(a1: 0x3EFu, a2: szFmt, a3: 128);
        sprintf(Buffer: szText, Format: szFmt, 18, *a1);
        SendMessageA(hWnd: ghWndChat, Msg: 0x468u, wParam: 0x12u, lParam: (LPARAM)szText);
      }
      if ( strcmp(Str1: *a1, Str2: sgszUserName) == 0 )
      {
        gnUserFlags = (int)a1[2];
        strcpy(Destination: sgszUserDesc, Source: a1[1]);
      }
      ChannelWindowUpdate();
      ListUpdateScrollbar(a1: hWndList);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19005A7B
 * CodeView symbol: IsUserSquelched
 * Demangled: IsUserSquelched
 * IDA name: IsUserSquelched
 * CodeView module: 25
 * CV address: segment 1 : 0x4A7B
 * Code length: 0x46
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall IsUserSquelched(const char *this)
{
  struct _SQUELCH_LIST *pCurr; // [esp+4h] [ebp-4h]

  for ( pCurr = sgpSquelchListHead; pCurr != nullptr; pCurr = *((struct _SQUELCH_LIST **)pCurr + 32) )
  {
    if ( strcmp(Str1: (const char *)pCurr, Str2: this) == 0 )
      return 1;
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x19005AC1
 * CodeView symbol: ChatDeleteUser
 * Demangled: ChatDeleteUser
 * IDA name: ?ChatDeleteUser@@YIXPAU_SNDELETEUSERREC@@@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x4AC1
 * Code length: 0x14F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatDeleteUser(const char **a1)
{
  WPARAM nIndex; // [esp+4h] [ebp-1A4h]
  HWND hWndList; // [esp+8h] [ebp-1A0h]
  LRESULT nTopIndex; // [esp+Ch] [ebp-19Ch]
  char szText[280]; // [esp+10h] [ebp-198h] BYREF
  char szFmt[128]; // [esp+128h] [ebp-80h] BYREF

  if ( ghWndChat != nullptr )
  {
    hWndList = GetDlgItem(hDlg: ghWndChat, nIDDlgItem: 1003);
    nIndex = ListFindName(a1: hWndList, a2: *a1);
    if ( nIndex != -1 )
    {
      nTopIndex = SendMessageA(hWnd: hWndList, Msg: 0x18Eu, wParam: 0, lParam: 0);
      SendMessageA(hWnd: hWndList, Msg: 0xBu, wParam: 0, lParam: 0);
      SendMessageA(hWnd: hWndList, Msg: 0x182u, wParam: nIndex, lParam: 0);
      SendMessageA(hWnd: hWndList, Msg: 0x197u, wParam: nTopIndex, lParam: 0);
      SendMessageA(hWnd: hWndList, Msg: 0xBu, wParam: 1u, lParam: 0);
      if ( sgbModeratedChannel == 0 && a1[2] != nullptr && sgdwVerboseMode != 0 )
      {
        LocaleLoadString(a1: 0x3EEu, a2: szFmt, a3: 128);
        sprintf(Buffer: szText, Format: szFmt, 18, *a1);
        SendMessageA(hWnd: ghWndChat, Msg: 0x468u, wParam: 0x12u, lParam: (LPARAM)szText);
      }
      ChannelWindowUpdate();
      ListUpdateScrollbar(a1: hWndList);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19005C10
 * CodeView symbol: ChatSetUserName
 * Demangled: ChatSetUserName
 * IDA name: ?ChatSetUserName@@YIXXZ
 * CodeView module: 25
 * CV address: segment 1 : 0x4C10
 * Code length: 0x19
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ChatSetUserName()
{
  SrvGetLocalPlayerName(a1: 1, a2: sgszUserName, a3: 0x80u);
}


// ==============================================================================
/*
 * EA: 0x19005C29
 * CodeView symbol: ChatChangeUserFlags
 * Demangled: ChatChangeUserFlags
 * IDA name: ?ChatChangeUserFlags@@YIXPAU_SNCHANGEUSERFLAGSREC@@@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x4C29
 * Code length: 0x2C5
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatChangeUserFlags(struct _SNCHANGEUSERFLAGSREC *a1)
{
  int v1; // eax
  int dwPingTime; // [esp+4h] [ebp-268h] BYREF
  char *pszInfo; // [esp+8h] [ebp-264h]
  LRESULT nCurSel; // [esp+Ch] [ebp-260h]
  WPARAM nOldIndex; // [esp+10h] [ebp-25Ch]
  int dwOldFlags; // [esp+14h] [ebp-258h] BYREF
  char szNewUserText[280]; // [esp+18h] [ebp-254h] BYREF
  char szOldUserText[280]; // [esp+130h] [ebp-13Ch] BYREF
  char *pszDesc; // [esp+248h] [ebp-24h]
  HWND hWndList; // [esp+24Ch] [ebp-20h]
  LRESULT nTopIndex; // [esp+250h] [ebp-1Ch]
  unsigned int newlatency; // [esp+254h] [ebp-18h]
  char *pszName; // [esp+258h] [ebp-14h]
  int bWasOfficial; // [esp+25Ch] [ebp-10h]
  int bIsOfficial; // [esp+260h] [ebp-Ch]
  LRESULT nNewIndex; // [esp+264h] [ebp-8h]
  int dwNewFlags; // [esp+268h] [ebp-4h]

  if ( ghWndChat != nullptr )
  {
    hWndList = GetDlgItem(hDlg: ghWndChat, nIDDlgItem: 1003);
    nOldIndex = ListFindName(a1: hWndList, a2: *(const char **)a1);
    if ( nOldIndex != -1 )
    {
      SendMessageA(hWnd: hWndList, Msg: 0x189u, wParam: nOldIndex, lParam: (LPARAM)szOldUserText);
      pszName = szOldUserText;
      pszInfo = strchr(Str: szOldUserText, Val: 9);
      if ( pszInfo != nullptr )
      {
        *pszInfo++ = 0;
        pszDesc = strchr(Str: pszInfo, Val: 9);
        if ( pszDesc != nullptr )
        {
          *pszDesc++ = 0;
          if ( sscanf(Buffer: pszInfo, Format: "%d %d", &dwOldFlags, &dwPingTime) == 2 )
          {
            dwNewFlags = *((_DWORD *)a1 + 1);
            if ( IsUserSquelched(this: *(const char **)a1) != 0 )
            {
              v1 = dwNewFlags;
              LOBYTE(v1) = dwNewFlags | 0x20;
              dwNewFlags = v1;
            }
            newlatency = NormalizeNetLag(a1: *((_DWORD *)a1 + 2));
            if ( dwNewFlags != dwOldFlags || dwPingTime != newlatency )
            {
              sprintf(Buffer: szNewUserText, Format: "%s\t%d %d\t%s", pszName, dwNewFlags, newlatency, pszDesc);
              if ( strcmp(Str1: *(const char **)a1, Str2: sgszUserName) == 0 )
                gnUserFlags = dwNewFlags;
              bWasOfficial = (dwOldFlags & 0xF) != 0;
              bIsOfficial = (dwNewFlags & 0xF) != 0;
              nTopIndex = SendMessageA(hWnd: hWndList, Msg: 0x18Eu, wParam: 0, lParam: 0);
              nCurSel = SendMessageA(hWnd: hWndList, Msg: 0x188u, wParam: 0, lParam: 0);
              SendMessageA(hWnd: hWndList, Msg: 0xBu, wParam: 0, lParam: 0);
              SendMessageA(hWnd: hWndList, Msg: 0x182u, wParam: nOldIndex, lParam: 0);
              if ( bWasOfficial == bIsOfficial )
                nNewIndex = SendMessageA(hWnd: hWndList, Msg: 0x181u, wParam: nOldIndex, lParam: (LPARAM)szNewUserText);
              else
                nNewIndex = SendMessageA(
                              hWnd: hWndList,
                              Msg: (bIsOfficial != 0) + 384,
                              wParam: 0,
                              lParam: (LPARAM)szNewUserText);
              SendMessageA(hWnd: hWndList, Msg: 0x197u, wParam: nTopIndex, lParam: 0);
              if ( nCurSel == nOldIndex )
                SendMessageA(hWnd: hWndList, Msg: 0x186u, wParam: nNewIndex, lParam: 0);
              SendMessageA(hWnd: hWndList, Msg: 0xBu, wParam: 1u, lParam: 0);
              ListUpdateScrollbar(a1: hWndList);
            }
          }
        }
      }
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19005EEE
 * CodeView symbol: ChatJoinChannel
 * Demangled: ChatJoinChannel
 * IDA name: ?ChatJoinChannel@@YIXPAU_SNJOINCHANNELREC@@@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x4EEE
 * Code length: 0xFE
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatJoinChannel(const char **a1)
{
  HWND hWndList; // [esp+4h] [ebp-208h]
  char szText[388]; // [esp+8h] [ebp-204h] BYREF
  char szFmt[128]; // [esp+18Ch] [ebp-80h] BYREF

  if ( sgbFirstChannel != 0 )
  {
    ChatAddChannel(a1: *a1);
    sgbFirstChannel = 0;
  }
  strcpy(Destination: gszCurrentChannel, Source: *a1);
  sgbModeratedChannel = (unsigned int)a1[1] & 2;
  ChatChannelJoined();
  if ( ghWndChat != nullptr )
  {
    hWndList = GetDlgItem(hDlg: ghWndChat, nIDDlgItem: 1003);
    SendMessageA(hWnd: hWndList, Msg: 0x184u, wParam: 0, lParam: 0);
    LocaleLoadString(a1: 0x3EDu, a2: szFmt, a3: 128);
    sprintf(Buffer: szText, Format: szFmt, 18, *a1);
    SendMessageA(hWnd: ghWndChat, Msg: 0x468u, wParam: 0x12u, lParam: (LPARAM)szText);
    ListUpdateScrollbar(a1: hWndList);
    ChannelWindowUpdate();
  }
}


// ==============================================================================
/*
 * EA: 0x19005FEC
 * CodeView symbol: ChatAddChannel
 * Demangled: ChatAddChannel
 * IDA name: ?ChatAddChannel@@YIXPBD@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x4FEC
 * Code length: 0x64
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatAddChannel(const char *a1)
{
  struct _CHANNEL_LIST *pCurr; // [esp+4h] [ebp-28h]
  char ChannelList[36]; // [esp+8h] [ebp-24h] BYREF

  strcpy(Destination: ChannelList, Source: a1);
  for ( pCurr = sgpChannelListHead; pCurr != nullptr; pCurr = *((struct _CHANNEL_LIST **)pCurr + 8) )
  {
    if ( strcmp(Str1: (const char *)pCurr, Str2: a1) == 0 )
      return;
  }
  TListAddEnd(a1: &sgpChannelListHead, a2: (struct _CHANNEL_LIST *)ChannelList, a3: aChatroomCpp, a4: 1485);
}


// ==============================================================================
/*
 * EA: 0x19006050
 * CodeView symbol: ChatDeleteChannel
 * Demangled: ChatDeleteChannel
 * IDA name: ?ChatDeleteChannel@@YIXPBD@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x5050
 * Code length: 0x53
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatDeleteChannel(const char *a1)
{
  struct _CHANNEL_LIST *pCurr; // [esp+4h] [ebp-4h]

  for ( pCurr = sgpChannelListHead; pCurr != nullptr; pCurr = *((struct _CHANNEL_LIST **)pCurr + 8) )
  {
    if ( strcmp(Str1: (const char *)pCurr, Str2: a1) == 0 )
    {
      TListFree(a1: &sgpChannelListHead, a2: pCurr, a3: aChatroomCpp_0, a4: 1497);
      return;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x190060A3
 * CodeView symbol: ChatSquelchUser
 * Demangled: ChatSquelchUser
 * IDA name: ?ChatSquelchUser@@YIXPAU_SNSQUELCHUSERREC@@@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x50A3
 * Code length: 0x102
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatSquelchUser(const char **a1)
{
  const char *v1; // eax
  char SquelchList[132]; // [esp+4h] [ebp-214h] BYREF
  _DWORD ChangeUserFlagsRec[3]; // [esp+88h] [ebp-190h] BYREF
  struct _SQUELCH_LIST *pCurr; // [esp+94h] [ebp-184h]
  char szText[256]; // [esp+98h] [ebp-180h] BYREF
  char szFmt[128]; // [esp+198h] [ebp-80h] BYREF

  strcpy(Destination: SquelchList, Source: *a1);
  for ( pCurr = sgpSquelchListHead; pCurr != nullptr; pCurr = *((struct _SQUELCH_LIST **)pCurr + 32) )
  {
    if ( strcmp(Str1: (const char *)pCurr, Str2: *a1) == 0 )
      return;
  }
  TListAddEnd(a1: &sgpSquelchListHead, a2: (struct _SQUELCH_LIST *)SquelchList, a3: aChatroomCpp_1, a4: 1524);
  ChangeUserFlagsRec[0] = *a1;
  v1 = a1[1];
  LOBYTE(v1) = (unsigned __int8)v1 | 0x20;
  ChangeUserFlagsRec[1] = v1;
  ChatChangeUserFlags(a1: (struct _SNCHANGEUSERFLAGSREC *)ChangeUserFlagsRec);
  LocaleLoadString(a1: 0x40Cu, a2: szFmt, a3: 128);
  sprintf(Buffer: szText, Format: szFmt, 18, *a1);
  SendMessageA(hWnd: ghWndChat, Msg: 0x468u, wParam: 0x12u, lParam: (LPARAM)szText);
}


// ==============================================================================
/*
 * EA: 0x190061A5
 * CodeView symbol: ChatUnsquelchUser
 * Demangled: ChatUnsquelchUser
 * IDA name: ?ChatUnsquelchUser@@YIXPAU_SNSQUELCHUSERREC@@@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x51A5
 * Code length: 0xE5
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatUnsquelchUser(const char **a1)
{
  _DWORD ChangeUserFlagsRec[3]; // [esp+4h] [ebp-190h] BYREF
  struct _SQUELCH_LIST *pCurr; // [esp+10h] [ebp-184h]
  char szText[256]; // [esp+14h] [ebp-180h] BYREF
  char szFmt[128]; // [esp+114h] [ebp-80h] BYREF

  for ( pCurr = sgpSquelchListHead; pCurr != nullptr; pCurr = *((struct _SQUELCH_LIST **)pCurr + 32) )
  {
    if ( strcmp(Str1: (const char *)pCurr, Str2: *a1) == 0 )
    {
      TListFree(a1: &sgpSquelchListHead, a2: pCurr, a3: aChatroomCpp_2, a4: 1549);
      break;
    }
  }
  ChangeUserFlagsRec[0] = *a1;
  ChangeUserFlagsRec[1] = a1[1];
  ChatChangeUserFlags(a1: (struct _SNCHANGEUSERFLAGSREC *)ChangeUserFlagsRec);
  LocaleLoadString(a1: 0x40Bu, a2: szFmt, a3: 128);
  sprintf(Buffer: szText, Format: szFmt, 18, *a1);
  SendMessageA(hWnd: ghWndChat, Msg: 0x468u, wParam: 0x12u, lParam: (LPARAM)szText);
}


// ==============================================================================
/*
 * EA: 0x1900628A
 * CodeView symbol: ChatReceiveMsg
 * Demangled: ChatReceiveMsg
 * IDA name: ?ChatReceiveMsg@@YIXPAU_SNDISPLAYSTRINGREC@@@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x528A
 * Code length: 0x28C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatReceiveMsg(struct _SNDISPLAYSTRINGREC *a1)
{
  int v1; // edx
  struct _SNDISPLAYSTRINGREC *v2; // eax
  const char *v3; // ecx
  char bColorName; // [esp+8h] [ebp-1B4h]
  char szFromTo[16]; // [esp+Ch] [ebp-1B0h] BYREF
  int bColorString; // [esp+1Ch] [ebp-1A0h]
  char szMsg[408]; // [esp+20h] [ebp-19Ch] BYREF
  int bShowName; // [esp+1B8h] [ebp-4h]

  LOBYTE(bShowName) = 1;
  szFromTo[0] = 0;
  switch ( *((_DWORD *)a1 + 2) )
  {
    case 1:
      if ( IsUserSquelched(this: *(const char **)a1) == 0 )
      {
        bColorName = 19;
        LOBYTE(bColorString) = 17;
        LocaleLoadString(a1: 0x3F7u, a2: szFromTo, a3: 16);
        goto LABEL_19;
      }
      return;
    case 2:
      bColorName = 20;
      LOBYTE(bColorString) = 17;
      LocaleLoadString(a1: 0x3F8u, a2: szFromTo, a3: 16);
      goto LABEL_19;
    case 3:
      goto $L49612;
    case 4:
      LOBYTE(bColorString) = 25;
      LOBYTE(bShowName) = 0;
      goto LABEL_19;
    case 5:
      LOBYTE(bColorString) = 26;
      LOBYTE(bShowName) = 0;
      goto LABEL_19;
    case 7:
      LOBYTE(bColorString) = 27;
      LOBYTE(bShowName) = 0;
      goto LABEL_19;
    case 8:
      LOBYTE(bShowName) = 0;
$L49612:
      if ( IsUserSquelched(this: *(const char **)a1) != 0 )
        return;
      if ( (*((_DWORD *)a1 + 1) & 1) != 0 )
      {
        bColorName = 21;
        LOBYTE(bColorString) = 21;
      }
      else if ( (*((_DWORD *)a1 + 1) & 8) != 0 )
      {
        bColorName = 22;
        LOBYTE(bColorString) = 22;
      }
      else if ( (*((_DWORD *)a1 + 1) & 4) != 0 )
      {
        bColorName = 24;
        LOBYTE(bColorString) = 24;
      }
      else if ( (*((_DWORD *)a1 + 1) & 2) != 0 )
      {
        bColorName = 23;
        LOBYTE(bColorString) = 23;
      }
      else
      {
LABEL_18:
        bColorName = 19;
        LOBYTE(bColorString) = 16;
      }
LABEL_19:
      v1 = (unsigned __int8)bShowName;
      if ( (_BYTE)bShowName != 0 && (v2 = a1, v3 = *(const char **)a1, v1 = **(char **)a1, **(_BYTE **)a1 != 0) )
      {
        LOBYTE(v2) = bColorString;
        LOBYTE(v3) = bColorName;
        BuildMsg(pszSender: *(_DWORD *)a1, pszString: *((_DWORD *)a1 + 3), bColorName: (int)v3, bColorString: (int)v2);
      }
      else if ( *((_DWORD *)a1 + 2) == 8 )
      {
        LOBYTE(bColorString) = bColorName;
        LOBYTE(v1) = bColorName;
        EmoteMsg(pszString: *((_DWORD *)a1 + 3), bColorString: v1);
      }
      else
      {
        sprintf(Buffer: szMsg, Format: "%c%s", (unsigned __int8)bColorString, *((const char **)a1 + 3));
      }
      SendMessageA(hWnd: ghWndChat, Msg: 0x468u, wParam: (unsigned __int8)bColorString, lParam: (LPARAM)szMsg);
      return;
    default:
      goto LABEL_18;
  }
}


// ==============================================================================
/*
 * EA: 0x19006516
 * CodeView symbol: BuildMsg
 * Demangled: BuildMsg
 * IDA name: BuildMsg
 * CodeView module: 25
 * CV address: segment 1 : 0x5516
 * Code length: 0x47
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall BuildMsg(
        char *a1,
        const char *a2,
        const char *pszSender,
        const char *pszString,
        unsigned __int8 bColorName,
        unsigned __int8 bColorString)
{
  return sprintf(Buffer: a1, Format: "%c%c%s%s%c %c%s", bColorName, 60, a2, pszSender, 62, bColorString, pszString);
}


// ==============================================================================
/*
 * EA: 0x1900655D
 * CodeView symbol: EmoteMsg
 * Demangled: EmoteMsg
 * IDA name: EmoteMsg
 * CodeView module: 25
 * CV address: segment 1 : 0x555D
 * Code length: 0x35
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall EmoteMsg(char *a1, const char *a2, const char *pszString, unsigned __int8 bColorString)
{
  return sprintf(Buffer: a1, Format: "%c<%s %s>", bColorString, a2, pszString);
}


// ==============================================================================
/*
 * EA: 0x19006592
 * CodeView symbol: ChatRoomDialogProc
 * Demangled: ChatRoomDialogProc
 * IDA name: ?ChatRoomDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x5592
 * Code length: 0xA9B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ChatRoomDialogProc(int a1, __int16 a2, HWND window, UINT message, HDC wparam, HWND lparam)
{
  int result; // eax
  HWND v7; // esi
  HWND DlgItem; // eax
  HWND v9; // eax
  HMODULE ModuleHandleA; // eax
  HWND FrameWindow; // eax
  HWND Parent; // eax
  LONG WindowLongA; // edx
  LONG v14; // eax
  HWND v15; // esi
  HWND v16; // eax
  HWND v17; // eax
  HWND v18; // eax
  HWND v19; // eax
  HWND v20; // eax
  HWND v21; // eax
  HWND v22; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-158h]
  int v24; // [esp-4h] [ebp-14Ch]
  HWND button; // [esp+10h] [ebp-138h]
  unsigned int index; // [esp+14h] [ebp-134h]
  _DWORD buttontbl[4]; // [esp+18h] [ebp-130h]
  char szText[256]; // [esp+28h] [ebp-120h] BYREF
  char szTitle[32]; // [esp+128h] [ebp-20h] BYREF

  if ( message > 0x110 )
  {
    if ( message > 0x138 )
    {
      if ( message == 1128 )
      {
        LOBYTE(a2) = (_BYTE)wparam;
        DisplayMsg(a1: lparam, a2);
        return 1;
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
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
    if ( message != 273 )
    {
      if ( message == 274 )
      {
        Parent = GetParent(hWnd: window);
        SendMessageA(hWnd: Parent, Msg: 0x112u, wParam: (WPARAM)wparam, lParam: (LPARAM)lparam);
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( (unsigned __int16)wparam > 0x3F1u )
    {
      switch ( (__int16)wparam )
      {
        case 1012:
          EnableWindow(hWnd: lparam, bEnable: false);
          if ( *(_DWORD *)(*(_DWORD *)(dword_19042050 + 12) + 36) != 0 )
            (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(dword_19042050 + 12) + 36))(
              a1: 1112425812,
              a2: 1,
              a3: 0);
          DoChatSelectChannel(
            a1: *(HWND **)(dword_19042050 + 12),
            a2: gszCurrentChannel,
            pChannelListHead: sgpChannelListHead);
          SetFocus(hWnd: hWndEdit);
          EnableWindow(hWnd: lparam, bEnable: true);
          result = 1;
          break;
        case 1035:
          if ( SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1003, Msg: 0x188u, wParam: 0, lParam: 0) != -1 )
            goto LABEL_33;
          LocaleLoadString(a1: 0x3F2u, a2: szTitle, a3: 32);
          LocaleLoadString(a1: 0x3FAu, a2: szText, a3: 256);
          EnableWindow(hWnd: lparam, bEnable: false);
          (*(void (__stdcall **)(HWND, char *, char *, int))(*(_DWORD *)(dword_19042050 + 12) + 32))(
            a1: window,
            a2: szText,
            a3: szTitle,
            a4: 16);
          EnableWindow(hWnd: lparam, bEnable: true);
          result = 1;
          break;
        case 1045:
          EnableWindow(hWnd: lparam, bEnable: false);
          if ( *(_DWORD *)(*(_DWORD *)(dword_19042050 + 12) + 36) != 0 )
            (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(dword_19042050 + 12) + 36))(
              a1: 1112425812,
              a2: 1,
              a3: 0);
          SDlgEndDialog(a1: window, a2: 0);
          SetCursor(hCursor: nullptr);
          result = 1;
          break;
        case 1051:
          ToggleVerboseMode();
          result = 1;
          break;
        case 1052:
          v7 = hWndEdit;
          DlgItem = GetDlgItem(hDlg: window, nIDDlgItem: 1003);
          CopyNameToEditCtl(a1: DlgItem, a2: v7);
          result = 1;
          break;
        default:
          return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
      }
      return result;
    }
    if ( (unsigned __int16)wparam == 1009 )
    {
      EnableWindow(hWnd: lparam, bEnable: false);
      if ( *(_DWORD *)(*(_DWORD *)(dword_19042050 + 12) + 36) != 0 )
        (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(dword_19042050 + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
      DoLadder(a1: dword_19042050);
      SetFocus(hWnd: hWndEdit);
      EnableWindow(hWnd: lparam, bEnable: true);
      return 1;
    }
    if ( (unsigned __int16)wparam <= 0x3EEu )
    {
      if ( (unsigned __int16)wparam != 1006 )
      {
        switch ( (unsigned __int16)wparam )
        {
          case 1u:
LABEL_33:
            EnableWindow(hWnd: lparam, bEnable: false);
            ChatOk((char)lparam);
            EnableWindow(hWnd: lparam, bEnable: true);
            return 1;
          case 0x3EBu:
            if ( HIWORD(wparam) == 1 )
            {
              SetFocus(hWnd: hWndEdit);
              ListUpdateScrollbar(a1: lparam);
            }
            else if ( HIWORD(wparam) == 2 )
            {
              EnableWindow(hWnd: lparam, bEnable: false);
              v9 = GetDlgItem(hDlg: window, nIDDlgItem: 1003);
              DoProfile(hListWnd: (int)v9);
              EnableWindow(hWnd: lparam, bEnable: true);
              SetFocus(hWnd: hWndEdit);
            }
            break;
          case 0x3EDu:
            SetFocus(hWnd: hWndEdit);
            return 0;
          default:
            break;
        }
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
      }
      if ( HIWORD(wparam) == 512 )
      {
        SendMessageA(hWnd: lparam, Msg: 0xB0u, wParam: (WPARAM)&start, lParam: (LPARAM)&end);
      }
      else if ( HIWORD(wparam) == 256 )
      {
        SendMessageA(hWnd: lparam, Msg: 0xB1u, wParam: start, lParam: end);
      }
      return 1;
    }
    if ( (unsigned __int16)wparam == 1007 )
    {
      EnableWindow(hWnd: lparam, bEnable: false);
      if ( *(_DWORD *)(*(_DWORD *)(dword_19042050 + 12) + 36) != 0 )
        (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(dword_19042050 + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
      if ( DoJoinGame(a1: dword_19042050) != 0 )
      {
LABEL_48:
        SDlgEndDialog(a1: window, a2: 1);
LABEL_50:
        EnableWindow(hWnd: lparam, bEnable: true);
        return 1;
      }
    }
    else
    {
      if ( (unsigned __int16)wparam != 1008 )
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
      EnableWindow(hWnd: lparam, bEnable: false);
      if ( *(_DWORD *)(*(_DWORD *)(dword_19042050 + 12) + 36) != 0 )
        (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(dword_19042050 + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
      if ( DoCreateGame(a1: window, a2: dword_19042050) != 0 )
        goto LABEL_48;
    }
    SetFocus(hWnd: hWndEdit);
    goto LABEL_50;
  }
  if ( message == 272 )
  {
    dword_19042050 = (int)lparam;
    sgbLadderEnabled = (*(_DWORD *)(*((_DWORD *)lparam + 3) + 4) & 1) != 0;
    sub_19007162(a1: window, a2: *(_DWORD *)(*((_DWORD *)lparam + 3) + 12));
    if ( (*(_DWORD *)(*(_DWORD *)(dword_19042050 + 12) + 4) & 2) != 0 )
    {
      buttontbl[0] = 1012;
      buttontbl[1] = 1008;
      buttontbl[2] = 1007;
      buttontbl[3] = 1045;
      for ( index = 0; index < 4; ++index )
      {
        button = GetDlgItem(hDlg: window, nIDDlgItem: buttontbl[index]);
        WindowLongA = GetWindowLongA(hWnd: button, nIndex: -16);
        BYTE1(WindowLongA) &= 0xF3u;
        v14 = WindowLongA;
        BYTE1(v14) = BYTE1(WindowLongA) | 4;
        SetWindowLongA(hWnd: button, nIndex: -16, dwNewLong: v14);
      }
    }
    sghChatFont = (HGDIOBJ)SetChatFonts(a1: window);
    ghWndChat = window;
    sghWndMsgList = GetDlgItem(hDlg: window, nIDDlgItem: 1005);
    hWndEdit = GetDlgItem(hDlg: window, nIDDlgItem: 1006);
    v15 = GetDlgItem(hDlg: window, nIDDlgItem: 1049);
    v16 = GetDlgItem(hDlg: window, nIDDlgItem: 1003);
    ScrollbarLink(a1: v16, a2: v15);
    v17 = GetDlgItem(hDlg: window, nIDDlgItem: 1050);
    ScrollbarLink(a1: sghWndMsgList, a2: v17);
    SendMessageA(hWnd: hWndEdit, Msg: 0xC5u, wParam: 0xFFu, lParam: 0);
    SendMessageA(hWnd: sghWndMsgList, Msg: 0x192u, wParam: 1u, lParam: (LPARAM)&sgnTab);
    SendMessageA(hWnd: sghWndMsgList, Msg: 0x1A0u, wParam: 0, lParam: 14);
    SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1003, Msg: 0x1A0u, wParam: 0, lParam: 16);
    sgbFirstChannel = 1;
    SrvGetLocalPlayerName(a1: 1, a2: sgszUserName, a3: 0x80u);
    if ( SrvBeginChat(
           a1: *(struct _SNETPROGRAMDATA **)(dword_19042050 + 4),
           a2: *(struct _SNETPLAYERDATA **)(dword_19042050 + 8),
           a3: gszCurrentChannel) == 0 )
    {
      v18 = GetParent(hWnd: sghWndMsgList);
      PostMessageA(hWnd: v18, Msg: 0x467u, wParam: 0, lParam: 0);
      SDlgEndDialog(a1: window, a2: 0);
    }
    if ( gbConnectionSucks != 0 )
    {
      v19 = GetDlgItem(hDlg: window, nIDDlgItem: 1008);
      EnableWindow(hWnd: v19, bEnable: false);
      v20 = GetDlgItem(hDlg: window, nIDDlgItem: 1007);
      EnableWindow(hWnd: v20, bEnable: false);
    }
    v21 = GetDlgItem(hDlg: window, nIDDlgItem: 1051);
    ShowWindow(hWnd: v21, nCmdShow: 0);
    v22 = GetDlgItem(hDlg: window, nIDDlgItem: 1052);
    ShowWindow(hWnd: v22, nCmdShow: 0);
    SRegLoadValue(a1: "Preferences", a2: "Verbose", a3: 2, a4: &sgdwVerboseMode);
    UiLoadCursors(a1: window, a2: *(struct _SNETUIDATA **)(dword_19042050 + 12));
    UiRestoreApp();
    return 1;
  }
  else
  {
    if ( message <= 0x2B )
    {
      switch ( message )
      {
        case 0x2Bu:
          if ( ChatDrawItem((int)lparam) != 0 )
            return 1;
          break;
        case 2u:
          SrvEndChat();
          sub_1900702D(a1: window);
          if ( sghChatFont != nullptr )
            DeleteObject(ho: sghChatFont);
          sghWndMsgList = nullptr;
          ghWndChat = nullptr;
          hWndEdit = nullptr;
          TListClear(a1: &sgpChannelListHead, a2: aChatroomCpp_3, a3: 1843);
          TListClear(a1: &sgpSquelchListHead, a2: aChatroomCpp_4, a3: 1844);
          break;
        case 0x10u:
          SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1045, Msg: 0xF5u, wParam: 0, lParam: 0);
          return 1;
        default:
          break;
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( message != 83 )
    {
      if ( message > 0x103 && message <= 0x105 )
      {
        FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
        SendMessageA(hWnd: FrameWindow, Msg: message, wParam: (WPARAM)wparam, lParam: (LPARAM)lparam);
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( *(_DWORD *)(*(_DWORD *)(dword_19042050 + 12) + 36) != 0 )
      (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(dword_19042050 + 12) + 36))(a1: 1112425812, a2: 1, a3: 0);
    v24 = dword_19042050;
    Dialog = LocaleGetDialog(a1: 0x66u);
    ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
    SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: window, a4: ChatHelpDialogProc, a5: v24);
    SetFocus(hWnd: hWndEdit);
    return 1;
  }
}


// ==============================================================================
/*
 * EA: 0x1900702D
 * CodeView symbol: DestroyArtwork
 * Demangled: DestroyArtwork
 * IDA name: sub_1900702D
 * CodeView module: 25
 * CV address: segment 1 : 0x602D
 * Code length: 0x135
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1900702D()
{
  int result; // eax

  if ( backgroundbitmap != nullptr )
  {
    result = SMemFree(a1: backgroundbitmap, a2: aChatroomCpp_5, a3: 156, a4: 0);
    backgroundbitmap = nullptr;
  }
  if ( bnBtnbitmap != nullptr )
  {
    result = SMemFree(a1: bnBtnbitmap, a2: aChatroomCpp_6, a3: 157, a4: 0);
    bnBtnbitmap = nullptr;
  }
  if ( SmlBtnbitmap != nullptr )
  {
    result = SMemFree(a1: SmlBtnbitmap, a2: aChatroomCpp_7, a3: 158, a4: 0);
    SmlBtnbitmap = nullptr;
  }
  if ( redlagbitmap != nullptr )
  {
    result = SMemFree(a1: redlagbitmap, a2: aChatroomCpp_8, a3: 159, a4: 0);
    redlagbitmap = nullptr;
  }
  if ( yellowlagbitmap != nullptr )
  {
    result = SMemFree(a1: yellowlagbitmap, a2: aChatroomCpp_9, a3: 160, a4: 0);
    yellowlagbitmap = nullptr;
  }
  if ( greenlagbitmap != nullptr )
  {
    result = SMemFree(a1: greenlagbitmap, a2: aChatroomCpp_10, a3: 161, a4: 0);
    greenlagbitmap = nullptr;
  }
  if ( bmpBadConn != nullptr )
  {
    result = SMemFree(a1: bmpBadConn, a2: aChatroomCpp_11, a3: 162, a4: 0);
    bmpBadConn = nullptr;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19007162
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: sub_19007162
 * CodeView module: 25
 * CV address: segment 1 : 0x6162
 * Code length: 0x217
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_19007162(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  _DWORD btn_ids[5]; // [esp+8h] [ebp-58h] BYREF
  struct tagSIZE bnBtnSize; // [esp+1Ch] [ebp-44h] BYREF
  _DWORD ladder_ids[6]; // [esp+24h] [ebp-3Ch] BYREF
  struct tagSIZE SmlBtnSize; // [esp+3Ch] [ebp-24h] BYREF
  struct tagSIZE bgSize; // [esp+44h] [ebp-1Ch] BYREF
  _DWORD btn_small[3]; // [esp+4Ch] [ebp-14h] BYREF
  _DWORD static_txt[2]; // [esp+58h] [ebp-8h] BYREF

  btn_ids[0] = 1012;
  btn_ids[1] = 1008;
  btn_ids[2] = 1007;
  btn_ids[3] = 1045;
  btn_ids[4] = 0;
  ladder_ids[0] = 1012;
  ladder_ids[1] = 1008;
  ladder_ids[2] = 1007;
  ladder_ids[3] = 1009;
  ladder_ids[4] = 1045;
  ladder_ids[5] = 0;
  btn_small[0] = 1;
  btn_small[1] = 1035;
  btn_small[2] = 0;
  static_txt[0] = 1046;
  static_txt[1] = 0;
  UiLoadArtwork(
    a1: a2,
    a2: a1,
    hWndParent: nullptr,
    artid: 0x80000001,
    controltype: byte_19042054,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &backgroundbitmap,
    a11: &bgSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 0x80000002,
    controltype: byte_19042058,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &greenlagbitmap,
    a11: &greenSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 0x80000003,
    controltype: byte_1904205C,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &yellowlagbitmap,
    a11: &yellowSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 0x80000004,
    controltype: byte_19042060,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &redlagbitmap,
    a11: &redSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 0x80000000,
    controltype: aButton_11,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &bnBtnbitmap,
    a11: &bnBtnSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 5u,
    controltype: aButton_12,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &SmlBtnbitmap,
    a11: &SmlBtnSize);
  UiLoadArtwork(
    a1: a2,
    a2: nullptr,
    hWndParent: nullptr,
    artid: 0x8000000B,
    controltype: byte_19042064,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &bmpBadConn,
    a11: &sizeBadConn);
  if ( sgbLadderEnabled != 0 )
    SDlgSetControlBitmaps(a1, a2: ladder_ids, a3: 0, a4: bnBtnbitmap, a5: &bnBtnSize, a6: 1, a7: -1);
  else
    SDlgSetControlBitmaps(a1, a2: btn_ids, a3: 0, a4: bnBtnbitmap, a5: &bnBtnSize, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: btn_small, a3: 0, a4: SmlBtnbitmap, a5: &SmlBtnSize, a6: 1, a7: -1);
  SDlgSetControlBitmaps(a1, a2: static_txt, a3: 0, a4: backgroundbitmap, a5: &bgSize, a6: 2, a7: -1);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x19007379
 * CodeView symbol: DoCreateGame
 * Demangled: DoCreateGame
 * IDA name: DoCreateGame
 * CodeView module: 25
 * CV address: segment 1 : 0x6379
 * Code length: 0x25F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall DoCreateGame(HWND a1, int a2)
{
  char szText[512]; // [esp+Ch] [ebp-378h] BYREF
  int dwReturn; // [esp+20Ch] [ebp-178h]
  char playername[128]; // [esp+210h] [ebp-174h] BYREF
  _QWORD playerdata[2]; // [esp+290h] [ebp-F4h] BYREF
  _DWORD createdata[4]; // [esp+2A0h] [ebp-E4h] BYREF
  _DWORD interfacedata[20]; // [esp+2B0h] [ebp-D4h] BYREF
  char playerdescription[128]; // [esp+300h] [ebp-84h] BYREF
  HWND hWndParent; // [esp+380h] [ebp-4h]

  hWndParent = *(HWND *)(*(_DWORD *)(a2 + 12) + 8);
  if ( *(_DWORD *)(*(_DWORD *)(a2 + 12) + 20) == 0 )
    return 0;
  if ( *(_DWORD *)(*(_DWORD *)(a2 + 4) + 52) != 0 )
  {
    if ( *(_DWORD *)(*(_DWORD *)(a2 + 12) + 32) != 0 )
    {
      LocaleLoadString(a1: 0x43Du, a2: szText, a3: 512);
      UiMessageBox(
        a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*(_DWORD *)(a2 + 12) + 32),
        a2: a1,
        lpText: szText,
        a4: byte_19042068,
        a5: 0x30u);
    }
    return 0;
  }
  else
  {
    memset(a1: interfacedata, Val: 0, Size: sizeof(interfacedata));
    if ( *(_DWORD *)(a2 + 12) != 0 )
      memcpy(a1: interfacedata, Src: *(const void **)(a2 + 12), Size: sizeof(interfacedata));
    interfacedata[0] = 80;
    interfacedata[2] = hWndParent;
    playername[0] = byte_1904206C;
    memset(&playername[1], 0, 127);
    playerdescription[0] = byte_19042070;
    memset(&playerdescription[1], 0, 127);
    memset(a1: playerdata, Val: 0, Size: sizeof(playerdata));
    if ( *(_DWORD *)(a2 + 8) != 0 )
      *(_OWORD *)playerdata = *(_OWORD *)*(_DWORD *)(a2 + 8);
    SrvGetLocalPlayerName(a1: 0, a2: playername, a3: 0x80u);
    SrvGetLocalPlayerDesc(a1: playerdescription, a2: 0x80u);
    LODWORD(playerdata[0]) = 16;
    HIDWORD(playerdata[0]) = playername;
    LODWORD(playerdata[1]) = playerdescription;
    createdata[0] = 16;
    createdata[1] = 1112425812;
    createdata[2] = global_maxplayers;
    createdata[3] = 1;
    SetActiveWindow(hWnd: hWndParent);
    ShowWindow(hWnd: ghWndChat, nCmdShow: 0);
    dwReturn = (*(int (__stdcall **)(_DWORD *, _DWORD, _QWORD *, _DWORD *, _DWORD, _DWORD))(*(_DWORD *)(a2 + 12) + 20))(
                 a1: createdata,
                 a2: *(_DWORD *)(a2 + 4),
                 a3: playerdata,
                 a4: interfacedata,
                 a5: *(_DWORD *)(a2 + 16),
                 a6: *(_DWORD *)(a2 + 20));
    if ( dwReturn == 0 )
      ShowWindow(hWnd: ghWndChat, nCmdShow: 5);
    return dwReturn;
  }
}


// ==============================================================================
/*
 * EA: 0x190075D8
 * CodeView symbol: DoJoinGame
 * Demangled: DoJoinGame
 * IDA name: DoJoinGame
 * CodeView module: 25
 * CV address: segment 1 : 0x65D8
 * Code length: 0x99
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __thiscall DoJoinGame(_DWORD *this)
{
  HMODULE ModuleHandleA; // eax
  int v2; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-20h]
  BOOL dwReturn; // [esp+4h] [ebp-Ch]
  unsigned int dialogid; // [esp+8h] [ebp-8h]
  HWND hWndParent; // [esp+Ch] [ebp-4h]

  hWndParent = *(HWND *)(*(this + 3) + 8);
  SetActiveWindow(hWnd: hWndParent);
  ShowWindow(hWnd: ghWndChat, nCmdShow: 0);
  if ( *(_DWORD *)(*(this + 3) + 52) != 0 )
    dialogid = 110;
  else
    dialogid = 101;
  Dialog = LocaleGetDialog(a1: dialogid);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  v2 = SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: hWndParent, a4: JoinGameDialogProc, a5: this);
  dwReturn = v2 == 1;
  if ( v2 != 1 )
    ShowWindow(hWnd: ghWndChat, nCmdShow: 5);
  return dwReturn;
}


// ==============================================================================
/*
 * EA: 0x19007671
 * CodeView symbol: DoLadder
 * Demangled: DoLadder
 * IDA name: DoLadder
 * CodeView module: 25
 * CV address: segment 1 : 0x6671
 * Code length: 0x6D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall DoLadder(_DWORD *this)
{
  HMODULE ModuleHandleA; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-1Ch]
  int dwReturn; // [esp+4h] [ebp-8h]
  HWND hWndParent; // [esp+8h] [ebp-4h]

  hWndParent = *(HWND *)(*(this + 3) + 8);
  SetActiveWindow(hWnd: hWndParent);
  ShowWindow(hWnd: ghWndChat, nCmdShow: 0);
  Dialog = LocaleGetDialog(a1: 0x70u);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  dwReturn = SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: hWndParent, a4: LadderDialogProc, a5: this);
  ShowWindow(hWnd: ghWndChat, nCmdShow: 5);
  return dwReturn;
}


// ==============================================================================
/*
 * EA: 0x190076DE
 * CodeView symbol: DoProfile
 * Demangled: DoProfile
 * IDA name: DoProfile
 * CodeView module: 25
 * CV address: segment 1 : 0x66DE
 * Code length: 0xD2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall DoProfile(struct _UIPARAMS *a1, HWND a2, HWND hListWnd)
{
  char *pUserEnd; // [esp+8h] [ebp-14h]
  LRESULT nLen; // [esp+Ch] [ebp-10h]
  LRESULT nPos; // [esp+10h] [ebp-Ch]
  const char *pszUser; // [esp+14h] [ebp-8h]

  nPos = SendMessageA(hWnd: hListWnd, Msg: 0x188u, wParam: 0, lParam: 0);
  if ( nPos == -1 )
    return 0;
  nLen = SendMessageA(hWnd: hListWnd, Msg: 0x18Au, wParam: nPos, lParam: 0);
  pszUser = (const char *)SMemAlloc(a1: nLen + 1, a2: aChatroomCpp_12, a3: 446, a4: 0);
  SendMessageA(hWnd: hListWnd, Msg: 0x189u, wParam: nPos, lParam: (LPARAM)pszUser);
  pUserEnd = strchr(Str: pszUser, Val: 9);
  if ( pUserEnd != nullptr )
  {
    *pUserEnd = 0;
    ProfileCall(a1: a2, a2: a1, a3: pszUser);
    SMemFree(a1: pszUser, a2: aChatroomCpp_13, a3: 457, a4: 0);
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x190077B0
 * CodeView symbol: SetChatFonts
 * Demangled: SetChatFonts
 * IDA name: SetChatFonts
 * CodeView module: 25
 * CV address: segment 1 : 0x67B0
 * Code length: 0x144
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HFONT __thiscall SetChatFonts(HWND this)
{
  HGDIOBJ hFont; // [esp+4h] [ebp-40h]
  HFONT hFonta; // [esp+4h] [ebp-40h]
  LOGFONTA lFont; // [esp+8h] [ebp-3Ch] BYREF

  hFont = (HGDIOBJ)SendMessageA(hWnd: this, Msg: 0x31u, wParam: 0, lParam: 0);
  if ( hFont == nullptr )
    hFont = GetStockObject(i: 13);
  if ( GetObjectA(h: hFont, c: 60, pv: &lFont) == 0 )
    return nullptr;
  lFont.lfHeight = -MulDiv(nNumber: 8, nNumerator: 96, nDenominator: 72);
  lFont.lfWidth = 0;
  lFont.lfWeight = 400;
  lFont.lfFaceName[0] = 0;
  SRegLoadString(a1: aConfiguration_13, a2: aFont, a3: 2, a4: lFont.lfFaceName, a5: 32);
  lFont.lfCharSet = 0x80;
  if ( lFont.lfFaceName[0] == 0 )
    strcpy(Destination: lFont.lfFaceName, Source: aArial);
  hFonta = CreateFontIndirectA(lplf: &lFont);
  if ( hFonta == nullptr )
  {
    strcpy(Destination: lFont.lfFaceName, Source: aArial_0);
    hFonta = CreateFontIndirectA(lplf: &lFont);
    if ( hFonta == nullptr )
      return nullptr;
  }
  SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1003, Msg: 0x30u, wParam: (WPARAM)hFonta, lParam: 0);
  SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1005, Msg: 0x30u, wParam: (WPARAM)hFonta, lParam: 0);
  SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1006, Msg: 0x30u, wParam: (WPARAM)hFonta, lParam: 0);
  SendDlgItemMessageA(hDlg: this, nIDDlgItem: 1046, Msg: 0x30u, wParam: (WPARAM)hFonta, lParam: 0);
  return hFonta;
}


// ==============================================================================
/*
 * EA: 0x190078F4
 * CodeView symbol: ToggleVerboseMode
 * Demangled: ToggleVerboseMode
 * IDA name: ToggleVerboseMode
 * CodeView module: 25
 * CV address: segment 1 : 0x68F4
 * Code length: 0x89
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int ToggleVerboseMode()
{
  char szText[256]; // [esp+0h] [ebp-180h] BYREF
  char szFmt[128]; // [esp+100h] [ebp-80h] BYREF

  sgdwVerboseMode = sgdwVerboseMode == 0;
  LocaleLoadString(a1: 1038 - (sgdwVerboseMode != 0), a2: szFmt, a3: 128);
  sprintf(Buffer: szText, Format: szFmt, 18, szText);
  SendMessageA(hWnd: ghWndChat, Msg: 0x468u, wParam: 0x12u, lParam: (LPARAM)szText);
  return SRegSaveValue(a1: "Preferences", a2: "Verbose", a3: 2, a4: sgdwVerboseMode);
}


// ==============================================================================
/*
 * EA: 0x1900797D
 * CodeView symbol: DisplayMsg
 * Demangled: DisplayMsg
 * IDA name: DisplayMsg
 * CodeView module: 25
 * CV address: segment 1 : 0x697D
 * Code length: 0x2F2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall DisplayMsg(char *a1, char a2)
{
  size_t nStrLen; // [esp+Ch] [ebp-54h]
  LONG nRight; // [esp+10h] [ebp-50h]
  int nLine; // [esp+14h] [ebp-4Ch]
  const char *p; // [esp+18h] [ebp-48h]
  BOOL bScrollToBtm; // [esp+1Ch] [ebp-44h]
  LRESULT nPixHt; // [esp+20h] [ebp-40h]
  HGDIOBJ hOldFont; // [esp+24h] [ebp-3Ch]
  LRESULT nItems; // [esp+28h] [ebp-38h]
  LRESULT nTopItem; // [esp+2Ch] [ebp-34h]
  LRESULT nTotalLines; // [esp+30h] [ebp-30h]
  char *pszText; // [esp+34h] [ebp-2Ch]
  char *pszTexta; // [esp+34h] [ebp-2Ch]
  HDC hDC; // [esp+38h] [ebp-28h]
  char charSave; // [esp+3Ch] [ebp-24h]
  struct tagRECT windowRect; // [esp+40h] [ebp-20h] BYREF
  int bDeletedLines; // [esp+50h] [ebp-10h]
  int nFit; // [esp+54h] [ebp-Ch] BYREF
  struct tagSIZE size; // [esp+58h] [ebp-8h] BYREF

  bScrollToBtm = true;
  bDeletedLines = 0;
  if ( sghWndMsgList != nullptr )
  {
    hDC = GetDC(hWnd: sghWndMsgList);
    if ( hDC != nullptr )
    {
      pszText = a1;
      hOldFont = SelectObject(hdc: hDC, h: sghChatFont);
      GetClientRect(hWnd: sghWndMsgList, lpRect: &windowRect);
      nPixHt = SendMessageA(hWnd: sghWndMsgList, Msg: 0x1A1u, wParam: 0, lParam: 0);
      nItems = SendMessageA(hWnd: sghWndMsgList, Msg: 0x18Bu, wParam: 0, lParam: 0);
      nTopItem = SendMessageA(hWnd: sghWndMsgList, Msg: 0x18Eu, wParam: 0, lParam: 0);
      if ( nPixHt != -1 && nItems != -1 )
        bScrollToBtm = windowRect.bottom / nPixHt + nTopItem >= nItems;
      SendMessageA(hWnd: sghWndMsgList, Msg: 0xBu, wParam: 0, lParam: 0);
      while ( 1 )
      {
        nRight = windowRect.right;
        p = pszText;
        if ( *pszText == 9 )
        {
          nRight = windowRect.right - 12;
          p = pszText + 1;
        }
        if ( *p >= 16 && *p <= 27 )
          ++p;
        nStrLen = strlen(Str: p);
        GetTextExtentExPointA(
          hdc: hDC,
          lpszString: p,
          cchString: nStrLen,
          nMaxExtent: nRight,
          lpnFit: &nFit,
          lpnDx: nullptr,
          lpSize: &size);
        if ( nFit != nStrLen )
        {
          for ( nLine = nFit; nLine > 0; --nLine )
          {
            if ( p[nLine] == 32 )
            {
              nFit = nLine;
              break;
            }
          }
        }
        charSave = p[nFit];
        p[nFit] = 0;
        nTotalLines = SendMessageA(hWnd: sghWndMsgList, Msg: 0x180u, wParam: 0, lParam: (LPARAM)pszText);
        if ( nStrLen == nFit )
          break;
        pszTexta = (char *)&p[nFit];
        if ( charSave == 32 )
          ++pszTexta;
        else
          *pszTexta = charSave;
        pszText = pszTexta - 2;
        *pszText = 9;
        pszText[1] = a2;
      }
      if ( nTotalLines >= 5000 )
      {
        while ( nTotalLines >= 5000 )
        {
          SendMessageA(hWnd: sghWndMsgList, Msg: 0x182u, wParam: 0, lParam: 0);
          --nTotalLines;
        }
        bDeletedLines = 1;
      }
      if ( bScrollToBtm )
      {
        ScrollToBtm(a1: sghWndMsgList);
      }
      else if ( bDeletedLines != 0 )
      {
        if ( nTopItem - 1 >= 0 )
          SendMessageA(hWnd: sghWndMsgList, Msg: 0x197u, wParam: nTopItem - 1, lParam: 0);
        else
          SendMessageA(hWnd: sghWndMsgList, Msg: 0x197u, wParam: 0, lParam: 0);
      }
      SendMessageA(hWnd: sghWndMsgList, Msg: 0xBu, wParam: 1u, lParam: 0);
      ListUpdateScrollbar(a1: sghWndMsgList);
      SelectObject(hdc: hDC, h: hOldFont);
      ReleaseDC(hWnd: sghWndMsgList, hDC);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19007C6F
 * CodeView symbol: ScrollToBtm
 * Demangled: ScrollToBtm
 * IDA name: ScrollToBtm
 * CodeView module: 25
 * CV address: segment 1 : 0x6C6F
 * Code length: 0x73
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __thiscall ScrollToBtm(HWND this)
{
  LPARAM lParam[7]; // [esp+4h] [ebp-24h] BYREF
  WPARAM nPos; // [esp+20h] [ebp-8h]
  HWND hWndScroll; // [esp+24h] [ebp-4h]

  nPos = SendMessageA(hWnd: this, Msg: 0x18Bu, wParam: 0, lParam: 0) - 1;
  SendMessageA(hWnd: this, Msg: 0x197u, wParam: nPos, lParam: 0);
  hWndScroll = (HWND)GetWindowLongA(hWnd: this, nIndex: -21);
  lParam[0] = 28;
  lParam[1] = 4;
  lParam[5] = nPos;
  return SendMessageA(hWnd: hWndScroll, Msg: 0xE9u, wParam: 1u, (LPARAM)lParam);
}


// ==============================================================================
/*
 * EA: 0x19007CE2
 * CodeView symbol: CopyNameToEditCtl
 * Demangled: CopyNameToEditCtl
 * IDA name: CopyNameToEditCtl
 * CodeView module: 25
 * CV address: segment 1 : 0x6CE2
 * Code length: 0xCD
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall CopyNameToEditCtl(HWND a1, HWND a2)
{
  LRESULT nLen; // [esp+8h] [ebp-10h]
  const char *pszUser; // [esp+Ch] [ebp-Ch]
  char *pUserName; // [esp+10h] [ebp-8h]
  LRESULT nIdx; // [esp+14h] [ebp-4h]

  nIdx = SendMessageA(hWnd: a1, Msg: 0x188u, wParam: 0, lParam: 0);
  if ( nIdx == -1 )
    return 0;
  nLen = SendMessageA(hWnd: a1, Msg: 0x18Au, wParam: nIdx, lParam: 0);
  pszUser = (const char *)SMemAlloc(a1: nLen + 1, a2: aChatroomCpp_14, a3: 783, a4: 0);
  SendMessageA(hWnd: a1, Msg: 0x189u, wParam: nIdx, lParam: (LPARAM)pszUser);
  pUserName = strchr(Str: pszUser, Val: 9);
  if ( pUserName == nullptr )
    return 0;
  *pUserName = 0;
  SendMessageA(hWnd: a2, Msg: 0xC2u, wParam: 1u, lParam: (LPARAM)pszUser);
  SMemFree(a1: pszUser, a2: aChatroomCpp_15, a3: 795, a4: 0);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19007DAF
 * CodeView symbol: ChatOk
 * Demangled: ChatOk
 * IDA name: ChatOk
 * CodeView module: 25
 * CV address: segment 1 : 0x6DAF
 * Code length: 0x154
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND __fastcall ChatOk(HWND a1, __int16 a2, char lparam)
{
  WPARAM v4; // [esp+0h] [ebp-39Ch]
  char szPlayerName[128]; // [esp+Ch] [ebp-390h] BYREF
  WPARAM nTextColor; // [esp+8Ch] [ebp-310h]
  char szMsg[392]; // [esp+90h] [ebp-30Ch] BYREF
  char szText[388]; // [esp+218h] [ebp-184h] BYREF

  if ( SendMessageA(hWnd: a1, Msg: 0xDu, wParam: 0x100u, lParam: (LPARAM)szText) != 0 )
  {
    FixChatString(a1: szText);
    SendChatMsg(a1: szText, a2: a2 == 1035);
    SendMessageA(hWnd: a1, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_19042074);
    if ( a2 == 1 && szText[0] != 47 )
    {
      SrvGetLocalPlayerName(a1: 1, a2: szPlayerName, a3: 0x80u);
      if ( sgbModeratedChannel == 0 || (gnUserFlags & 0xF) != 0 )
        v4 = 16;
      else
        v4 = 17;
      nTextColor = v4;
      BuildMsg(
        a1: szMsg,
        a2: byte_19042078,
        pszSender: szPlayerName,
        pszString: szText,
        bColorName: 0x14u,
        bColorString: v4);
      SendMessageA(hWnd: ghWndChat, Msg: 0x468u, wParam: nTextColor, lParam: (LPARAM)szMsg);
    }
    ScrollToBtm(this: sghWndMsgList);
  }
  return SetFocus(hWnd: a1);
}


// ==============================================================================
/*
 * EA: 0x19007F03
 * CodeView symbol: SendChatMsg
 * Demangled: SendChatMsg
 * IDA name: SendChatMsg
 * CodeView module: 25
 * CV address: segment 1 : 0x6F03
 * Code length: 0xD7
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
char *__fastcall SendChatMsg(const char *a1, int a2)
{
  char *result; // eax
  HWND hWndUserList; // [esp+8h] [ebp-28Ch]
  char szString[512]; // [esp+Ch] [ebp-288h] BYREF
  char szUser[128]; // [esp+20Ch] [ebp-88h] BYREF
  char *pUserName; // [esp+28Ch] [ebp-8h]
  char *nIdx; // [esp+290h] [ebp-4h]

  if ( a2 == 0 )
    return (char *)SrvSendChatString(a1);
  hWndUserList = GetDlgItem(hDlg: ghWndChat, nIDDlgItem: 1003);
  result = (char *)SendMessageA(hWnd: hWndUserList, Msg: 0x188u, wParam: 0, lParam: 0);
  nIdx = result;
  if ( result != (char *)-1 )
  {
    SendMessageA(hWnd: hWndUserList, Msg: 0x189u, wParam: (WPARAM)nIdx, lParam: (LPARAM)szUser);
    result = strchr(Str: szUser, Val: 9);
    pUserName = result;
    if ( result != nullptr )
    {
      *pUserName = 0;
      sprintf(Buffer: szString, Format: "/whisper %s %s", szUser, a1);
      return (char *)SrvSendChatString(a1: szString);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19007FDA
 * CodeView symbol: FixChatString
 * Demangled: FixChatString
 * IDA name: FixChatString
 * CodeView module: 25
 * CV address: segment 1 : 0x6FDA
 * Code length: 0x40
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_BYTE *__thiscall FixChatString(_BYTE *this)
{
  _BYTE *result; // eax

  while ( 1 )
  {
    result = this;
    if ( *this == 0 )
      break;
    if ( (unsigned __int8)*this < 0x20u )
      *this = 45;
    ++this;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900801A
 * CodeView symbol: ChatDrawItem
 * Demangled: ChatDrawItem
 * IDA name: ChatDrawItem
 * CodeView module: 25
 * CV address: segment 1 : 0x701A
 * Code length: 0x52
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ChatDrawItem(int a1, int a2, int lparam)
{
  if ( a2 == 1003 )
  {
    ChatDrawUserList(a1, a2: lparam);
    return 1;
  }
  else if ( a2 == 1005 )
  {
    ChatDrawChatWindow(a1, a2: lparam);
    return 1;
  }
  else
  {
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1900806C
 * CodeView symbol: ChatDrawChatWindow
 * Demangled: ChatDrawChatWindow
 * IDA name: ChatDrawChatWindow
 * CodeView module: 25
 * CV address: segment 1 : 0x706C
 * Code length: 0x276
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __fastcall ChatDrawChatWindow(int a1, int a2)
{
  LRESULT result; // eax
  int nCount; // [esp+8h] [ebp-128h]
  const CHAR *p; // [esp+Ch] [ebp-124h]
  const CHAR *pText; // [esp+10h] [ebp-120h]
  struct tagPOINT pt; // [esp+14h] [ebp-11Ch] BYREF
  COLORREF oldBkColor; // [esp+1Ch] [ebp-114h]
  char szString; // [esp+20h] [ebp-110h] BYREF
  _BYTE v10[3]; // [esp+21h] [ebp-10Fh] BYREF
  int bFirstLine; // [esp+120h] [ebp-10h]
  int nStartX; // [esp+124h] [ebp-Ch]
  UINT nAlignMode; // [esp+128h] [ebp-8h]
  COLORREF oldTextColor; // [esp+12Ch] [ebp-4h]

  bFirstLine = 1;
  pText = &szString;
  result = SendMessageA(hWnd: *(HWND *)(a2 + 20), Msg: 0x189u, wParam: *(_DWORD *)(a2 + 8), lParam: (LPARAM)&szString);
  if ( result != -1 )
  {
    result = szString;
    if ( szString != 0 && *(_DWORD *)(a2 + 24) != 0 )
    {
      oldTextColor = SetTextColor(hdc: *(HDC *)(a2 + 24), color: 0xFFFFFFu);
      oldBkColor = SetBkColor(hdc: *(HDC *)(a2 + 24), color: 0);
      nStartX = *(_DWORD *)(a2 + 28) + 1;
      if ( szString == 9 )
      {
        nStartX += 12;
        pText = v10;
        bFirstLine = 0;
      }
      MoveToEx(hdc: *(HDC *)(a2 + 24), x: nStartX, y: *(_DWORD *)(a2 + 32), lppt: &pt);
      nAlignMode = SetTextAlign(hdc: *(HDC *)(a2 + 24), align: 1u);
      while ( *pText != 0 )
      {
        if ( *pText >= 16 && *pText <= 27 )
          SetTextColor(hdc: *(HDC *)(a2 + 24), color: *(&color + *pText++));
        p = pText;
        nCount = 0;
        while ( *p != 0 && (*p < 16 || *p > 27) )
        {
          ++nCount;
          ++p;
        }
        TextOutA(hdc: *(HDC *)(a2 + 24), x: 0, y: 0, lpString: pText, c: nCount);
        pText += nCount;
      }
      SetTextColor(hdc: *(HDC *)(a2 + 24), color: oldTextColor);
      SetBkColor(hdc: *(HDC *)(a2 + 24), color: oldBkColor);
      SetTextAlign(hdc: *(HDC *)(a2 + 24), align: nAlignMode);
      return MoveToEx(hdc: *(HDC *)(a2 + 24), x: pt.x, y: pt.y, lppt: nullptr);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190082E2
 * CodeView symbol: ChatDrawUserList
 * Demangled: ChatDrawUserList
 * IDA name: ChatDrawUserList
 * CodeView module: 25
 * CV address: segment 1 : 0x72E2
 * Code length: 0x3A2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ChatDrawUserList(int a1, struct tagDRAWITEMSTRUCT *a2)
{
  COLORREF v2; // eax
  int XSpacing; // eax
  COLORREF v4; // eax
  UINT v5; // eax
  int v6; // [esp-18h] [ebp-154h]
  UINT v7; // [esp-8h] [ebp-144h]
  DWORD SysColor; // [esp+0h] [ebp-13Ch]
  DWORD color; // [esp+4h] [ebp-138h]
  COLORREF v12; // [esp+10h] [ebp-12Ch]
  COLORREF v13; // [esp+14h] [ebp-128h]
  int bSelected; // [esp+18h] [ebp-124h]
  COLORREF oldBkColor; // [esp+1Ch] [ebp-120h]
  COLORREF oldTextColor; // [esp+20h] [ebp-11Ch]
  const char *szPlayerData; // [esp+24h] [ebp-118h]
  char szString[256]; // [esp+28h] [ebp-114h] BYREF
  unsigned int dwProgramID; // [esp+128h] [ebp-14h]
  unsigned int dwItemFlags; // [esp+12Ch] [ebp-10h] BYREF
  int nNetLag; // [esp+130h] [ebp-Ch] BYREF
  unsigned int *szDesc; // [esp+134h] [ebp-8h]
  char *p; // [esp+138h] [ebp-4h]

  szDesc = (unsigned int *)&unk_1904207C;
  szPlayerData = (const char *)&unk_19042080;
  nNetLag = 0;
  dwItemFlags = 0;
  if ( SendMessageA(hWnd: a2->hwndItem, Msg: 0x189u, wParam: a2->itemID, lParam: (LPARAM)szString) != -1
    && szString[0] != 0
    && a2->hDC != nullptr )
  {
    p = strchr(Str: szString, Val: 9);
    if ( p != nullptr )
    {
      *p++ = 0;
      szPlayerData = p;
      p = strchr(Str: p, Val: 9);
      if ( p != nullptr )
      {
        *p++ = 0;
        szDesc = (unsigned int *)p;
      }
    }
    sscanf(Buffer: szPlayerData, Format: "%d %d", &dwItemFlags, &nNetLag);
    dwProgramID = *(_DWORD *)(*(_DWORD *)(a1 + 4) + 12);
    if ( *(_BYTE *)szDesc != 0 )
      dwProgramID = *szDesc;
    if ( (dwItemFlags & 0x62F) == 0 && dwProgramID == *(_DWORD *)(*(_DWORD *)(a1 + 4) + 12) )
    {
      if ( *(_DWORD *)(a1 + 12) != 0 && *(_DWORD *)(*(_DWORD *)(a1 + 12) + 24) != 0 )
      {
        (*(void (__stdcall **)(int, int, char *, unsigned int *, unsigned int, int, _DWORD, struct tagDRAWITEMSTRUCT *))(*(_DWORD *)(a1 + 12) + 24))(
          a1: 1112425812,
          a2: 2,
          a3: szString,
          a4: szDesc,
          a5: dwItemFlags,
          a6: 1,
          a7: 0,
          a8: a2);
      }
      else
      {
        bSelected = a2->itemState & 1;
        v13 = SetTextColor(hdc: a2->hDC, color: 0xFFFFFFu);
        if ( bSelected != 0 )
        {
          SysColor = GetSysColor(nIndex: 13);
          v4 = SetBkColor(hdc: a2->hDC, color: SysColor);
        }
        else
        {
          v4 = SetBkColor(hdc: a2->hDC, color: 0);
        }
        v12 = v4;
        v5 = strlen(Str: szString);
        ExtTextOutA(
          hdc: a2->hDC,
          x: a2->rcItem.left,
          y: a2->rcItem.top,
          options: 6u,
          lprect: &a2->rcItem,
          lpString: szString,
          c: v5,
          lpDx: nullptr);
        SetTextColor(hdc: a2->hDC, color: v13);
        SetBkColor(hdc: a2->hDC, color: v12);
      }
    }
    else
    {
      oldTextColor = SetTextColor(hdc: a2->hDC, color: 0xFFFFu);
      if ( (a2->itemState & 1) != 0 )
      {
        color = GetSysColor(nIndex: 13);
        v2 = SetBkColor(hdc: a2->hDC, color);
      }
      else
      {
        v2 = SetBkColor(hdc: a2->hDC, color: 0);
      }
      oldBkColor = v2;
      v7 = strlen(Str: szString);
      v6 = IconGetYSpacing() + a2->rcItem.top;
      XSpacing = IconGetXSpacing();
      ExtTextOutA(
        hdc: a2->hDC,
        x: XSpacing + a2->rcItem.left,
        y: v6,
        options: 6u,
        lprect: &a2->rcItem,
        lpString: szString,
        c: v7,
        lpDx: nullptr);
      SetTextColor(hdc: a2->hDC, color: oldTextColor);
      SetBkColor(hdc: a2->hDC, color: oldBkColor);
      IconDraw(a1: a2, a2: dwItemFlags, a3: dwProgramID);
    }
    if ( (dwItemFlags & 0x10) != 0 )
      DrawBadConnection(a1: a2);
    else
      DrawNetLag(a1: nNetLag, a2);
  }
}


// ==============================================================================
/*
 * EA: 0x19008684
 * CodeView symbol: DrawBadConnection
 * Demangled: DrawBadConnection
 * IDA name: DrawBadConnection
 * CodeView module: 25
 * CV address: segment 1 : 0x7684
 * Code length: 0x7F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall DrawBadConnection(_DWORD *this)
{
  struct tagRECT srcRect; // [esp+8h] [ebp-10h] BYREF

  SetRect(lprc: &srcRect, xLeft: 0, yTop: 0, xRight: 17, yBottom: sizeBadConn.cy - 1);
  return SDlgBltToWindowI(
           a1: *(this + 5),
           a2: 0,
           a3: *(this + 9) - 21,
           a4: (*(this + 10) - *(this + 8) + 1 - sizeBadConn.cy) / 2 + *(this + 8),
           a5: bmpBadConn,
           a6: &srcRect,
           a7: &sizeBadConn,
           a8: -1,
           a9: 0,
           a10: 13369376);
}


// ==============================================================================
/*
 * EA: 0x19008710
 * CodeView symbol: TListAddEnd
 * Demangled: TListAddEnd
 * IDA name: ?TListAddEnd@@YIHPAPAU_CHANNEL_LIST@@PAU1@PBDH@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x7710
 * Code length: 0x84
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAddEnd(struct _CHANNEL_LIST **a1, struct _CHANNEL_LIST *a2, const char *filename, int linenumber)
{
  struct _CHANNEL_LIST **next; // [esp+8h] [ebp-8h]
  _DWORD *newptr; // [esp+Ch] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  newptr = (_DWORD *)SMemAlloc(a1: 36, a2: filename, a3: linenumber, a4: 0);
  if ( newptr == nullptr )
    return 0;
  memcpy(a1: newptr, Src: a2, Size: 0x24u);
  newptr[8] = 0;
  for ( next = a1; *next != nullptr; next = (struct _CHANNEL_LIST **)((char *)*next + 32) )
    ;
  *next = (struct _CHANNEL_LIST *)newptr;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x190087A0
 * CodeView symbol: TListFree
 * Demangled: TListFree
 * IDA name: ?TListFree@@YIHPAPAU_CHANNEL_LIST@@PAU1@PBDH@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x77A0
 * Code length: 0x7A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall TListFree(struct _CHANNEL_LIST **a1, struct _CHANNEL_LIST *a2, const char *filename, int linenumber)
{
  struct _CHANNEL_LIST **next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return false;
  for ( next = a1; *next != nullptr && *next != a2; next = (struct _CHANNEL_LIST **)((char *)*next + 32) )
    ;
  if ( *next != nullptr )
    *next = *((struct _CHANNEL_LIST **)*next + 8);
  SMemFree(a1: a2, a2: filename, a3: linenumber, a4: 0);
  return *next != nullptr;
}


// ==============================================================================
/*
 * EA: 0x19008820
 * CodeView symbol: TListAddEnd
 * Demangled: TListAddEnd
 * IDA name: ?TListAddEnd@@YIHPAPAU_SQUELCH_LIST@@PAU1@PBDH@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x7820
 * Code length: 0x90
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAddEnd(struct _SQUELCH_LIST **a1, struct _SQUELCH_LIST *a2, const char *filename, int linenumber)
{
  struct _SQUELCH_LIST **next; // [esp+8h] [ebp-8h]
  _DWORD *newptr; // [esp+Ch] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  newptr = (_DWORD *)SMemAlloc(a1: 132, a2: filename, a3: linenumber, a4: 0);
  if ( newptr == nullptr )
    return 0;
  memcpy(a1: newptr, Src: a2, Size: 0x84u);
  newptr[32] = 0;
  for ( next = a1; *next != nullptr; next = (struct _SQUELCH_LIST **)((char *)*next + 128) )
    ;
  *next = (struct _SQUELCH_LIST *)newptr;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x190088B0
 * CodeView symbol: TListFree
 * Demangled: TListFree
 * IDA name: ?TListFree@@YIHPAPAU_SQUELCH_LIST@@PAU1@PBDH@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x78B0
 * Code length: 0x80
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall TListFree(struct _SQUELCH_LIST **a1, struct _SQUELCH_LIST *a2, const char *filename, int linenumber)
{
  struct _SQUELCH_LIST **next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return false;
  for ( next = a1; *next != nullptr && *next != a2; next = (struct _SQUELCH_LIST **)((char *)*next + 128) )
    ;
  if ( *next != nullptr )
    *next = *((struct _SQUELCH_LIST **)*next + 32);
  SMemFree(a1: a2, a2: filename, a3: linenumber, a4: 0);
  return *next != nullptr;
}


// ==============================================================================
/*
 * EA: 0x19008930
 * CodeView symbol: TListClear
 * Demangled: TListClear
 * IDA name: ?TListClear@@YIHPAPAU_CHANNEL_LIST@@PBDH@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x7930
 * Code length: 0x53
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListClear(struct _CHANNEL_LIST **a1, const char *a2, int linenumber)
{
  struct _CHANNEL_LIST *next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr )
    return 0;
  while ( *a1 != nullptr )
  {
    next = *((struct _CHANNEL_LIST **)*a1 + 8);
    SMemFree(a1: *a1, a2, a3: linenumber, a4: 0);
    *a1 = next;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19008990
 * CodeView symbol: TListClear
 * Demangled: TListClear
 * IDA name: ?TListClear@@YIHPAPAU_SQUELCH_LIST@@PBDH@Z
 * CodeView module: 25
 * CV address: segment 1 : 0x7990
 * Code length: 0x56
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chatroom.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chatroom.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListClear(struct _SQUELCH_LIST **a1, const char *a2, int linenumber)
{
  struct _SQUELCH_LIST *next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr )
    return 0;
  while ( *a1 != nullptr )
  {
    next = *((struct _SQUELCH_LIST **)*a1 + 32);
    SMemFree(a1: *a1, a2, a3: linenumber, a4: 0);
    *a1 = next;
  }
  return 1;
}

