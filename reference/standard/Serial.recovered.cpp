/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 *
 * CodeView module object:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 31
 */


// ==============================================================================
/*
 * EA: 0x18008860
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: $E4
 * CodeView module: 4
 * CV address: segment 1 : 0x7860
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E4()
{
  _E1();
  return _E3();
}


// ==============================================================================
/*
 * EA: 0x1800886F
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: $E1
 * CodeView module: 4
 * CV address: segment 1 : 0x786F
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *_E1()
{
  return CCritSect::CCritSect(this: (CCritSect *)&serial_critsect);
}


// ==============================================================================
/*
 * EA: 0x1800887E
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: $E3
 * CodeView module: 4
 * CV address: segment 1 : 0x787E
 * Code length: 0x12
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E3()
{
  return atexit(a1: _E2);
}


// ==============================================================================
/*
 * EA: 0x18008890
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: $E2
 * CodeView module: 4
 * CV address: segment 1 : 0x7890
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl _E2()
{
  CCritSect::~CCritSect(this: (CCritSect *)&serial_critsect);
}


// ==============================================================================
/*
 * EA: 0x1800889F
 * CodeView symbol: SerialCompareNetAddresses
 * Demangled: SerialCompareNetAddresses
 * IDA name: ?SerialCompareNetAddresses@@YGHPAU_SNETADDR@@0PAK@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x789F
 * Code length: 0x4F
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialCompareNetAddresses(struct _SNETADDR *addr1, struct _SNETADDR *addr2, unsigned int *diffmagnitude)
{
  if ( diffmagnitude != nullptr )
    *diffmagnitude = 0;
  if ( addr1 != nullptr && addr2 != nullptr )
  {
    *diffmagnitude = memcmp(Buf1: addr1, Buf2: addr2, Size: 0x10u) != 0;
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
 * EA: 0x180088EE
 * CodeView symbol: SerialDestroy
 * Demangled: SerialDestroy
 * IDA name: ?SerialDestroy@@YGHXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x78EE
 * Code length: 0x2CE
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialDestroy()
{
  DWORD v0; // eax
  unsigned int j; // [esp+0h] [ebp-F0h]
  HANDLE event[56]; // [esp+4h] [ebp-ECh] BYREF
  DWORD numsends; // [esp+E4h] [ebp-Ch]
  unsigned int i; // [esp+E8h] [ebp-8h]
  unsigned int port; // [esp+ECh] [ebp-4h]

  if ( serial_thread != nullptr )
  {
    serial_state = 3;
    for ( i = 0; i < 8; ++i )
      SetEvent(hEvent: serial_event[i]);
    WaitForSingleObject(hHandle: serial_thread, dwMilliseconds: 0xFFFFFFFF);
    CloseHandle(hObject: serial_thread);
    serial_thread = nullptr;
  }
  numsends = 0;
  for ( j = 0; j < 8; ++j )
  {
    if ( *(_DWORD *)*(&serial_port + j) != -1 )
    {
      memset(a1: &event[5 * numsends + 16], Val: 0, Size: 0x14u);
      event[numsends] = CreateEventA(
                          lpEventAttributes: nullptr,
                          bManualReset: false,
                          bInitialState: false,
                          lpName: nullptr);
      event[5 * numsends + 20] = event[numsends];
      WriteFile(
        hFile: *(HANDLE *)*(&serial_port + j),
        lpBuffer: asc_180291EC,
        nNumberOfBytesToWrite: 2u,
        lpNumberOfBytesWritten: (LPDWORD)&event[numsends + 8],
        lpOverlapped: (LPOVERLAPPED)&event[5 * numsends + 16]);
      ++numsends;
      PerfIncrement(a1: 0);
      PerfAdd(a1: 2u, a2: 2u);
    }
  }
  if ( numsends != 0 )
  {
    WaitForMultipleObjects(nCount: numsends, lpHandles: event, bWaitAll: true, dwMilliseconds: 0xFFFFFFFF);
    while ( 1 )
    {
      v0 = numsends--;
      if ( v0 == 0 )
        break;
      CloseHandle(hObject: event[numsends]);
    }
  }
  for ( port = 0; port < 8; ++port )
  {
    CloseHandle(hObject: serial_event[port]);
    if ( *(_DWORD *)*(&serial_port + port) != -1 )
      CloseHandle(hObject: *(HANDLE *)*(&serial_port + port));
    if ( *((_DWORD *)*(&serial_port + port) + 23) != 0 )
      SMemFree(a1: *((_DWORD *)*(&serial_port + port) + 23), a2: aSerialCpp, a3: 692, a4: 0);
    SMemFree(a1: *(&serial_port + port), a2: aSerialCpp_0, a3: 693, a4: 0);
    *(&serial_port + port) = nullptr;
  }
  SerialStopAdvertisingGame();
  TListClear(a1: &serial_messagehead, a2: aSerialCpp_1, a3: 701);
  if ( serial_gamelist != nullptr )
  {
    if ( *((_DWORD *)serial_gamelist + 75) != 0 )
      SMemFree(a1: *((_DWORD *)serial_gamelist + 75), a2: aSerialCpp_2, a3: 705, a4: 0);
    SMemFree(a1: serial_gamelist, a2: aSerialCpp_3, a3: 706, a4: 0);
    serial_gamelist = nullptr;
  }
  serial_networkid = 0;
  serial_networkidlocked = 0;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18008BBC
 * CodeView symbol: SerialFree
 * Demangled: SerialFree
 * IDA name: ?SerialFree@@YGHPAU_SNETADDR@@PAXK@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x7BBC
 * Code length: 0x38
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialFree(struct _SNETADDR *addr, void *data, char databytes)
{
  if ( addr != nullptr && data != nullptr )
  {
    SMemFree(a1: addr, a2: aSerialCpp_4, a3: 725, a4: 0);
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
 * EA: 0x18008BF4
 * CodeView symbol: SerialFreeExternalMessage
 * Demangled: SerialFreeExternalMessage
 * IDA name: ?SerialFreeExternalMessage@@YGHPBD00@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x7BF4
 * Code length: 0x10
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialFreeExternalMessage(const char *senderpath, const char *sendername, const char *mesage)
{
  SErrSetLastError(a1: 87);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18008C04
 * CodeView symbol: SerialGetGameInfo
 * Demangled: SerialGetGameInfo
 * IDA name: ?SerialGetGameInfo@@YGHKPBD0PAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x7C04
 * Code length: 0xCF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialGetGameInfo(
        unsigned int gameid,
        const char *gamename,
        const char *gamepassword,
        struct _SNETSPI_GAMELIST *gameinfo)
{
  if ( gameinfo != nullptr )
    memset(a1: gameinfo, Val: 0, Size: 0x13Cu);
  if ( gamename != nullptr && gameinfo != nullptr && (gameid != 0 || *gamename != 0) )
  {
    CCritSect::Enter(this: (CCritSect *)&serial_critsect);
    if ( serial_gamelist != nullptr
      && (gameid == 0 || gameid == *(_DWORD *)serial_gamelist)
      && (*gamename == 0 || _stricmp(String1: gamename, String2: (const char *)serial_gamelist + 40) == 0) )
    {
      memcpy(a1: gameinfo, Src: serial_gamelist, Size: 0x13Cu);
    }
    CCritSect::Leave(this: (CCritSect *)&serial_critsect);
    if ( *(_DWORD *)gameinfo != 0 )
    {
      return 1;
    }
    else
    {
      SErrSetLastError(a1: -2062548888);
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
 * EA: 0x18008CD3
 * CodeView symbol: SerialGetPerformanceData
 * Demangled: SerialGetPerformanceData
 * IDA name: ?SerialGetPerformanceData@@YGHKPAKPAT_LARGE_INTEGER@@1@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x7CD3
 * Code length: 0x1A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialGetPerformanceData(
        unsigned int counterid,
        unsigned int *countervalue,
        union _LARGE_INTEGER *measurementtime,
        union _LARGE_INTEGER *measurementfreq)
{
  return PerfGetPerformanceData(a1: counterid, a2: countervalue, a3: measurementtime, a4: measurementfreq);
}


// ==============================================================================
/*
 * EA: 0x18008CED
 * CodeView symbol: SerialInitialize
 * Demangled: SerialInitialize
 * IDA name: ?SerialInitialize@@YGHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAX@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x7CED
 * Code length: 0x11A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialInitialize(
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        void *event)
{
  int v6; // [esp+0h] [ebp-Ch]
  unsigned int threadid; // [esp+4h] [ebp-8h] BYREF
  unsigned int loop; // [esp+8h] [ebp-4h]

  serial_programid = *((_DWORD *)programdata + 3);
  serial_versionid = *((_DWORD *)programdata + 4);
  if ( *((_DWORD *)programdata + 6) >= 4u )
    v6 = 4;
  else
    v6 = *((_DWORD *)programdata + 6);
  serial_maxplayers = v6;
  serial_recvevent = event;
  PerfReset();
  LocaleInitialize(a1: programdata);
  for ( loop = 0; loop < 8; ++loop )
  {
    if ( InitializePort(a1: loop) == 0 )
    {
      SErrSetLastError(a1: 8);
      return 0;
    }
  }
  if ( serial_thread == nullptr )
  {
    serial_thread = (HANDLE)_beginthreadex(
                              Security: nullptr,
                              StackSize: 0,
                              StartAddress: ThreadProc,
                              ArgList: nullptr,
                              InitFlag: 0,
                              ThrdAddr: &threadid);
    if ( serial_thread == nullptr )
    {
      SerialDestroy();
      SErrSetLastError(a1: 164);
      return 0;
    }
    SetThreadPriority(hThread: serial_thread, nPriority: 2);
  }
  if ( FindNetworkId() != 0 )
  {
    sub_180091A9(data: 0, databytes: 0);
    return SelectName(a1: programdata, a2: playerdata, interfacedata, versiondata);
  }
  else
  {
    SErrSetLastError(a1: 68);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18008E07
 * CodeView symbol: FindNetworkId
 * Demangled: FindNetworkId
 * IDA name: FindNetworkId
 * CodeView module: 4
 * CV address: segment 1 : 0x7E07
 * Code length: 0x100
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int FindNetworkId()
{
  int v0; // eax
  DWORD starttime; // [esp+0h] [ebp-14h]
  _DWORD messagedata[2]; // [esp+4h] [ebp-10h] BYREF
  int localnetworkid; // [esp+Ch] [ebp-8h] BYREF
  int timeout; // [esp+10h] [ebp-4h]

  timeout = 16;
  while ( 1 )
  {
    v0 = timeout--;
    if ( v0 == 0 )
      break;
    for ( serial_routeloopcheck = 0; serial_routeloopcheck == 0; serial_routeloopcheck = PickRandomNumber() )
      ;
    for ( serial_networkid = 0; serial_networkid == 0; serial_networkid = PickRandomNumber() & 7 )
      ;
    messagedata[0] = (unsigned __int8)serial_networkid;
    messagedata[1] = serial_routeloopcheck;
    sub_180091A9(data: (int)messagedata, databytes: 8);
    starttime = GetTickCount();
    while ( serial_networkid != 0 && GetTickCount() - starttime < 0x1F4 )
      Sleep(dwMilliseconds: 1u);
    if ( serial_networkid != 0 )
    {
      serial_networkidlocked = 1;
      serial_routeloopcheck = 0;
      localnetworkid = (unsigned __int8)serial_networkid;
      sub_180091A9(data: (int)&localnetworkid, databytes: 4);
      return 1;
    }
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18008F07
 * CodeView symbol: InitializePort
 * Demangled: InitializePort
 * IDA name: InitializePort
 * CodeView module: 4
 * CV address: segment 1 : 0x7F07
 * Code length: 0x20C
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall InitializePort(int a1)
{
  void *v1; // eax
  int v3; // eax
  struct _COMMTIMEOUTS timeouts; // [esp+8h] [ebp-40h] BYREF
  struct _DCB dcb; // [esp+1Ch] [ebp-2Ch] BYREF
  CHAR portname[16]; // [esp+38h] [ebp-10h] BYREF

  if ( *(&serial_port + a1) == nullptr )
  {
    v1 = (void *)SMemAlloc(a1: 100, a2: aSerialCpp_5, a3: 159, a4: 0);
    *(&serial_port + a1) = operator new(__formal: 100, ptr: v1);
    if ( *(&serial_port + a1) == nullptr )
      return 0;
    memset(a1: *(&serial_port + a1), Val: 0, Size: 0x64u);
  }
  if ( serial_event[a1] == nullptr )
  {
    serial_event[a1] = CreateEventA(
                         lpEventAttributes: nullptr,
                         bManualReset: true,
                         bInitialState: false,
                         lpName: nullptr);
    if ( serial_event[a1] == nullptr )
      return 0;
    *((_DWORD *)*(&serial_port + a1) + 5) = serial_event[a1];
  }
  _wsprintfA(a1: portname, a2: "\\\\.\\COM%u", a1 + 1);
  *(_DWORD *)*(&serial_port + a1) = CreateFileA(
                                      lpFileName: portname,
                                      dwDesiredAccess: 0xC0000000,
                                      dwShareMode: 1u,
                                      lpSecurityAttributes: nullptr,
                                      dwCreationDisposition: 3u,
                                      dwFlagsAndAttributes: 0x40000000u,
                                      hTemplateFile: nullptr);
  if ( *(_DWORD *)*(&serial_port + a1) != -1 )
  {
    memset(a1: &dcb, Val: 0, Size: sizeof(dcb));
    dcb.DCBlength = 28;
    dcb.BaudRate = 56000;
    SRegLoadValue(a1: aNetworkProvide, a2: aBaudRate, a3: 0, a4: &dcb.BaudRate);
    *((_DWORD *)&dcb + 2) |= 1u;
    *((_DWORD *)&dcb + 2) &= 0xFFFFFFCF;
    v3 = *((_DWORD *)&dcb + 2);
    BYTE1(v3) = *((_BYTE *)&dcb + 9) & 0xCF;
    *((_DWORD *)&dcb + 2) = v3;
    dcb.ByteSize = 8;
    dcb.Parity = 0;
    dcb.StopBits = 0;
    dcb.ErrorChar = 63;
    dcb.EofChar = 26;
    SetCommState(hFile: *(HANDLE *)*(&serial_port + a1), lpDCB: &dcb);
    memset(a1: &timeouts, Val: 0, Size: sizeof(timeouts));
    timeouts.ReadIntervalTimeout = 5;
    SetCommTimeouts(hFile: *(HANDLE *)*(&serial_port + a1), lpCommTimeouts: &timeouts);
    ReadFile(
      hFile: *(HANDLE *)*(&serial_port + a1),
      lpBuffer: (char *)*(&serial_port + a1) + 24,
      nNumberOfBytesToRead: 0x40u,
      lpNumberOfBytesRead: (LPDWORD)*(&serial_port + a1) + 22,
      lpOverlapped: (LPOVERLAPPED)((char *)*(&serial_port + a1) + 4));
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18009113
 * CodeView symbol: PickRandomNumber
 * Demangled: PickRandomNumber
 * IDA name: PickRandomNumber
 * CodeView module: 4
 * CV address: segment 1 : 0x8113
 * Code length: 0x96
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int PickRandomNumber()
{
  LARGE_INTEGER perfcount; // [esp+0h] [ebp-18h] BYREF
  struct tagPOINT pos; // [esp+8h] [ebp-10h] BYREF
  int rand1; // [esp+14h] [ebp-4h]

  QueryPerformanceCounter(lpPerformanceCount: &perfcount);
  GetCursorPos(lpPoint: &pos);
  seed ^= pos.y ^ pos.x ^ GetTickCount() ^ perfcount.LowPart;
  seed = (125 * seed + 3) % 0x2AAAABu;
  rand1 = (unsigned __int16)seed;
  seed = (125 * seed + 3) % 0x2AAAABu;
  return (unsigned __int16)seed | (rand1 << 16);
}


// ==============================================================================
/*
 * EA: 0x180091A9
 * CodeView symbol: SendMessageA
 * Demangled: SendMessageA
 * IDA name: SendMessageA
 * CodeView module: 4
 * CV address: segment 1 : 0x81A9
 * Code length: 0x13A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SendMessageA(char a1, char a2, const void *data, size_t databytes)
{
  _BYTE messagedata[516]; // [esp+8h] [ebp-208h] BYREF
  BOOL systemmessage; // [esp+20Ch] [ebp-4h]

  systemmessage = a1 != 0;
  if ( a1 != 0 )
  {
    a2 = -1;
  }
  else if ( serial_networkid == 0 || serial_networkidlocked == 0 )
  {
    return 0;
  }
  memset(a1: messagedata, Val: 0, Size: sizeof(messagedata));
  messagedata[0] = (a1 != 0) - 2;
  messagedata[1] = (databytes + 7) >> 2;
  messagedata[3] = messagedata[3] & 0xF | 0x40;
  messagedata[2] = a2;
  if ( systemmessage )
    messagedata[3] = a1 & 0xF | messagedata[3] & 0xF0;
  else
    messagedata[3] = serial_networkid & 0xF | messagedata[3] & 0xF0;
  if ( data != nullptr && databytes != 0 )
    memcpy(a1: &messagedata[4], Src: data, Size: databytes);
  SendFormedMessage(wait: 1);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x180092E3
 * CodeView symbol: SendFormedMessage
 * Demangled: SendFormedMessage
 * IDA name: SendFormedMessage
 * CodeView module: 4
 * CV address: segment 1 : 0x82E3
 * Code length: 0x1BE
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
DWORD __fastcall SendFormedMessage(unsigned __int8 *a1, int a2, int wait)
{
  DWORD result; // eax
  unsigned int port; // [esp+8h] [ebp-ECh]
  HANDLE event[56]; // [esp+Ch] [ebp-E8h] BYREF
  BOOL systemmessage; // [esp+ECh] [ebp-8h]
  DWORD numsends; // [esp+F0h] [ebp-4h]

  result = (DWORD)a1;
  systemmessage = *a1 == 255;
  numsends = 0;
  for ( port = 0; port < 8; ++port )
  {
    if ( port != a2
      && *(_DWORD *)*(&serial_port + port) != -1
      && (systemmessage || (a1[2] & *((_BYTE *)*(&serial_port + port) + 96)) != 0) )
    {
      memset(a1: &event[5 * numsends + 16], Val: 0, Size: 0x14u);
      if ( wait != 0 )
        event[numsends] = CreateEventA(
                            lpEventAttributes: nullptr,
                            bManualReset: false,
                            bInitialState: false,
                            lpName: nullptr);
      event[5 * numsends + 20] = event[numsends];
      WriteFile(
        hFile: *(HANDLE *)*(&serial_port + port),
        lpBuffer: a1,
        nNumberOfBytesToWrite: 4 * a1[1],
        lpNumberOfBytesWritten: (LPDWORD)&event[numsends + 8],
        lpOverlapped: (LPOVERLAPPED)&event[5 * numsends + 16]);
      ++numsends;
      PerfIncrement(a1: 0);
      PerfAdd(a1: 2u, a2: 4 * a1[1]);
    }
    result = port + 1;
  }
  if ( numsends != 0 && wait != 0 )
  {
    WaitForMultipleObjects(nCount: numsends, lpHandles: event, bWaitAll: true, dwMilliseconds: 0xFFFFFFFF);
    while ( 1 )
    {
      result = numsends--;
      if ( result == 0 )
        break;
      CloseHandle(hObject: event[numsends]);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x180094A1
 * CodeView symbol: ThreadProc
 * Demangled: ThreadProc
 * IDA name: ThreadProc
 * CodeView module: 4
 * CV address: segment 1 : 0x84A1
 * Code length: 0x462
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall __noreturn ThreadProc(void *__formal)
{
  void *v1; // eax
  size_t v2; // [esp+0h] [ebp-38h]
  size_t v3; // [esp+4h] [ebp-34h]
  int v4; // [esp+8h] [ebp-30h]
  size_t v5; // [esp+Ch] [ebp-2Ch]
  int v6; // [esp+10h] [ebp-28h]
  _DWORD *messageptr; // [esp+20h] [ebp-18h]
  unsigned int bytesleft; // [esp+2Ch] [ebp-Ch]
  unsigned __int8 *dataptr; // [esp+30h] [ebp-8h]
  unsigned int port; // [esp+34h] [ebp-4h]

  while ( serial_state != 3 )
  {
    CCritSect::Enter(this: (CCritSect *)&serial_critsect);
    for ( port = 0; port < 8; ++port )
    {
      if ( *(_DWORD *)*(&serial_port + port) != -1
        && GetOverlappedResult(
             hFile: *(HANDLE *)*(&serial_port + port),
             lpOverlapped: (LPOVERLAPPED)((char *)*(&serial_port + port) + 4),
             lpNumberOfBytesTransferred: (LPDWORD)*(&serial_port + port) + 22,
             bWait: false) )
      {
        bytesleft = *((_DWORD *)*(&serial_port + port) + 22);
        dataptr = (unsigned __int8 *)*(&serial_port + port) + 24;
        while ( bytesleft != 0 )
        {
          if ( *((_DWORD *)*(&serial_port + port) + 23) != 0 )
          {
            if ( *(_DWORD *)(*((_DWORD *)*(&serial_port + port) + 23) + 532) == 0 )
            {
              if ( 4 * (unsigned int)*dataptr <= 0x204 )
                v6 = 4 * *dataptr;
              else
                v6 = 516;
              *(_DWORD *)(*((_DWORD *)*(&serial_port + port) + 23) + 532) = v6;
            }
            if ( bytesleft >= *(_DWORD *)(*((_DWORD *)*(&serial_port + port) + 23) + 532)
                            - *(_DWORD *)(*((_DWORD *)*(&serial_port + port) + 23) + 536) )
              v5 = *(_DWORD *)(*((_DWORD *)*(&serial_port + port) + 23) + 532)
                 - *(_DWORD *)(*((_DWORD *)*(&serial_port + port) + 23) + 536);
            else
              v5 = bytesleft;
            memcpy(
              a1: (void *)(*(_DWORD *)(*((_DWORD *)*(&serial_port + port) + 23) + 536)
                     + *((_DWORD *)*(&serial_port + port) + 23)
                     + 16),
              Src: dataptr,
              Size: v5);
            *(_DWORD *)(*((_DWORD *)*(&serial_port + port) + 23) + 536) += v5;
            dataptr += v5;
            bytesleft -= v5;
            if ( *(_DWORD *)(*((_DWORD *)*(&serial_port + port) + 23) + 536) >= *(_DWORD *)(*((_DWORD *)*(&serial_port + port)
                                                                                            + 23)
                                                                                          + 532) )
            {
              ProcessIncomingMessage(a1: port, a2: *((_DWORD *)*(&serial_port + port) + 23));
              *((_DWORD *)*(&serial_port + port) + 23) = 0;
            }
          }
          while ( bytesleft != 0 && *dataptr < 0xFEu )
          {
            ++dataptr;
            --bytesleft;
          }
          if ( bytesleft != 0 )
          {
            v1 = (void *)SMemAlloc(a1: 548, a2: aSerialCpp_6, a3: 574, a4: 0);
            messageptr = operator new(__formal: 36, ptr: v1);
            if ( messageptr != nullptr )
            {
              memset(a1: messageptr, Val: 0, Size: 0x224u);
              messageptr[135] = port;
              if ( bytesleft < 2 )
                v4 = 0;
              else
                v4 = 4 * dataptr[1];
              messageptr[133] = v4;
              if ( messageptr[133] > 0x204u )
                messageptr[133] = 516;
              if ( messageptr[133] != 0 )
              {
                if ( bytesleft >= messageptr[133] )
                  v3 = messageptr[133];
                else
                  v3 = bytesleft;
                v2 = v3;
              }
              else
              {
                v2 = 1;
              }
              memcpy(a1: messageptr + 4, Src: dataptr, Size: v2);
              dataptr += v2;
              bytesleft -= v2;
              messageptr[134] = v2;
              if ( messageptr[133] != 0 && messageptr[134] >= messageptr[133] )
                ProcessIncomingMessage(a1: port, a2: messageptr);
              else
                *((_DWORD *)*(&serial_port + port) + 23) = messageptr;
            }
            else
            {
              bytesleft = 0;
            }
          }
        }
        ResetEvent(hEvent: serial_event[port]);
        ReadFile(
          hFile: *(HANDLE *)*(&serial_port + port),
          lpBuffer: (char *)*(&serial_port + port) + 24,
          nNumberOfBytesToRead: 0x40u,
          lpNumberOfBytesRead: (LPDWORD)*(&serial_port + port) + 22,
          lpOverlapped: (LPOVERLAPPED)((char *)*(&serial_port + port) + 4));
      }
    }
    CCritSect::Leave(this: (CCritSect *)&serial_critsect);
    WaitForMultipleObjects(nCount: 8u, lpHandles: serial_event, bWaitAll: false, dwMilliseconds: 0xFFFFFFFF);
  }
  serial_state = 0;
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x18009903
 * CodeView symbol: ProcessIncomingMessage
 * Demangled: ProcessIncomingMessage
 * IDA name: ProcessIncomingMessage
 * CodeView module: 4
 * CV address: segment 1 : 0x8903
 * Code length: 0x3AB
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ProcessIncomingMessage(unsigned int a1, int a2)
{
  int result; // eax
  int v3; // ecx
  int v6; // [esp+Ch] [ebp-18h]
  int localnetworkid; // [esp+10h] [ebp-14h] BYREF
  int routeloopcheck; // [esp+14h] [ebp-10h]
  int networkid; // [esp+18h] [ebp-Ch]
  BOOL systemmessage; // [esp+1Ch] [ebp-8h]
  int origtargetmask; // [esp+20h] [ebp-4h]

  PerfIncrement(a1: 1u);
  PerfAdd(a1: 3u, a2: *(_DWORD *)(a2 + 536));
  systemmessage = *(unsigned __int8 *)(a2 + 16) == 255;
  if ( a1 >= 8 )
    return SMemFree(a1: a2, a2: aSerialCpp_7, a3: 358, a4: 0);
  LOBYTE(origtargetmask) = *(_BYTE *)(a2 + 18);
  v3 = *(_BYTE *)(a2 + 19) >> 4;
  *(_BYTE *)(a2 + 19) = (16 * (((*(_BYTE *)(a2 + 19) >> 4) - 1) & 0xF)) | *(_BYTE *)(a2 + 19) & 0xF;
  if ( v3 != 0 )
  {
    if ( serial_networkid != 0 && serial_networkidlocked != 0 )
      *(_BYTE *)(a2 + 18) &= ~(unsigned __int8)(1 << serial_networkid);
    if ( *(_BYTE *)(a2 + 18) != 0 )
      SendFormedMessage(a1: (unsigned __int8 *)(a2 + 16), a2: a1, wait: 1);
  }
  if ( serial_networkid != 0
    && serial_networkidlocked != 0
    && (((1 << serial_networkid) & (unsigned __int8)origtargetmask) == 0
     || !systemmessage && (*(_BYTE *)(a2 + 19) & 0xF) == serial_networkid) )
  {
    return SMemFree(a1: a2, a2: aSerialCpp_8, a3: 380, a4: 0);
  }
  if ( systemmessage )
  {
    switch ( *(_BYTE *)(a2 + 19) & 0xF )
    {
      case 1:
        if ( *(_DWORD *)(a2 + 536) >= 0xCu )
        {
          networkid = *(_DWORD *)(a2 + 20);
          routeloopcheck = *(_DWORD *)(a2 + 24);
          if ( routeloopcheck != serial_routeloopcheck )
          {
            if ( serial_networkidlocked != 0 )
            {
              localnetworkid = (unsigned __int8)serial_networkid;
              SendMessageA(a1: 2, a2: 255, data: &localnetworkid, databytes: 4u);
            }
            else if ( networkid == (unsigned __int8)serial_networkid )
            {
              serial_networkid = 0;
            }
          }
        }
        break;
      case 2:
        if ( *(_DWORD *)(a2 + 536) >= 8u )
        {
          v6 = *(_DWORD *)(a2 + 20);
          *((_BYTE *)*(&serial_port + a1) + 96) |= 1 << v6;
          if ( serial_networkidlocked == 0 && v6 == (unsigned __int8)serial_networkid )
            serial_networkid = 0;
        }
        break;
      case 9:
        if ( serial_gameadvinfo != nullptr )
          SendMessageA(a1: 10, a2: 255, data: serial_gameadvinfo, databytes: *((_DWORD *)serial_gameadvinfo + 133));
        break;
      case 0xA:
        if ( *(_DWORD *)(a2 + 536) >= 4u )
          RecvGameInfoPacket(a1: a2 + 20, a2: *(_DWORD *)(a2 + 536) - 4);
        break;
      case 0xB:
        if ( serial_gamelist != nullptr )
        {
          if ( *((_DWORD *)serial_gamelist + 75) != 0 )
            SMemFree(a1: *((_DWORD *)serial_gamelist + 75), a2: aSerialCpp_10, a3: 430, a4: 0);
          SMemFree(a1: serial_gamelist, a2: aSerialCpp_11, a3: 431, a4: 0);
          serial_gamelist = nullptr;
        }
        break;
      default:
        return SMemFree(a1: a2, a2: aSerialCpp_12, a3: 444, a4: 0);
    }
    return SMemFree(a1: a2, a2: aSerialCpp_12, a3: 444, a4: 0);
  }
  else if ( serial_networkid != 0 && serial_networkidlocked != 0 )
  {
    *(_BYTE *)a2 = *(_BYTE *)(a2 + 19) & 0xF;
    result = TListAddPtrEnd(a1: &serial_messagehead, (struct _MESSAGEREC *)a2);
    if ( serial_recvevent != nullptr )
      return SetEvent(hEvent: serial_recvevent);
  }
  else
  {
    return SMemFree(a1: a2, a2: aSerialCpp_9, a3: 393, a4: 0);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x18009CAE
 * CodeView symbol: RecvGameInfoPacket
 * Demangled: RecvGameInfoPacket
 * IDA name: RecvGameInfoPacket
 * CodeView module: 4
 * CV address: segment 1 : 0x8CAE
 * Code length: 0x2D3
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall RecvGameInfoPacket(_DWORD *a1, unsigned int a2)
{
  void *v2; // eax
  int v3; // eax
  int v4; // eax
  unsigned int packetbytes; // [esp+18h] [ebp-18h]
  unsigned __int16 checksum; // [esp+1Ch] [ebp-14h]
  unsigned int bytesleft; // [esp+20h] [ebp-10h]
  unsigned int byteslefta; // [esp+20h] [ebp-10h]
  unsigned int bytesleftb; // [esp+20h] [ebp-10h]
  unsigned int bytesleftc; // [esp+20h] [ebp-10h]
  unsigned int bytesleftd; // [esp+20h] [ebp-10h]
  size_t byteslefte; // [esp+20h] [ebp-10h]
  char *currptr; // [esp+24h] [ebp-Ch]
  char *currptra; // [esp+24h] [ebp-Ch]
  unsigned int *currptrb; // [esp+24h] [ebp-Ch]
  unsigned __int16 *currptrc; // [esp+24h] [ebp-Ch]
  const void *currptrd; // [esp+24h] [ebp-Ch]
  char *temp_gamelist; // [esp+2Ch] [ebp-4h]

  if ( a2 >= 0x12 )
  {
    if ( a1[1] != serial_programid )
    {
      serial_versionmismatch = 1;
      return;
    }
    v2 = (void *)SMemAlloc(a1: 316, a2: aSerialCpp_13, a3: 263, a4: 8);
    temp_gamelist = (char *)operator new(__formal: 60, ptr: v2);
    temp_gamelist[12] = *(_BYTE *)a1;
    *(_DWORD *)temp_gamelist = 1;
    *((_DWORD *)temp_gamelist + 78) = a1[2];
    *((_DWORD *)temp_gamelist + 77) = a1[1];
    bytesleft = a2 - 18;
    currptr = (char *)(a1 + 3);
    if ( a2 - 18 >= 0x80 )
      v3 = SStrCopy(a1: temp_gamelist + 40, a2: currptr, a3: 128);
    else
      v3 = SStrCopy(a1: temp_gamelist + 40, a2: currptr, a3: bytesleft);
    currptra = &currptr[v3 + 1];
    byteslefta = bytesleft - (v3 + 1);
    if ( byteslefta >= 0x80 )
      v4 = SStrCopy(a1: temp_gamelist + 168, a2: currptra, a3: 128);
    else
      v4 = SStrCopy(a1: temp_gamelist + 168, a2: currptra, a3: byteslefta);
    currptrb = (unsigned int *)&currptra[v4 + 1];
    bytesleftb = byteslefta - (v4 + 1);
    if ( bytesleftb >= 4 )
    {
      if ( bytesleftb < 6 )
        goto finallylabel;
      packetbytes = *currptrb;
      currptrc = (unsigned __int16 *)(currptrb + 1);
      bytesleftc = bytesleftb - 4;
      if ( (packetbytes + 3) >> 2 != a2 >> 2
        || (bytesleftd = packetbytes - a2 + bytesleftc) > 0x7FFFFFFF
        || (checksum = *currptrc,
            *currptrc = 0,
            currptrd = currptrc + 1,
            byteslefte = bytesleftd - 2,
            checksum != ComputeChecksum(a1, a2: packetbytes)) )
      {
finallylabel:
        if ( temp_gamelist != nullptr )
          SMemFree(a1: temp_gamelist, a2: aSerialCpp_17, a3: 345, a4: 0);
        return;
      }
      if ( byteslefte != 0 )
      {
        *((_DWORD *)temp_gamelist + 75) = SMemAlloc(a1: byteslefte, a2: aSerialCpp_14, a3: 329, a4: 0);
        *((_DWORD *)temp_gamelist + 76) = byteslefte;
        memcpy(a1: *((void **)temp_gamelist + 75), Src: currptrd, Size: byteslefte);
      }
    }
    if ( serial_gamelist != nullptr )
    {
      if ( *((_DWORD *)serial_gamelist + 75) != 0 )
        SMemFree(a1: *((_DWORD *)serial_gamelist + 75), a2: aSerialCpp_15, a3: 337, a4: 0);
      *((_DWORD *)serial_gamelist + 75) = 0;
      SMemFree(a1: serial_gamelist, a2: aSerialCpp_16, a3: 339, a4: 0);
    }
    serial_gamelist = temp_gamelist;
    temp_gamelist = nullptr;
    goto finallylabel;
  }
}


// ==============================================================================
/*
 * EA: 0x18009F81
 * CodeView symbol: SerialInitializeDevice
 * Demangled: SerialInitializeDevice
 * IDA name: ?SerialInitializeDevice@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x8F81
 * Code length: 0x9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialInitializeDevice(
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
 * EA: 0x18009F8A
 * CodeView symbol: SerialLockDeviceList
 * Demangled: SerialLockDeviceList
 * IDA name: ?SerialLockDeviceList@@YGHPAPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x8F8A
 * Code length: 0x26
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialLockDeviceList(struct _SNETSPI_DEVICELIST **devicelist)
{
  if ( devicelist != nullptr )
  {
    *devicelist = nullptr;
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
 * EA: 0x18009FB0
 * CodeView symbol: SerialLockGameList
 * Demangled: SerialLockGameList
 * IDA name: ?SerialLockGameList@@YGHKKPAPAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x8FB0
 * Code length: 0x49
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialLockGameList(char categorybits, char categorymask, struct _SNETSPI_GAMELIST **gamelist)
{
  if ( gamelist != nullptr )
  {
    if ( serial_versionmismatch != 0 )
    {
      SErrSetLastError(a1: -2062548871);
      return 0;
    }
    else
    {
      CCritSect::Enter(this: (CCritSect *)&serial_critsect);
      *gamelist = (struct _SNETSPI_GAMELIST *)serial_gamelist;
      return 1;
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
 * EA: 0x18009FF9
 * CodeView symbol: SerialReceive
 * Demangled: SerialReceive
 * IDA name: ?SerialReceive@@YGHPAPAU_SNETADDR@@PAPAXPAK@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x8FF9
 * Code length: 0xC8
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialReceive(struct _SNETADDR **addr, void **data, unsigned int *databytes)
{
  if ( addr != nullptr )
    *addr = nullptr;
  if ( data != nullptr )
    *data = nullptr;
  if ( databytes != nullptr )
    *databytes = 0;
  if ( addr != nullptr && data != nullptr && databytes != nullptr )
  {
    if ( serial_messagehead != nullptr )
    {
      CCritSect::Enter(this: (CCritSect *)&serial_critsect);
      *addr = serial_messagehead;
      *data = (char *)serial_messagehead + 20;
      *databytes = *((_DWORD *)serial_messagehead + 134) - 4;
      TListFreePtr(a1: &serial_messagehead, a2: serial_messagehead, a3: aSerialCpp_18, a4: 899);
      CCritSect::Leave(this: (CCritSect *)&serial_critsect);
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
 * EA: 0x1800A0C1
 * CodeView symbol: SerialReceiveExternalMessage
 * Demangled: SerialReceiveExternalMessage
 * IDA name: ?SerialReceiveExternalMessage@@YGHPAPBD00@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x90C1
 * Code length: 0x40
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialReceiveExternalMessage(const char **senderpath, const char **sendername, const char **message)
{
  if ( senderpath != nullptr )
    *senderpath = nullptr;
  if ( sendername != nullptr )
    *sendername = nullptr;
  if ( message != nullptr )
    *message = nullptr;
  SErrSetLastError(a1: -2062548885);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1800A101
 * CodeView symbol: SerialSelectGame
 * Demangled: SerialSelectGame
 * IDA name: ?SerialSelectGame@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAK@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x9101
 * Code length: 0x114
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialSelectGame(
        char flags,
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        unsigned int *playerid)
{
  _DWORD createdata[4]; // [esp+0h] [ebp-14Ch] BYREF
  _DWORD gameinfo[79]; // [esp+10h] [ebp-13Ch] BYREF

  if ( serial_gamelist == nullptr )
    Sleep(dwMilliseconds: 0x1F4u);
  CCritSect::Enter(this: (CCritSect *)&serial_critsect);
  if ( serial_gamelist != nullptr )
    memcpy(a1: gameinfo, Src: serial_gamelist, Size: sizeof(gameinfo));
  else
    memset(a1: gameinfo, Val: 0, Size: sizeof(gameinfo));
  CCritSect::Leave(this: (CCritSect *)&serial_critsect);
  if ( serial_gamelist != nullptr )
    return SNetJoinGame(
             a1: gameinfo[0],
             a2: &gameinfo[10],
             a3: 0,
             a4: *((_DWORD *)playerdata + 1),
             a5: *((_DWORD *)playerdata + 2),
             a6: playerid);
  if ( interfacedata == nullptr || *((_DWORD *)interfacedata + 5) == 0 )
    return 0;
  createdata[0] = 16;
  createdata[1] = 1396916812;
  createdata[2] = serial_maxplayers;
  createdata[3] = 0;
  return (*((int (__stdcall **)(_DWORD *, struct _SNETPROGRAMDATA *, struct _SNETPLAYERDATA *, struct _SNETUIDATA *, struct _SNETVERSIONDATA *, unsigned int *))interfacedata
          + 5))(
           a1: createdata,
           a2: programdata,
           a3: playerdata,
           a4: interfacedata,
           a5: versiondata,
           a6: playerid);
}


// ==============================================================================
/*
 * EA: 0x1800A215
 * CodeView symbol: SerialSend
 * Demangled: SerialSend
 * IDA name: ?SerialSend@@YGHKPAPAU_SNETADDR@@PAXK@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x9215
 * Code length: 0x73
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialSend(unsigned int addresses, struct _SNETADDR **addrlist, void *data, size_t databytes)
{
  unsigned int v5; // eax
  char targetmask; // [esp+0h] [ebp-4h]

  if ( addresses != 0 && addrlist != nullptr && data != nullptr && databytes != 0 )
  {
    for ( targetmask = 0; ; targetmask |= 1 << *(_BYTE *)addrlist[addresses] )
    {
      v5 = addresses--;
      if ( v5 == 0 )
        break;
    }
    return SendMessageA(a1: 0, a2: targetmask, data, databytes);
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1800A288
 * CodeView symbol: SerialSendExternalMessage
 * Demangled: SerialSendExternalMessage
 * IDA name: ?SerialSendExternalMessage@@YGHPBD0000@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x9288
 * Code length: 0x9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialSendExternalMessage(
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
 * EA: 0x1800A291
 * CodeView symbol: SerialStartAdvertisingGame
 * Demangled: SerialStartAdvertisingGame
 * IDA name: ?SerialStartAdvertisingGame@@YGHPBD00KKKKKPBXK@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x9291
 * Code length: 0x2DE
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialStartAdvertisingGame(
        const char *gamename,
        const char *gamepassword,
        const char *gamedescription,
        char gamemode,
        char gameage,
        char gamecategorybits,
        char optcategorybits,
        char ladderid,
        const void *clientdata,
        size_t clientdatabytes)
{
  void *v11; // eax
  void *v12; // eax
  size_t v13; // esi
  unsigned int bytes; // [esp+14h] [ebp-Ch]
  char *currptra; // [esp+18h] [ebp-8h]
  int currptrb; // [esp+18h] [ebp-8h]
  int currptrc; // [esp+18h] [ebp-8h]
  void *currptr; // [esp+18h] [ebp-8h]
  _WORD *checksumptr; // [esp+1Ch] [ebp-4h]

  if ( gamename == nullptr || gamedescription == nullptr )
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
  CCritSect::Enter(this: (CCritSect *)&serial_critsect);
  if ( serial_gamelist != nullptr )
  {
    if ( (gamemode & 4) == 0 )
    {
      CCritSect::Leave(this: (CCritSect *)&serial_critsect);
      SErrSetLastError(a1: 68);
      return 0;
    }
    if ( *((_DWORD *)serial_gamelist + 75) != 0 )
      SMemFree(a1: *((_DWORD *)serial_gamelist + 75), a2: aSerialCpp_19, a3: 1027, a4: 0);
    SMemFree(a1: serial_gamelist, a2: aSerialCpp_20, a3: 1028, a4: 0);
    serial_gamelist = nullptr;
  }
  if ( serial_gameadvinfo != nullptr )
    SerialStopAdvertisingGame();
  if ( serial_gameadvinfo != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aSerialCpp_21, a3: 1042, a4: aSerialGameadvi, a5: 0, a6: 1);
  if ( serial_gamelist != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aSerialCpp_22, a3: 1043, a4: aSerialGamelist, a5: 0, a6: 1);
  v11 = (void *)SMemAlloc(a1: 536, a2: aSerialCpp_23, a3: 1044, a4: 0);
  serial_gameadvinfo = operator new(__formal: 24, ptr: v11);
  v12 = (void *)SMemAlloc(a1: 316, a2: aSerialCpp_24, a3: 1045, a4: 0);
  serial_gamelist = operator new(__formal: 60, ptr: v12);
  memset(a1: serial_gameadvinfo, Val: 0, Size: 0x218u);
  memset(a1: serial_gamelist, Val: 0, Size: 0x13Cu);
  *(_DWORD *)serial_gameadvinfo = (unsigned __int8)serial_networkid;
  *((_DWORD *)serial_gameadvinfo + 1) = serial_programid;
  *((_DWORD *)serial_gameadvinfo + 2) = serial_versionid;
  currptra = (char *)serial_gameadvinfo + 12;
  currptrb = (int)&currptra[SStrCopy(a1: (char *)serial_gameadvinfo + 12, a2: gamename, a3: 128) + 1];
  currptrc = currptrb + SStrCopy(a1: currptrb, a2: gamedescription, a3: 128) + 1;
  v13 = strlen(Str: gamename);
  bytes = strlen(Str: gamedescription) + v13 + clientdatabytes + 26;
  *((_DWORD *)serial_gameadvinfo + 133) = bytes;
  *(_DWORD *)currptrc = bytes;
  checksumptr = (_WORD *)(currptrc + 4);
  *(_WORD *)(currptrc + 4) = 0;
  currptr = (void *)(currptrc + 6);
  if ( clientdatabytes != 0 )
    memcpy(a1: currptr, Src: clientdata, Size: clientdatabytes);
  *checksumptr = ComputeChecksum(a1: serial_gameadvinfo, a2: bytes);
  CCritSect::Leave(this: (CCritSect *)&serial_critsect);
  SendMessageA(a1: 10, a2: 255, data: serial_gameadvinfo, databytes: bytes);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800A56F
 * CodeView symbol: SerialStopAdvertisingGame
 * Demangled: SerialStopAdvertisingGame
 * IDA name: ?SerialStopAdvertisingGame@@YGHXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x956F
 * Code length: 0xDD
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialStopAdvertisingGame()
{
  if ( serial_gameadvinfo != nullptr )
  {
    CCritSect::Enter(this: (CCritSect *)&serial_critsect);
    if ( serial_gameadvinfo != nullptr )
    {
      SendMessageA(a1: 11, a2: 255, data: serial_gameadvinfo, databytes: *((_DWORD *)serial_gameadvinfo + 133));
      SMemFree(a1: serial_gameadvinfo, a2: aSerialCpp_25, a3: 1107, a4: 0);
      serial_gameadvinfo = nullptr;
    }
    if ( serial_gamelist != nullptr )
    {
      if ( *((_DWORD *)serial_gamelist + 75) != 0 )
        SMemFree(a1: *((_DWORD *)serial_gamelist + 75), a2: aSerialCpp_26, a3: 1111, a4: 0);
      SMemFree(a1: serial_gamelist, a2: aSerialCpp_27, a3: 1112, a4: 0);
      serial_gamelist = nullptr;
    }
    CCritSect::Leave(this: (CCritSect *)&serial_critsect);
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 288);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1800A64C
 * CodeView symbol: SerialUnlockDeviceList
 * Demangled: SerialUnlockDeviceList
 * IDA name: ?SerialUnlockDeviceList@@YGHPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x964C
 * Code length: 0xC
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialUnlockDeviceList(struct _SNETSPI_DEVICELIST *devicelist)
{
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800A658
 * CodeView symbol: SerialUnlockGameList
 * Demangled: SerialUnlockGameList
 * IDA name: ?SerialUnlockGameList@@YGHPAU_SNETSPI_GAMELIST@@PAK@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x9658
 * Code length: 0x32
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Serial.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Serial.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SerialUnlockGameList(struct _SNETSPI_GAMELIST *gamelist, unsigned int *hintnextcall)
{
  CCritSect::Leave(this: (CCritSect *)&serial_critsect);
  if ( hintnextcall != nullptr )
    *hintnextcall = gamelist != nullptr ? 0 : 500;
  return 1;
}

