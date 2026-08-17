/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 *
 * CodeView module object:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 46
 */


// ==============================================================================
/*
 * EA: 0x180010D0
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: unknown_libname_2
 * CodeView module: 10
 * CV address: segment 1 : 0xD0
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_2()
{
  sub_180010DF();
  return sub_180010EE();
}


// ==============================================================================
/*
 * EA: 0x180010DF
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: sub_180010DF
 * CodeView module: 10
 * CV address: segment 1 : 0xDF
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *sub_180010DF()
{
  return CCritSect::CCritSect(this: (CCritSect *)&ipx_critsect);
}


// ==============================================================================
/*
 * EA: 0x180010EE
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: sub_180010EE
 * CodeView module: 10
 * CV address: segment 1 : 0xEE
 * Code length: 0x12
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_180010EE()
{
  return atexit(a1: sub_18001100);
}


// ==============================================================================
/*
 * EA: 0x18001100
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: sub_18001100
 * CodeView module: 10
 * CV address: segment 1 : 0x100
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_18001100()
{
  CCritSect::~CCritSect(this: (CCritSect *)&ipx_critsect);
}


// ==============================================================================
/*
 * EA: 0x1800110F
 * CodeView symbol: ComputeChecksum
 * Demangled: ComputeChecksum
 * IDA name: ?ComputeChecksum@@YIGPAXK@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x10F
 * Code length: 0xA2
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
unsigned int __fastcall ComputeChecksum(char *a1, unsigned int a2)
{
  unsigned int v2; // eax
  unsigned int databytes; // [esp+0h] [ebp-14h]
  unsigned int checkval2; // [esp+8h] [ebp-Ch]
  unsigned int checkval1; // [esp+Ch] [ebp-8h]
  char *ptr; // [esp+10h] [ebp-4h]

  databytes = a2;
  checkval1 = 0;
  checkval2 = 0;
  for ( ptr = &a1[a2 - 1]; ; --ptr )
  {
    v2 = databytes--;
    if ( v2 == 0 )
      break;
    checkval1 += (unsigned __int8)*ptr;
    if ( checkval1 >= 0xFF )
      checkval1 -= 255;
    checkval2 += checkval1;
  }
  return ((unsigned __int8)checkval1 << 8) | (checkval2 % 0xFF);
}


// ==============================================================================
/*
 * EA: 0x180011B1
 * CodeView symbol: IpxCompareNetAddresses
 * Demangled: IpxCompareNetAddresses
 * IDA name: ?IpxCompareNetAddresses@@YGHPAU_SNETADDR@@0PAK@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x1B1
 * Code length: 0x8D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxCompareNetAddresses(struct _SNETADDR *addr1, struct _SNETADDR *addr2, unsigned int *diffmagnitude)
{
  if ( diffmagnitude != nullptr )
    *diffmagnitude = 0;
  if ( addr1 != nullptr && addr2 != nullptr && diffmagnitude != nullptr )
  {
    if ( *(_DWORD *)((char *)addr1 + 2) == *(_DWORD *)((char *)addr2 + 2) )
      *diffmagnitude = memcmp(Buf1: addr1, Buf2: addr2, Size: 0xEu) != 0;
    else
      *diffmagnitude = 2;
    return 1;
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1800123E
 * CodeView symbol: IpxDestroy
 * Demangled: IpxDestroy
 * IDA name: ?IpxDestroy@@YGHXZ
 * CodeView module: 10
 * CV address: segment 1 : 0x23E
 * Code length: 0x35E
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxDestroy()
{
  struct _SNETSPI_GAMELIST *v1; // [esp+0h] [ebp-3Ch]
  DWORD timeout; // [esp+4h] [ebp-38h]
  unsigned int v3; // [esp+8h] [ebp-34h]
  int loop; // [esp+Ch] [ebp-30h]
  _BYTE buffer[8]; // [esp+10h] [ebp-2Ch] BYREF
  _WORD sendaddr[8]; // [esp+18h] [ebp-24h] BYREF
  struct _THREAD *next; // [esp+28h] [ebp-14h]
  DWORD startTime; // [esp+2Ch] [ebp-10h]
  struct _THREAD *curr; // [esp+30h] [ebp-Ch]
  unsigned int elapsed; // [esp+34h] [ebp-8h]
  unsigned int tries; // [esp+38h] [ebp-4h]

  ipx_shutdown = 1;
  for ( tries = 0; tries < 5; ++tries )
  {
    memcpy(a1: sendaddr, Src: &ipx_broadcastaddr, Size: 0xEu);
    buffer[0] = 8;
    buffer[1] = -17;
    buffer[2] = 8;
    buffer[3] = 0;
    buffer[4] = 0;
    buffer[5] = 0;
    buffer[6] = 0;
    buffer[7] = 0;
    if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 8) != 0 )
      sendaddr[6] = (*(int (__stdcall **)(int))(ipx_winsockapi + 8))(a1: 6111);
    if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 16) != 0 )
      (*(void (__stdcall **)(int, _BYTE *, int, _DWORD, _WORD *, int))(ipx_winsockapi + 16))(
        a1: ipx_advsocket,
        a2: buffer,
        a3: 8,
        a4: 0,
        a5: sendaddr,
        a6: 14);
    if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 8) != 0 )
      sendaddr[6] = (*(int (__stdcall **)(int))(ipx_winsockapi + 8))(a1: 6112);
    if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 16) != 0 )
    {
      for ( loop = 0; loop < 2; ++loop )
        (*(void (__stdcall **)(int, _BYTE *, int, _DWORD, _WORD *, int))(ipx_winsockapi + 16))(
          a1: ipx_mainsocket,
          a2: buffer,
          a3: 8,
          a4: 0,
          a5: sendaddr,
          a6: 14);
    }
    startTime = GetTickCount();
    for ( curr = ipx_threadhead; curr != nullptr; curr = next )
    {
      next = *((struct _THREAD **)curr + 2);
      v3 = GetTickCount() - startTime;
      timeout = 0;
      if ( v3 < 0x1F4 )
        timeout = 500 - v3;
      if ( WaitForSingleObject(hHandle: *((HANDLE *)curr + 1), dwMilliseconds: timeout) != 258 )
      {
        CloseHandle(hObject: *((HANDLE *)curr + 1));
        TListFree(a1: &ipx_threadhead, a2: curr, a3: aIpxCpp, a4: 832);
      }
    }
    if ( ipx_threadhead == nullptr )
      break;
    elapsed = GetTickCount() - startTime;
    if ( elapsed < 0x1F4 && tries < 4 )
      Sleep(dwMilliseconds: 500 - elapsed);
  }
  if ( ipx_mainsocket != 0 )
  {
    if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 4) != 0 )
      (*(void (__stdcall **)(int))(ipx_winsockapi + 4))(a1: ipx_mainsocket);
    ipx_mainsocket = 0;
  }
  if ( ipx_advsocket != 0 )
  {
    if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 4) != 0 )
      (*(void (__stdcall **)(int))(ipx_winsockapi + 4))(a1: ipx_advsocket);
    ipx_advsocket = 0;
  }
  CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
  IpxStopAdvertisingGame();
  while ( 1 )
  {
    v1 = ipx_gamehead;
    if ( ipx_gamehead == nullptr )
      break;
    if ( *((_DWORD *)ipx_gamehead + 75) != 0 )
      SMemFree(a1: *((_DWORD *)ipx_gamehead + 75), a2: aIpxCpp_0, a3: 867, a4: 0);
    TListFree(a1: &ipx_gamehead, a2: v1, a3: aIpxCpp_1, a4: 868);
  }
  TListClear(a1: &ipx_packethead, a2: aIpxCpp_2, a3: 873);
  if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 32) != 0 )
    (*(void (**)(void))(ipx_winsockapi + 32))();
  if ( ipx_winsockapi != 0 )
  {
    SMemFree(a1: ipx_winsockapi, a2: aIpxCpp_3, a3: 886, a4: 0);
    ipx_winsockapi = 0;
  }
  if ( ipx_winsocklib != nullptr )
  {
    FreeLibrary(hLibModule: ipx_winsocklib);
    ipx_winsocklib = nullptr;
  }
  CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
  ipx_shutdown = 0;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800159C
 * CodeView symbol: IpxFree
 * Demangled: IpxFree
 * IDA name: ?IpxFree@@YGHPAU_SNETADDR@@PAXK@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x59C
 * Code length: 0x39
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxFree(struct _SNETADDR *addr, void *data, char databytes)
{
  if ( addr != nullptr && data != nullptr )
  {
    SMemFree(a1: addr, a2: aIpxCpp_4, a3: 912, a4: 0);
    return 1;
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x180015D5
 * CodeView symbol: IpxFreeExternalMessage
 * Demangled: IpxFreeExternalMessage
 * IDA name: ?IpxFreeExternalMessage@@YGHPBD00@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x5D5
 * Code length: 0x11
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxFreeExternalMessage(const char *senderpath, const char *sendername, const char *mesage)
{
  SetLastError(dwErrCode: 0x57u);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x180015E6
 * CodeView symbol: IpxGetGameInfo
 * Demangled: IpxGetGameInfo
 * IDA name: ?IpxGetGameInfo@@YGHKPBD0PAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x5E6
 * Code length: 0xE2
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxGetGameInfo(
        unsigned int gameid,
        const char *gamename,
        const char *gamepassword,
        struct _SNETSPI_GAMELIST *gameinfo)
{
  struct _SNETSPI_GAMELIST *curr; // [esp+0h] [ebp-4h]

  if ( gameinfo != nullptr )
    memset(a1: gameinfo, Val: 0, Size: 0x13Cu);
  if ( gamename != nullptr && gameinfo != nullptr && (gameid != 0 || *gamename != 0) )
  {
    CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
    for ( curr = ipx_gamehead; curr != nullptr; curr = *((struct _SNETSPI_GAMELIST **)curr + 74) )
    {
      if ( (gameid == 0 || gameid == *(_DWORD *)curr)
        && (*gamename == 0 || _stricmp(String1: gamename, String2: (const char *)curr + 40) == 0) )
      {
        memcpy(a1: gameinfo, Src: curr, Size: 0x13Cu);
        break;
      }
    }
    CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
    if ( *(_DWORD *)gameinfo != 0 )
    {
      return 1;
    }
    else
    {
      SetLastError(dwErrCode: 0x85100068);
      return 0;
    }
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x180016C8
 * CodeView symbol: IpxGetPerformanceData
 * Demangled: IpxGetPerformanceData
 * IDA name: ?IpxGetPerformanceData@@YGHKPAKPAT_LARGE_INTEGER@@1@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x6C8
 * Code length: 0x1A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxGetPerformanceData(
        unsigned int counterid,
        unsigned int *countervalue,
        union _LARGE_INTEGER *measurementtime,
        union _LARGE_INTEGER *measurementfreq)
{
  return PerfGetPerformanceData(a1: counterid, a2: countervalue, measurementtime, measurementfreq);
}


// ==============================================================================
/*
 * EA: 0x180016E2
 * CodeView symbol: IpxInitialize
 * Demangled: IpxInitialize
 * IDA name: ?IpxInitialize@@YGHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAX@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x6E2
 * Code length: 0x377
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxInitialize(
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        void *event)
{
  signed int Version; // eax
  int v7; // [esp+0h] [ebp-1D0h]
  unsigned int ThrdAddr; // [esp+4h] [ebp-1CCh] BYREF
  HANDLE v9; // [esp+8h] [ebp-1C8h]
  int threads; // [esp+10h] [ebp-1C0h]
  unsigned int win95; // [esp+14h] [ebp-1BCh]
  void *ArgList; // [esp+18h] [ebp-1B8h]
  unsigned int thread; // [esp+1Ch] [ebp-1B4h] BYREF
  HANDLE hThread; // [esp+20h] [ebp-1B0h]
  int value; // [esp+28h] [ebp-1A8h] BYREF
  _WORD advaddr[8]; // [esp+2Ch] [ebp-1A4h] BYREF
  int loop; // [esp+3Ch] [ebp-194h]
  _BYTE data[400]; // [esp+40h] [ebp-190h] BYREF

  ipx_programid = *((_DWORD *)programdata + 3);
  ipx_versionid = *((_DWORD *)programdata + 4);
  if ( *((_DWORD *)programdata + 6) >= 0x100u )
    v7 = 256;
  else
    v7 = *((_DWORD *)programdata + 6);
  ipx_maxplayers = v7;
  ipx_recvevent = event;
  PerfReset();
  LocaleInitialize(a1: programdata);
  if ( BindToWinsock() == 0 )
    return 0;
  if ( (*(int (__stdcall **)(int, _BYTE *))(ipx_winsockapi + 28))(a1: 257, a2: data) != 0 )
    goto LABEL_14;
  ipx_advsocket = (*(int (__stdcall **)(int, int, int))(ipx_winsockapi + 24))(a1: 6, a2: 2, a3: 1000);
  ipx_mainsocket = (*(int (__stdcall **)(int, int, int))(ipx_winsockapi + 24))(a1: 6, a2: 2, a3: 1000);
  if ( ipx_advsocket == 0 )
    goto LABEL_14;
  if ( ipx_mainsocket == 0 )
    goto LABEL_14;
  memset(a1: &ipx_broadcastaddr, Val: 0, Size: 0xEu);
  for ( loop = 0; loop < 6; byte_1802C9C6[loop++] = -1 )
    ;
  ipx_broadcastaddr = 6;
  word_1802C9CC = (*(int (__stdcall **)(int))(ipx_winsockapi + 8))(a1: 6111);
  memset(a1: &ipx_localaddr, Val: 0, Size: 0xEu);
  ipx_localaddr = 6;
  word_1802C9E4 = (*(int (__stdcall **)(int))(ipx_winsockapi + 8))(a1: 6112);
  if ( (*(int (__stdcall **)(int, __int16 *, int))ipx_winsockapi)(a1: ipx_mainsocket, a2: &ipx_localaddr, a3: 14) == 0 )
  {
    memcpy(a1: advaddr, Src: &ipx_localaddr, Size: 0xEu);
    advaddr[6] = (*(int (__stdcall **)(int))(ipx_winsockapi + 8))(a1: 6111);
    if ( (*(int (__stdcall **)(int, _WORD *, int))ipx_winsockapi)(a1: ipx_advsocket, a2: advaddr, a3: 14) != 0 )
    {
      IpxDestroy();
      SetLastError(dwErrCode: 0x4C6u);
      return 0;
    }
    else
    {
      value = 1;
      (*(void (__stdcall **)(int, int, int, int *, int))(ipx_winsockapi + 20))(
        a1: ipx_advsocket,
        a2: 0xFFFF,
        a3: 32,
        a4: &value,
        a5: 4);
      (*(void (__stdcall **)(int, int, int, int *, int))(ipx_winsockapi + 20))(
        a1: ipx_mainsocket,
        a2: 0xFFFF,
        a3: 32,
        a4: &value,
        a5: 4);
      hThread = (HANDLE)_beginthreadex(
                          Security: nullptr,
                          StackSize: 0,
                          StartAddress: RecvAdThreadProc,
                          ArgList: nullptr,
                          InitFlag: 0,
                          ThrdAddr: &thread);
      if ( hThread != nullptr )
      {
        SetThreadPriority(hThread, nPriority: 1);
        TListAdd(a1: &ipx_threadhead, a2: (struct _THREAD *)&thread, a3: aIpxCpp_5, a4: 1074);
      }
      Version = GetVersion();
      win95 = Version & 0x80000000;
      threads = 2 - (Version < 0);
      for ( ArgList = nullptr; (int)ArgList < threads; ArgList = (char *)ArgList + 1 )
      {
        v9 = (HANDLE)_beginthreadex(
                       Security: nullptr,
                       StackSize: 0,
                       StartAddress: RecvDataThreadProc,
                       ArgList,
                       InitFlag: 0,
                       &ThrdAddr);
        if ( v9 != nullptr )
        {
          SetThreadPriority(hThread: v9, nPriority: 2);
          TListAdd(a1: &ipx_threadhead, a2: (struct _THREAD *)&ThrdAddr, a3: aIpxCpp_6, a4: 1092);
        }
      }
      return SelectName(a1: programdata, a2: playerdata, interfacedata, versiondata);
    }
  }
  else
  {
LABEL_14:
    IpxDestroy();
    SetLastError(dwErrCode: 0x4C6u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18001A59
 * CodeView symbol: BindToWinsock
 * Demangled: BindToWinsock
 * IDA name: BindToWinsock
 * CodeView module: 10
 * CV address: segment 1 : 0xA59
 * Code length: 0x1F4
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL BindToWinsock()
{
  void *v1; // eax
  BOOL success; // [esp+4h] [ebp-4h]

  if ( ipx_winsocklib == nullptr )
  {
    ipx_winsocklib = LoadLibraryA(lpLibFileName: LibFileName);
    if ( ipx_winsocklib == nullptr )
      return false;
  }
  if ( ipx_winsockapi == 0 )
  {
    v1 = (void *)SMemAlloc(a1: 36, a2: aIpxCpp_7, a3: 120, a4: 0);
    ipx_winsockapi = (int)operator new(__formal: 36, ptr: v1);
    if ( ipx_winsockapi == 0 )
      return false;
  }
  *(_DWORD *)ipx_winsockapi = GetProcAddress(hModule: ipx_winsocklib, lpProcName: (LPCSTR)2);
  success = *(_DWORD *)ipx_winsockapi != 0;
  *(_DWORD *)(ipx_winsockapi + 4) = GetProcAddress(hModule: ipx_winsocklib, lpProcName: (LPCSTR)3);
  if ( *(_DWORD *)(ipx_winsockapi + 4) == 0 )
    success = false;
  *(_DWORD *)(ipx_winsockapi + 8) = GetProcAddress(hModule: ipx_winsocklib, lpProcName: (LPCSTR)9);
  if ( *(_DWORD *)(ipx_winsockapi + 8) == 0 )
    success = false;
  *(_DWORD *)(ipx_winsockapi + 12) = GetProcAddress(hModule: ipx_winsocklib, lpProcName: (LPCSTR)0x11);
  if ( *(_DWORD *)(ipx_winsockapi + 12) == 0 )
    success = false;
  *(_DWORD *)(ipx_winsockapi + 16) = GetProcAddress(hModule: ipx_winsocklib, lpProcName: (LPCSTR)0x14);
  if ( *(_DWORD *)(ipx_winsockapi + 16) == 0 )
    success = false;
  *(_DWORD *)(ipx_winsockapi + 20) = GetProcAddress(hModule: ipx_winsocklib, lpProcName: (LPCSTR)0x15);
  if ( *(_DWORD *)(ipx_winsockapi + 20) == 0 )
    success = false;
  *(_DWORD *)(ipx_winsockapi + 24) = GetProcAddress(hModule: ipx_winsocklib, lpProcName: (LPCSTR)0x17);
  if ( *(_DWORD *)(ipx_winsockapi + 24) == 0 )
    success = false;
  *(_DWORD *)(ipx_winsockapi + 28) = GetProcAddress(hModule: ipx_winsocklib, lpProcName: (LPCSTR)0x73);
  if ( *(_DWORD *)(ipx_winsockapi + 28) == 0 )
    success = false;
  *(_DWORD *)(ipx_winsockapi + 32) = GetProcAddress(hModule: ipx_winsocklib, lpProcName: (LPCSTR)0x74);
  if ( *(_DWORD *)(ipx_winsockapi + 32) == 0 )
    return false;
  return success;
}


// ==============================================================================
/*
 * EA: 0x18001C4D
 * CodeView symbol: RecvAdThreadProc
 * Demangled: RecvAdThreadProc
 * IDA name: RecvAdThreadProc
 * CodeView module: 10
 * CV address: segment 1 : 0xC4D
 * Code length: 0x1E7
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall __noreturn RecvAdThreadProc(void *__formal)
{
  struct _SNETSPI_GAMELIST *curr; // [esp+4h] [ebp-230h]
  int addrsize; // [esp+8h] [ebp-22Ch] BYREF
  _BYTE incomingaddr[16]; // [esp+Ch] [ebp-228h] BYREF
  unsigned int bytesread; // [esp+1Ch] [ebp-218h]
  char incomingad[2]; // [esp+20h] [ebp-214h] BYREF
  unsigned __int16 v6; // [esp+22h] [ebp-212h]
  unsigned __int16 v7; // [esp+24h] [ebp-210h]

  if ( ipx_winsockapi == 0 || *(_DWORD *)(ipx_winsockapi + 12) == 0 )
    _endthreadex(ReturnCode: 0);
  SendRequest();
  while ( ipx_advsocket != 0 && ipx_shutdown == 0 )
  {
    addrsize = 14;
    bytesread = 0;
    bytesread = (*(int (__stdcall **)(int, char *, int, _DWORD, _BYTE *, int *))(ipx_winsockapi + 12))(
                  a1: ipx_advsocket,
                  a2: incomingad,
                  a3: 532,
                  a4: 0,
                  a5: incomingaddr,
                  a6: &addrsize);
    if ( ipx_shutdown != 0 )
      _endthreadex(ReturnCode: 0);
    PerfIncrement(a1: 1u);
    PerfAdd(a1: 3u, a2: bytesread);
    if ( bytesread >= 0x14 && v6 == bytesread && (unsigned __int16)ComputeChecksum(a1: incomingad, a2: v6) == 0 )
    {
      if ( v7 <= 1u )
      {
        ProcessIncomingAd(remove: v7 == 1);
      }
      else if ( v7 == 2 )
      {
        SendAdvertisement(a1: incomingaddr);
      }
    }
  }
  while ( 1 )
  {
    curr = ipx_gamehead;
    if ( ipx_gamehead == nullptr )
      break;
    if ( *((_DWORD *)ipx_gamehead + 75) != 0 )
      SMemFree(a1: *((_DWORD *)ipx_gamehead + 75), a2: aIpxCpp_8, a3: 349, a4: 0);
    TListFree(a1: &ipx_gamehead, a2: curr, a3: aIpxCpp_9, a4: 350);
  }
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x18001E34
 * CodeView symbol: ProcessIncomingAd
 * Demangled: ProcessIncomingAd
 * IDA name: ProcessIncomingAd
 * CodeView module: 10
 * CV address: segment 1 : 0xE34
 * Code length: 0x294
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ProcessIncomingAd(_WORD *a1, int a2, int remove)
{
  int currptr; // [esp+8h] [ebp-14Ch]
  const void *currptra; // [esp+8h] [ebp-14Ch]
  _DWORD game[79]; // [esp+Ch] [ebp-148h] BYREF
  struct _SNETSPI_GAMELIST *next; // [esp+148h] [ebp-Ch]
  struct _SNETSPI_GAMELIST *curr; // [esp+14Ch] [ebp-8h]
  int gameid; // [esp+150h] [ebp-4h]

  if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 8) != 0 )
    a1[6] = (*(int (__stdcall **)(int))(ipx_winsockapi + 8))(a1: 6112);
  CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
  gameid = 0;
  curr = ipx_gamehead;
  while ( curr != nullptr )
  {
    if ( memcmp(Buf1: (char *)curr + 12, Buf2: a1, Size: 0xEu) == 0 )
    {
      gameid = *(_DWORD *)curr;
      next = *((struct _SNETSPI_GAMELIST **)curr + 74);
      if ( *((_DWORD *)curr + 75) != 0 )
        SMemFree(a1: *((_DWORD *)curr + 75), a2: aIpxCpp_10, a3: 254, a4: 0);
      TListFree(a1: &ipx_gamehead, a2: curr, a3: aIpxCpp_11, a4: 255);
      curr = next;
    }
    else
    {
      curr = *((struct _SNETSPI_GAMELIST **)curr + 74);
    }
  }
  if ( gameid == 0 )
    gameid = ++ipx_nextgameid;
  if ( gameid == 0 )
    gameid = ++ipx_nextgameid;
  if ( *(_DWORD *)(a2 + 8) == ipx_programid && remove == 0 )
  {
    memset(a1: game, Val: 0, Size: sizeof(game));
    game[0] = gameid;
    game[1] = *(_DWORD *)(a2 + 16);
    memcpy(a1: &game[3], Src: a1, Size: 0xEu);
    game[7] = 50;
    game[8] = GetTickCount();
    currptr = a2 + 20 + SStrCopy(a1: &game[10], a2: a2 + 20, a3: 128) + 1;
    currptra = (const void *)(currptr + SStrCopy(a1: &game[42], a2: currptr, a3: 128) + 1);
    game[76] = *(unsigned __int16 *)(a2 + 2) - ((unsigned int)currptra - a2);
    game[75] = SMemAlloc(a1: game[76], a2: aIpxCpp_12, a3: 284, a4: 0);
    game[77] = *(_DWORD *)(a2 + 8);
    game[78] = *(_DWORD *)(a2 + 12);
    memcpy(a1: (void *)game[75], Src: currptra, Size: game[76]);
    TListAdd(a1: &ipx_gamehead, a2: (struct _SNETSPI_GAMELIST *)game, a3: aIpxCpp_13, a4: 288);
  }
  CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
}


// ==============================================================================
/*
 * EA: 0x180020C8
 * CodeView symbol: RecvDataThreadProc
 * Demangled: RecvDataThreadProc
 * IDA name: RecvDataThreadProc
 * CodeView module: 10
 * CV address: segment 1 : 0x10C8
 * Code length: 0x1B4
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
unsigned int __stdcall RecvDataThreadProc(void *param)
{
  void *v1; // eax
  BOOL v3; // [esp+0h] [ebp-20h]
  _DWORD *pkt; // [esp+10h] [ebp-10h]
  int addrsize; // [esp+18h] [ebp-8h] BYREF
  signed int bytesread; // [esp+1Ch] [ebp-4h]

  if ( ipx_winsockapi == 0 || *(_DWORD *)(ipx_winsockapi + 12) == 0 )
    _endthreadex(ReturnCode: 0);
  while ( ipx_mainsocket != 0 && ipx_shutdown == 0 )
  {
    v1 = (void *)SMemAlloc(a1: 528, a2: aIpxCpp_14, a3: 369, a4: 0);
    pkt = operator new(__formal: 16, ptr: v1);
    addrsize = 14;
    bytesread = 0;
    bytesread = (*(int (__stdcall **)(int, _DWORD *, int, _DWORD, _DWORD *, int *))(ipx_winsockapi + 12))(
                  a1: ipx_mainsocket,
                  a2: pkt + 4,
                  a3: 504,
                  a4: 0,
                  a5: pkt,
                  a6: &addrsize);
    if ( ipx_shutdown != 0 )
    {
      if ( !IsBadWritePtr(lp: pkt, ucb: 0x210u) )
        operator delete(ptr: pkt);
      _endthreadex(ReturnCode: 0);
    }
    PerfIncrement(a1: 1u);
    PerfAdd(a1: 3u, a2: bytesread);
    pkt[130] = bytesread;
    memset(a1: (char *)pkt + addrsize, Val: 0, Size: 16 - addrsize);
    CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
    v3 = bytesread >= 0 && ipx_shutdown == 0;
    if ( v3 )
      TListAddPtrEnd(a1: &ipx_packethead, a2: (struct _PACKET *)pkt);
    else
      operator delete(ptr: pkt);
    CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
    if ( !v3 )
      _endthreadex(ReturnCode: 0);
    SetEvent(hEvent: ipx_recvevent);
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1800227C
 * CodeView symbol: SendAdvertisement
 * Demangled: SendAdvertisement
 * IDA name: SendAdvertisement
 * CodeView module: 10
 * CV address: segment 1 : 0x127C
 * Code length: 0x9C
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall SendAdvertisement(void *this)
{
  CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
  if ( ipx_advgameinfo != nullptr && ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 16) != 0 )
  {
    if ( this != nullptr )
      (*(void (__stdcall **)(int, void *, _DWORD, _DWORD, void *, int))(ipx_winsockapi + 16))(
        a1: ipx_advsocket,
        a2: ipx_advgameinfo,
        a3: *((unsigned __int16 *)ipx_advgameinfo + 1),
        a4: 0,
        a5: this,
        a6: 14);
    else
      (*(void (__stdcall **)(int, void *, _DWORD, _DWORD, __int16 *, int))(ipx_winsockapi + 16))(
        a1: ipx_advsocket,
        a2: ipx_advgameinfo,
        a3: *((unsigned __int16 *)ipx_advgameinfo + 1),
        a4: 0,
        a5: &ipx_broadcastaddr,
        a6: 14);
    PerfIncrement(a1: 0);
    PerfAdd(a1: 2u, a2: *((unsigned __int16 *)ipx_advgameinfo + 1));
  }
  CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
}


// ==============================================================================
/*
 * EA: 0x18002318
 * CodeView symbol: SendRequest
 * Demangled: SendRequest
 * IDA name: SendRequest
 * CodeView module: 10
 * CV address: segment 1 : 0x1318
 * Code length: 0x9D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void SendRequest()
{
  _DWORD request[5]; // [esp+0h] [ebp-14h] BYREF

  memset(a1: request, Val: 0, Size: sizeof(request));
  *(_DWORD *)((char *)request + 2) = 131092;
  request[2] = ipx_programid;
  request[3] = ipx_versionid;
  LOWORD(request[0]) = GenerateChecksum(a1: request, a2: 20);
  if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 16) != 0 )
    (*(void (__stdcall **)(int, _DWORD *, _DWORD, _DWORD, __int16 *, int))(ipx_winsockapi + 16))(
      a1: ipx_advsocket,
      a2: request,
      a3: HIWORD(request[0]),
      a4: 0,
      a5: &ipx_broadcastaddr,
      a6: 14);
  PerfIncrement(a1: 0);
  PerfAdd(a1: 2u, a2: HIWORD(request[0]));
}


// ==============================================================================
/*
 * EA: 0x180023B5
 * CodeView symbol: GenerateChecksum
 * Demangled: GenerateChecksum
 * IDA name: GenerateChecksum
 * CodeView module: 10
 * CV address: segment 1 : 0x13B5
 * Code length: 0x91
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall GenerateChecksum(int a1, int a2)
{
  unsigned __int16 v2; // ax
  unsigned __int8 hibyte; // [esp+8h] [ebp-Ch]

  v2 = ComputeChecksum(a1: (char *)(a1 + 2), a2: a2 - 2);
  hibyte = -1 - ((unsigned __int8)v2 + ((int)v2 >> 8)) % 255;
  return (hibyte << 8) | (unsigned __int8)(-1 - (hibyte + ((int)v2 >> 8)) % 255);
}


// ==============================================================================
/*
 * EA: 0x18002446
 * CodeView symbol: IpxInitializeDevice
 * Demangled: IpxInitializeDevice
 * IDA name: ?IpxInitializeDevice@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x1446
 * Code length: 0x9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxInitializeDevice(
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
 * EA: 0x1800244F
 * CodeView symbol: IpxLockDeviceList
 * Demangled: IpxLockDeviceList
 * IDA name: ?IpxLockDeviceList@@YGHPAPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x144F
 * Code length: 0x15
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxLockDeviceList(struct _SNETSPI_DEVICELIST **devicelist)
{
  *devicelist = nullptr;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18002464
 * CodeView symbol: IpxLockGameList
 * Demangled: IpxLockGameList
 * IDA name: ?IpxLockGameList@@YGHKKPAPAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x1464
 * Code length: 0x3D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxLockGameList(char categorybits, char categorymask, struct _SNETSPI_GAMELIST **gamelist)
{
  if ( gamelist != nullptr )
  {
    TrimGameList(a1: 10000);
    CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
    *gamelist = ipx_gamehead;
    return 1;
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x180024A1
 * CodeView symbol: TrimGameList
 * Demangled: TrimGameList
 * IDA name: TrimGameList
 * CodeView module: 10
 * CV address: segment 1 : 0x14A1
 * Code length: 0xB8
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall TrimGameList(unsigned int a1)
{
  struct _SNETSPI_GAMELIST *freeptr; // [esp+4h] [ebp-Ch]
  struct _SNETSPI_GAMELIST **next; // [esp+8h] [ebp-8h]
  DWORD currtime; // [esp+Ch] [ebp-4h]

  CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
  currtime = GetTickCount();
  next = &ipx_gamehead;
  while ( *next != nullptr )
  {
    if ( currtime - *((_DWORD *)*next + 8) <= a1 )
    {
      next = (struct _SNETSPI_GAMELIST **)((char *)*next + 296);
    }
    else
    {
      freeptr = *next;
      *next = *((struct _SNETSPI_GAMELIST **)*next + 74);
      if ( *((_DWORD *)freeptr + 75) != 0 )
        SMemFree(a1: *((_DWORD *)freeptr + 75), a2: aIpxCpp_15, a3: 688, a4: 0);
      SMemFree(a1: freeptr, a2: aIpxCpp_16, a3: 689, a4: 0);
    }
  }
  CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
}


// ==============================================================================
/*
 * EA: 0x18002559
 * CodeView symbol: IpxReceive
 * Demangled: IpxReceive
 * IDA name: ?IpxReceive@@YGHPAPAU_SNETADDR@@PAPAXPAK@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x1559
 * Code length: 0xC8
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxReceive(struct _SNETADDR **addr, void **data, unsigned int *databytes)
{
  if ( addr != nullptr )
    *addr = nullptr;
  if ( data != nullptr )
    *data = nullptr;
  if ( databytes != nullptr )
    *databytes = 0;
  if ( addr != nullptr && data != nullptr && databytes != nullptr && ipx_mainsocket != 0 )
  {
    if ( ipx_packethead != nullptr )
    {
      CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
      *addr = ipx_packethead;
      *data = (char *)ipx_packethead + 16;
      *databytes = *((_DWORD *)ipx_packethead + 130);
      ipx_packethead = *((struct _PACKET **)ipx_packethead + 131);
      CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
      return 1;
    }
    else
    {
      SetLastError(dwErrCode: 0x8510006B);
      return 0;
    }
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18002621
 * CodeView symbol: IpxReceiveExternalMessage
 * Demangled: IpxReceiveExternalMessage
 * IDA name: ?IpxReceiveExternalMessage@@YGHPAPBD00@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x1621
 * Code length: 0x41
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxReceiveExternalMessage(const char **senderpath, const char **sendername, const char **message)
{
  if ( senderpath != nullptr )
    *senderpath = nullptr;
  if ( sendername != nullptr )
    *sendername = nullptr;
  if ( message != nullptr )
    *message = nullptr;
  SetLastError(dwErrCode: 0x8510006B);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18002662
 * CodeView symbol: IpxSelectGame
 * Demangled: IpxSelectGame
 * IDA name: ?IpxSelectGame@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAK@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x1662
 * Code length: 0xA1
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __stdcall IpxSelectGame(
        unsigned int flags,
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        unsigned int *playerid)
{
  const struct DLGTEMPLATE *Dialog; // eax
  int FrameWindow; // [esp+4h] [ebp-20h]
  _DWORD uiparams[6]; // [esp+8h] [ebp-1Ch] BYREF
  int v11; // [esp+20h] [ebp-4h]

  uiparams[0] = flags;
  uiparams[1] = programdata;
  uiparams[2] = playerdata;
  uiparams[3] = interfacedata;
  uiparams[4] = versiondata;
  uiparams[5] = playerid;
  if ( interfacedata != nullptr )
    FrameWindow = *((_DWORD *)interfacedata + 2);
  else
    FrameWindow = SDrawGetFrameWindow(a1: 0);
  Dialog = LocaleGetDialog(a1: 0x69u);
  v11 = SDlgDialogBoxIndirectParam(
          a1: global_hinstance,
          a2: Dialog,
          a3: FrameWindow,
          a4: SelectGameDialogProc,
          a5: uiparams);
  return v11 != 0 && v11 != -1;
}


// ==============================================================================
/*
 * EA: 0x18002703
 * CodeView symbol: SelectGameDialogProc
 * Demangled: SelectGameDialogProc
 * IDA name: SelectGameDialogProc
 * CodeView module: 10
 * CV address: segment 1 : 0x1703
 * Code length: 0x6E7
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SelectGameDialogProc(HWND window, unsigned int message, int wparam, int lparam)
{
  HWND v4; // eax
  HWND v5; // eax
  HWND v7; // eax
  HWND DlgItem; // eax
  struct tagRECT rect; // [esp+8h] [ebp-39Ch] BYREF
  _DWORD v10[4]; // [esp+18h] [ebp-38Ch] BYREF
  _DWORD controllist[2]; // [esp+28h] [ebp-37Ch] BYREF
  _DWORD size[2]; // [esp+30h] [ebp-374h] BYREF
  CHAR buffer[255]; // [esp+38h] [ebp-36Ch] BYREF
  char v14; // [esp+137h] [ebp-26Dh]
  WPARAM sel; // [esp+138h] [ebp-26Ch]
  char name[256]; // [esp+13Ch] [ebp-268h] BYREF
  char *description; // [esp+23Ch] [ebp-168h]
  _DWORD createdata[4]; // [esp+240h] [ebp-164h] BYREF
  _DWORD interfacedata[20]; // [esp+250h] [ebp-154h] BYREF
  char fullname[253]; // [esp+2A0h] [ebp-104h] BYREF
  __int16 v21; // [esp+39Dh] [ebp-7h]
  char v22; // [esp+39Fh] [ebp-5h]
  WPARAM cursel; // [esp+3A0h] [ebp-4h]

  if ( message > 0x111 )
  {
    if ( message == 275 || message == 1024 )
    {
      SendRequest();
      TrimGameList(a1: 0x2710u);
      DlgItem = GetDlgItem(hDlg: window, nIDDlgItem: 1001);
      UpdateGameList(a1: window, a2: DlgItem);
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  switch ( message )
  {
    case 0x111u:
      if ( (unsigned __int16)wparam == 1 )
      {
        cursel = SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1001, Msg: 0x188u, wParam: 0, lParam: 0);
        if ( cursel != -1 )
        {
          fullname[0] = byte_1802CA20;
          memset(&fullname[1], 0, 0xFCu);
          v21 = 0;
          v22 = 0;
          SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1001, Msg: 0x189u, wParam: cursel, lParam: (LPARAM)fullname);
          if ( fullname[0] != 0 )
          {
            if ( strchr(Str: fullname, Val: 9) != nullptr )
              *strchr(Str: fullname, Val: 9) = 0;
            if ( SNetJoinGame(
                   a1: 0,
                   a2: fullname,
                   a3: 0,
                   a4: *(_DWORD *)(*(_DWORD *)(uiparams + 8) + 4),
                   a5: *(_DWORD *)(*(_DWORD *)(uiparams + 8) + 8),
                   a6: *(_DWORD *)(uiparams + 20)) != 0 )
            {
              KillTimer(hWnd: window, uIDEvent: 1u);
              SDlgEndDialog(a1: window, a2: 1);
            }
            else
            {
              (*(void (__stdcall **)(HWND, char *, _DWORD, _DWORD))(*(_DWORD *)(uiparams + 12) + 32))(
                a1: window,
                a2: aUnableToConnec,
                a3: *(_DWORD *)(*(_DWORD *)(uiparams + 4) + 4),
                a4: 0);
            }
          }
        }
      }
      else if ( (unsigned __int16)wparam == 3 && *(_DWORD *)(*(_DWORD *)(uiparams + 12) + 20) != 0 )
      {
        memcpy(a1: interfacedata, Src: *(const void **)(uiparams + 12), Size: sizeof(interfacedata));
        interfacedata[0] = 80;
        interfacedata[2] = window;
        createdata[0] = 16;
        createdata[1] = 1230002254;
        createdata[2] = ipx_maxplayers;
        createdata[3] = 0;
        if ( (*(int (__stdcall **)(_DWORD *, _DWORD, _DWORD, _DWORD *, _DWORD, _DWORD))(*(_DWORD *)(uiparams + 12) + 20))(
               a1: createdata,
               a2: *(_DWORD *)(uiparams + 4),
               a3: *(_DWORD *)(uiparams + 8),
               a4: interfacedata,
               a5: *(_DWORD *)(uiparams + 16),
               a6: *(_DWORD *)(uiparams + 20)) != 0 )
        {
          KillTimer(hWnd: window, uIDEvent: 1u);
          SDlgEndDialog(a1: window, a2: 1);
        }
      }
      else if ( (unsigned __int16)wparam == 2 )
      {
        KillTimer(hWnd: window, uIDEvent: 1u);
        SDlgEndDialog(a1: window, a2: 0);
      }
      else if ( (unsigned __int16)wparam == 1001 && HIWORD(wparam) == 1 )
      {
        v4 = GetDlgItem(hDlg: window, nIDDlgItem: 103);
        InvalidateRect(hWnd: v4, lpRect: nullptr, bErase: true);
      }
      else if ( (unsigned __int16)wparam == 1001 && HIWORD(wparam) == 2 )
      {
        v5 = GetDlgItem(hDlg: window, nIDDlgItem: 1);
        PostMessageA(hWnd: window, Msg: 0x111u, wParam: 1u, lParam: (LPARAM)v5);
      }
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    case 2u:
      if ( background != 0 )
      {
        SMemFree(a1: background, a2: aIpxCpp_17, a3: 514, a4: 0);
        background = 0;
      }
      if ( buttontexture != 0 )
      {
        SMemFree(a1: buttontexture, a2: aIpxCpp_18, a3: 518, a4: 0);
        buttontexture = 0;
      }
      uiparams = 0;
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    case 0x2Bu:
      if ( wparam != 103 )
        return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
      name[0] = byte_1802CA24;
      memset(&name[1], 0, 255);
      sel = SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1001, Msg: 0x188u, wParam: 0, lParam: 0);
      if ( sel != -1 )
        SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1001, Msg: 0x189u, wParam: sel, lParam: (LPARAM)name);
      description = (char *)&unk_1802CA28;
      if ( strchr(Str: name, Val: 9) != nullptr )
      {
        description = strchr(Str: name, Val: 9);
        *description++ = 0;
      }
      buffer[0] = byte_1802CA2C;
      memset(&buffer[1], 0, 254);
      v14 = 0;
      GetDlgItemTextA(hDlg: window, nIDDlgItem: 103, lpString: buffer, cchMax: 256);
      v14 = 0;
      if ( strcmp(Str1: buffer, Str2: description) != 0 )
        SetDlgItemTextA(hDlg: window, nIDDlgItem: 103, lpString: description);
      if ( *(_DWORD *)(*(_DWORD *)(uiparams + 12) + 24) != 0 )
        return (*(int (__stdcall **)(int, int, char *, char *, _DWORD, _DWORD, int, int))(*(_DWORD *)(uiparams + 12) + 24))(
                 a1: 1230002254,
                 a2: 1,
                 a3: name,
                 a4: description,
                 a5: 0,
                 a6: 0,
                 a7: 2,
                 a8: lparam);
      else
        return 0;
    case 0x110u:
      uiparams = lparam;
      if ( LoadArtwork(artid: 0, setpalette: 1, data: (int)&background, (int)size) != 0 )
      {
        SDlgSetBitmapE(
          a1: window,
          a2: 0,
          a3: &unk_1802CA30,
          a4: -1,
          a5: 1,
          a6: background,
          a7: 0,
          a8: size[0],
          a9: size[1],
          a10: -1);
        controllist[0] = 103;
        controllist[1] = 0;
        SDlgSetControlBitmaps(a1: window, a2: controllist, a3: 0, a4: background, a5: size, a6: 2, a7: -1);
      }
      if ( LoadArtwork(artid: 1, setpalette: 0, data: (int)&buttontexture, (int)size) != 0 )
      {
        v10[0] = 3;
        v10[1] = 1;
        v10[2] = 2;
        v10[3] = 0;
        SDlgSetControlBitmaps(a1: window, a2: v10, a3: 0, a4: buttontexture, a5: size, a6: 1, a7: -1);
      }
      SetDlgItemTextA(hDlg: window, nIDDlgItem: 104, lpString: *(LPCSTR *)(*(_DWORD *)(uiparams + 4) + 8));
      v7 = GetDlgItem(hDlg: window, nIDDlgItem: 1001);
      GetClientRect(hWnd: v7, lpRect: &rect);
      SendDlgItemMessageA(hDlg: window, nIDDlgItem: 1001, Msg: 0x192u, wParam: 1u, lParam: (LPARAM)&rect.right);
      PostMessageA(hWnd: window, Msg: 0x400u, wParam: 0, lParam: 0);
      SetTimer(hWnd: window, nIDEvent: 1u, uElapse: 0x1F4u, lpTimerFunc: nullptr);
      return 1;
    default:
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
}


// ==============================================================================
/*
 * EA: 0x18002DEA
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: LoadArtwork
 * CodeView module: 10
 * CV address: segment 1 : 0x1DEA
 * Code length: 0x15D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall LoadArtwork(
        int (__stdcall *a1)(int, int, _BYTE *, _DWORD, int, int *, int *, int *),
        int a2,
        int artid,
        int setpalette,
        _DWORD *data,
        _DWORD *size)
{
  int bytes; // [esp+8h] [ebp-410h]
  int height; // [esp+Ch] [ebp-40Ch] BYREF
  int bitdepth; // [esp+10h] [ebp-408h] BYREF
  _BYTE pe[4]; // [esp+14h] [ebp-404h] BYREF
  _BYTE v13[1020]; // [esp+18h] [ebp-400h] BYREF
  int width; // [esp+414h] [ebp-4h] BYREF

  *data = 0;
  *size = 0;
  size[1] = 0;
  if ( a1 == nullptr )
    return 0;
  if ( a1(a1: a2, a2: artid, a3: nullptr, a4: 0, a5: 0, a6: &width, a7: &height, a8: &bitdepth) == 0 )
    return 0;
  if ( size != nullptr )
  {
    *size = width;
    size[1] = height;
  }
  bytes = bitdepth * height * width / 8;
  *data = SMemAlloc(a1: bytes, a2: aIpxCpp_19, a3: 208, a4: 0);
  if ( *data == 0 )
    return 0;
  if ( a1(a1: a2, a2: artid, a3: pe, a4: *data, a5: bytes, a6: &width, a7: &height, a8: &bitdepth) != 0 )
  {
    if ( setpalette != 0 )
      SDrawUpdatePalette(a1: 1, a2: 254, a3: v13, a4: 0);
    return 1;
  }
  else
  {
    SMemFree(a1: *data, a2: aIpxCpp_20, a3: 221, a4: 0);
    *data = 0;
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18002F47
 * CodeView symbol: UpdateGameList
 * Demangled: UpdateGameList
 * IDA name: UpdateGameList
 * CodeView module: 10
 * CV address: segment 1 : 0x1F47
 * Code length: 0x292
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall UpdateGameList(HWND a1, HWND a2)
{
  HWND DlgItem; // eax
  LRESULT v3; // eax
  HWND v4; // eax
  char Buffer[256]; // [esp+8h] [ebp-30Ch] BYREF
  struct _SNETSPI_GAMELIST *i; // [esp+108h] [ebp-20Ch]
  char liststring[256]; // [esp+10Ch] [ebp-208h] BYREF
  WPARAM index; // [esp+20Ch] [ebp-108h]
  char fullstring[256]; // [esp+210h] [ebp-104h] BYREF
  struct _SNETSPI_GAMELIST *curr; // [esp+310h] [ebp-4h]

  CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
  for ( curr = ipx_gamehead; curr != nullptr; curr = *((struct _SNETSPI_GAMELIST **)curr + 74) )
  {
    if ( (*((_DWORD *)curr + 1) & 0xB) == 0 )
    {
      sprintf(Buffer: fullstring, Format: "%s\t%s", (const char *)curr + 40, (const char *)curr + 168);
      if ( SendMessageA(hWnd: a2, Msg: 0x1A2u, wParam: 0xFFFFFFFF, lParam: (LPARAM)fullstring) == -1 )
      {
        SendMessageA(hWnd: a2, Msg: 0x180u, wParam: 0, lParam: (LPARAM)fullstring);
        DlgItem = GetDlgItem(hDlg: a1, nIDDlgItem: 1);
        EnableWindow(hWnd: DlgItem, bEnable: true);
        if ( SendMessageA(hWnd: a2, Msg: 0x188u, wParam: 0, lParam: 0) == -1 )
        {
          SendMessageA(hWnd: a2, Msg: 0x186u, wParam: 0, lParam: 0);
          SendMessageA(hWnd: a1, Msg: 0x111u, wParam: 0x103E9u, lParam: (LPARAM)a2);
        }
      }
    }
  }
  index = 0;
  while ( SendMessageA(hWnd: a2, Msg: 0x189u, wParam: index, lParam: (LPARAM)liststring) != -1 )
  {
    for ( i = ipx_gamehead; i != nullptr; i = *((struct _SNETSPI_GAMELIST **)i + 74) )
    {
      if ( (*((_DWORD *)i + 1) & 0xB) == 0 )
      {
        sprintf(Buffer, Format: "%s\t%s", (const char *)i + 40, (const char *)i + 168);
        if ( strcmp(Str1: Buffer, Str2: liststring) == 0 )
          break;
      }
    }
    if ( i != nullptr )
    {
      ++index;
    }
    else
    {
      v3 = SendMessageA(hWnd: a2, Msg: 0x188u, wParam: 0, lParam: 0);
      if ( v3 == index )
      {
        SendMessageA(hWnd: a2, Msg: 0x186u, wParam: index - 1, lParam: 0);
        SendMessageA(hWnd: a1, Msg: 0x111u, wParam: 0x103E9u, lParam: (LPARAM)a2);
      }
      if ( SendMessageA(hWnd: a2, Msg: 0x182u, wParam: index, lParam: 0) == 0 )
      {
        v4 = GetDlgItem(hDlg: a1, nIDDlgItem: 1);
        EnableWindow(hWnd: v4, bEnable: false);
      }
    }
  }
  CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
}


// ==============================================================================
/*
 * EA: 0x180031D9
 * CodeView symbol: IpxSend
 * Demangled: IpxSend
 * IDA name: ?IpxSend@@YGHKPAPAU_SNETADDR@@PAXK@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x21D9
 * Code length: 0x98
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxSend(unsigned int addresses, struct _SNETADDR **addrlist, void *data, unsigned int databytes)
{
  unsigned int v5; // ecx

  if ( addresses != 0 && addrlist != nullptr && data != nullptr && databytes != 0 && ipx_mainsocket != 0 )
  {
    if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 16) != 0 )
    {
      while ( 1 )
      {
        v5 = addresses--;
        if ( v5 == 0 )
          break;
        (*(void (__stdcall **)(int, void *, unsigned int, _DWORD, struct _SNETADDR *, int))(ipx_winsockapi + 16))(
          a1: ipx_mainsocket,
          a2: data,
          a3: databytes,
          a4: 0,
          a5: addrlist[addresses],
          a6: 14);
        PerfIncrement(a1: 0);
        PerfAdd(a1: 2u, a2: databytes);
      }
    }
    return 1;
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18003271
 * CodeView symbol: IpxSendExternalMessage
 * Demangled: IpxSendExternalMessage
 * IDA name: ?IpxSendExternalMessage@@YGHPBD0000@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x2271
 * Code length: 0x9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxSendExternalMessage(
        const char *senderpath,
        const char *sendername,
        const char *targetpath,
        const char *targetname,
        const char *message)
{
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1800327A
 * CodeView symbol: IpxStartAdvertisingGame
 * Demangled: IpxStartAdvertisingGame
 * IDA name: ?IpxStartAdvertisingGame@@YGHPBD00KKKKKPBXK@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x227A
 * Code length: 0x1C0
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxStartAdvertisingGame(
        const char *gamename,
        const char *gamepassword,
        const char *gamedescription,
        unsigned int gamemode,
        char gameage,
        char gamecategorybits,
        char optcategorybits,
        char ladderid,
        const void *clientdata,
        size_t clientdatabytes)
{
  void *v11; // eax
  __int16 v12; // si
  size_t v13; // eax
  const char *currptr; // [esp+8h] [ebp-4h]
  const char *currptra; // [esp+8h] [ebp-4h]

  if ( gamename != nullptr && gamedescription != nullptr )
  {
    CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
    if ( ipx_advgameinfo != nullptr
      || (v11 = (void *)SMemAlloc(a1: 532, a2: aIpxCpp_21, a3: 1275, a4: 0),
          ipx_advgameinfo = operator new(__formal: 20, ptr: v11),
          ipx_advgameinfo != nullptr) )
    {
      memset(a1: ipx_advgameinfo, Val: 0, Size: 0x214u);
      *(_WORD *)ipx_advgameinfo = 0;
      v12 = strlen(Str: gamename);
      *((_WORD *)ipx_advgameinfo + 1) = strlen(Str: gamedescription) + v12 + clientdatabytes + 22;
      *((_WORD *)ipx_advgameinfo + 2) = 0;
      *((_WORD *)ipx_advgameinfo + 3) = 0;
      *((_DWORD *)ipx_advgameinfo + 2) = ipx_programid;
      *((_DWORD *)ipx_advgameinfo + 3) = ipx_versionid;
      *((_DWORD *)ipx_advgameinfo + 4) = gamemode;
      currptr = (char *)ipx_advgameinfo + 20;
      SStrCopy(a1: (char *)ipx_advgameinfo + 20, a2: gamename, a3: 128);
      currptra = &currptr[strlen(Str: currptr) + 1];
      SStrCopy(a1: currptra, a2: gamedescription, a3: 128);
      v13 = strlen(Str: currptra);
      memcpy(a1: (void *)&currptra[v13 + 1], Src: clientdata, Size: clientdatabytes);
      *(_WORD *)ipx_advgameinfo = GenerateChecksum(
                                    a1: (int)ipx_advgameinfo,
                                    a2: *((unsigned __int16 *)ipx_advgameinfo + 1));
      CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
      SendAdvertisement(this: nullptr);
      return 1;
    }
    else
    {
      CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
      SetLastError(dwErrCode: 8u);
      return 0;
    }
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1800343A
 * CodeView symbol: IpxStopAdvertisingGame
 * Demangled: IpxStopAdvertisingGame
 * IDA name: ?IpxStopAdvertisingGame@@YGHXZ
 * CodeView module: 10
 * CV address: segment 1 : 0x243A
 * Code length: 0xE4
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxStopAdvertisingGame()
{
  CCritSect::Enter(this: (CCritSect *)&ipx_critsect);
  if ( ipx_advgameinfo != nullptr )
  {
    *(_WORD *)ipx_advgameinfo = 0;
    *((_WORD *)ipx_advgameinfo + 2) = 1;
    *(_WORD *)ipx_advgameinfo = GenerateChecksum(
                                  a1: (int)ipx_advgameinfo,
                                  a2: *((unsigned __int16 *)ipx_advgameinfo + 1));
    if ( ipx_winsockapi != 0 && *(_DWORD *)(ipx_winsockapi + 16) != 0 )
      (*(void (__stdcall **)(int, void *, _DWORD, _DWORD, __int16 *, int))(ipx_winsockapi + 16))(
        a1: ipx_advsocket,
        a2: ipx_advgameinfo,
        a3: *((unsigned __int16 *)ipx_advgameinfo + 1),
        a4: 0,
        a5: &ipx_broadcastaddr,
        a6: 14);
    PerfIncrement(a1: 0);
    PerfAdd(a1: 2u, a2: *((unsigned __int16 *)ipx_advgameinfo + 1));
    SMemFree(a1: ipx_advgameinfo, a2: aIpxCpp_22, a3: 1335, a4: 0);
    ipx_advgameinfo = nullptr;
  }
  CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800351E
 * CodeView symbol: IpxUnlockDeviceList
 * Demangled: IpxUnlockDeviceList
 * IDA name: ?IpxUnlockDeviceList@@YGHPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x251E
 * Code length: 0xC
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxUnlockDeviceList(struct _SNETSPI_DEVICELIST *devicelist)
{
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800352A
 * CodeView symbol: IpxUnlockGameList
 * Demangled: IpxUnlockGameList
 * IDA name: ?IpxUnlockGameList@@YGHPAU_SNETSPI_GAMELIST@@PAK@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x252A
 * Code length: 0x66
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxUnlockGameList(struct _SNETSPI_GAMELIST *gamelist, unsigned int *hintnextcall)
{
  DWORD currtime; // [esp+0h] [ebp-4h]

  if ( gamelist == ipx_gamehead )
  {
    CCritSect::Leave(this: (CCritSect *)&ipx_critsect);
    if ( hintnextcall != nullptr )
      *hintnextcall = 500;
    currtime = GetTickCount();
    if ( currtime - lasttime > 0x190 )
    {
      lasttime = currtime;
      SendRequest();
    }
    return 1;
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18003590
 * CodeView symbol: IpxDestroy_DEBUG
 * Demangled: IpxDestroy_DEBUG
 * IDA name: ?IpxDestroy_DEBUG@@YGHXZ
 * CodeView module: 10
 * CV address: segment 1 : 0x2590
 * Code length: 0x20
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxDestroy_DEBUG()
{
  int v1; // [esp+0h] [ebp-4h]

  v1 = IpxDestroy();
  return LatencyDestroy() & v1;
}


// ==============================================================================
/*
 * EA: 0x180035B0
 * CodeView symbol: IpxInitialize_DEBUG
 * Demangled: IpxInitialize_DEBUG
 * IDA name: ?IpxInitialize_DEBUG@@YGHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAX@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x25B0
 * Code length: 0x37
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall IpxInitialize_DEBUG(
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        void *event)
{
  if ( IpxInitialize(programdata, playerdata, interfacedata, versiondata, event) != 0 )
    return LatencyInitialize(a1: IpxSend, a2: 15);
  else
    return 0;
}


// ==============================================================================
/*
 * EA: 0x180035F0
 * CodeView symbol: CCritSect::Enter
 * Demangled: CCritSect::Enter
 * IDA name: ?Enter@CCritSect@@QAEXXZ
 * CodeView module: 10
 * CV address: segment 1 : 0x25F0
 * Code length: 0x15
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall CCritSect::Enter(struct _RTL_CRITICAL_SECTION *this)
{
  EnterCriticalSection(lpCriticalSection: this);
}


// ==============================================================================
/*
 * EA: 0x18003610
 * CodeView symbol: CCritSect::Leave
 * Demangled: CCritSect::Leave
 * IDA name: ?Leave@CCritSect@@QAEXXZ
 * CodeView module: 10
 * CV address: segment 1 : 0x2610
 * Code length: 0x15
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall CCritSect::Leave(struct _RTL_CRITICAL_SECTION *this)
{
  LeaveCriticalSection(lpCriticalSection: this);
}


// ==============================================================================
/*
 * EA: 0x18003630
 * CodeView symbol: operator new
 * Demangled: operator new
 * IDA name: ??2@YAPAXIPAX@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x2630
 * Code length: 0x8
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__cdecl operator new(char __formal, void *ptr)
{
  return ptr;
}


// ==============================================================================
/*
 * EA: 0x18003640
 * CodeView symbol: TListFree
 * Demangled: TListFree
 * IDA name: ?TListFree@@YIHPAPAU_SNETSPI_GAMELIST@@PAU1@PBDH@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x2640
 * Code length: 0x80
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
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
 * EA: 0x180036C0
 * CodeView symbol: TListAdd
 * Demangled: TListAdd
 * IDA name: ?TListAdd@@YIHPAPAU_SNETSPI_GAMELIST@@PAU1@PBDH@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x26C0
 * Code length: 0x73
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAdd(
        struct _SNETSPI_GAMELIST **a1,
        struct _SNETSPI_GAMELIST *a2,
        const char *filename,
        int linenumber)
{
  _DWORD *newptr; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  newptr = (_DWORD *)SMemAlloc(a1: 316, a2: filename, a3: linenumber, a4: 0);
  if ( newptr == nullptr )
    return 0;
  memcpy(a1: newptr, Src: a2, Size: 0x13Cu);
  newptr[74] = *a1;
  *a1 = (struct _SNETSPI_GAMELIST *)newptr;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18003740
 * CodeView symbol: TListAddPtrEnd
 * Demangled: TListAddPtrEnd
 * IDA name: ?TListAddPtrEnd@@YIHPAPAU_PACKET@@PAU1@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x2740
 * Code length: 0x58
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAddPtrEnd(struct _PACKET **a1, struct _PACKET *a2)
{
  struct _PACKET **next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  *((_DWORD *)a2 + 131) = 0;
  for ( next = a1; *next != nullptr; next = (struct _PACKET **)((char *)*next + 524) )
    ;
  *next = a2;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x180037A0
 * CodeView symbol: TListFree
 * Demangled: TListFree
 * IDA name: ?TListFree@@YIHPAPAU_THREAD@@PAU1@PBDH@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x27A0
 * Code length: 0x7A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
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
 * EA: 0x18003820
 * CodeView symbol: TListClear
 * Demangled: TListClear
 * IDA name: ?TListClear@@YIHPAPAU_PACKET@@PBDH@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x2820
 * Code length: 0x56
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
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
    next = *((struct _PACKET **)*a1 + 131);
    SMemFree(a1: *a1, a2, a3: linenumber, a4: 0);
    *a1 = next;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18003880
 * CodeView symbol: TListAdd
 * Demangled: TListAdd
 * IDA name: ?TListAdd@@YIHPAPAU_THREAD@@PAU1@PBDH@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x2880
 * Code length: 0x6A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Ipx.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Ipx.cpp
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

