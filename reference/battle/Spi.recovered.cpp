/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 49
 */


// ==============================================================================
/*
 * EA: 0x19015F60
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: unknown_libname_5
 * CodeView module: 7
 * CV address: segment 1 : 0x14F60
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_5()
{
  sub_19015F6F();
  return sub_19015F7E();
}


// ==============================================================================
/*
 * EA: 0x19015F6F
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: sub_19015F6F
 * CodeView module: 7
 * CV address: segment 1 : 0x14F6F
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *sub_19015F6F()
{
  return CCritSect::CCritSect(this: (CCritSect *)&spi_extmsgcritsect);
}


// ==============================================================================
/*
 * EA: 0x19015F7E
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: sub_19015F7E
 * CodeView module: 7
 * CV address: segment 1 : 0x14F7E
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_19015F7E()
{
  return atexit(a1: sub_19015F90);
}


// ==============================================================================
/*
 * EA: 0x19015F90
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: sub_19015F90
 * CodeView module: 7
 * CV address: segment 1 : 0x14F90
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_19015F90()
{
  CCritSect::~CCritSect(this: &spi_extmsgcritsect);
}


// ==============================================================================
/*
 * EA: 0x19015F9F
 * CodeView symbol: $E9
 * Demangled: $E9
 * IDA name: unknown_libname_6
 * CodeView module: 7
 * CV address: segment 1 : 0x14F9F
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_6()
{
  sub_19015FAE();
  return sub_19015FBD();
}


// ==============================================================================
/*
 * EA: 0x19015FAE
 * CodeView symbol: $E6
 * Demangled: $E6
 * IDA name: sub_19015FAE
 * CodeView module: 7
 * CV address: segment 1 : 0x14FAE
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *sub_19015FAE()
{
  return CCritSect::CCritSect(this: (CCritSect *)&spi_gamecritsect);
}


// ==============================================================================
/*
 * EA: 0x19015FBD
 * CodeView symbol: $E8
 * Demangled: $E8
 * IDA name: sub_19015FBD
 * CodeView module: 7
 * CV address: segment 1 : 0x14FBD
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_19015FBD()
{
  return atexit(a1: sub_19015FCF);
}


// ==============================================================================
/*
 * EA: 0x19015FCF
 * CodeView symbol: $E7
 * Demangled: $E7
 * IDA name: sub_19015FCF
 * CodeView module: 7
 * CV address: segment 1 : 0x14FCF
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_19015FCF()
{
  CCritSect::~CCritSect(this: &spi_gamecritsect);
}


// ==============================================================================
/*
 * EA: 0x19015FDE
 * CodeView symbol: $E14
 * Demangled: $E14
 * IDA name: unknown_libname_7
 * CodeView module: 7
 * CV address: segment 1 : 0x14FDE
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_7()
{
  sub_19015FED();
  return sub_19015FFC();
}


// ==============================================================================
/*
 * EA: 0x19015FED
 * CodeView symbol: $E11
 * Demangled: $E11
 * IDA name: sub_19015FED
 * CodeView module: 7
 * CV address: segment 1 : 0x14FED
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *sub_19015FED()
{
  return CCritSect::CCritSect(this: (CCritSect *)&spi_packetcritsect);
}


// ==============================================================================
/*
 * EA: 0x19015FFC
 * CodeView symbol: $E13
 * Demangled: $E13
 * IDA name: sub_19015FFC
 * CodeView module: 7
 * CV address: segment 1 : 0x14FFC
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_19015FFC()
{
  return atexit(a1: sub_1901600E);
}


// ==============================================================================
/*
 * EA: 0x1901600E
 * CodeView symbol: $E12
 * Demangled: $E12
 * IDA name: sub_1901600E
 * CodeView module: 7
 * CV address: segment 1 : 0x1500E
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_1901600E()
{
  CCritSect::~CCritSect(this: &spi_packetcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901601D
 * CodeView symbol: SpiAddGame
 * Demangled: SpiAddGame
 * IDA name: ?SpiAddGame@@YIXPAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x1501D
 * Code length: 0x126
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SpiAddGame(struct _SNETSPI_GAMELIST *a1)
{
  void *v1; // eax
  struct _SNETSPI_GAMELIST *curr; // [esp+8h] [ebp-Ch]
  struct _SNETSPI_GAMELIST **nextptr; // [esp+Ch] [ebp-8h]
  _DWORD *newgame; // [esp+10h] [ebp-4h]

  v1 = (void *)SMemAlloc(a1: 316, a2: aSpiCpp, a3: 243, a4: 0);
  newgame = operator new(__formal: 60, ptr: v1);
  if ( newgame != nullptr )
  {
    memcpy(a1: newgame, Src: a1, Size: 0x13Cu);
    CCritSect::Enter(this: &spi_gamecritsect);
    nextptr = &spi_gamehead;
    while ( *nextptr != nullptr )
    {
      if ( memcmp(Buf1: (char *)*nextptr + 12, Buf2: newgame + 3, Size: 0x10u) == 0 )
      {
        curr = *nextptr;
        *nextptr = *((struct _SNETSPI_GAMELIST **)*nextptr + 74);
        SMemFree(a1: curr, a2: aSpiCpp_0, a3: 255, a4: 0);
      }
      else
      {
        nextptr = (struct _SNETSPI_GAMELIST **)((char *)*nextptr + 296);
      }
    }
    *newgame = ++idsequence;
    if ( idsequence == 0 )
    {
      idsequence = 1;
      *newgame = 1;
    }
    newgame[74] = 0;
    *nextptr = (struct _SNETSPI_GAMELIST *)newgame;
    CCritSect::Leave(this: &spi_gamecritsect);
  }
}


// ==============================================================================
/*
 * EA: 0x19016143
 * CodeView symbol: SpiCheckDataFile
 * Demangled: SpiCheckDataFile
 * IDA name: ?SpiCheckDataFile@@YGHPBDPBXKPAK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15143
 * Code length: 0x26
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __stdcall SpiCheckDataFile(
        const char *filename,
        const void *data,
        unsigned int bytes,
        unsigned int *extendedresult)
{
  *extendedresult = SrvCheckFile(a1: filename, a2: data, a3: bytes);
  return *extendedresult != 0;
}


// ==============================================================================
/*
 * EA: 0x19016169
 * CodeView symbol: SpiCompareNetAddresses
 * Demangled: SpiCompareNetAddresses
 * IDA name: ?SpiCompareNetAddresses@@YGHPAU_SNETADDR@@0PAK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15169
 * Code length: 0xF5
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiCompareNetAddresses(struct _SNETADDR *addr1, struct _SNETADDR *addr2, unsigned int *diffmagnitude)
{
  if ( diffmagnitude != nullptr )
    *diffmagnitude = 0;
  if ( addr1 != nullptr && addr2 != nullptr && diffmagnitude != nullptr )
  {
    if ( *((unsigned __int8 *)addr1 + 4) == *((unsigned __int8 *)addr2 + 4) )
    {
      if ( *((unsigned __int8 *)addr1 + 5) == *((unsigned __int8 *)addr2 + 5) )
      {
        if ( *((unsigned __int8 *)addr1 + 6) == *((unsigned __int8 *)addr2 + 6) )
        {
          if ( *((unsigned __int8 *)addr1 + 7) == *((unsigned __int8 *)addr2 + 7) )
            *diffmagnitude = 0;
          else
            *diffmagnitude = 2;
        }
        else
        {
          *diffmagnitude = 3;
        }
      }
      else
      {
        *diffmagnitude = 4;
      }
    }
    else
    {
      *diffmagnitude = 5;
    }
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901625E
 * CodeView symbol: SpiDestroy
 * Demangled: SpiDestroy
 * IDA name: ?SpiDestroy@@YGHXZ
 * CodeView module: 7
 * CV address: segment 1 : 0x1525E
 * Code length: 0x213
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiDestroy()
{
  DWORD timeout; // [esp+0h] [ebp-38h]
  unsigned int v2; // [esp+4h] [ebp-34h]
  struct sockaddr addr; // [esp+8h] [ebp-30h] BYREF
  __int16 buffer; // [esp+18h] [ebp-20h] BYREF
  int loop; // [esp+1Ch] [ebp-1Ch]
  SOCKET newsocket; // [esp+20h] [ebp-18h]
  struct _THREAD *next; // [esp+24h] [ebp-14h]
  DWORD startTime; // [esp+28h] [ebp-10h]
  struct _THREAD *curr; // [esp+2Ch] [ebp-Ch]
  unsigned int elapsed; // [esp+30h] [ebp-8h]
  unsigned int tries; // [esp+34h] [ebp-4h]

  spi_shutdown = 1;
  SpiStopAdvertisingGame();
  SrvDestroy();
  CCritSect::Enter(this: &spi_extmsgcritsect);
  TListClear(a1: &spi_extmsghead, a2: aSpiCpp_1, a3: 329);
  CCritSect::Leave(this: &spi_extmsgcritsect);
  for ( tries = 0; tries < 5; ++tries )
  {
    SpiGetGameAddress(a1: (struct _SNETADDR *)&addr);
    buffer = word_1903D68C;
    newsocket = socket(af: 2, type: 2, protocol: 0);
    for ( loop = 0; loop < 2; ++loop )
      sendto(s: newsocket, buf: (const char *)&buffer, len: 1, flags: 0, to: &addr, tolen: 16);
    closesocket(s: newsocket);
    startTime = GetTickCount();
    for ( curr = spi_threadhead; curr != nullptr; curr = next )
    {
      next = *((struct _THREAD **)curr + 2);
      v2 = GetTickCount() - startTime;
      timeout = 0;
      if ( v2 < 0x1F4 )
        timeout = 500 - v2;
      if ( WaitForSingleObject(hHandle: *((HANDLE *)curr + 1), dwMilliseconds: timeout) != 258 )
      {
        CloseHandle(hObject: *((HANDLE *)curr + 1));
        TListFree(a1: &spi_threadhead, a2: curr, a3: aSpiCpp_2, a4: 367);
      }
    }
    if ( spi_threadhead == nullptr )
      break;
    elapsed = GetTickCount() - startTime;
    if ( elapsed < 0x1F4 && tries < 4 )
      Sleep(dwMilliseconds: 500 - elapsed);
  }
  if ( spi_datasocket != -1 )
  {
    closesocket(s: spi_datasocket);
    spi_datasocket = -1;
  }
  CCritSect::Enter(this: &spi_packetcritsect);
  TListClear(a1: &spi_packethead, a2: aSpiCpp_3, a3: 392);
  TListClear(a1: &spi_gamehead, a2: aSpiCpp_4, a3: 395);
  WSACleanup();
  CCritSect::Leave(this: &spi_packetcritsect);
  IconDestroy();
  TOSDestroy();
  UiDestroy();
  CacheDestroy();
  ProfileDestroy();
  spi_shutdown = 0;
  UiDisconnect();
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19016471
 * CodeView symbol: SpiFree
 * Demangled: SpiFree
 * IDA name: ?SpiFree@@YGHPAU_SNETADDR@@PAXK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15471
 * Code length: 0x38
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiFree(struct _SNETADDR *addr, void *data, char databytes)
{
  if ( addr != nullptr && data != nullptr )
  {
    SMemFree(a1: addr, a2: aSpiCpp_5, a3: 427, a4: 0);
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x190164A9
 * CodeView symbol: SpiFreeExternalMessage
 * Demangled: SpiFreeExternalMessage
 * IDA name: ?SpiFreeExternalMessage@@YGHPBD00@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x154A9
 * Code length: 0x3E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiFreeExternalMessage(const char *senderpath, const char *sendername, const char *message)
{
  if ( senderpath != nullptr && sendername != nullptr && message != nullptr )
  {
    SMemFree(a1: senderpath, a2: aSpiCpp_6, a3: 439, a4: 0);
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x190164E7
 * CodeView symbol: SpiGetGameAddress
 * Demangled: SpiGetGameAddress
 * IDA name: ?SpiGetGameAddress@@YIHPAU_SNETADDR@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x154E7
 * Code length: 0x6C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SpiGetGameAddress(struct sockaddr *a1)
{
  int addrlen; // [esp+8h] [ebp-4h] BYREF

  memset(a1, Val: 0, Size: sizeof(struct sockaddr));
  if ( spi_datasocket == -1 )
    return 0;
  addrlen = 16;
  if ( getsockname(s: spi_datasocket, name: a1, namelen: &addrlen) == -1 )
    return 0;
  memset(a1: &a1->sa_data[6], Val: 0, Size: 8u);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19016553
 * CodeView symbol: SpiGetGameInfo
 * Demangled: SpiGetGameInfo
 * IDA name: ?SpiGetGameInfo@@YGHKPBD0PAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15553
 * Code length: 0x203
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiGetGameInfo(
        unsigned int gameid,
        const char *gamename,
        const char *gamepassword,
        struct _SNETSPI_GAMELIST *gameinfo)
{
  struct _SNETSPI_GAMELIST *i; // [esp+4h] [ebp-14h]
  struct _SNETSPI_GAMELIST *curr; // [esp+8h] [ebp-10h]
  int remove; // [esp+Ch] [ebp-Ch]
  struct _SNETSPI_GAMELIST *found; // [esp+10h] [ebp-8h]
  unsigned int errorcode; // [esp+14h] [ebp-4h] BYREF

  if ( gameinfo != nullptr )
    memset(a1: gameinfo, Val: 0, Size: 0x13Cu);
  if ( gamename != nullptr && gameinfo != nullptr && (gameid != 0 || *gamename != 0) )
  {
    CCritSect::Enter(this: &spi_extmsgcritsect);
    TListClear(a1: &spi_extmsghead, a2: aSpiCpp_7, a3: 474);
    CCritSect::Leave(this: &spi_extmsgcritsect);
    CCritSect::Enter(this: &spi_gamecritsect);
    found = nullptr;
    remove = 0;
    if ( gameid != 0 )
    {
      for ( curr = spi_gamehead; curr != nullptr; curr = *((struct _SNETSPI_GAMELIST **)curr + 74) )
      {
        if ( *(_DWORD *)curr == gameid )
        {
          found = curr;
          break;
        }
      }
    }
    errorcode = 1;
    if ( found == nullptr && gameid == 0 )
    {
      CCritSect::Leave(this: &spi_gamecritsect);
      SrvGetGameList(a1: gamename, a2: gamepassword, categorybits: 0, categorymask: 0, a5: 1u, a6: &errorcode);
      CCritSect::Enter(this: &spi_gamecritsect);
      for ( i = spi_gamehead; i != nullptr; i = *((struct _SNETSPI_GAMELIST **)i + 74) )
      {
        if ( _stricmp(String1: gamename, String2: (const char *)i + 40) == 0 )
        {
          found = i;
          remove = 1;
          break;
        }
      }
    }
    if ( found != nullptr )
      memcpy(a1: gameinfo, Src: found, Size: 0x13Cu);
    if ( remove != 0 )
      TListFree(a1: &spi_gamehead, a2: found, a3: aSpiCpp_8, a4: 517);
    CCritSect::Leave(this: &spi_gamecritsect);
    if ( found != nullptr )
    {
      return 1;
    }
    else
    {
      switch ( errorcode )
      {
        case 2u:
          SErrSetLastError(a1: 86);
          break;
        case 3u:
          SErrSetLastError(a1: -2062548889);
          break;
        case 4u:
          SErrSetLastError(a1: -2062548890);
          break;
        case 5u:
          SErrSetLastError(a1: -2062548862);
          break;
        default:
          SErrSetLastError(a1: -2062548888);
          break;
      }
      return 0;
    }
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19016756
 * CodeView symbol: SpiGetLocalPlayerName
 * Demangled: SpiGetLocalPlayerName
 * IDA name: ?SpiGetLocalPlayerName@@YGHPADK0K@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15756
 * Code length: 0x3D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiGetLocalPlayerName(char *namebuffer, unsigned int namechars, char *descbuffer, unsigned int descchars)
{
  if ( namebuffer != nullptr && namechars != 0 )
    SrvGetLocalPlayerName(a1: 0, a2: namebuffer, a3: namechars);
  if ( descbuffer != nullptr && descchars != 0 )
    SrvGetLocalPlayerDesc(a1: descbuffer, a2: descchars);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19016793
 * CodeView symbol: SpiGetPerformanceData
 * Demangled: SpiGetPerformanceData
 * IDA name: ?SpiGetPerformanceData@@YGHKPAKPAT_LARGE_INTEGER@@1@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15793
 * Code length: 0x82
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiGetPerformanceData(
        unsigned int counterid,
        unsigned int *countervalue,
        union _LARGE_INTEGER *measurementtime,
        union _LARGE_INTEGER *measurementfreq)
{
  int result; // eax

  switch ( counterid )
  {
    case 0xCu:
      *countervalue = spi_perfdata;
      result = 1;
      break;
    case 0xDu:
      *countervalue = dword_1904327C;
      result = 1;
      break;
    case 0xEu:
      *countervalue = dword_19043280;
      result = 1;
      break;
    case 0xFu:
      *countervalue = dword_19043284;
      result = 1;
      break;
    default:
      result = 0;
      break;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19016815
 * CodeView symbol: SpiInitialize
 * Demangled: SpiInitialize
 * IDA name: ?SpiInitialize@@YGHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAX@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15815
 * Code length: 0x196
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiInitialize(
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        void *event)
{
  unsigned int v6; // [esp+0h] [ebp-Ch]
  int lasterror; // [esp+4h] [ebp-8h]
  int v8; // [esp+8h] [ebp-4h]

  global_programid = *((_DWORD *)programdata + 3);
  global_versionid = *((_DWORD *)programdata + 4);
  if ( *((_DWORD *)programdata + 6) >= 0x100u )
    v6 = 256;
  else
    v6 = *((_DWORD *)programdata + 6);
  global_maxplayers = v6;
  spi_recvevent = event;
  memset(a1: &spi_perfdata, Val: 0, Size: 0x10u);
  CacheInitialize();
  UiInitialize(a1: interfacedata);
  LocaleInitialize(a1: programdata);
  if ( UiBeginConnect(a1: programdata, a2: interfacedata) == 0 )
  {
    SrvDestroy();
    SpiDestroy();
    SErrSetLastError(a1: 87);
  }
  lasterror = 0;
  UiProcessWindowMessages();
  if ( InitializeSockets() == 0 )
  {
    UiWSockErrMessage();
    lasterror = SErrGetLastError();
  }
  v8 = SrvConnectToServer();
  if ( v8 != 0 )
  {
    UiProcessWindowMessages();
    if ( InitializeGameDataSocket() == 0 )
    {
      v8 = 0;
      lasterror = SErrGetLastError();
      UiProcessWindowMessages();
    }
  }
  if ( v8 != 0 )
  {
    UiProcessWindowMessages();
    if ( SrvInitialize(a1: programdata, a2: playerdata, a3: versiondata) == 0 )
    {
      v8 = 0;
      lasterror = SErrGetLastError();
      UiProcessWindowMessages();
    }
  }
  if ( v8 != 0 )
  {
    ProfileInitialize(a1: *((const char **)interfacedata + 16));
    TOSInitialize();
    if ( UiLogon(a1: programdata, a2: playerdata, interfacedata, versiondata) == 0 )
    {
      lasterror = SErrGetLastError();
      v8 = 0;
      UiEndConnect(a1: 0);
    }
  }
  else
  {
    UiEndConnect(a1: 0);
  }
  if ( v8 != 0 )
    return 1;
  SpiDestroy();
  SErrSetLastError(a1: lasterror);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x190169AB
 * CodeView symbol: InitializeSockets
 * Demangled: InitializeSockets
 * IDA name: InitializeSockets
 * CodeView module: 7
 * CV address: segment 1 : 0x159AB
 * Code length: 0x3F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int InitializeSockets()
{
  struct WSAData data; // [esp+0h] [ebp-190h] BYREF

  InitRASManager();
  if ( WSAStartup(wVersionRequested: 0x101u, lpWSAData: &data) == 0 )
    return 1;
  SpiDestroy();
  SErrSetLastError(a1: 1222);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x190169EA
 * CodeView symbol: InitializeGameDataSocket
 * Demangled: InitializeGameDataSocket
 * IDA name: InitializeGameDataSocket
 * CodeView module: 7
 * CV address: segment 1 : 0x159EA
 * Code length: 0x15C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int InitializeGameDataSocket()
{
  signed int Version; // eax
  unsigned int thread; // [esp+0h] [ebp-3Ch] BYREF
  HANDLE hThread; // [esp+4h] [ebp-38h]
  int threads; // [esp+Ch] [ebp-30h]
  unsigned int win95; // [esp+10h] [ebp-2Ch]
  int loop; // [esp+14h] [ebp-28h]
  struct sockaddr addr; // [esp+18h] [ebp-24h] BYREF
  _DWORD srvaddr[4]; // [esp+28h] [ebp-14h] BYREF
  int dataport; // [esp+38h] [ebp-4h] BYREF

  spi_datasocket = socket(af: 2, type: 2, protocol: 0);
  if ( spi_datasocket == -1 )
  {
    SpiDestroy();
    SErrSetLastError(a1: 1222);
    return 0;
  }
  else
  {
    dataport = 6112;
    SRegLoadValue(a1: aConfiguration_12, a2: aGameDataPort, a3: 2, a4: &dataport);
    if ( SrvGetLocalAddress(a1: (struct _SNETADDR *)srvaddr) != 0 )
    {
      memset(a1: &addr, Val: 0, Size: sizeof(addr));
      addr.sa_family = 2;
      *(_DWORD *)&addr.sa_data[2] = srvaddr[1];
      *(_WORD *)addr.sa_data = htons(hostshort: dataport);
      if ( bind(s: spi_datasocket, name: &addr, namelen: 16) != 0 )
      {
        SpiDestroy();
        SErrSetLastError(a1: 1222);
        return 0;
      }
      else
      {
        Version = GetVersion();
        win95 = Version & 0x80000000;
        threads = 2 - (Version < 0);
        for ( loop = 0; loop < threads; ++loop )
        {
          hThread = (HANDLE)_beginthreadex(
                              Security: nullptr,
                              StackSize: 0,
                              StartAddress: RecvDataThreadProc,
                              ArgList: nullptr,
                              InitFlag: 0,
                              ThrdAddr: &thread);
          if ( hThread == nullptr )
          {
            SErrSetLastError(a1: 8);
            return 0;
          }
          SetThreadPriority(hThread, nPriority: 2);
          TListAdd(a1: &spi_threadhead, a2: (struct _THREAD *)&thread, a3: aSpiCpp_9, a4: 138);
        }
        return 1;
      }
    }
    else
    {
      return 0;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19016B46
 * CodeView symbol: RecvDataThreadProc
 * Demangled: RecvDataThreadProc
 * IDA name: RecvDataThreadProc
 * CodeView module: 7
 * CV address: segment 1 : 0x15B46
 * Code length: 0x29E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall __noreturn RecvDataThreadProc(void *param)
{
  void *v1; // eax
  int v2; // [esp+4h] [ebp-20h]
  int v3; // [esp+8h] [ebp-1Ch]
  int v4; // [esp+Ch] [ebp-18h]
  char *pkt; // [esp+18h] [ebp-Ch]
  int addrsize; // [esp+1Ch] [ebp-8h] BYREF
  int bytesread; // [esp+20h] [ebp-4h]

  while ( spi_datasocket != -1 && spi_shutdown == 0 )
  {
    v1 = (void *)SMemAlloc(a1: 540, a2: aSpiCpp_10, a3: 156, a4: 0);
    pkt = (char *)operator new(__formal: 28, ptr: v1);
    addrsize = 16;
    bytesread = recvfrom(
                  s: spi_datasocket,
                  buf: pkt + 16,
                  len: 516,
                  flags: 0,
                  from: (struct sockaddr *)pkt,
                  fromlen: &addrsize);
    if ( spi_shutdown != 0 )
    {
      if ( !IsBadWritePtr(lp: pkt, ucb: 0x21Cu) )
        operator delete(ptr: pkt);
      _endthreadex(ReturnCode: 0);
    }
    if ( (unsigned int)bytesread < 4 )
      v4 = 0;
    else
      v4 = bytesread - 4;
    *((_DWORD *)pkt + 133) = v4;
    if ( addrsize <= 8 )
      v3 = addrsize;
    else
      v3 = 8;
    if ( addrsize <= 8 )
      v2 = addrsize;
    else
      v2 = 8;
    memset(a1: &pkt[v2], Val: 0, Size: 16 - v3);
    if ( bytesread < 0 )
    {
      SMemFree(a1: pkt, a2: aSpiCpp_11, a3: 184, a4: 0);
      _endthreadex(ReturnCode: 0);
    }
    ++dword_1904327C;
    dword_19043284 += bytesread + 39;
    switch ( *((_DWORD *)pkt + 4) )
    {
      case 0:
        CCritSect::Enter(this: &spi_packetcritsect);
        spi_lastgamepacket = GetTickCount();
        TListAddPtrEnd(a1: &spi_packethead, a2: (struct _PACKET *)pkt);
        CCritSect::Leave(this: &spi_packetcritsect);
        SetEvent(hEvent: spi_recvevent);
        break;
      case 3:
        SrvProcessClientReq(a1: (struct _SNETADDR *)pkt, a2: (unsigned __int8 *)pkt + 20, a3: *((_DWORD *)pkt + 133));
        SMemFree(a1: pkt, a2: aSpiCpp_12, a3: 206, a4: 0);
        break;
      case 5:
        SrvProcessServerPing(a1: (unsigned __int8 *)pkt + 20, a2: *((_DWORD *)pkt + 133));
        SMemFree(a1: pkt, a2: aSpiCpp_13, a3: 212, a4: 0);
        break;
      case 6:
        if ( *((_DWORD *)pkt + 133) == 4 )
          spi_latencytoself = GetTickCount() - *((_DWORD *)pkt + 5);
        SMemFree(a1: pkt, a2: aSpiCpp_14, a3: 218, a4: 0);
        break;
      default:
        SMemFree(a1: pkt, a2: aSpiCpp_15, a3: 222, a4: 0);
        break;
    }
  }
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x19016DE4
 * CodeView symbol: SpiInitializeDevice
 * Demangled: SpiInitializeDevice
 * IDA name: ?SpiInitializeDevice@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15DE4
 * Code length: 0x9
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiInitializeDevice(
        char deviceid,
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata)
{
  return 0;
}


// ==============================================================================
/*
 * EA: 0x19016DED
 * CodeView symbol: SpiLockDeviceList
 * Demangled: SpiLockDeviceList
 * IDA name: ?SpiLockDeviceList@@YGHPAPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15DED
 * Code length: 0x15
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiLockDeviceList(struct _SNETSPI_DEVICELIST **devicelist)
{
  *devicelist = nullptr;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19016E02
 * CodeView symbol: SpiLockGameList
 * Demangled: SpiLockGameList
 * IDA name: ?SpiLockGameList@@YGHKKPAPAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15E02
 * Code length: 0x150
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiLockGameList(
        unsigned int categorybits,
        unsigned int categorymask,
        struct _SNETSPI_GAMELIST **gamelist)
{
  struct _SNETSPI_GAMELIST *curr; // [esp+4h] [ebp-A8h]
  char lastgame[125]; // [esp+8h] [ebp-A4h] BYREF
  __int16 v6; // [esp+85h] [ebp-27h]
  char v7; // [esp+87h] [ebp-25h]
  unsigned int errorcode; // [esp+88h] [ebp-24h] BYREF
  CHAR progvers[32]; // [esp+8Ch] [ebp-20h] BYREF

  if ( gamelist != nullptr )
  {
    CCritSect::Enter(this: &spi_extmsgcritsect);
    TListClear(a1: &spi_extmsghead, a2: aSpiCpp_16, a3: 717);
    CCritSect::Leave(this: &spi_extmsgcritsect);
    TListClear(a1: &spi_gamehead, a2: aSpiCpp_17, a3: 721);
    _wsprintfA(a1: progvers, a2: "%08x%08x", global_programid, global_versionid);
    lastgame[0] = byte_19043298;
    memset(&lastgame[1], 0, 0x7Cu);
    v6 = 0;
    v7 = 0;
    SRegLoadString(a1: aRecentGames, a2: progvers, a3: 2, a4: lastgame, a5: 128);
    if ( lastgame[0] != 0 )
      SrvGetGameList(a1: lastgame, a2: nullptr, categorybits, categorymask, a5: 1u, a6: &errorcode);
    SrvGetGameList(a1: nullptr, a2: nullptr, categorybits, categorymask, a5: 0x19u, a6: &errorcode);
    CCritSect::Enter(this: &spi_gamecritsect);
    for ( curr = spi_gamehead; curr != nullptr; curr = *((struct _SNETSPI_GAMELIST **)curr + 74) )
      SrvGetLatency(a1: (struct _SNETSPI_GAMELIST *)((char *)curr + 12), a2: (unsigned int *)curr + 7);
    *gamelist = spi_gamehead;
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19016F52
 * CodeView symbol: SpiQueueExternalMessage
 * Demangled: SpiQueueExternalMessage
 * IDA name: ?SpiQueueExternalMessage@@YIXPBD00@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x15F52
 * Code length: 0xD8
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SpiQueueExternalMessage(const char *a1, const char *a2, const char *message)
{
  void *v3; // eax
  char *newptr; // [esp+Ch] [ebp-4h]

  CCritSect::Enter(this: &spi_extmsgcritsect);
  v3 = (void *)SMemAlloc(a1: 772, a2: aSpiCpp_18, a3: 754, a4: 0);
  newptr = (char *)operator new(__formal: 4, ptr: v3);
  if ( newptr != nullptr )
  {
    strncpy(Destination: newptr, Source: a1, Count: 0x80u);
    strncpy(Destination: newptr + 128, Source: a2, Count: 0x80u);
    strncpy(Destination: newptr + 256, Source: message, Count: 0x200u);
    newptr[127] = 0;
    newptr[255] = 0;
    newptr[767] = 0;
    TListAddPtrEnd(a1: &spi_extmsghead, a2: (struct _EXTMSG *)newptr);
  }
  CCritSect::Leave(this: &spi_extmsgcritsect);
  SetEvent(hEvent: spi_recvevent);
}


// ==============================================================================
/*
 * EA: 0x1901702A
 * CodeView symbol: SpiReceive
 * Demangled: SpiReceive
 * IDA name: ?SpiReceive@@YGHPAPAU_SNETADDR@@PAPAXPAK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x1602A
 * Code length: 0xC6
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiReceive(struct _SNETADDR **addr, void **data, unsigned int *databytes)
{
  if ( addr != nullptr )
    *addr = nullptr;
  if ( data != nullptr )
    *data = nullptr;
  if ( databytes != nullptr )
    *databytes = 0;
  if ( addr != nullptr && data != nullptr && databytes != nullptr && spi_datasocket != -1 )
  {
    if ( spi_packethead != nullptr )
    {
      CCritSect::Enter(this: &spi_packetcritsect);
      *addr = spi_packethead;
      *data = (char *)spi_packethead + 20;
      *databytes = *((_DWORD *)spi_packethead + 133);
      spi_packethead = *((struct _PACKET **)spi_packethead + 134);
      CCritSect::Leave(this: &spi_packetcritsect);
      return 1;
    }
    else
    {
      SErrSetLastError(a1: -2062548885);
      return 0;
    }
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x190170F0
 * CodeView symbol: SpiReceiveExternalMessage
 * Demangled: SpiReceiveExternalMessage
 * IDA name: ?SpiReceiveExternalMessage@@YGHPAPBD00@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x160F0
 * Code length: 0xBF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiReceiveExternalMessage(const char **senderpath, const char **sendername, const char **message)
{
  if ( senderpath != nullptr )
    *senderpath = nullptr;
  if ( sendername != nullptr )
    *sendername = nullptr;
  if ( message != nullptr )
    *message = nullptr;
  if ( senderpath != nullptr && sendername != nullptr && message != nullptr )
  {
    if ( spi_extmsghead != nullptr )
    {
      CCritSect::Enter(this: &spi_extmsgcritsect);
      *senderpath = (const char *)spi_extmsghead;
      *sendername = (char *)spi_extmsghead + 128;
      *message = (char *)spi_extmsghead + 256;
      spi_extmsghead = *((struct _EXTMSG **)spi_extmsghead + 192);
      CCritSect::Leave(this: &spi_extmsgcritsect);
      return 1;
    }
    else
    {
      SErrSetLastError(a1: -2062548885);
      return 0;
    }
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x190171AF
 * CodeView symbol: SpiReportGameResult
 * Demangled: SpiReportGameResult
 * IDA name: ?SpiReportGameResult@@YGHKKPAPBDPAKPBD2@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x161AF
 * Code length: 0x22
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiReportGameResult(
        unsigned int ladderid,
        unsigned int arraysize,
        const char **namearray,
        unsigned int *resultarray,
        const char *textgameresult,
        const char *textplayerresult)
{
  return SrvSendGameResult(
           a1: ladderid,
           a2: arraysize,
           namearray,
           resultarray,
           a5: textgameresult,
           a6: textplayerresult);
}


// ==============================================================================
/*
 * EA: 0x190171D1
 * CodeView symbol: SpiSelectGame
 * Demangled: SpiSelectGame
 * IDA name: ?SpiSelectGame@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x161D1
 * Code length: 0x22
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiSelectGame(
        unsigned int flags,
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        unsigned int *playerid)
{
  return UiSelectGame(a1: flags, a2: programdata, playerdata, interfacedata, a5: versiondata, a6: playerid);
}


// ==============================================================================
/*
 * EA: 0x190171F3
 * CodeView symbol: SpiSend
 * Demangled: SpiSend
 * IDA name: ?SpiSend@@YGHKPAPAU_SNETADDR@@PAXK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x161F3
 * Code length: 0x112
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiSend(unsigned int addresses, struct _SNETADDR **addrlist, void *data, size_t databytes)
{
  unsigned int v5; // eax
  char addr[16]; // [esp+0h] [ebp-230h] BYREF
  char buf[4]; // [esp+20h] [ebp-210h] BYREF
  _BYTE v8[520]; // [esp+24h] [ebp-20Ch] BYREF
  DWORD currtime; // [esp+22Ch] [ebp-4h] BYREF

  if ( addresses != 0
    && addrlist != nullptr
    && data != nullptr
    && databytes != 0
    && databytes <= 0x200
    && spi_datasocket != -1 )
  {
    currtime = GetTickCount();
    if ( currtime - spi_lastgamepacket > 0x3E8 )
    {
      spi_lastgamepacket = currtime;
      if ( SrvGetLocalAddress(a1: (struct _SNETADDR *)addr) != 0 )
        SpiSendSpecial(a1: (struct _SNETADDR *)addr, a2: 6u, data: &currtime, a4: 4u, a5: 1u);
    }
    *(_DWORD *)buf = 0;
    memcpy(a1: v8, Src: data, Size: databytes);
    while ( 1 )
    {
      v5 = addresses--;
      if ( v5 == 0 )
        break;
      sendto(
        s: spi_datasocket,
        buf,
        len: databytes + 4,
        flags: 0,
        to: (const struct sockaddr *)addrlist[addresses],
        tolen: 16);
      ++spi_perfdata;
      dword_19043280 += databytes + 43;
    }
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19017305
 * CodeView symbol: SpiSendExternalMessage
 * Demangled: SpiSendExternalMessage
 * IDA name: ?SpiSendExternalMessage@@YGHPBD0000@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x16305
 * Code length: 0x28
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiSendExternalMessage(
        const char *senderpath,
        const char *sendername,
        const char *targetpath,
        const char *targetname,
        const char *message)
{
  if ( *targetpath == 0 && *targetname == 0 )
    SrvSendChatString(a1: message);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901732D
 * CodeView symbol: SpiSendSpecial
 * Demangled: SpiSendSpecial
 * IDA name: ?SpiSendSpecial@@YIHPAU_SNETADDR@@KPAXKK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x1632D
 * Code length: 0xD9
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SpiSendSpecial(
        const struct sockaddr *a1,
        unsigned int a2,
        void *data,
        size_t databytes,
        unsigned int count)
{
  char buf[4]; // [esp+18h] [ebp-210h] BYREF
  _BYTE v8[520]; // [esp+1Ch] [ebp-20Ch] BYREF
  unsigned int i; // [esp+224h] [ebp-4h]

  if ( a1 != nullptr && data != nullptr && databytes != 0 && databytes <= 0x200 && spi_datasocket != -1 )
  {
    *(_DWORD *)buf = a2;
    memcpy(a1: v8, Src: data, Size: databytes);
    for ( i = 0; i < count; ++i )
    {
      sendto(s: spi_datasocket, buf, len: databytes + 4, flags: 0, to: a1, tolen: 16);
      ++spi_perfdata;
      dword_19043280 += databytes + 43;
    }
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19017406
 * CodeView symbol: SpiStartAdvertisingGame
 * Demangled: SpiStartAdvertisingGame
 * IDA name: ?SpiStartAdvertisingGame@@YGHPBD00KKKKKPBXK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x16406
 * Code length: 0x65
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __stdcall SpiStartAdvertisingGame(
        const char *gamename,
        const char *gamepassword,
        const char *gamedescription,
        unsigned int gamemode,
        unsigned int gameage,
        unsigned int gamecategorybits,
        unsigned int optcategorybits,
        unsigned int ladderid,
        const void *clientdata,
        char clientdatabytes)
{
  if ( gamename != nullptr && gamedescription != nullptr )
  {
    if ( SrvIsConnected() != 0 )
    {
      return SrvStartAdvertisingGame(
               a1: gamename,
               a2: gamepassword,
               gamedescription,
               gamemode,
               gameage,
               gamecategorybits,
               a7: optcategorybits,
               a8: ladderid) != 0;
    }
    else
    {
      SErrSetLastError(a1: 2250);
      return false;
    }
  }
  else
  {
    SErrSetLastError(a1: 87);
    return false;
  }
}


// ==============================================================================
/*
 * EA: 0x1901746B
 * CodeView symbol: SpiStopAdvertisingGame
 * Demangled: SpiStopAdvertisingGame
 * IDA name: ?SpiStopAdvertisingGame@@YGHXZ
 * CodeView module: 7
 * CV address: segment 1 : 0x1646B
 * Code length: 0x26
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiStopAdvertisingGame()
{
  if ( SrvIsConnected() != 0 )
  {
    SrvStopAdvertisingGame();
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 2250);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19017491
 * CodeView symbol: SpiUnlockDeviceList
 * Demangled: SpiUnlockDeviceList
 * IDA name: ?SpiUnlockDeviceList@@YGHPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x16491
 * Code length: 0xC
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiUnlockDeviceList(struct _SNETSPI_DEVICELIST *devicelist)
{
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901749D
 * CodeView symbol: SpiUnlockGameList
 * Demangled: SpiUnlockGameList
 * IDA name: ?SpiUnlockGameList@@YGHPAU_SNETSPI_GAMELIST@@PAK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x1649D
 * Code length: 0x25
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SpiUnlockGameList(struct _SNETSPI_GAMELIST *gamelist, unsigned int *hintnextcall)
{
  CCritSect::Leave(this: &spi_gamecritsect);
  if ( hintnextcall != nullptr )
    *hintnextcall = 1000;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x190174D0
 * CodeView symbol: TListAdd
 * Demangled: TListAdd
 * IDA name: ?TListAdd@@YIHPAPAU_THREAD@@PAU1@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x164D0
 * Code length: 0x6A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAdd(struct _THREAD **a1, struct _THREAD *a2, const char *filename, int linenumber)
{
  _DWORD *newptr; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  newptr = (_DWORD *)SMemAlloc(a1: 12, a2: filename, a3: linenumber, a4: 0);
  if ( newptr == nullptr )
    return 0;
  memcpy(a1: newptr, Src: a2, Size: 0xCu);
  newptr[2] = *a1;
  *a1 = (struct _THREAD *)newptr;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19017540
 * CodeView symbol: TListAddPtrEnd
 * Demangled: TListAddPtrEnd
 * IDA name: ?TListAddPtrEnd@@YIHPAPAU_PACKET@@PAU1@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x16540
 * Code length: 0x58
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAddPtrEnd(struct _PACKET **a1, struct _PACKET *a2)
{
  struct _PACKET **next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  *((_DWORD *)a2 + 134) = 0;
  for ( next = a1; *next != nullptr; next = (struct _PACKET **)((char *)*next + 536) )
    ;
  *next = a2;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x190175A0
 * CodeView symbol: TListClear
 * Demangled: TListClear
 * IDA name: ?TListClear@@YIHPAPAU_EXTMSG@@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x165A0
 * Code length: 0x56
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListClear(struct _EXTMSG **a1, const char *a2, int linenumber)
{
  struct _EXTMSG *next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr )
    return 0;
  while ( *a1 != nullptr )
  {
    next = *((struct _EXTMSG **)*a1 + 192);
    SMemFree(a1: *a1, a2, a3: linenumber, a4: 0);
    *a1 = next;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19017600
 * CodeView symbol: TListFree
 * Demangled: TListFree
 * IDA name: ?TListFree@@YIHPAPAU_THREAD@@PAU1@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x16600
 * Code length: 0x7A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall TListFree(struct _THREAD **a1, struct _THREAD *a2, const char *filename, int linenumber)
{
  struct _THREAD **next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return false;
  for ( next = a1; *next != nullptr && *next != a2; next = (struct _THREAD **)((char *)*next + 8) )
    ;
  if ( *next != nullptr )
    *next = *((struct _THREAD **)*next + 2);
  SMemFree(a1: a2, a2: filename, a3: linenumber, a4: 0);
  return *next != nullptr;
}


// ==============================================================================
/*
 * EA: 0x19017680
 * CodeView symbol: TListClear
 * Demangled: TListClear
 * IDA name: ?TListClear@@YIHPAPAU_PACKET@@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x16680
 * Code length: 0x56
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListClear(struct _PACKET **a1, const char *a2, int linenumber)
{
  struct _PACKET *next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr )
    return 0;
  while ( *a1 != nullptr )
  {
    next = *((struct _PACKET **)*a1 + 134);
    SMemFree(a1: *a1, a2, a3: linenumber, a4: 0);
    *a1 = next;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x190176E0
 * CodeView symbol: TListClear
 * Demangled: TListClear
 * IDA name: ?TListClear@@YIHPAPAU_SNETSPI_GAMELIST@@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x166E0
 * Code length: 0x56
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListClear(struct _SNETSPI_GAMELIST **a1, const char *a2, int linenumber)
{
  struct _SNETSPI_GAMELIST *next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr )
    return 0;
  while ( *a1 != nullptr )
  {
    next = *((struct _SNETSPI_GAMELIST **)*a1 + 74);
    SMemFree(a1: *a1, a2, a3: linenumber, a4: 0);
    *a1 = next;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19017740
 * CodeView symbol: TListFree
 * Demangled: TListFree
 * IDA name: ?TListFree@@YIHPAPAU_SNETSPI_GAMELIST@@PAU1@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x16740
 * Code length: 0x80
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall TListFree(
        struct _SNETSPI_GAMELIST **a1,
        struct _SNETSPI_GAMELIST *a2,
        const char *filename,
        int linenumber)
{
  struct _SNETSPI_GAMELIST **next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return false;
  for ( next = a1; *next != nullptr && *next != a2; next = (struct _SNETSPI_GAMELIST **)((char *)*next + 296) )
    ;
  if ( *next != nullptr )
    *next = *((struct _SNETSPI_GAMELIST **)*next + 74);
  SMemFree(a1: a2, a2: filename, a3: linenumber, a4: 0);
  return *next != nullptr;
}


// ==============================================================================
/*
 * EA: 0x190177C0
 * CodeView symbol: TListAddPtrEnd
 * Demangled: TListAddPtrEnd
 * IDA name: ?TListAddPtrEnd@@YIHPAPAU_EXTMSG@@PAU1@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x167C0
 * Code length: 0x58
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Spi.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Spi.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAddPtrEnd(struct _EXTMSG **a1, struct _EXTMSG *a2)
{
  struct _EXTMSG **next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  *((_DWORD *)a2 + 192) = 0;
  for ( next = a1; *next != nullptr; next = (struct _EXTMSG **)((char *)*next + 768) )
    ;
  *next = a2;
  return 1;
}

