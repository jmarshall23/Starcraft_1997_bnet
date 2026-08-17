/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 117
 */


// ==============================================================================
/*
 * EA: 0x19017820
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: $E4
 * CodeView module: 6
 * CV address: segment 1 : 0x16820
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
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
 * EA: 0x1901782F
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: $E1
 * CodeView module: 6
 * CV address: segment 1 : 0x1682F
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *_E1()
{
  return CCritSect::CCritSect(this: (CCritSect *)&srv_connectcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901783E
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: $E3
 * CodeView module: 6
 * CV address: segment 1 : 0x1683E
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E3()
{
  return atexit(a1: _E2);
}


// ==============================================================================
/*
 * EA: 0x19017850
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: $E2
 * CodeView module: 6
 * CV address: segment 1 : 0x16850
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl _E2()
{
  CCritSect::~CCritSect(this: &srv_connectcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901785F
 * CodeView symbol: $E9
 * Demangled: $E9
 * IDA name: $E9
 * CodeView module: 6
 * CV address: segment 1 : 0x1685F
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E9()
{
  _E6();
  return _E8();
}


// ==============================================================================
/*
 * EA: 0x1901786E
 * CodeView symbol: $E6
 * Demangled: $E6
 * IDA name: $E6
 * CodeView module: 6
 * CV address: segment 1 : 0x1686E
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *_E6()
{
  return CCritSect::CCritSect(this: (CCritSect *)&srv_patchcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901787D
 * CodeView symbol: $E8
 * Demangled: $E8
 * IDA name: $E8
 * CodeView module: 6
 * CV address: segment 1 : 0x1687D
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E8()
{
  return atexit(a1: _E7);
}


// ==============================================================================
/*
 * EA: 0x1901788F
 * CodeView symbol: $E7
 * Demangled: $E7
 * IDA name: $E7
 * CodeView module: 6
 * CV address: segment 1 : 0x1688F
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl _E7()
{
  CCritSect::~CCritSect(this: &srv_patchcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901789E
 * CodeView symbol: $E14
 * Demangled: $E14
 * IDA name: $E14
 * CodeView module: 6
 * CV address: segment 1 : 0x1689E
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E14()
{
  _E11();
  return _E13();
}


// ==============================================================================
/*
 * EA: 0x190178AD
 * CodeView symbol: $E11
 * Demangled: $E11
 * IDA name: $E11
 * CodeView module: 6
 * CV address: segment 1 : 0x168AD
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *_E11()
{
  return CCritSect::CCritSect(this: (CCritSect *)&srv_pingcritsect);
}


// ==============================================================================
/*
 * EA: 0x190178BC
 * CodeView symbol: $E13
 * Demangled: $E13
 * IDA name: $E13
 * CodeView module: 6
 * CV address: segment 1 : 0x168BC
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E13()
{
  return atexit(a1: _E12);
}


// ==============================================================================
/*
 * EA: 0x190178CE
 * CodeView symbol: $E12
 * Demangled: $E12
 * IDA name: $E12
 * CodeView module: 6
 * CV address: segment 1 : 0x168CE
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl _E12()
{
  CCritSect::~CCritSect(this: &srv_pingcritsect);
}


// ==============================================================================
/*
 * EA: 0x190178DD
 * CodeView symbol: $E19
 * Demangled: $E19
 * IDA name: $E19
 * CodeView module: 6
 * CV address: segment 1 : 0x168DD
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E19()
{
  _E16();
  return _E18();
}


// ==============================================================================
/*
 * EA: 0x190178EC
 * CodeView symbol: $E16
 * Demangled: $E16
 * IDA name: $E16
 * CodeView module: 6
 * CV address: segment 1 : 0x168EC
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *_E16()
{
  return CCritSect::CCritSect(this: (CCritSect *)&srv_uinotificationcritsect);
}


// ==============================================================================
/*
 * EA: 0x190178FB
 * CodeView symbol: $E18
 * Demangled: $E18
 * IDA name: $E18
 * CodeView module: 6
 * CV address: segment 1 : 0x168FB
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E18()
{
  return atexit(a1: _E17);
}


// ==============================================================================
/*
 * EA: 0x1901790D
 * CodeView symbol: $E17
 * Demangled: $E17
 * IDA name: $E17
 * CodeView module: 6
 * CV address: segment 1 : 0x1690D
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl _E17()
{
  CCritSect::~CCritSect(this: &srv_uinotificationcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901791C
 * CodeView symbol: $E24
 * Demangled: $E24
 * IDA name: $E24
 * CodeView module: 6
 * CV address: segment 1 : 0x1691C
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E24()
{
  _E21();
  return _E23();
}


// ==============================================================================
/*
 * EA: 0x1901792B
 * CodeView symbol: $E21
 * Demangled: $E21
 * IDA name: $E21
 * CodeView module: 6
 * CV address: segment 1 : 0x1692B
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *_E21()
{
  return CCritSect::CCritSect(this: (CCritSect *)&srv_newsocketcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901793A
 * CodeView symbol: $E23
 * Demangled: $E23
 * IDA name: $E23
 * CodeView module: 6
 * CV address: segment 1 : 0x1693A
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E23()
{
  return atexit(a1: _E22);
}


// ==============================================================================
/*
 * EA: 0x1901794C
 * CodeView symbol: $E22
 * Demangled: $E22
 * IDA name: $E22
 * CodeView module: 6
 * CV address: segment 1 : 0x1694C
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl _E22()
{
  CCritSect::~CCritSect(this: &srv_newsocketcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901795B
 * CodeView symbol: $E29
 * Demangled: $E29
 * IDA name: $E29
 * CodeView module: 6
 * CV address: segment 1 : 0x1695B
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E29()
{
  _E26();
  return _E28();
}


// ==============================================================================
/*
 * EA: 0x1901796A
 * CodeView symbol: $E26
 * Demangled: $E26
 * IDA name: $E26
 * CodeView module: 6
 * CV address: segment 1 : 0x1696A
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *_E26()
{
  return CCritSect::CCritSect(this: (CCritSect *)&srv_sendcritsect);
}


// ==============================================================================
/*
 * EA: 0x19017979
 * CodeView symbol: $E28
 * Demangled: $E28
 * IDA name: $E28
 * CodeView module: 6
 * CV address: segment 1 : 0x16979
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E28()
{
  return atexit(a1: _E27);
}


// ==============================================================================
/*
 * EA: 0x1901798B
 * CodeView symbol: $E27
 * Demangled: $E27
 * IDA name: $E27
 * CodeView module: 6
 * CV address: segment 1 : 0x1698B
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl _E27()
{
  CCritSect::~CCritSect(this: &srv_sendcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901799A
 * CodeView symbol: SrvConnectToServer
 * Demangled: SrvConnectToServer
 * IDA name: ?SrvConnectToServer@@YIHXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x1699A
 * Code length: 0x277
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl SrvConnectToServer()
{
  HANDLE hObject; // [esp+0h] [ebp-1Ch]
  unsigned int ThrdAddr; // [esp+4h] [ebp-18h] BYREF
  char *target; // [esp+8h] [ebp-14h]
  int error; // [esp+Ch] [ebp-10h]
  HANDLE threadhandle; // [esp+10h] [ebp-Ch]
  unsigned int threadid; // [esp+14h] [ebp-8h] BYREF
  char *server; // [esp+18h] [ebp-4h]

  srv_shutdownevent = CreateEventA(
                        lpEventAttributes: nullptr,
                        bManualReset: true,
                        bInitialState: false,
                        lpName: nullptr);
  SRegLoadValue(a1: aConfiguration, a2: aServerVersion, a3: 2, a4: &serverversion);
  if ( serverversion == 1 )
    SRegLoadString(a1: aConfiguration_0, a2: aServerList, a3: 2, a4: &serverlist, a5: 256);
  if ( serverlist == 0 )
    strcpy(Destination: &serverlist, Source: a20967136170Exo);
  srv_connectthreads = 0;
  for ( server = strtok(String: &serverlist, Delimiter: Delimiter);
        server != nullptr && *server != 0;
        server = strtok(String: nullptr, Delimiter: asc_1903D7C4) )
  {
    threadhandle = (HANDLE)_beginthreadex(
                             Security: nullptr,
                             StackSize: 0,
                             StartAddress: ConnectThreadProc,
                             ArgList: server,
                             InitFlag: 0,
                             ThrdAddr: &threadid);
    if ( threadhandle != nullptr )
    {
      InterlockedIncrement(lpAddend: &srv_connectthreads);
      CloseHandle(hObject: threadhandle);
    }
  }
  while ( srv_serversocket == -1 && srv_connectthreads > 0 )
  {
    if ( srv_cancelwait != 0 )
    {
      SErrSetLastError(a1: 1223);
      return 0;
    }
    if ( UiProcessWindowMessages() == 0 )
      return 0;
    Sleep(dwMilliseconds: 0xAu);
  }
  if ( srv_serversocket != -1 )
    return 1;
  srv_connectthreads = 0;
  srv_pingsuccess = 0;
  SRegLoadString(a1: aConfiguration_1, a2: aPingList, a3: 2, a4: &pinglist, a5: 256);
  if ( pinglist == 0 )
    strcpy(Destination: &pinglist, Source: a20967136167209);
  srv_pingthreads = 0;
  for ( target = strtok(String: &pinglist, Delimiter: asc_1903D810);
        target != nullptr && *target != 0;
        target = strtok(String: nullptr, Delimiter: asc_1903D814) )
  {
    hObject = (HANDLE)_beginthreadex(
                        Security: nullptr,
                        StackSize: 0,
                        StartAddress: PingThreadProc,
                        ArgList: target,
                        InitFlag: 0,
                        &ThrdAddr);
    if ( hObject != nullptr )
    {
      InterlockedIncrement(lpAddend: &srv_pingthreads);
      CloseHandle(hObject);
    }
  }
  while ( srv_pingthreads > 0 )
  {
    if ( srv_cancelwait != 0 )
    {
      SErrSetLastError(a1: 1223);
      return 0;
    }
    if ( UiProcessWindowMessages() == 0 )
      return 0;
    Sleep(dwMilliseconds: 0xAu);
  }
  if ( srv_pingsuccess != 0 )
    error = 1;
  else
    error = 4;
  QueueUiNotification(parambytes: 4, string1ptr: 0, string2ptr: 0);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x19017C11
 * CodeView symbol: ConnectThreadProc
 * Demangled: ConnectThreadProc
 * IDA name: ConnectThreadProc
 * CodeView module: 6
 * CV address: segment 1 : 0x16C11
 * Code length: 0x188
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall __noreturn ConnectThreadProc(const char *param)
{
  struct sockaddr addr; // [esp+0h] [ebp-1Ch] BYREF
  struct hostent *host; // [esp+10h] [ebp-Ch]
  unsigned int ipaddress; // [esp+14h] [ebp-8h]
  SOCKET newsocket; // [esp+18h] [ebp-4h]

  newsocket = socket(af: 2, type: 1, protocol: 0);
  if ( srv_serversocket != -1 || newsocket == -1 )
  {
    if ( newsocket != -1 )
      closesocket(s: newsocket);
    InterlockedDecrement(lpAddend: &srv_connectthreads);
    _endthreadex(ReturnCode: 0);
  }
  AddConnectThread(a1: newsocket);
  if ( isdigit(C: *param) != 0 )
  {
    ipaddress = inet_addr(cp: param);
  }
  else
  {
    host = gethostbyname(name: param);
    if ( srv_serversocket != -1 || host == nullptr )
    {
      DeleteConnectThread(a1: newsocket);
      closesocket(s: newsocket);
      InterlockedDecrement(lpAddend: &srv_connectthreads);
      _endthreadex(ReturnCode: 0);
    }
    ipaddress = **(_DWORD **)host->h_addr_list;
  }
  memset(a1: &addr, Val: 0, Size: sizeof(addr));
  addr.sa_family = 2;
  *(_WORD *)addr.sa_data = htons(hostshort: 0x17E0u);
  *(_DWORD *)&addr.sa_data[2] = ipaddress;
  if ( connect(s: newsocket, name: &addr, namelen: 16) != 0 )
  {
    DeleteConnectThread(a1: newsocket);
    closesocket(s: newsocket);
    InterlockedDecrement(lpAddend: &srv_connectthreads);
    _endthreadex(ReturnCode: 0);
  }
  CCritSect::Enter(this: &srv_newsocketcritsect);
  if ( srv_serversocket == -1 )
    srv_serversocket = newsocket;
  else
    closesocket(s: newsocket);
  CCritSect::Leave(this: &srv_newsocketcritsect);
  DeleteConnectThread(a1: newsocket);
  InterlockedDecrement(lpAddend: &srv_connectthreads);
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x19017D99
 * CodeView symbol: AddConnectThread
 * Demangled: AddConnectThread
 * IDA name: AddConnectThread
 * CodeView module: 6
 * CV address: segment 1 : 0x16D99
 * Code length: 0x8D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall AddConnectThread(void *this)
{
  void *v1; // eax
  HANDLE v2; // eax
  HANDLE CurrentThread; // [esp-18h] [ebp-24h]
  HANDLE CurrentProcess; // [esp-14h] [ebp-20h]
  HANDLE *newptr; // [esp+8h] [ebp-4h]

  v1 = (void *)SMemAlloc(a1: 12, a2: aSrvCpp, a3: 266, a4: 0);
  newptr = (HANDLE *)operator new(__formal: 12, ptr: v1);
  if ( newptr != nullptr )
  {
    CurrentProcess = GetCurrentProcess();
    CurrentThread = GetCurrentThread();
    v2 = GetCurrentProcess();
    DuplicateHandle(
      hSourceProcessHandle: v2,
      hSourceHandle: CurrentThread,
      hTargetProcessHandle: CurrentProcess,
      lpTargetHandle: newptr + 1,
      dwDesiredAccess: 0,
      bInheritHandle: false,
      dwOptions: 2u);
    *newptr = this;
    CCritSect::Enter(this: &srv_connectcritsect);
    TListAddPtr(a1: &srv_connecthead, a2: (struct _CONNECTREC *)newptr);
    CCritSect::Leave(this: &srv_connectcritsect);
  }
}


// ==============================================================================
/*
 * EA: 0x19017E26
 * CodeView symbol: DeleteConnectThread
 * Demangled: DeleteConnectThread
 * IDA name: DeleteConnectThread
 * CodeView module: 6
 * CV address: segment 1 : 0x16E26
 * Code length: 0x73
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall DeleteConnectThread(void *this)
{
  struct _CONNECTREC *v1; // ecx
  struct _CONNECTREC **nextptr; // [esp+8h] [ebp-4h]

  CCritSect::Enter(this: &srv_connectcritsect);
  nextptr = &srv_connecthead;
  while ( *nextptr != nullptr )
  {
    if ( *(void **)*nextptr == this )
    {
      v1 = *nextptr;
      *nextptr = *((struct _CONNECTREC **)*nextptr + 2);
      SMemFree(a1: v1, a2: aSrvCpp_0, a3: 630, a4: 0);
    }
    else
    {
      nextptr = (struct _CONNECTREC **)((char *)*nextptr + 8);
    }
  }
  CCritSect::Leave(this: &srv_connectcritsect);
}


// ==============================================================================
/*
 * EA: 0x19017E99
 * CodeView symbol: PingThreadProc
 * Demangled: PingThreadProc
 * IDA name: PingThreadProc
 * CodeView module: 6
 * CV address: segment 1 : 0x16E99
 * Code length: 0x84
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall __noreturn PingThreadProc(const char *param)
{
  struct sockaddr addr; // [esp+0h] [ebp-14h] BYREF
  void *s; // [esp+10h] [ebp-4h]

  s = (void *)socket(af: 2, type: 1, protocol: 0);
  AddConnectThread(this: s);
  addr.sa_family = 2;
  *(_WORD *)addr.sa_data = htons(hostshort: 0x50u);
  *(_DWORD *)&addr.sa_data[2] = inet_addr(cp: param);
  if ( connect((SOCKET)s, name: &addr, namelen: 16) == 0 )
    srv_pingsuccess = 1;
  closesocket((SOCKET)s);
  DeleteConnectThread(this: s);
  InterlockedDecrement(lpAddend: &srv_pingthreads);
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x19017F1D
 * CodeView symbol: QueueUiNotification
 * Demangled: QueueUiNotification
 * IDA name: QueueUiNotification
 * CodeView module: 6
 * CV address: segment 1 : 0x16F1D
 * Code length: 0x1F7
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall QueueUiNotification(int a1, const void *a2, size_t parambytes, char *string1ptr, char *string2ptr)
{
  void *v5; // eax
  const char *v6; // [esp+0h] [ebp-3Ch]
  const char *v7; // [esp+4h] [ebp-38h]
  size_t string1len; // [esp+1Ch] [ebp-20h]
  const char *string2; // [esp+20h] [ebp-1Ch]
  int *nextptr; // [esp+24h] [ebp-18h]
  const char *string1; // [esp+28h] [ebp-14h]
  size_t string2len; // [esp+2Ch] [ebp-10h]
  void *newnotification; // [esp+30h] [ebp-Ch]
  char *newstring2; // [esp+34h] [ebp-8h]
  char *newstring1; // [esp+38h] [ebp-4h]

  if ( string1ptr != nullptr )
    v7 = *(const char **)string1ptr;
  else
    v7 = nullptr;
  string1 = v7;
  if ( string2ptr != nullptr )
    v6 = *(const char **)string2ptr;
  else
    v6 = nullptr;
  string2 = v6;
  if ( v7 == nullptr )
    string1 = (const char *)&unk_19043BAC;
  if ( v6 == nullptr )
    string2 = (const char *)&unk_19043BB0;
  string1len = strlen(Str: string1);
  string2len = strlen(Str: string2);
  v5 = (void *)SMemAlloc(a1: 16, a2: aSrvCpp_1, a3: 1619, a4: 0);
  newnotification = operator new(__formal: 16, ptr: v5);
  if ( newnotification != nullptr )
  {
    *(_DWORD *)newnotification = a1;
    *((_DWORD *)newnotification + 2) = parambytes;
    *((_DWORD *)newnotification + 1) = SMemAlloc(
                                         a1: string1len + parambytes + string2len + 2,
                                         a2: aSrvCpp_2,
                                         a3: 1626,
                                         a4: 0);
    if ( *((_DWORD *)newnotification + 1) != 0 )
    {
      newstring1 = (char *)(parambytes + *((_DWORD *)newnotification + 1));
      newstring2 = &newstring1[string1len + 1];
      if ( a2 != nullptr && parambytes != 0 )
        memcpy(a1: *((void **)newnotification + 1), Src: a2, Size: parambytes);
      memcpy(a1: newstring1, Src: string1, Size: string1len + 1);
      memcpy(a1: newstring2, Src: string2, Size: string2len + 1);
      if ( string1ptr != nullptr && *(_DWORD *)string1ptr != 0 )
        *(_DWORD *)(*((_DWORD *)newnotification + 1) + string1ptr - (_BYTE *)a2) = newstring1;
      if ( string2ptr != nullptr && *(_DWORD *)string2ptr != 0 )
        *(_DWORD *)(*((_DWORD *)newnotification + 1) + string2ptr - (_BYTE *)a2) = newstring2;
      CCritSect::Enter(this: &srv_uinotificationcritsect);
      for ( nextptr = &srv_uinotificationhead; *nextptr != 0; nextptr = (int *)(*nextptr + 12) )
        ;
      *nextptr = (int)newnotification;
      *((_DWORD *)newnotification + 3) = 0;
      CCritSect::Leave(this: &srv_uinotificationcritsect);
      UiNotificationWaiting();
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19018114
 * CodeView symbol: SrvBeginChat
 * Demangled: SrvBeginChat
 * IDA name: ?SrvBeginChat@@YIHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PBD@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17114
 * Code length: 0x21A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall SrvBeginChat(struct _SNETPROGRAMDATA *a1, struct _SNETPLAYERDATA *a2, const char *preferredchannel)
{
  size_t v4; // eax
  size_t v5; // esi
  size_t v6; // eax
  size_t v7; // eax
  size_t v8; // eax
  _DWORD req[33]; // [esp+10h] [ebp-184h] BYREF
  char namedesc[256]; // [esp+94h] [ebp-100h] BYREF

  if ( srv_connected == 0 )
    return false;
  srv_inchat = 1;
  if ( *((_DWORD *)a2 + 1) != 0
    && *((_DWORD *)a2 + 2) != 0
    && _strnicmp(String1: *((const char **)a2 + 1), String2: srv_username, MaxCount: 0x80u) == 0 )
  {
    SStrCopy(a1: &srv_userdesc, a2: *((_DWORD *)a2 + 2), a3: 128);
  }
  namedesc[0] = byte_19043BB4;
  memset(&namedesc[1], 0, 255);
  SStrCopy(a1: namedesc, a2: srv_username, a3: 128);
  v4 = strlen(Str: namedesc);
  SStrCopy(a1: &namedesc[v4 + 1], a2: &srv_userdesc, a3: 128);
  v5 = strlen(Str: namedesc);
  v6 = strlen(Str: namedesc);
  v7 = strlen(Str: &namedesc[v6 + 1]);
  if ( SendServerMessage(databytes: v5 + v7 + 2) == 0 )
    return false;
  if ( SendServerMessage(databytes: 4) == 0 )
    return false;
  memset(a1: req, Val: 0, Size: sizeof(req));
  if ( preferredchannel != nullptr && *preferredchannel != 0 )
  {
    req[0] = 2;
    SStrCopy(a1: &req[1], a2: preferredchannel, a3: 128);
  }
  else
  {
    req[0] = 1;
    SStrCopy(a1: &req[1], a2: *((_DWORD *)a1 + 1), a3: 128);
  }
  v8 = strlen(Str: (const char *)&req[1]);
  SendServerMessage(databytes: v8 + 5);
  if ( WaitForServerResponse(a1: 10, a2: 0) == 0 )
    return false;
  if ( WaitForServerResponse(a1: 11, a2: 0) != 0 )
    return WaitForServerResponse(a1: 15, a2: 0) != 0;
  return false;
}


// ==============================================================================
/*
 * EA: 0x1901832E
 * CodeView symbol: SendServerMessage
 * Demangled: SendServerMessage
 * IDA name: SendServerMessage
 * CodeView module: 6
 * CV address: segment 1 : 0x1732E
 * Code length: 0xF2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SendServerMessage(unsigned __int8 a1, const char *a2, int databytes)
{
  int v6; // [esp+8h] [ebp-Ch] BYREF
  int error; // [esp+Ch] [ebp-8h] BYREF
  char msg[2]; // [esp+10h] [ebp-4h] BYREF
  __int16 v9; // [esp+12h] [ebp-2h]

  if ( srv_serversocket == -1 )
    return 0;
  CCritSect::Enter(this: &srv_sendcritsect);
  srv_responded[a1] = 0;
  msg[0] = -1;
  msg[1] = a1;
  v9 = databytes + 4;
  if ( send(s: srv_serversocket, buf: msg, len: 4, flags: 0) == 4 )
  {
    if ( a2 != nullptr && databytes != 0 && send(s: srv_serversocket, buf: a2, len: databytes, flags: 0) != databytes )
    {
      v6 = 1;
      QueueUiNotification(a1: 11, a2: &v6, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
      CCritSect::Leave(this: &srv_sendcritsect);
      return 0;
    }
    else
    {
      CCritSect::Leave(this: &srv_sendcritsect);
      return 1;
    }
  }
  else
  {
    error = 1;
    QueueUiNotification(a1: 11, a2: &error, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
    CCritSect::Leave(this: &srv_sendcritsect);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19018420
 * CodeView symbol: WaitForServerResponse
 * Demangled: WaitForServerResponse
 * IDA name: WaitForServerResponse
 * CodeView module: 6
 * CV address: segment 1 : 0x17420
 * Code length: 0xC3
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall WaitForServerResponse(int a1, int a2)
{
  int error; // [esp+8h] [ebp-8h] BYREF
  DWORD starttime; // [esp+Ch] [ebp-4h]

  srv_cancelwait = 0;
  InterlockedIncrement(lpAddend: &srv_waiting);
  starttime = GetTickCount();
  while ( srv_responded[a1] == 0 )
  {
    if ( GetTickCount() - starttime > 0x2BF20 && (a2 & 1) == 0 )
    {
      InterlockedDecrement(lpAddend: &srv_waiting);
      error = 1;
      QueueUiNotification(a1: 11, a2: &error, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
      SErrSetLastError(a1: 1232);
      return 0;
    }
    if ( WaitOnce(a1: a2) == 0 )
    {
      InterlockedDecrement(lpAddend: &srv_waiting);
      return 0;
    }
  }
  srv_responded[a1] = 0;
  InterlockedDecrement(lpAddend: &srv_waiting);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x190184E3
 * CodeView symbol: WaitOnce
 * Demangled: WaitOnce
 * IDA name: WaitOnce
 * CodeView module: 6
 * CV address: segment 1 : 0x174E3
 * Code length: 0xE9
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall WaitOnce(char a1)
{
  int error; // [esp+4h] [ebp-1Ch] BYREF
  struct sockaddr addr; // [esp+8h] [ebp-18h] BYREF
  int addrlen; // [esp+18h] [ebp-8h] BYREF
  DWORD v6; // [esp+1Ch] [ebp-4h]

  if ( (a1 & 4) != 0 )
    v6 = WaitForSingleObject(hHandle: srv_waitevent, dwMilliseconds: 0x3E8u);
  else
    v6 = MsgWaitForMultipleObjects(
           nCount: 1u,
           pHandles: &srv_waitevent,
           fWaitAll: false,
           dwMilliseconds: 0xAu,
           dwWakeMask: 0xFFu);
  if ( v6 == 0 || (a1 & 4) != 0 || UiProcessWindowMessages() != 0 )
  {
    if ( srv_cancelwait != 0 )
    {
      srv_cancelwait = 0;
      SErrSetLastError(a1: 1223);
      return 0;
    }
    else if ( srv_serversocket == -1 )
    {
      return 0;
    }
    else
    {
      addrlen = 16;
      if ( getpeername(s: srv_serversocket, name: &addr, namelen: &addrlen) == -1 )
      {
        error = 1;
        QueueUiNotification(a1: 11, a2: &error, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
        SErrSetLastError(a1: 1232);
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  else
  {
    SErrSetLastError(a1: 1223);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x190185CC
 * CodeView symbol: SrvCancel
 * Demangled: SrvCancel
 * IDA name: ?SrvCancel@@YIXXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x175CC
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl SrvCancel()
{
  srv_cancelwait = 1;
}


// ==============================================================================
/*
 * EA: 0x190185DB
 * CodeView symbol: SrvChangePass
 * Demangled: SrvChangePass
 * IDA name: ?SrvChangePass@@YIHPBD00PAK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x175DB
 * Code length: 0x148
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvChangePass(const char *a1, const char *a2, const char *newpassword, unsigned int *errorcode)
{
  int v4; // eax
  DWORD rec; // [esp+8h] [ebp-154h] BYREF
  int v8; // [esp+Ch] [ebp-150h]
  _BYTE v9[20]; // [esp+10h] [ebp-14Ch] BYREF
  CHAR localpassword[128]; // [esp+24h] [ebp-138h] BYREF
  _DWORD response[2]; // [esp+A4h] [ebp-B8h] BYREF
  _BYTE v12[20]; // [esp+ACh] [ebp-B0h] BYREF
  _BYTE v13[20]; // [esp+C0h] [ebp-9Ch] BYREF
  _BYTE v14[128]; // [esp+D4h] [ebp-88h] BYREF
  DWORD stringlength; // [esp+154h] [ebp-8h]
  int usernamelength; // [esp+158h] [ebp-4h]

  *errorcode = 0;
  SStrCopy(a1: localpassword, a2, a3: 128);
  stringlength = SStrLen(a1: localpassword);
  CharLowerBuffA(lpsz: localpassword, cchLength: stringlength);
  rec = GetTickCount();
  v8 = srv_logonchallenge;
  ShaHash(a1: v9, a2: localpassword, bytes: stringlength);
  SStrCopy(a1: localpassword, a2: newpassword, a3: 128);
  stringlength = SStrLen(a1: localpassword);
  CharLowerBuffA(lpsz: localpassword, cchLength: stringlength);
  response[0] = rec;
  response[1] = v8;
  ShaHash(a1: v12, a2: &rec, bytes: 0x1Cu);
  ShaHash(a1: v13, a2: localpassword, bytes: stringlength);
  v4 = SStrCopy(a1: v14, a2: a1, a3: 128);
  usernamelength = v4 + 1;
  SendServerMessage(a1: 0x31u, a2: (const char *)response, databytes: v4 + 49);
  if ( WaitForServerResponse(a1: 49, a2: 0) != 0 )
    return srv_changepassresult;
  else
    return 0;
}


// ==============================================================================
/*
 * EA: 0x19018723
 * CodeView symbol: SrvCheckFile
 * Demangled: SrvCheckFile
 * IDA name: ?SrvCheckFile@@YIKPBDPBXK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17723
 * Code length: 0x92
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvCheckFile(const char *a1, const void *a2, unsigned int bytes)
{
  int v3; // eax
  char req[20]; // [esp+8h] [ebp-11Ch] BYREF
  _BYTE v7[260]; // [esp+1Ch] [ebp-108h] BYREF
  _BYTE *curr; // [esp+120h] [ebp-4h]

  ShaHash(a1: req, a2, bytes);
  curr = v7;
  v3 = SStrCopy(a1: v7, a2: a1, a3: 260);
  curr += v3 + 1;
  if ( SendServerMessage(a1: 0x32u, a2: req, databytes: curr - req) == 0 )
    return 0;
  if ( WaitForServerResponse(a1: 50, a2: 0) != 0 )
    return srv_checkfileresponse;
  return 0;
}


// ==============================================================================
/*
 * EA: 0x190187B5
 * CodeView symbol: SrvCreateAccount
 * Demangled: SrvCreateAccount
 * IDA name: ?SrvCreateAccount@@YIHPBD0@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x177B5
 * Code length: 0xCE
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvCreateAccount(const char *a1, const char *a2)
{
  int v2; // eax
  char request[20]; // [esp+8h] [ebp-11Ch] BYREF
  _BYTE v7[128]; // [esp+1Ch] [ebp-108h] BYREF
  CHAR localpassword[128]; // [esp+9Ch] [ebp-88h] BYREF
  DWORD stringlength; // [esp+11Ch] [ebp-8h]
  int usernamelength; // [esp+120h] [ebp-4h]

  SStrCopy(a1: srv_username, a2: a1, a3: 128);
  srv_userdesc = 0;
  SStrCopy(a1: localpassword, a2, a3: 128);
  stringlength = SStrLen(a1: localpassword);
  CharLowerBuffA(lpsz: localpassword, cchLength: stringlength);
  ShaHash(a1: request, a2: localpassword, bytes: stringlength);
  v2 = SStrCopy(a1: v7, a2: a1, a3: 128);
  usernamelength = v2 + 1;
  SendServerMessage(a1: 0x2Au, a2: request, databytes: v2 + 21);
  if ( WaitForServerResponse(a1: 42, a2: 0) != 0 )
    return srv_createaccountresult;
  else
    return 0;
}


// ==============================================================================
/*
 * EA: 0x19018883
 * CodeView symbol: SrvDestroy
 * Demangled: SrvDestroy
 * IDA name: ?SrvDestroy@@YIXXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x17883
 * Code length: 0x237
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl SrvDestroy()
{
  int next; // [esp+0h] [ebp-14h]
  DWORD newphase; // [esp+8h] [ebp-Ch]
  DWORD phase; // [esp+Ch] [ebp-8h]
  DWORD starttime; // [esp+10h] [ebp-4h]

  SrvCancel();
  srv_shutdown = 1;
  SetEvent(hEvent: srv_shutdownevent);
  phase = 0;
  starttime = GetTickCount();
  do
  {
    Sleep(dwMilliseconds: 0xAu);
    newphase = (GetTickCount() - starttime) / 0x1F4;
    if ( srv_connectthreads != 0 && newphase != phase )
    {
      phase = newphase;
      DestroyConnectThreads(a1: newphase);
    }
  }
  while ( srv_connectthreads != 0 );
  do
    Sleep(dwMilliseconds: 0xAu);
  while ( srv_downloadthreads != 0 );
  SrvDisconnect();
  if ( srv_keepalivethread != nullptr )
    WaitForSingleObject(hHandle: srv_keepalivethread, dwMilliseconds: 0xFFFFFFFF);
  if ( srv_serverthread != nullptr )
    WaitForSingleObject(hHandle: srv_serverthread, dwMilliseconds: 0xFFFFFFFF);
  CloseHandle(hObject: srv_keepalivethread);
  CloseHandle(hObject: srv_serverthread);
  CloseHandle(hObject: srv_waitevent);
  CloseHandle(hObject: srv_shutdownevent);
  srv_keepalivethread = nullptr;
  srv_serverthread = nullptr;
  srv_waitevent = nullptr;
  srv_shutdownevent = nullptr;
  srv_shutdown = 0;
  RemovePingUser(a1: 0);
  CCritSect::Enter(this: &srv_uinotificationcritsect);
  while ( srv_uinotificationhead != 0 )
  {
    SMemFree(a1: *(_DWORD *)(srv_uinotificationhead + 4), a2: aSrvCpp_3, a3: 2503, a4: 0);
    next = *(_DWORD *)(srv_uinotificationhead + 12);
    SMemFree(a1: srv_uinotificationhead, a2: aSrvCpp_4, a3: 2505, a4: 0);
    srv_uinotificationhead = next;
  }
  if ( srv_uinotificationhold != 0 )
  {
    SMemFree(a1: srv_uinotificationhold, a2: aSrvCpp_5, a3: 2509, a4: 0);
    srv_uinotificationhold = 0;
  }
  CCritSect::Leave(this: &srv_uinotificationcritsect);
  srv_serverthread = nullptr;
  srv_waitevent = nullptr;
  if ( srv_ladderdata != nullptr )
    SMemFree(a1: srv_ladderdata, a2: aSrvCpp_6, a3: 2519, a4: 0);
  if ( srv_userdata != nullptr )
    SMemFree(a1: srv_userdata, a2: aSrvCpp_7, a3: 2520, a4: 0);
  srv_ladderdata = nullptr;
  srv_userdata = nullptr;
}


// ==============================================================================
/*
 * EA: 0x19018ABA
 * CodeView symbol: DestroyConnectThreads
 * Demangled: DestroyConnectThreads
 * IDA name: DestroyConnectThreads
 * CodeView module: 6
 * CV address: segment 1 : 0x17ABA
 * Code length: 0x90
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall DestroyConnectThreads(void *this)
{
  SOCKET *curr; // [esp+8h] [ebp-4h]

  CCritSect::Enter(this: &srv_connectcritsect);
  if ( this == (void *)1 )
  {
    for ( curr = (SOCKET *)srv_connecthead; curr != nullptr; curr = (SOCKET *)curr[2] )
      closesocket(s: *curr);
  }
  else if ( this == (void *)2 )
  {
    while ( srv_connecthead != nullptr )
    {
      TerminateThread(hThread: *((HANDLE *)srv_connecthead + 1), dwExitCode: 0);
      DeleteConnectThread(this: *(void **)srv_connecthead);
      InterlockedDecrement(lpAddend: &srv_connectthreads);
    }
  }
  CCritSect::Leave(this: &srv_connectcritsect);
}


// ==============================================================================
/*
 * EA: 0x19018B4A
 * CodeView symbol: RemovePingUser
 * Demangled: RemovePingUser
 * IDA name: RemovePingUser
 * CodeView module: 6
 * CV address: segment 1 : 0x17B4A
 * Code length: 0xBB
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall RemovePingUser(__int128 *this)
{
  const void *curr; // [esp+4h] [ebp-18h]
  const void **nextptr; // [esp+8h] [ebp-14h]
  __int128 dataportaddr; // [esp+Ch] [ebp-10h] BYREF

  if ( this != nullptr )
  {
    dataportaddr = *this;
    WORD1(dataportaddr) = htons(hostshort: 0x17E0u);
    memset(a1: (char *)&dataportaddr + 8, Val: 0, Size: 8u);
  }
  CCritSect::Enter(this: &srv_pingcritsect);
  nextptr = (const void **)&srv_pinghead;
  while ( *nextptr != nullptr )
  {
    if ( this != nullptr && memcmp(Buf1: &dataportaddr, Buf2: *nextptr, Size: 0x10u) != 0 )
    {
      nextptr = (const void **)((char *)*nextptr + 28);
    }
    else
    {
      curr = *nextptr;
      *nextptr = *((const void **)*nextptr + 7);
      SMemFree(a1: curr, a2: aSrvCpp_8, a3: 1679, a4: 0);
    }
  }
  CCritSect::Leave(this: &srv_pingcritsect);
}


// ==============================================================================
/*
 * EA: 0x19018C05
 * CodeView symbol: SrvDisconnect
 * Demangled: SrvDisconnect
 * IDA name: ?SrvDisconnect@@YIHXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x17C05
 * Code length: 0x21
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl SrvDisconnect()
{
  if ( srv_connected != 0 || srv_serversocket != -1 )
    DisconnectFromServer();
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19018C26
 * CodeView symbol: DisconnectFromServer
 * Demangled: DisconnectFromServer
 * IDA name: DisconnectFromServer
 * CodeView module: 6
 * CV address: segment 1 : 0x17C26
 * Code length: 0x2D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int DisconnectFromServer()
{
  int result; // eax

  srv_connected = 0;
  if ( srv_serversocket != -1 )
  {
    result = closesocket(s: srv_serversocket);
    srv_serversocket = -1;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19018C53
 * CodeView symbol: SrvEndChat
 * Demangled: SrvEndChat
 * IDA name: ?SrvEndChat@@YIHXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x17C53
 * Code length: 0x34
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl SrvEndChat()
{
  if ( srv_connected == 0 )
    return 0;
  if ( SendServerMessage(a1: 0x10u, a2: nullptr, databytes: 0) == 0 )
    return 0;
  srv_inchat = 0;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19018C87
 * CodeView symbol: SrvFindLadderUser
 * Demangled: SrvFindLadderUser
 * IDA name: ?SrvFindLadderUser@@YIXKKKPBDPAK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17C87
 * Code length: 0x94
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvFindLadderUser(
        unsigned int a1,
        unsigned int a2,
        unsigned int sortcriterion,
        const char *username,
        unsigned int *index)
{
  int v5; // eax
  _DWORD req[3]; // [esp+8h] [ebp-8Ch] BYREF
  _BYTE v7[128]; // [esp+14h] [ebp-80h] BYREF

  *index = -1;
  req[0] = a1;
  req[1] = a2;
  req[2] = sortcriterion;
  SStrCopy(a1: v7, a2: username, a3: 128);
  v5 = SStrLen(a1: v7);
  if ( SendServerMessage(a1: 0x2Fu, a2: (const char *)req, databytes: v5 + 13) != 0
    && WaitForServerResponse(a1: 47, a2: 0) != 0 )
  {
    *index = srv_ladderuser;
  }
}


// ==============================================================================
/*
 * EA: 0x19018D1B
 * CodeView symbol: SrvGetCdKeyUser
 * Demangled: SrvGetCdKeyUser
 * IDA name: ?SrvGetCdKeyUser@@YIXPADK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17D1B
 * Code length: 0x22
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvGetCdKeyUser(char *a1, unsigned int a2)
{
  SStrCopy(a1, a2: &srv_cdkeyuser, a3: a2);
}


// ==============================================================================
/*
 * EA: 0x19018D3D
 * CodeView symbol: SrvGetGameList
 * Demangled: SrvGetGameList
 * IDA name: ?SrvGetGameList@@YIHPBD0KKKPAK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17D3D
 * Code length: 0x127
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvGetGameList(
        const char *a1,
        const char *a2,
        unsigned int categorybits,
        unsigned int categorymask,
        unsigned int maxitems,
        unsigned int *errorcode)
{
  int v6; // eax
  int v7; // eax
  int v8; // eax
  const char *gamepassword; // [esp+0h] [ebp-1A0h]
  const char *gamename; // [esp+4h] [ebp-19Ch]
  _DWORD req[4]; // [esp+Ch] [ebp-194h] BYREF
  _BYTE v13[384]; // [esp+1Ch] [ebp-184h] BYREF
  _BYTE *curr; // [esp+19Ch] [ebp-4h]

  gamepassword = a2;
  gamename = a1;
  *errorcode = 0;
  if ( a1 == nullptr )
    gamename = (const char *)&unk_19043BB8;
  if ( a2 == nullptr )
    gamepassword = (const char *)&unk_19043BBC;
  req[0] = categorybits;
  req[1] = categorymask;
  req[2] = 0;
  req[3] = maxitems;
  curr = v13;
  v6 = SStrCopy(a1: v13, a2: gamename, a3: 128);
  curr += v6 + 1;
  v7 = SStrCopy(a1: curr, a2: gamepassword, a3: 128);
  curr += v7 + 1;
  v8 = SStrCopy(a1: curr, a2: &unk_19043BC0, a3: 128);
  curr += v8 + 1;
  if ( SendServerMessage(a1: 9u, a2: (const char *)req, databytes: curr + 16 - v13) == 0 )
    return 0;
  if ( WaitForServerResponse(a1: 9, a2: 0) == 0 )
    return 0;
  *errorcode = srv_getadverrorcode;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19018E64
 * CodeView symbol: SrvGetLatency
 * Demangled: SrvGetLatency
 * IDA name: ?SrvGetLatency@@YIHPAU_SNETADDR@@PAK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17E64
 * Code length: 0xB3
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall SrvGetLatency(struct _SNETADDR *a1, unsigned int *a2)
{
  __int128 dataportaddr; // [esp+8h] [ebp-14h] BYREF
  _DWORD *curr; // [esp+18h] [ebp-4h]

  if ( a2 != nullptr )
    *a2 = 0;
  if ( a1 != nullptr )
  {
    dataportaddr = *(_OWORD *)a1;
    WORD1(dataportaddr) = htons(hostshort: 0x17E0u);
    memset(a1: (char *)&dataportaddr + 8, Val: 0, Size: 8u);
  }
  CCritSect::Enter(this: &srv_pingcritsect);
  for ( curr = (_DWORD *)srv_pinghead;
        curr != nullptr && memcmp(Buf1: &dataportaddr, Buf2: curr, Size: 0x10u) != 0;
        curr = (_DWORD *)curr[7] )
  {
    ;
  }
  if ( curr != nullptr )
    *a2 = curr[4];
  CCritSect::Leave(this: &srv_pingcritsect);
  return curr != nullptr;
}


// ==============================================================================
/*
 * EA: 0x19018F17
 * CodeView symbol: SrvGetLocalAddress
 * Demangled: SrvGetLocalAddress
 * IDA name: ?SrvGetLocalAddress@@YIHPAU_SNETADDR@@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17F17
 * Code length: 0x74
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvGetLocalAddress(struct sockaddr *a1)
{
  int addrlen; // [esp+8h] [ebp-4h] BYREF

  memset(a1, Val: 0, Size: sizeof(struct sockaddr));
  if ( srv_serversocket == -1 )
    return 0;
  addrlen = 16;
  getsockname(s: srv_serversocket, name: a1, namelen: &addrlen);
  *(_WORD *)a1->sa_data = htons(hostshort: 0x17E0u);
  memset(a1: &a1->sa_data[6], Val: 0, Size: 8u);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19018F8B
 * CodeView symbol: SrvGetLocalPlayerDesc
 * Demangled: SrvGetLocalPlayerDesc
 * IDA name: ?SrvGetLocalPlayerDesc@@YIXPADK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17F8B
 * Code length: 0x22
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvGetLocalPlayerDesc(char *a1, unsigned int a2)
{
  SStrCopy(a1, a2: &srv_userdesc, a3: a2);
}


// ==============================================================================
/*
 * EA: 0x19018FAD
 * CodeView symbol: SrvGetLocalPlayerName
 * Demangled: SrvGetLocalPlayerName
 * IDA name: ?SrvGetLocalPlayerName@@YIXHPADK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17FAD
 * Code length: 0x39
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvGetLocalPlayerName(int a1, char *a2, unsigned int bufferchars)
{
  if ( a1 != 0 )
    SStrCopy(a1: a2, a2: &srv_uniqueusername, a3: bufferchars);
  else
    SStrCopy(a1: a2, a2: srv_username, a3: bufferchars);
}


// ==============================================================================
/*
 * EA: 0x19018FE6
 * CodeView symbol: SrvGetUiNotification
 * Demangled: SrvGetUiNotification
 * IDA name: ?SrvGetUiNotification@@YIHPAKPAPAX0@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x17FE6
 * Code length: 0xFA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvGetUiNotification(unsigned int *a1, void **a2, unsigned int *parambytes)
{
  int freenotification; // [esp+8h] [ebp-4h]

  if ( a1 != nullptr )
    *a1 = 0;
  if ( a2 != nullptr )
    *a2 = nullptr;
  if ( parambytes != nullptr )
    *parambytes = 0;
  CCritSect::Enter(this: &srv_uinotificationcritsect);
  if ( srv_uinotificationhold != 0 )
  {
    SMemFree(a1: srv_uinotificationhold, a2: aSrvCpp_9, a3: 2699, a4: 0);
    srv_uinotificationhold = 0;
  }
  if ( srv_uinotificationhead != 0 )
  {
    *a1 = *(_DWORD *)srv_uinotificationhead;
    *a2 = *(void **)(srv_uinotificationhead + 4);
    *parambytes = *(_DWORD *)(srv_uinotificationhead + 8);
    srv_uinotificationhold = *(_DWORD *)(srv_uinotificationhead + 4);
    freenotification = srv_uinotificationhead;
    srv_uinotificationhead = *(_DWORD *)(srv_uinotificationhead + 12);
    SMemFree(a1: freenotification, a2: aSrvCpp_10, a3: 2719, a4: 0);
    CCritSect::Leave(this: &srv_uinotificationcritsect);
    return 1;
  }
  else
  {
    CCritSect::Leave(this: &srv_uinotificationcritsect);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x190190E0
 * CodeView symbol: SrvInitialize
 * Demangled: SrvInitialize
 * IDA name: ?SrvInitialize@@YIHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETVERSIONDATA@@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x180E0
 * Code length: 0x7AD
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvInitialize(
        struct _SNETPROGRAMDATA *a1,
        struct _SNETPLAYERDATA *a2,
        struct _SNETVERSIONDATA *versiondata)
{
  int result; // eax
  size_t v4; // eax
  int v5; // eax
  int v6; // eax
  int v8; // [esp+10h] [ebp-418h] BYREF
  int v9; // [esp+14h] [ebp-414h] BYREF
  int v10; // [esp+18h] [ebp-410h] BYREF
  int v11; // [esp+1Ch] [ebp-40Ch] BYREF
  int v12; // [esp+20h] [ebp-408h] BYREF
  _BYTE v13[256]; // [esp+24h] [ebp-404h] BYREF
  _BYTE *v14; // [esp+124h] [ebp-304h]
  DWORD starttime; // [esp+128h] [ebp-300h]
  int lastpercent; // [esp+12Ch] [ebp-2FCh]
  int v17; // [esp+130h] [ebp-2F8h] BYREF
  _DWORD v18[69]; // [esp+134h] [ebp-2F4h] BYREF
  int v19; // [esp+248h] [ebp-1E0h] BYREF
  int checkvalue; // [esp+24Ch] [ebp-1DCh] BYREF
  char comment[256]; // [esp+250h] [ebp-1D8h] BYREF
  int revisionid; // [esp+350h] [ebp-D8h] BYREF
  _DWORD v23[4]; // [esp+354h] [ebp-D4h] BYREF
  _DWORD req[42]; // [esp+364h] [ebp-C4h] BYREF
  CHAR *curr; // [esp+40Ch] [ebp-1Ch]
  DWORD size; // [esp+410h] [ebp-18h] BYREF
  int v27; // [esp+414h] [ebp-14h] BYREF
  int v28; // [esp+418h] [ebp-10h] BYREF
  int error; // [esp+41Ch] [ebp-Ch] BYREF
  char conntype[4]; // [esp+420h] [ebp-8h] BYREF
  unsigned int threadid; // [esp+424h] [ebp-4h] BYREF

  srv_cancelwait = 0;
  srv_waitevent = CreateEventA(lpEventAttributes: nullptr, bManualReset: false, bInitialState: false, lpName: nullptr);
  if ( srv_waitevent == nullptr )
  {
    error = 5;
    QueueUiNotification(a1: 11, a2: &error, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
    SErrSetLastError(a1: 8);
    return 0;
  }
  srv_serverthread = (HANDLE)_beginthreadex(
                               Security: nullptr,
                               StackSize: 0,
                               StartAddress: ServerThreadProc,
                               ArgList: nullptr,
                               InitFlag: 0,
                               ThrdAddr: &threadid);
  if ( srv_serverthread == nullptr )
  {
    v28 = 5;
    QueueUiNotification(a1: 11, a2: &v28, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
    SErrSetLastError(a1: 8);
    return 0;
  }
  conntype[0] = 1;
  if ( send(s: srv_serversocket, buf: conntype, len: 1, flags: 0) != 1 )
  {
    v27 = 2;
    QueueUiNotification(a1: 11, a2: &v27, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
    SErrSetLastError(a1: 1232);
    return 0;
  }
  memset(a1: req, Val: 0, Size: sizeof(req));
  req[0] = 1;
  SRegLoadValue(a1: aConfiguration_2, a2: aRegistrationVe, a3: 2, a4: &req[1]);
  SRegLoadValue(a1: aConfiguration_3, a2: aRegistrationAu, a3: 2, a4: &req[2]);
  SRegLoadValue(a1: aConfiguration_4, a2: aClientId, a3: 2, a4: &req[3]);
  SRegLoadValue(a1: aConfiguration_5, a2: aClientToken, a3: 2, a4: &req[4]);
  size = 16;
  GetComputerNameA(lpBuffer: (LPSTR)&req[5], nSize: &size);
  HIBYTE(req[8]) = 0;
  curr = (char *)&req[5] + strlen(Str: (const char *)&req[5]) + 1;
  size = 128;
  GetUserNameA(lpBuffer: curr, pcbBuffer: &size);
  curr += strlen(Str: curr) + 1;
  if ( SendServerMessage(a1: 0x1Eu, a2: (const char *)req, databytes: curr + 20 - (CHAR *)&req[5]) == 0 )
    return 0;
  if ( SendLocaleInfo() == 0 )
    return 0;
  if ( SendSystemInfo() == 0 )
    return 0;
  srv_versionfile = 0;
  dword_19043424 = 0;
  memset(a1: v23, Val: 0, Size: sizeof(v23));
  v23[0] = 1230518326;
  v23[1] = *((_DWORD *)a1 + 3);
  v23[2] = *((_DWORD *)a1 + 4);
  if ( SendServerMessage(a1: 6u, a2: (const char *)v23, databytes: 16) == 0 )
    return 0;
  while ( srv_versionfile == 0 && dword_19043424 == 0 )
  {
    if ( WaitOnce(a1: 0) == 0 )
      return 0;
  }
  if ( dword_19043424 == 0 )
  {
    comment[0] = byte_19043BC4;
    memset(&comment[1], 0, 255);
    if ( CheckVersion(a1: &revisionid, a2: &checkvalue, a3: comment) == 0 )
    {
      v19 = 3;
      QueueUiNotification(a1: 11, a2: &v19, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
      SErrSetLastError(a1: 1232);
      return 0;
    }
    srv_patchfiles = 0;
    byte_190438A5 = 0;
    srv_patchpercent = 0;
    memset(a1: v18, Val: 0, Size: sizeof(v18));
    v18[0] = 1230518326;
    v18[1] = *((_DWORD *)a1 + 3);
    v18[2] = *((_DWORD *)a1 + 4);
    v18[3] = revisionid;
    v18[4] = checkvalue;
    SStrCopy(a1: &v18[5], a2: comment, a3: 256);
    v4 = strlen(Str: comment);
    if ( SendServerMessage(a1: 7u, a2: (const char *)v18, databytes: v4 + 21) == 0 )
      return 0;
    if ( WaitForServerResponse(a1: 7, a2: 0) == 0 )
      return 0;
  }
  if ( srv_authenticated == 0 )
  {
    v17 = 3;
    QueueUiNotification(a1: 11, a2: &v17, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
    SErrSetLastError(a1: 1232);
    return 0;
  }
  if ( srv_authenticated == 1 )
  {
    if ( UiUpgradeMessage() != 0 )
    {
      lastpercent = -1;
      while ( srv_patchfiles != 0 )
      {
        if ( WaitOnce(a1: 0) == 0 && SErrGetLastError() == 1223 )
          return 0;
        if ( srv_patchpercent != lastpercent )
        {
          lastpercent = srv_patchpercent;
          QueueUiNotification(a1: 8, a2: &srv_patchpercent, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
        }
      }
      srv_patchpercent = 100;
      QueueUiNotification(a1: 8, a2: &srv_patchpercent, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
      starttime = GetTickCount();
      do
        WaitOnce(a1: 0);
      while ( GetTickCount() - starttime < 0xC8 );
      QueueUiNotification(a1: 10, a2: nullptr, parambytes: 0, string1ptr: nullptr, string2ptr: nullptr);
      SRegSaveString(a1: aPatch, a2: aLauncher, a3: 2, a4: *((_DWORD *)versiondata + 2));
      SRegSaveString(a1: aPatch_0, a2: aSrcdata, a3: 2, a4: *((_DWORD *)versiondata + 3));
      SRegSaveString(a1: aPatch_1, a2: aDstdata, a3: 2, a4: *((_DWORD *)versiondata + 4));
      SErrSetLastError(a1: -2062548873);
      return 0;
    }
    else
    {
      SErrSetLastError(a1: 1223);
      return 0;
    }
  }
  else
  {
    if ( SendServerMessage(a1: 0x2Du, a2: nullptr, databytes: 0) == 0 )
      return 0;
    if ( *((_DWORD *)a1 + 11) != 0 && *((_DWORD *)a1 + 12) != 0 )
    {
      v12 = *((_DWORD *)a1 + 13);
      v14 = v13;
      v5 = SStrCopy(a1: v13, a2: *((_DWORD *)a1 + 11), a3: 128);
      v14 += v5 + 1;
      v6 = SStrCopy(a1: v14, a2: *((_DWORD *)a1 + 12), a3: 128);
      v14 += v6 + 1;
      if ( SendServerMessage(a1: 0x30u, a2: (const char *)&v12, databytes: v14 - (_BYTE *)&v12) != 0 )
      {
        if ( WaitForServerResponse(a1: 48, a2: 0) != 0 )
        {
          switch ( srv_cdkeyresult )
          {
            case 2:
              v11 = 6;
              QueueUiNotification(a1: 11, a2: &v11, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
              SErrSetLastError(a1: 87);
              result = 0;
              break;
            case 3:
              v10 = 9;
              QueueUiNotification(a1: 11, a2: &v10, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
              SErrSetLastError(a1: 87);
              result = 0;
              break;
            case 4:
              v9 = 7;
              QueueUiNotification(a1: 11, a2: &v9, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
              SErrSetLastError(a1: 87);
              result = 0;
              break;
            case 5:
              v8 = 8;
              QueueUiNotification(a1: 11, a2: &v8, parambytes: 4u, string1ptr: nullptr, string2ptr: nullptr);
              SErrSetLastError(a1: 87);
              result = 0;
              break;
            default:
              goto LABEL_56;
          }
        }
        else
        {
          return 0;
        }
      }
      else
      {
        return 0;
      }
    }
    else
    {
LABEL_56:
      if ( srv_udppingdata != 0 )
      {
        if ( SendServerMessage(a1: 0x14u, a2: (const char *)&srv_udppingdata, databytes: 4) == 0 )
          return 0;
      }
      else
      {
        QueueUiNotification(a1: 21, a2: nullptr, parambytes: 0, string1ptr: nullptr, string2ptr: nullptr);
      }
      srv_keepalivethread = (HANDLE)_beginthreadex(
                                      Security: nullptr,
                                      StackSize: 0,
                                      StartAddress: KeepAliveThreadProc,
                                      ArgList: nullptr,
                                      InitFlag: 0,
                                      ThrdAddr: &threadid);
      srv_connected = 1;
      return 1;
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901988D
 * CodeView symbol: CheckVersion
 * Demangled: CheckVersion
 * IDA name: CheckVersion
 * CodeView module: 6
 * CV address: segment 1 : 0x1888D
 * Code length: 0x3D1
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall CheckVersion(_DWORD *a1, _DWORD *a2, _BYTE *comment)
{
  HMODULE LibraryA; // eax
  HMODULE ModuleHandleA; // eax
  HMODULE Instance; // eax
  DWORD byteswritten; // [esp+Ch] [ebp-438h] BYREF
  HANDLE outfile; // [esp+10h] [ebp-434h]
  CHAR providerfilename[260]; // [esp+14h] [ebp-430h] BYREF
  FARPROC checkrevision; // [esp+118h] [ebp-32Ch]
  CHAR stormfilename[260]; // [esp+11Ch] [ebp-328h] BYREF
  unsigned int authtype; // [esp+220h] [ebp-224h] BYREF
  DWORD size; // [esp+224h] [ebp-220h]
  CHAR appfilename[260]; // [esp+228h] [ebp-21Ch] BYREF
  int file; // [esp+32Ch] [ebp-118h] BYREF
  HMODULE lib; // [esp+330h] [ebp-114h]
  _BYTE dllname[257]; // [esp+334h] [ebp-110h] BYREF
  __int16 v20; // [esp+435h] [ebp-Fh]
  char v21; // [esp+437h] [ebp-Dh]
  int success; // [esp+438h] [ebp-Ch]
  LPCVOID buffer; // [esp+43Ch] [ebp-8h]
  int archive; // [esp+440h] [ebp-4h] BYREF

  if ( a1 != nullptr )
    *a1 = 0;
  if ( a2 != nullptr )
    *a2 = 0;
  if ( comment != nullptr )
    *comment = 0;
  archive = 0;
  buffer = nullptr;
  dllname[0] = byte_19043BC8;
  memset(&dllname[1], 0, 0x100u);
  v20 = 0;
  v21 = 0;
  file = 0;
  lib = nullptr;
  success = 0;
  if ( SFileOpenArchive(a1: srv_versionfile, a2: 0, a3: 0, a4: &archive) == 0 )
    return 0;
  SFileAuthenticateArchive(a1: archive, a2: &authtype);
  if ( authtype == 0 || authtype >= 5 )
  {
    strcpy(Destination: dllname, Source: srv_versionfile);
    if ( strchr(Str: dllname, Val: 46) != nullptr )
      *strchr(Str: dllname, Val: 46) = 0;
    strcat(Destination: dllname, Source: aDll);
    if ( SFileOpenFileEx(a1: archive, a2: dllname, a3: 0, a4: &file) != 0 )
    {
      size = SFileGetFileSize(a1: file, a2: 0);
      buffer = (LPCVOID)SMemAlloc(a1: size, a2: aSrvCpp_11, a3: 391, a4: 0);
      if ( buffer != nullptr )
      {
        SFileReadFile(a1: file, a2: buffer, a3: size, a4: 0, a5: 0);
        outfile = CreateFileA(
                    lpFileName: dllname,
                    dwDesiredAccess: 0x40000000u,
                    dwShareMode: 0,
                    lpSecurityAttributes: nullptr,
                    dwCreationDisposition: 2u,
                    dwFlagsAndAttributes: 0x80u,
                    hTemplateFile: nullptr);
        if ( outfile != (HANDLE)-1 )
        {
          WriteFile(
            hFile: outfile,
            lpBuffer: buffer,
            nNumberOfBytesToWrite: size,
            lpNumberOfBytesWritten: &byteswritten,
            lpOverlapped: nullptr);
          CloseHandle(hObject: outfile);
          LibraryA = LoadLibraryA(lpLibFileName: dllname);
          lib = LibraryA;
          if ( LibraryA != nullptr )
          {
            checkrevision = GetProcAddress(hModule: lib, lpProcName: aCheckrevision);
            if ( checkrevision != nullptr )
            {
              appfilename[0] = byte_19043BCC;
              memset(&appfilename[1], 0, 259);
              stormfilename[0] = byte_19043BD0;
              memset(&stormfilename[1], 0, 259);
              providerfilename[0] = byte_19043BD4;
              memset(&providerfilename[1], 0, 259);
              ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
              GetModuleFileNameA(hModule: ModuleHandleA, lpFilename: appfilename, nSize: 0x104u);
              Instance = (HMODULE)StormGetInstance();
              GetModuleFileNameA(hModule: Instance, lpFilename: stormfilename, nSize: 0x104u);
              GetModuleFileNameA(hModule: global_hinstance, lpFilename: providerfilename, nSize: 0x104u);
              success = ((int (__stdcall *)(CHAR *, CHAR *, CHAR *, void *, _DWORD *, _DWORD *, _BYTE *))checkrevision)(
                          a1: appfilename,
                          a2: stormfilename,
                          a3: providerfilename,
                          a4: &srv_argstring,
                          a5: a1,
                          a6: a2,
                          a7: comment);
            }
          }
        }
      }
    }
  }
  if ( lib != nullptr )
    FreeLibrary(hLibModule: lib);
  if ( buffer != nullptr )
    SMemFree(a1: buffer, a2: aSrvCpp_12, a3: 441, a4: 0);
  if ( file != 0 )
    SFileCloseFile(a1: file);
  if ( archive != 0 )
    SFileCloseArchive(a1: archive);
  DeleteFileA(lpFileName: srv_versionfile);
  if ( dllname[0] != 0 )
    DeleteFileA(lpFileName: dllname);
  return success;
}


// ==============================================================================
/*
 * EA: 0x19019C5E
 * CodeView symbol: KeepAliveThreadProc
 * Demangled: KeepAliveThreadProc
 * IDA name: KeepAliveThreadProc
 * CodeView module: 6
 * CV address: segment 1 : 0x18C5E
 * Code length: 0x61
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall __noreturn KeepAliveThreadProc(void *this, void *__formal)
{
  DWORD data; // [esp+0h] [ebp-4h] BYREF

  data = (DWORD)this;
  while ( srv_shutdown == 0 )
  {
    if ( WaitForSingleObject(hHandle: srv_shutdownevent, dwMilliseconds: 0x15F90u) != 0 )
    {
      SendServerMessage(a1: 0, a2: nullptr, databytes: 0);
      data = GetTickCount();
      SpiSendSpecial(a1: (const struct sockaddr *)&srv_udpaddr, a2: 7u, &data, databytes: 4u, count: 1u);
    }
  }
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x19019CBF
 * CodeView symbol: SendLocaleInfo
 * Demangled: SendLocaleInfo
 * IDA name: SendLocaleInfo
 * CodeView module: 6
 * CV address: segment 1 : 0x18CBF
 * Code length: 0x186
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int SendLocaleInfo()
{
  size_t v0; // eax
  struct _TIME_ZONE_INFORMATION tzinfo; // [esp+0h] [ebp-1F4h] BYREF
  struct _SYSTEMTIME sysutctime; // [esp+ACh] [ebp-148h] BYREF
  struct _SYSTEMTIME syslocaltime; // [esp+BCh] [ebp-138h] BYREF
  struct _FILETIME req; // [esp+D0h] [ebp-124h] BYREF
  struct _FILETIME FileTime; // [esp+D8h] [ebp-11Ch] BYREF
  LONG Bias; // [esp+E0h] [ebp-114h]
  LCID SystemDefaultLCID; // [esp+E4h] [ebp-110h]
  LCID UserDefaultLCID; // [esp+E8h] [ebp-10Ch]
  int UserDefaultLangID; // [esp+ECh] [ebp-108h]
  CHAR v11[256]; // [esp+F0h] [ebp-104h] BYREF
  char *curr; // [esp+1F0h] [ebp-4h]

  memset(a1: &tzinfo, Val: 0, Size: sizeof(tzinfo));
  GetTimeZoneInformation(lpTimeZoneInformation: &tzinfo);
  GetSystemTime(lpSystemTime: &sysutctime);
  GetLocalTime(lpSystemTime: &syslocaltime);
  SystemTimeToFileTime(lpSystemTime: &sysutctime, lpFileTime: &req);
  SystemTimeToFileTime(lpSystemTime: &syslocaltime, lpFileTime: &FileTime);
  Bias = tzinfo.Bias;
  SystemDefaultLCID = GetSystemDefaultLCID();
  UserDefaultLCID = GetUserDefaultLCID();
  UserDefaultLangID = GetUserDefaultLangID();
  v11[0] = 0;
  curr = v11;
  GetLocaleInfoA(Locale: 0x400u, LCType: 3u, lpLCData: v11, cchData: 64);
  curr += strlen(Str: curr) + 1;
  GetLocaleInfoA(Locale: 0x400u, LCType: 5u, lpLCData: curr, cchData: 64);
  curr += strlen(Str: curr) + 1;
  GetLocaleInfoA(Locale: 0x400u, LCType: 7u, lpLCData: curr, cchData: 64);
  curr += strlen(Str: curr) + 1;
  GetLocaleInfoA(Locale: 0x400u, LCType: 0x1002u, lpLCData: curr, cchData: 64);
  v0 = strlen(Str: curr);
  return SendServerMessage(a1: 0x12u, a2: (const char *)&req, databytes: &curr[v0 + 33] - v11);
}


// ==============================================================================
/*
 * EA: 0x19019E45
 * CodeView symbol: SendSystemInfo
 * Demangled: SendSystemInfo
 * IDA name: SendSystemInfo
 * CodeView module: 6
 * CV address: segment 1 : 0x18E45
 * Code length: 0x19D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __usercall SendSystemInfo@<eax>(int a1@<ebx>)
{
  unsigned int v1; // eax
  unsigned int loop; // [esp+Ch] [ebp-184h]
  DWORD lasttime; // [esp+10h] [ebp-180h]
  _DWORD msg[7]; // [esp+14h] [ebp-17Ch] BYREF
  DWORD currtime; // [esp+30h] [ebp-160h]
  DWORD sectorspercluster; // [esp+34h] [ebp-15Ch] BYREF
  struct _MEMORYSTATUS memstat; // [esp+38h] [ebp-158h] BYREF
  DWORD bytespersector; // [esp+58h] [ebp-138h] BYREF
  struct _SYSTEM_INFO sysinfo; // [esp+5Ch] [ebp-134h] BYREF
  CHAR filename[260]; // [esp+80h] [ebp-110h] BYREF
  DWORD freeclusters; // [esp+184h] [ebp-Ch] BYREF
  DWORD totalclusters; // [esp+188h] [ebp-8h] BYREF
  int iterations; // [esp+18Ch] [ebp-4h]

  lasttime = GetTickCount();
  do
    currtime = GetTickCount();
  while ( currtime == lasttime );
  for ( iterations = 0; ; ++iterations )
  {
    v1 = GetTickCount() - currtime;
    if ( v1 >= 0x32 )
      break;
    for ( loop = 0; loop < 0x2710; ++loop )
      v1 += a1 + a1 + a1 + a1 + a1 + a1 + a1 + a1 + a1;
  }
  GetSystemInfo(lpSystemInfo: &sysinfo);
  memstat.dwLength = 32;
  GlobalMemoryStatus(lpBuffer: &memstat);
  GetModuleFileNameA(hModule: nullptr, lpFilename: filename, nSize: 0x104u);
  filename[3] = 0;
  GetDiskFreeSpaceA(
    lpRootPathName: filename,
    lpSectorsPerCluster: &sectorspercluster,
    lpBytesPerSector: &bytespersector,
    lpNumberOfFreeClusters: &freeclusters,
    lpTotalNumberOfClusters: &totalclusters);
  msg[0] = sysinfo.dwNumberOfProcessors;
  msg[1] = sysinfo.wProcessorArchitecture;
  msg[2] = sysinfo.wProcessorLevel;
  msg[3] = 10 * ((2 * iterations + 5) / 0xAu);
  msg[4] = (memstat.dwTotalPhys + 655360) >> 20;
  msg[5] = memstat.dwTotalPageFile >> 20;
  msg[6] = (totalclusters * bytespersector * sectorspercluster) >> 20;
  return SendServerMessage(a1: 0x2Bu, a2: (const char *)msg, databytes: 28);
}


// ==============================================================================
/*
 * EA: 0x19019FE2
 * CodeView symbol: ServerThreadProc
 * Demangled: ServerThreadProc
 * IDA name: ServerThreadProc
 * CodeView module: 6
 * CV address: segment 1 : 0x18FE2
 * Code length: 0x49B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall __noreturn ServerThreadProc(void *__formal)
{
  int v1; // [esp+0h] [ebp-28h]
  int databytes; // [esp+8h] [ebp-20h]
  char *data; // [esp+Ch] [ebp-1Ch]
  char *msgptr; // [esp+10h] [ebp-18h]
  unsigned int lastoffset; // [esp+14h] [ebp-14h]
  unsigned int offset; // [esp+18h] [ebp-10h]
  int result; // [esp+1Ch] [ebp-Ch]
  unsigned int serverbytes; // [esp+20h] [ebp-8h]
  char *serverdata; // [esp+24h] [ebp-4h]

  serverbytes = 0;
  serverdata = (char *)SMemAlloc(a1: 0x4000, a2: aSrvCpp_13, a3: 1913, a4: 0);
  while ( 1 )
  {
    if ( serverdata == nullptr || srv_serversocket == -1 || srv_shutdown != 0 )
      goto LABEL_58;
    result = recv(s: srv_serversocket, buf: &serverdata[serverbytes], len: 0x4000 - serverbytes, flags: 0);
    if ( result == -1 || result == 0 )
      break;
    if ( srv_shutdown != 0 )
      goto LABEL_58;
    serverbytes += result;
    offset = 0;
    do
    {
      while ( offset < serverbytes && (unsigned __int8)serverdata[offset] != 255 )
        ++offset;
      msgptr = &serverdata[offset];
      lastoffset = offset;
      if ( serverbytes - offset >= 4 && serverbytes - offset >= *((unsigned __int16 *)msgptr + 1) )
      {
        data = msgptr + 4;
        databytes = *((unsigned __int16 *)msgptr + 1) - 4;
        switch ( msgptr[1] )
        {
          case 4:
            OnServerList(a1: data, a2: databytes);
            break;
          case 5:
            OnClientId(a1: data, a2: databytes);
            break;
          case 6:
            OnStartVersioning(a1: data, a2: databytes);
            break;
          case 7:
            OnReportVersion(a1: data, a2: databytes);
            break;
          case 9:
            OnGetAdvListEx(a1: data, a2: databytes);
            break;
          case 10:
            OnEnterChat(a1: data, a2: databytes);
            break;
          case 11:
            OnGetChannelList(a1: data, a2: databytes);
            break;
          case 15:
            OnChatEvent(a1: data, a2: databytes);
            break;
          case 21:
            OnCheckAd(a1: data, a2: databytes);
            break;
          case 23:
            OnQueryMem(a1: data, a2: databytes);
            break;
          case 25:
            OnMessageBox(a1: data, a2: databytes);
            break;
          case 28:
            OnStartAdvEx3(a1: data, a2: databytes);
            break;
          case 29:
            OnLogonChallengeEx(a1: data, a2: databytes);
            break;
          case 32:
            OnBroadcast(a1: data, a2: databytes);
            break;
          case 35:
            OnSetCookie(a1: data, a2: databytes);
            break;
          case 36:
            OnGetCookie(a1: data, a2: databytes);
            break;
          case 37:
            OnPing(a1: data, a2: databytes);
            break;
          case 38:
            OnReadUserData(a1: data, a2: databytes);
            break;
          case 40:
            OnLogonChallenge(a1: data, a2: databytes);
            break;
          case 41:
            OnLogonResponse(a1: data, a2: databytes);
            break;
          case 42:
            OnCreateAccount(a1: data, a2: databytes);
            break;
          case 45:
            OnGetIconData(a1: data, a2: databytes);
            break;
          case 46:
            OnGetLadderData(a1: data, a2: databytes);
            break;
          case 47:
            OnFindLadderUser(a1: data, a2: databytes);
            break;
          case 48:
            OnCdKey(a1: data, a2: databytes);
            break;
          case 49:
            OnChangePassword(a1: data, a2: databytes);
            break;
          case 50:
            OnCheckDataFile(a1: data, a2: databytes);
            break;
          case 51:
            OnGetFileTime(a1: data, a2: databytes);
            break;
          default:
            break;
        }
        if ( (unsigned __int8)msgptr[1] < 0x34u )
          srv_responded[(unsigned __int8)msgptr[1]] = 1;
        if ( srv_waitevent != nullptr )
          SetEvent(hEvent: srv_waitevent);
        if ( *((unsigned __int16 *)msgptr + 1) >= 4u )
          v1 = *((unsigned __int16 *)msgptr + 1);
        else
          v1 = 4;
        offset += v1;
      }
    }
    while ( offset > lastoffset );
    if ( offset != 0 )
    {
      if ( offset >= serverbytes )
      {
        serverbytes = 0;
      }
      else
      {
        memmove(a1: serverdata, Src: &serverdata[offset], Size: serverbytes - offset);
        serverbytes -= offset;
      }
    }
  }
  SpiQueueExternalMessage(a1: byte_19043BDC, a2: byte_19043BD8, message: aYourConnection);
  QueueUiNotification(a1: 13, a2: nullptr, parambytes: 0, string1ptr: nullptr, string2ptr: nullptr);
  closesocket(s: srv_serversocket);
  srv_serversocket = -1;
LABEL_58:
  DisconnectFromServer();
  SMemFree(a1: serverdata, a2: aSrvCpp_14, a3: 2097, a4: 0);
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x1901A47D
 * CodeView symbol: OnBroadcast
 * Demangled: OnBroadcast
 * IDA name: OnBroadcast
 * CodeView module: 6
 * CV address: segment 1 : 0x1947D
 * Code length: 0x6E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnBroadcast(const char *a1, int a2)
{
  _DWORD rec[3]; // [esp+8h] [ebp-10h] BYREF
  int string2ptr; // [esp+14h] [ebp-4h] BYREF

  if ( a1 != nullptr && a2 != 0 )
  {
    if ( srv_inchat != 0 )
    {
      rec[0] = &unk_19043BE0;
      rec[1] = 0;
      string2ptr = (int)a1;
      rec[2] = 4;
      QueueUiNotification(a1: 6, a2: rec, parambytes: 0x10u, string1ptr: (char *)rec, (char *)&string2ptr);
    }
    else
    {
      SpiQueueExternalMessage(a1: byte_19043BE8, a2: byte_19043BE4, message: a1);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1901A4EB
 * CodeView symbol: OnCdKey
 * Demangled: OnCdKey
 * IDA name: OnCdKey
 * CodeView module: 6
 * CV address: segment 1 : 0x194EB
 * Code length: 0x43
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall OnCdKey(int *this)
{
  int *result; // eax

  result = this;
  srv_cdkeyresult = *this;
  if ( srv_cdkeyresult == 5 )
    return (int *)SStrCopy(a1: &srv_cdkeyuser, a2: this + 1, a3: 128);
  srv_cdkeyuser = 0;
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901A52E
 * CodeView symbol: OnChangePassword
 * Demangled: OnChangePassword
 * IDA name: OnChangePassword
 * CodeView module: 6
 * CV address: segment 1 : 0x1952E
 * Code length: 0x1B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall OnChangePassword(int *this)
{
  srv_changepassresult = *this;
  return this;
}


// ==============================================================================
/*
 * EA: 0x1901A549
 * CodeView symbol: OnChatEvent
 * Demangled: OnChatEvent
 * IDA name: OnChatEvent
 * CodeView module: 6
 * CV address: segment 1 : 0x19549
 * Code length: 0x460
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnChatEvent(const char *a1, unsigned int a2)
{
  size_t v2; // eax
  size_t v3; // eax
  size_t v4; // eax
  int v5[2]; // [esp+14h] [ebp-288h] BYREF
  int v6[2]; // [esp+1Ch] [ebp-280h] BYREF
  int v7[3]; // [esp+24h] [ebp-278h] BYREF
  int v8[2]; // [esp+30h] [ebp-26Ch] BYREF
  CHAR v9[272]; // [esp+38h] [ebp-264h] BYREF
  CHAR buffer[272]; // [esp+148h] [ebp-154h] BYREF
  int v11[2]; // [esp+258h] [ebp-44h] BYREF
  int v12; // [esp+260h] [ebp-3Ch]
  int v13; // [esp+264h] [ebp-38h] BYREF
  int string1ptr[3]; // [esp+268h] [ebp-34h] BYREF
  const char *rec; // [esp+274h] [ebp-28h] BYREF
  int string2ptr[4]; // [esp+278h] [ebp-24h] BYREF
  int eventid; // [esp+288h] [ebp-14h]
  const char *username; // [esp+28Ch] [ebp-10h]
  int latency; // [esp+290h] [ebp-Ch]
  const char *string; // [esp+294h] [ebp-8h]
  int flags; // [esp+298h] [ebp-4h]

  if ( a1 != nullptr && a2 >= 0xA )
  {
    eventid = *(_DWORD *)a1;
    flags = *((_DWORD *)a1 + 1);
    latency = *((_DWORD *)a1 + 2);
    username = a1 + 24;
    string = &a1[strlen(Str: a1 + 24) + 25];
    switch ( eventid )
    {
      case 1:
      case 2:
        rec = username;
        string2ptr[0] = (int)string;
        string2ptr[1] = flags;
        string2ptr[2] = latency;
        string2ptr[3] = eventid == 2;
        QueueUiNotification(a1: 4, a2: &rec, parambytes: 0x14u, string1ptr: (char *)&rec, (char *)string2ptr);
        break;
      case 3:
        string1ptr[0] = (int)username;
        string1ptr[1] = (int)string;
        string1ptr[2] = 1;
        QueueUiNotification(a1: 5, a2: string1ptr, parambytes: 0xCu, (char *)string1ptr, string2ptr: nullptr);
        break;
      case 4:
      case 5:
      case 6:
      case 10:
      case 18:
      case 19:
      case 23:
        if ( srv_inchat != 0 )
        {
          v11[0] = (int)username;
          v11[1] = flags;
          v13 = (int)string;
          v12 = 0;
          switch ( eventid )
          {
            case 4:
              v12 = 1;
              break;
            case 5:
              v12 = 3;
              break;
            case 6:
              v12 = 4;
              break;
            case 10:
              v12 = 2;
              break;
            case 18:
              v12 = 5;
              break;
            case 19:
              v12 = 7;
              break;
            case 23:
              v12 = 8;
              break;
            default:
              break;
          }
          QueueUiNotification(a1: 6, a2: v11, parambytes: 0x10u, string1ptr: (char *)v11, string2ptr: (char *)&v13);
        }
        else
        {
          switch ( eventid )
          {
            case 4:
              _wsprintfA(a1: buffer, a2: "<From: %s> %s", username, string);
              SpiQueueExternalMessage(a1: byte_19043BF8, a2: byte_19043BF4, message: buffer);
              break;
            case 6:
            case 18:
            case 19:
              SpiQueueExternalMessage(a1: byte_19043BF0, a2: byte_19043BEC, message: string);
              break;
            case 10:
              _wsprintfA(a1: v9, a2: "<To: %s> %s", username, string);
              SpiQueueExternalMessage(a1: byte_19043C00, a2: byte_19043BFC, message: v9);
              break;
            default:
              return;
          }
        }
        break;
      case 7:
        v8[0] = (int)string;
        v8[1] = flags;
        QueueUiNotification(a1: 3, a2: v8, parambytes: 8u, string1ptr: (char *)v8, string2ptr: nullptr);
        RemovePingUser(this: nullptr);
        break;
      case 9:
        v7[0] = (int)username;
        v7[1] = flags;
        v7[2] = latency;
        QueueUiNotification(a1: 14, a2: v7, parambytes: 0xCu, string1ptr: (char *)v7, string2ptr: nullptr);
        break;
      case 13:
        v2 = strlen(Str: string);
        QueueUiNotification(a1: 16, a2: string, parambytes: v2 + 1, string1ptr: nullptr, string2ptr: nullptr);
        break;
      case 14:
        v3 = strlen(Str: string);
        QueueUiNotification(a1: 17, a2: string, parambytes: v3 + 1, string1ptr: nullptr, string2ptr: nullptr);
        break;
      case 15:
        v4 = strlen(Str: string);
        QueueUiNotification(a1: 18, a2: string, parambytes: v4 + 1, string1ptr: nullptr, string2ptr: nullptr);
        break;
      case 21:
        v6[0] = (int)username;
        v6[1] = flags;
        QueueUiNotification(a1: 19, a2: v6, parambytes: 8u, string1ptr: (char *)v6, string2ptr: nullptr);
        break;
      case 22:
        v5[0] = (int)username;
        v5[1] = flags;
        QueueUiNotification(a1: 20, a2: v5, parambytes: 8u, string1ptr: (char *)v5, string2ptr: nullptr);
        break;
      default:
        return;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1901A9A9
 * CodeView symbol: OnCheckAd
 * Demangled: OnCheckAd
 * IDA name: OnCheckAd
 * CodeView module: 6
 * CV address: segment 1 : 0x199A9
 * Code length: 0xA1
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnCheckAd(int a1, unsigned int a2)
{
  _BYTE *newurl; // [esp+Ch] [ebp-10h]

  if ( a2 >= 0x11 && *(_DWORD *)a1 != srv_adnumber )
  {
    srv_adnumber = *(_DWORD *)a1;
    newurl = (_BYTE *)(a1 + 16 + strlen(Str: (const char *)(a1 + 16)) + 1);
    if ( *(_BYTE *)(a1 + 16) != 0 && *newurl != 0 )
      RequestFile(
        fileid: *(_DWORD *)a1,
        filedatatype: *(_DWORD *)(a1 + 4),
        filetime: a1 + 8,
        filename: a1 + 16,
        url: (int)newurl);
  }
}


// ==============================================================================
/*
 * EA: 0x1901AA4A
 * CodeView symbol: OnCheckDataFile
 * Demangled: OnCheckDataFile
 * IDA name: OnCheckDataFile
 * CodeView module: 6
 * CV address: segment 1 : 0x19A4A
 * Code length: 0x23
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__fastcall OnCheckDataFile(int *a1, unsigned int a2)
{
  int *result; // eax

  if ( a2 >= 4 )
  {
    srv_checkfileresponse = *a1;
    return a1;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901AA6D
 * CodeView symbol: OnClientId
 * Demangled: OnClientId
 * IDA name: OnClientId
 * CodeView module: 6
 * CV address: segment 1 : 0x19A6D
 * Code length: 0x7D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall OnClientId(_DWORD *a1, unsigned int a2)
{
  int result; // eax

  if ( a1 != nullptr && a2 >= 0x10 )
  {
    SRegSaveValue(a1: aConfiguration_6, a2: aRegistrationVe_0, a3: 2, a4: *a1);
    SRegSaveValue(a1: aConfiguration_7, a2: aRegistrationAu_0, a3: 2, a4: a1[1]);
    SRegSaveValue(a1: aConfiguration_8, a2: aClientId_0, a3: 2, a4: a1[2]);
    return SRegSaveValue(a1: aConfiguration_9, a2: aClientToken_0, a3: 2, a4: a1[3]);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901AAEA
 * CodeView symbol: OnCreateAccount
 * Demangled: OnCreateAccount
 * IDA name: OnCreateAccount
 * CodeView module: 6
 * CV address: segment 1 : 0x19AEA
 * Code length: 0x1B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall OnCreateAccount(int *this)
{
  srv_createaccountresult = *this;
  return this;
}


// ==============================================================================
/*
 * EA: 0x1901AB05
 * CodeView symbol: OnEnterChat
 * Demangled: OnEnterChat
 * IDA name: OnEnterChat
 * CodeView module: 6
 * CV address: segment 1 : 0x19B05
 * Code length: 0x92
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnEnterChat(int a1, int a2)
{
  int curra; // [esp+8h] [ebp-4h]
  int curr; // [esp+8h] [ebp-4h]

  if ( a1 != 0 && a2 != 0 )
  {
    curra = a1 + SStrCopy(a1: &srv_uniqueusername, a2: a1, a3: 128) + 1;
    curr = curra + SStrCopy(a1: &srv_userdesc, a2: curra, a3: 128) + 1;
    if ( curr - a1 >= a2 )
      SStrCopy(a1: srv_username, a2: &srv_uniqueusername, a3: 128);
    else
      SStrCopy(a1: srv_username, a2: curr, a3: 128);
  }
}


// ==============================================================================
/*
 * EA: 0x1901AB97
 * CodeView symbol: OnFindLadderUser
 * Demangled: OnFindLadderUser
 * IDA name: OnFindLadderUser
 * CodeView module: 6
 * CV address: segment 1 : 0x19B97
 * Code length: 0x1B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall OnFindLadderUser(int *this)
{
  srv_ladderuser = *this;
  return this;
}


// ==============================================================================
/*
 * EA: 0x1901ABB2
 * CodeView symbol: OnGetAdvListEx
 * Demangled: OnGetAdvListEx
 * IDA name: OnGetAdvListEx
 * CodeView module: 6
 * CV address: segment 1 : 0x19BB2
 * Code length: 0x1D9
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnGetAdvListEx(int *a1, unsigned int a2)
{
  int v2; // edx
  int gameage; // [esp+Ch] [ebp-158h]
  int gamemode; // [esp+10h] [ebp-154h]
  const char *namepassdesc; // [esp+14h] [ebp-150h]
  const char *namepassdesca; // [esp+14h] [ebp-150h]
  const char *namepassdescb; // [esp+14h] [ebp-150h]
  _DWORD newgame[79]; // [esp+18h] [ebp-14Ch] BYREF
  int categorybits; // [esp+154h] [ebp-10h]
  struct _SNETADDR *address; // [esp+158h] [ebp-Ch]
  size_t currptr; // [esp+15Ch] [ebp-8h]
  int games; // [esp+160h] [ebp-4h]

  srv_getadverrorcode = 0;
  if ( a1 != nullptr && a2 >= 4 )
  {
    games = *a1;
    currptr = (size_t)(a1 + 1);
    if ( games == 0 )
      srv_getadverrorcode = *(_DWORD *)currptr;
    while ( 1 )
    {
      v2 = games--;
      if ( v2 == 0 )
        break;
      categorybits = *(_DWORD *)currptr;
      address = (struct _SNETADDR *)(currptr + 8);
      gamemode = *(_DWORD *)(currptr + 24);
      gameage = *(_DWORD *)(currptr + 28);
      namepassdesc = (const char *)(currptr + 32);
      memset(a1: newgame, Val: 0, Size: sizeof(newgame));
      *(_OWORD *)&newgame[3] = *(_OWORD *)(currptr + 8);
      newgame[1] = gamemode;
      newgame[2] = time(Time: nullptr) - gameage;
      newgame[8] = GetTickCount();
      newgame[9] = categorybits;
      newgame[77] = global_programid;
      newgame[78] = global_versionid;
      SrvPingAddress(a1: address);
      SStrCopy(a1: &newgame[10], a2: namepassdesc, a3: 128);
      namepassdesca = &namepassdesc[strlen(Str: namepassdesc) + 1];
      namepassdescb = &namepassdesca[strlen(Str: namepassdesca) + 1];
      SStrCopy(a1: &newgame[42], a2: namepassdescb, a3: 128);
      currptr = (size_t)&namepassdescb[strlen(Str: namepassdescb) + 1];
      SpiAddGame(a1: (struct _SNETSPI_GAMELIST *)newgame);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1901AD8B
 * CodeView symbol: OnGetChannelList
 * Demangled: OnGetChannelList
 * IDA name: OnGetChannelList
 * CodeView module: 6
 * CV address: segment 1 : 0x19D8B
 * Code length: 0x67
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnGetChannelList(const char *a1, unsigned int a2)
{
  size_t v2; // eax
  const char *curr; // [esp+8h] [ebp-4h]

  if ( a1 != nullptr && a2 >= 2 )
  {
    for ( curr = a1; *curr != 0; curr += strlen(Str: curr) + 1 )
    {
      v2 = strlen(Str: curr);
      QueueUiNotification(a1: 1, a2: curr, parambytes: v2 + 1, string1ptr: nullptr, string2ptr: nullptr);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1901ADF2
 * CodeView symbol: OnGetCookie
 * Demangled: OnGetCookie
 * IDA name: OnGetCookie
 * CodeView module: 6
 * CV address: segment 1 : 0x19DF2
 * Code length: 0xCC
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnGetCookie(const char *a1, unsigned int a2)
{
  size_t v2; // esi
  size_t v3; // eax
  __int64 req; // [esp+Ch] [ebp-20Ch] BYREF
  char Destination[512]; // [esp+14h] [ebp-204h] BYREF
  const char *value; // [esp+214h] [ebp-4h]

  if ( a1 != nullptr && a2 >= 9 )
  {
    req = *(_QWORD *)a1;
    strcpy(Destination, Source: a1 + 8);
    value = &Destination[strlen(Str: Destination) + 1];
    SRegLoadString(a1: aCookies, a2: Destination, a3: 2, a4: value, a5: 256);
    v2 = strlen(Str: Destination);
    v3 = strlen(Str: value);
    SendServerMessage(a1: 0x24u, a2: (const char *)&req, databytes: v2 + v3 + 10);
  }
}


// ==============================================================================
/*
 * EA: 0x1901AEBE
 * CodeView symbol: OnGetIconData
 * Demangled: OnGetIconData
 * IDA name: OnGetIconData
 * CodeView module: 6
 * CV address: segment 1 : 0x19EBE
 * Code length: 0x3C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall OnGetIconData(void *this)
{
  return RequestFile(fileid: 0, filedatatype: 0, filetime: (int)this, filename: (int)this + 8, url: 0);
}


// ==============================================================================
/*
 * EA: 0x1901AEFA
 * CodeView symbol: OnGetFileTime
 * Demangled: OnGetFileTime
 * IDA name: OnGetFileTime
 * CodeView module: 6
 * CV address: segment 1 : 0x19EFA
 * Code length: 0x45
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall OnGetFileTime(void *this)
{
  return RequestFile(fileid: 0, filedatatype: 0, filetime: (int)this + 8, filename: (int)this + 16, url: 0);
}


// ==============================================================================
/*
 * EA: 0x1901AF3F
 * CodeView symbol: OnGetLadderData
 * Demangled: OnGetLadderData
 * IDA name: OnGetLadderData
 * CodeView module: 6
 * CV address: segment 1 : 0x19F3F
 * Code length: 0x67
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__fastcall OnGetLadderData(const void *a1, size_t a2)
{
  if ( srv_ladderdata != nullptr )
    SMemFree(a1: srv_ladderdata, a2: aSrvCpp_15, a3: 1325, a4: 0);
  srv_ladderdata = (void *)SMemAlloc(a1: a2, a2: aSrvCpp_16, a3: 1326, a4: 0);
  return memcpy(a1: srv_ladderdata, Src: a1, Size: a2);
}


// ==============================================================================
/*
 * EA: 0x1901AFA6
 * CodeView symbol: OnLogonChallenge
 * Demangled: OnLogonChallenge
 * IDA name: OnLogonChallenge
 * CodeView module: 6
 * CV address: segment 1 : 0x19FA6
 * Code length: 0x78
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall OnLogonChallenge(int *this)
{
  int result; // eax
  struct sockaddr srvaddr; // [esp+8h] [ebp-14h] BYREF
  size_t addrlen; // [esp+18h] [ebp-4h] BYREF

  srv_logonchallenge = *this;
  addrlen = 16;
  result = getpeername(s: srv_serversocket, name: &srvaddr, namelen: (int *)&addrlen);
  if ( result != -1 )
  {
    memcpy(a1: &srv_udpaddr, Src: &srvaddr, Size: addrlen);
    word_190435BA = htons(hostshort: 0x17E0u);
    return SpiSendSpecial(
             a1: (const struct sockaddr *)&srv_udpaddr,
             a2: 8u,
             data: &srv_logonchallenge,
             databytes: 4u,
             count: 2u);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901B01E
 * CodeView symbol: OnLogonChallengeEx
 * Demangled: OnLogonChallengeEx
 * IDA name: OnLogonChallengeEx
 * CodeView module: 6
 * CV address: segment 1 : 0x1A01E
 * Code length: 0x89
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall OnLogonChallengeEx(_DWORD *this)
{
  int result; // eax
  struct sockaddr srvaddr; // [esp+8h] [ebp-1Ch] BYREF
  size_t addrlen; // [esp+18h] [ebp-Ch] BYREF
  _DWORD response[2]; // [esp+1Ch] [ebp-8h] BYREF

  srv_logonchallenge = *(this + 1);
  response[0] = srv_logonchallenge;
  response[1] = *this;
  addrlen = 16;
  result = getpeername(s: srv_serversocket, name: &srvaddr, namelen: (int *)&addrlen);
  if ( result != -1 )
  {
    memcpy(a1: &srv_udpaddr, Src: &srvaddr, Size: addrlen);
    word_190435BA = htons(hostshort: 0x17E0u);
    return SpiSendSpecial(a1: (const struct sockaddr *)&srv_udpaddr, a2: 9u, data: response, databytes: 8u, count: 2u);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901B0A7
 * CodeView symbol: OnLogonResponse
 * Demangled: OnLogonResponse
 * IDA name: OnLogonResponse
 * CodeView module: 6
 * CV address: segment 1 : 0x1A0A7
 * Code length: 0x1B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall OnLogonResponse(int *this)
{
  srv_logonresult = *this;
  return this;
}


// ==============================================================================
/*
 * EA: 0x1901B0C2
 * CodeView symbol: OnMessageBox
 * Demangled: OnMessageBox
 * IDA name: OnMessageBox
 * CodeView module: 6
 * CV address: segment 1 : 0x1A0C2
 * Code length: 0x5C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnMessageBox(int a1, unsigned int a2)
{
  int rec; // [esp+8h] [ebp-Ch] BYREF
  int string2ptr[2]; // [esp+Ch] [ebp-8h] BYREF

  if ( a1 != 0 && a2 >= 6 )
  {
    rec = a1 + 4;
    string2ptr[0] = a1 + 4 + strlen(Str: (const char *)(a1 + 4)) + 1;
    string2ptr[1] = *(_DWORD *)a1;
    QueueUiNotification(a1: 24, a2: &rec, parambytes: 0xCu, string1ptr: (char *)&rec, (char *)string2ptr);
  }
}


// ==============================================================================
/*
 * EA: 0x1901B11E
 * CodeView symbol: OnPing
 * Demangled: OnPing
 * IDA name: OnPing
 * CodeView module: 6
 * CV address: segment 1 : 0x1A11E
 * Code length: 0x60
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall OnPing(const char *a1, int a2)
{
  int result; // eax
  int value; // [esp+8h] [ebp-4h] BYREF

  if ( a1 != nullptr )
  {
    value = 1;
    setsockopt(s: srv_serversocket, level: 6, optname: 1, optval: (const char *)&value, optlen: 4);
    SendServerMessage(a1: 0x25u, a2: a1, databytes: a2);
    value = 0;
    return setsockopt(s: srv_serversocket, level: 6, optname: 1, optval: (const char *)&value, optlen: 4);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901B17E
 * CodeView symbol: OnQueryMem
 * Demangled: OnQueryMem
 * IDA name: OnQueryMem
 * CodeView module: 6
 * CV address: segment 1 : 0x1A17E
 * Code length: 0xB7
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnQueryMem(int *a1, unsigned int a2)
{
  size_t bytes; // [esp+8h] [ebp-10h]
  char *buffer; // [esp+Ch] [ebp-Ch]
  int queryid; // [esp+10h] [ebp-8h]
  const void *address; // [esp+14h] [ebp-4h]

  if ( a1 != nullptr && a2 >= 0xC )
  {
    queryid = *a1;
    address = (const void *)a1[1];
    bytes = a1[2];
    if ( !IsBadReadPtr(lp: address, ucb: bytes) )
    {
      buffer = (char *)SMemAlloc(a1: bytes + 4, a2: aSrvCpp_17, a3: 1417, a4: 0);
      if ( buffer != nullptr )
      {
        *(_DWORD *)buffer = queryid;
        memcpy(a1: buffer + 4, Src: address, Size: bytes);
        SendServerMessage(a1: 0x17u, a2: buffer, databytes: bytes + 4);
        SMemFree(a1: buffer, a2: aSrvCpp_18, a3: 1423, a4: 0);
      }
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1901B235
 * CodeView symbol: OnReadUserData
 * Demangled: OnReadUserData
 * IDA name: OnReadUserData
 * CodeView module: 6
 * CV address: segment 1 : 0x1A235
 * Code length: 0x67
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__fastcall OnReadUserData(const void *a1, size_t a2)
{
  if ( srv_userdata != nullptr )
    SMemFree(a1: srv_userdata, a2: aSrvCpp_19, a3: 1428, a4: 0);
  srv_userdata = (void *)SMemAlloc(a1: a2, a2: aSrvCpp_20, a3: 1429, a4: 0);
  return memcpy(a1: srv_userdata, Src: a1, Size: a2);
}


// ==============================================================================
/*
 * EA: 0x1901B29C
 * CodeView symbol: OnReportVersion
 * Demangled: OnReportVersion
 * IDA name: OnReportVersion
 * CodeView module: 6
 * CV address: segment 1 : 0x1A29C
 * Code length: 0x9B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall OnReportVersion(int *a1, unsigned int a2)
{
  int result; // eax
  _DWORD filetime[2]; // [esp+8h] [ebp-8h] BYREF

  if ( a1 != nullptr && a2 >= 6 )
  {
    result = (int)a1;
    srv_authenticated = *a1;
    if ( srv_authenticated == 1 )
    {
      memcpy(a1: &srv_patchfiles, Src: a1 + 1, Size: a2 - 4);
      SRegSaveData(a1: aPatch_2, a2: aPatches, a3: 130, a4: a1 + 1, a5: a2 - 4);
      filetime[0] = 0;
      filetime[1] = 0;
      return RequestFile(fileid: 0, filedatatype: 0, (int)filetime, filename: (int)&srv_patchfiles, url: 0);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901B337
 * CodeView symbol: OnServerList
 * Demangled: OnServerList
 * IDA name: OnServerList
 * CodeView module: 6
 * CV address: segment 1 : 0x1A337
 * Code length: 0x49
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall OnServerList(int a1, unsigned int a2)
{
  int result; // eax

  if ( a1 != 0 && a2 >= 5 )
  {
    SRegSaveValue(a1: aConfiguration_10, a2: aServerVersion_0, a3: 2, a4: 1);
    return SRegSaveString(a1: aConfiguration_11, a2: aServerList_0, a3: 2, a4: a1 + 4);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901B380
 * CodeView symbol: OnSetCookie
 * Demangled: OnSetCookie
 * IDA name: OnSetCookie
 * CodeView module: 6
 * CV address: segment 1 : 0x1A380
 * Code length: 0x51
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall OnSetCookie(int a1, unsigned int a2)
{
  size_t v2; // eax

  if ( a1 != 0 && a2 >= 0xA )
  {
    v2 = strlen(Str: (const char *)(a1 + 8));
    SRegSaveString(a1: aCookies_0, a2: a1 + 8, a3: 2, a4: a1 + 8 + v2 + 1);
  }
}


// ==============================================================================
/*
 * EA: 0x1901B3D1
 * CodeView symbol: OnStartAdvEx3
 * Demangled: OnStartAdvEx3
 * IDA name: OnStartAdvEx3
 * CodeView module: 6
 * CV address: segment 1 : 0x1A3D1
 * Code length: 0x29
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__fastcall OnStartAdvEx3(int *a1, unsigned int a2)
{
  int *result; // eax

  if ( a1 != nullptr && a2 >= 4 )
  {
    srv_startadvresult = *a1;
    return a1;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901B3FA
 * CodeView symbol: OnStartVersioning
 * Demangled: OnStartVersioning
 * IDA name: OnStartVersioning
 * CodeView module: 6
 * CV address: segment 1 : 0x1A3FA
 * Code length: 0x7A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall OnStartVersioning(int a1, size_t a2)
{
  size_t v2; // eax
  const char *filename; // [esp+Ch] [ebp-4h]

  filename = (const char *)(a1 + 8);
  if ( strlen(Str: (const char *)(a1 + 8)) + 9 < a2 )
  {
    v2 = strlen(Str: filename);
    SStrCopy(a1: &srv_argstring, a2: &filename[v2 + 1], a3: 255);
    byte_1904386F = 0;
  }
  return RequestFile(fileid: 0, filedatatype: 0, filetime: a1, (int)filename, url: 0);
}


// ==============================================================================
/*
 * EA: 0x1901B474
 * CodeView symbol: RequestFile
 * Demangled: RequestFile
 * IDA name: RequestFile
 * CodeView module: 6
 * CV address: segment 1 : 0x1A474
 * Code length: 0x1EF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall RequestFile(
        DWORD a1,
        DWORD a2,
        int fileid,
        int filedatatype,
        const FILETIME *filetime,
        const char *filename,
        void *url)
{
  void *v7; // eax
  void *v8; // eax
  void *cacheddata; // [esp+10h] [ebp-18h] BYREF
  struct _FILETIME cachedfiletime; // [esp+14h] [ebp-14h] BYREF
  FILETIME *newreq; // [esp+1Ch] [ebp-Ch]
  unsigned int threadid; // [esp+20h] [ebp-8h] BYREF
  unsigned int cacheddatabytes; // [esp+24h] [ebp-4h] BYREF

  if ( filename == nullptr || *filename == 0 )
    return;
  if ( url == nullptr )
    url = &unk_19043C04;
  if ( CacheLoadFile(a1: filename, a2: &cachedfiletime, data: &cacheddata, databytes: &cacheddatabytes) != 0 )
  {
    if ( CompareFileTime(lpFileTime1: filetime, lpFileTime2: &cachedfiletime) == 0 )
    {
      if ( (a2 & 2) != 0 )
        SaveFile(bytes: cacheddatabytes, (int)filetime);
      ProcessFile(
        fileid,
        filedatatype,
        (char)filetime,
        (int)filename,
        (int)url,
        data: (int)cacheddata,
        databytes: cacheddatabytes);
      CacheFree(a1: cacheddata, a2: cacheddatabytes);
      return;
    }
    CacheFree(a1: cacheddata, a2: cacheddatabytes);
  }
  v7 = (void *)SMemAlloc(a1: 560, a2: aSrvCpp_21, a3: 1727, a4: 0);
  newreq = (FILETIME *)operator new(__formal: 48, ptr: v7);
  if ( newreq != nullptr )
  {
    memset(a1: newreq, Val: 0, Size: 0x230u);
    LOWORD(newreq->dwLowDateTime) = strlen(Str: filename) + 33;
    BYTE2(newreq->dwLowDateTime) = 0;
    HIBYTE(newreq->dwLowDateTime) = 1;
    newreq->dwHighDateTime = 1230518326;
    newreq[1].dwLowDateTime = global_programid;
    newreq[1].dwHighDateTime = fileid;
    newreq[2].dwLowDateTime = filedatatype;
    newreq[2].dwHighDateTime = 0;
    newreq[69].dwLowDateTime = a1;
    newreq[69].dwHighDateTime = a2;
    newreq[3] = *filetime;
    SStrCopy(a1: &newreq[4], a2: filename, a3: 260);
    SStrCopy(a1: &newreq[36].dwHighDateTime, a2: url, a3: 260);
    InterlockedIncrement(lpAddend: &srv_downloadthreads);
    v8 = (void *)_beginthreadex(
                   Security: nullptr,
                   StackSize: 0,
                   StartAddress: DownloadThreadProc,
                   ArgList: newreq,
                   InitFlag: 0,
                   ThrdAddr: &threadid);
    if ( v8 != nullptr )
      CloseHandle(hObject: v8);
  }
}


// ==============================================================================
/*
 * EA: 0x1901B663
 * CodeView symbol: DownloadThreadProc
 * Demangled: DownloadThreadProc
 * IDA name: DownloadThreadProc
 * CodeView module: 6
 * CV address: segment 1 : 0x1A663
 * Code length: 0x43D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall __noreturn DownloadThreadProc(const char *param)
{
  int v1; // eax
  int v2; // eax
  int existingdata; // [esp+0h] [ebp-3Ch]
  struct sockaddr addr; // [esp+8h] [ebp-34h] BYREF
  int rcvtimeout; // [esp+18h] [ebp-24h] BYREF
  char conntype[4]; // [esp+1Ch] [ebp-20h] BYREF
  int headersize; // [esp+20h] [ebp-1Ch] BYREF
  int addrlen; // [esp+24h] [ebp-18h] BYREF
  int keepalive; // [esp+28h] [ebp-14h] BYREF
  int buffer; // [esp+2Ch] [ebp-10h] BYREF
  int responseptr; // [esp+30h] [ebp-Ch]
  const char *requestptr; // [esp+34h] [ebp-8h]
  SOCKET newsocket; // [esp+38h] [ebp-4h]

  buffer = 0;
  newsocket = -1;
  requestptr = param;
  responseptr = 0;
  addrlen = 16;
  if ( srv_serversocket != -1 && getpeername(s: srv_serversocket, name: &addr, namelen: &addrlen) != -1 )
  {
    *(_WORD *)addr.sa_data = htons(hostshort: 0x17E0u);
    memset(a1: &addr.sa_data[6], Val: 0, Size: 8u);
    while ( 1 )
    {
      while ( 1 )
      {
        while ( 1 )
        {
          while ( 1 )
          {
            newsocket = socket(af: 2, type: 1, protocol: 0);
            if ( newsocket == -1 )
              goto LABEL_28;
            if ( connect(s: newsocket, name: &addr, namelen: 16) == 0 )
              break;
            if ( srv_cancelwait != 0 || *((_DWORD *)requestptr + 138) != -2147483646 )
              goto LABEL_28;
            closesocket(s: newsocket);
            newsocket = -1;
            Sleep(dwMilliseconds: 0x1388u);
          }
          keepalive = 1;
          setsockopt(s: newsocket, level: 0xFFFF, optname: 8, optval: (const char *)&keepalive, optlen: 4);
          rcvtimeout = 15000;
          setsockopt(s: newsocket, level: 0xFFFF, optname: 4102, optval: (const char *)&rcvtimeout, optlen: 4);
          conntype[0] = 2;
          if ( send(s: newsocket, buf: conntype, len: 1, flags: 0) != 1 )
            goto LABEL_28;
          v1 = send(s: newsocket, buf: requestptr, len: *(unsigned __int16 *)requestptr, flags: 0);
          if ( v1 != *(unsigned __int16 *)requestptr )
            goto LABEL_28;
          if ( recv(s: newsocket, buf: (char *)&headersize, len: 4, flags: 0) != 4 )
            goto LABEL_28;
          responseptr = SMemAlloc(a1: headersize, a2: aSrvCpp_22, a3: 866, a4: 0);
          if ( responseptr == 0 )
            goto LABEL_28;
          v2 = recv(s: newsocket, buf: (char *)(responseptr + 4), len: headersize - 4, flags: 0);
          if ( v2 != headersize - 4 )
            goto LABEL_28;
          if ( *((_DWORD *)requestptr + 139) != 2 )
            break;
          existingdata = CheckPartialFile(a1: responseptr + 24, a2: responseptr + 16);
          if ( existingdata == *((_DWORD *)requestptr + 5) )
            break;
          *((_DWORD *)requestptr + 5) = existingdata;
          closesocket(s: newsocket);
          newsocket = -1;
          SMemFree(a1: responseptr, a2: aSrvCpp_23, a3: 885, a4: 0);
          responseptr = 0;
        }
        if ( *((_DWORD *)requestptr + 139) != 2 )
          break;
        if ( DownloadToDisk(
               offset: *((_DWORD *)requestptr + 5),
               filebytes: *(_DWORD *)(responseptr + 4),
               filetime: responseptr + 16,
               downloadtype: *((_DWORD *)requestptr + 138)) != 0 )
          goto LABEL_27;
        if ( srv_cancelwait != 0 )
          goto LABEL_28;
        closesocket(s: newsocket);
        newsocket = -1;
        SMemFree(a1: responseptr, a2: aSrvCpp_24, a3: 906, a4: 0);
        responseptr = 0;
      }
      if ( DownloadToAny(
             filebytes: *(_DWORD *)(responseptr + 4),
             filetime: responseptr + 16,
             downloadtype: *((_DWORD *)requestptr + 138),
             savetype: *((_DWORD *)requestptr + 139),
             (int)&buffer) != 0 )
        break;
      if ( srv_cancelwait != 0 )
        goto LABEL_28;
      closesocket(s: newsocket);
      newsocket = -1;
      SMemFree(a1: responseptr, a2: aSrvCpp_25, a3: 927, a4: 0);
      responseptr = 0;
    }
LABEL_27:
    ProcessFile(
      fileid: *(_DWORD *)(responseptr + 8),
      filedatatype: *(_DWORD *)(responseptr + 12),
      filetime: responseptr + 16,
      filename: responseptr + 24,
      url: (int)(requestptr + 292),
      data: buffer,
      databytes: *(_DWORD *)(responseptr + 4));
  }
LABEL_28:
  if ( newsocket != -1 )
    closesocket(s: newsocket);
  if ( buffer != 0 )
    SMemFree(a1: buffer, a2: aSrvCpp_26, a3: 948, a4: 0);
  if ( responseptr != 0 )
    SMemFree(a1: responseptr, a2: aSrvCpp_27, a3: 949, a4: 0);
  if ( requestptr != nullptr )
    SMemFree(a1: requestptr, a2: aSrvCpp_28, a3: 950, a4: 0);
  InterlockedDecrement(lpAddend: &srv_downloadthreads);
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x1901BAA0
 * CodeView symbol: CheckPartialFile
 * Demangled: CheckPartialFile
 * IDA name: CheckPartialFile
 * CodeView module: 6
 * CV address: segment 1 : 0x1AAA0
 * Code length: 0xE0
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
DWORD __fastcall CheckPartialFile(const CHAR *a1, _QWORD *a2)
{
  HANDLE file; // [esp+8h] [ebp-30h]
  struct _FILETIME diff64; // [esp+1Ch] [ebp-1Ch]
  struct _FILETIME lastwritetime[2]; // [esp+24h] [ebp-14h] BYREF
  DWORD size; // [esp+34h] [ebp-4h]

  file = CreateFileA(
           lpFileName: a1,
           dwDesiredAccess: 0x80000000,
           dwShareMode: 3u,
           lpSecurityAttributes: nullptr,
           dwCreationDisposition: 3u,
           dwFlagsAndAttributes: 0x8000000u,
           hTemplateFile: nullptr);
  if ( file == (HANDLE)-1 )
    return 0;
  GetFileTime(hFile: file, lpCreationTime: nullptr, lpLastAccessTime: nullptr, lpLastWriteTime: lastwritetime);
  diff64 = (struct _FILETIME)(*(_QWORD *)lastwritetime - *a2);
  if ( (diff64.dwHighDateTime & 0x80000000) != 0 )
    diff64 = (struct _FILETIME)(*a2 - *(_QWORD *)lastwritetime);
  lastwritetime[1] = diff64;
  if ( *(unsigned __int64 *)&diff64 <= 0x2625A00 )
  {
    size = GetFileSize(hFile: file, lpFileSizeHigh: nullptr);
    CloseHandle(hObject: file);
    return size;
  }
  else
  {
    CloseHandle(hObject: file);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901BB80
 * CodeView symbol: DownloadToAny
 * Demangled: DownloadToAny
 * IDA name: DownloadToAny
 * CodeView module: 6
 * CV address: segment 1 : 0x1AB80
 * Code length: 0x125
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall DownloadToAny(
        SOCKET a1,
        char *a2,
        unsigned int filebytes,
        struct _FILETIME *filetime,
        int downloadtype,
        char savetype,
        void **buffer)
{
  int v7; // eax
  unsigned int currpos; // [esp+10h] [ebp-4h]

  *buffer = (void *)SMemAlloc(a1: filebytes, a2: aSrvCpp_29, a3: 685, a4: 8);
  currpos = 0;
  while ( currpos < filebytes )
  {
    if ( filebytes - currpos <= 0x5B4 )
      v7 = recv(s: a1, buf: (char *)*buffer + currpos, len: filebytes - currpos, flags: 0);
    else
      v7 = recv(s: a1, buf: (char *)*buffer + currpos, len: 1460, flags: 0);
    if ( v7 <= 0 || srv_cancelwait != 0 )
    {
      SMemFree(a1: *buffer, a2: aSrvCpp_30, a3: 695, a4: 0);
      *buffer = nullptr;
      return 0;
    }
    currpos += v7;
    if ( downloadtype == -2147483646 )
      UpdatePatchPercent(totalsize: filebytes);
  }
  if ( (savetype & 1) != 0 )
    CacheSaveFile(
      a1: a2,
      a2: *buffer,
      databytes: filebytes,
      lastwritetime: filetime,
      minexpireseconds: 1296000,
      maxexpireseconds: 2592000);
  if ( (savetype & 2) != 0 )
    SaveFile(bytes: filebytes, (int)filetime);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901BCA5
 * CodeView symbol: DownloadToDisk
 * Demangled: DownloadToDisk
 * IDA name: DownloadToDisk
 * CodeView module: 6
 * CV address: segment 1 : 0x1ACA5
 * Code length: 0x14E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall DownloadToDisk(
        SOCKET a1,
        const CHAR *a2,
        unsigned int offset,
        unsigned int filebytes,
        const FILETIME *filetime,
        int downloadtype)
{
  signed int v7; // eax
  signed int bytes; // [esp+Ch] [ebp-5C0h]
  char buffer[1460]; // [esp+10h] [ebp-5BCh] BYREF
  DWORD byteswritten; // [esp+5C4h] [ebp-8h] BYREF
  HANDLE file; // [esp+5C8h] [ebp-4h]

  file = CreateFileA(
           lpFileName: a2,
           dwDesiredAccess: 0x40000000u,
           dwShareMode: 0,
           lpSecurityAttributes: nullptr,
           dwCreationDisposition: 4u,
           dwFlagsAndAttributes: 0x80u,
           hTemplateFile: nullptr);
  if ( file == (HANDLE)-1 )
    return 0;
  if ( offset != 0 )
    SetFilePointer(hFile: file, lDistanceToMove: offset, lpDistanceToMoveHigh: nullptr, dwMoveMethod: 0);
  while ( offset < filebytes )
  {
    if ( filebytes - offset <= 0x5B4 )
      v7 = recv(s: a1, buf: buffer, len: filebytes - offset, flags: 0);
    else
      v7 = recv(s: a1, buf: buffer, len: 1460, flags: 0);
    bytes = v7;
    if ( v7 <= 0 || srv_cancelwait != 0 )
    {
      CloseHandle(hObject: file);
      return 0;
    }
    WriteFile(
      hFile: file,
      lpBuffer: buffer,
      nNumberOfBytesToWrite: v7,
      lpNumberOfBytesWritten: &byteswritten,
      lpOverlapped: nullptr);
    SetFileTime(hFile: file, lpCreationTime: filetime, lpLastAccessTime: filetime, lpLastWriteTime: filetime);
    offset += bytes;
    if ( downloadtype == -2147483646 )
      UpdatePatchPercent(totalsize: filebytes);
  }
  CloseHandle(hObject: file);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901BDF3
 * CodeView symbol: ProcessFile
 * Demangled: ProcessFile
 * IDA name: ProcessFile
 * CodeView module: 6
 * CV address: segment 1 : 0x1ADF3
 * Code length: 0x1A0
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ProcessFile(
        unsigned int a1,
        int a2,
        int fileid,
        int filedatatype,
        char filetime,
        const char *filename,
        int url,
        const void *data,
        size_t databytes)
{
  FILETIME v9; // [esp+Ch] [ebp-1Ch] BYREF
  const char *curr; // [esp+14h] [ebp-14h]
  _DWORD rec[2]; // [esp+18h] [ebp-10h] BYREF
  int string1ptr; // [esp+20h] [ebp-8h] BYREF
  int string2ptr; // [esp+24h] [ebp-4h] BYREF

  if ( a1 >= 0x80000000 )
  {
    switch ( a1 )
    {
      case 0x80000001:
        strcpy(Destination: srv_versionfile, Source: filename);
        break;
      case 0x80000002:
        CCritSect::Enter(this: &srv_patchcritsect);
        for ( curr = &srv_patchfiles;
              *curr != 0 && _stricmp(String1: curr, String2: filename) != 0;
              curr += strlen(Str: curr) + 1 )
        {
          ;
        }
        if ( *curr != 0 && curr[strlen(Str: curr) + 1] != 0 )
        {
          curr += strlen(Str: curr) + 1;
          v9.dwLowDateTime = 0;
          v9.dwHighDateTime = 0;
          RequestFile(a1: 0x80000002, a2: 2u, fileid: 0, filedatatype: 0, filetime: &v9, filename: curr, url: nullptr);
        }
        else
        {
          srv_patchfiles = 0;
          byte_190438A5 = 0;
        }
        CCritSect::Leave(this: &srv_patchcritsect);
        break;
      case 0x80000003:
        rec[0] = fileid;
        rec[1] = filedatatype;
        string1ptr = (int)filename;
        string2ptr = url;
        QueueUiNotification(a1: 22, a2: rec, parambytes: 0x10u, (char *)&string1ptr, (char *)&string2ptr);
        QueueUiNotification(a1: 23, a2: data, parambytes: databytes, string1ptr: nullptr, string2ptr: nullptr);
        break;
      case 0x80000004:
        QueueUiNotification(a1: 25, a2: data, parambytes: databytes, string1ptr: nullptr, string2ptr: nullptr);
        break;
      default:
        return;
    }
  }
  else
  {
    QueueUiNotification(a1, a2: data, parambytes: databytes, string1ptr: nullptr, string2ptr: nullptr);
  }
}


// ==============================================================================
/*
 * EA: 0x1901BF93
 * CodeView symbol: SaveFile
 * Demangled: SaveFile
 * IDA name: SaveFile
 * CodeView module: 6
 * CV address: segment 1 : 0x1AF93
 * Code length: 0x71
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HANDLE __fastcall SaveFile(const CHAR *a1, const void *a2, DWORD bytes, const FILETIME *filetime)
{
  HANDLE result; // eax
  HANDLE file; // [esp+8h] [ebp-8h]
  DWORD byteswritten; // [esp+Ch] [ebp-4h] BYREF

  result = CreateFileA(
             lpFileName: a1,
             dwDesiredAccess: 0x40000000u,
             dwShareMode: 0,
             lpSecurityAttributes: nullptr,
             dwCreationDisposition: 2u,
             dwFlagsAndAttributes: 0x80u,
             hTemplateFile: nullptr);
  file = result;
  if ( result != (HANDLE)-1 )
  {
    WriteFile(
      hFile: result,
      lpBuffer: a2,
      nNumberOfBytesToWrite: bytes,
      lpNumberOfBytesWritten: &byteswritten,
      lpOverlapped: nullptr);
    SetFileTime(hFile: file, lpCreationTime: filetime, lpLastAccessTime: filetime, lpLastWriteTime: filetime);
    return (HANDLE)CloseHandle(hObject: file);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901C004
 * CodeView symbol: UpdatePatchPercent
 * Demangled: UpdatePatchPercent
 * IDA name: UpdatePatchPercent
 * CodeView module: 6
 * CV address: segment 1 : 0x1B004
 * Code length: 0x15A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall UpdatePatchPercent(const char *a1, int a2, unsigned int totalsize)
{
  unsigned int v3; // [esp+0h] [ebp-1Ch]
  unsigned int v4; // [esp+4h] [ebp-18h]
  const char *curr; // [esp+10h] [ebp-Ch]
  int patchnumber; // [esp+14h] [ebp-8h]
  unsigned int totalpatches; // [esp+18h] [ebp-4h]

  patchnumber = 0;
  totalpatches = 0;
  CCritSect::Enter(this: &srv_patchcritsect);
  for ( curr = &srv_patchfiles; *curr != 0 && _stricmp(String1: curr, String2: a1) != 0; curr += strlen(Str: curr) + 1 )
  {
    ++patchnumber;
    ++totalpatches;
  }
  while ( *curr != 0 )
  {
    ++totalpatches;
    curr += strlen(Str: curr) + 1;
  }
  CCritSect::Leave(this: &srv_patchcritsect);
  if ( totalsize != 0 )
  {
    if ( totalpatches > 1 )
    {
      if ( totalsize * totalpatches / 0x64 != 0 )
        v3 = totalsize * totalpatches / 0x64;
      else
        v3 = 1;
      srv_patchpercent = (a2 + totalsize * patchnumber) / v3;
      if ( (unsigned int)srv_patchpercent > 0x64 )
        srv_patchpercent = 100;
    }
    else if ( totalsize <= 0x64 )
    {
      srv_patchpercent = 100;
    }
    else
    {
      if ( 100 * a2 / totalsize <= 0x64 )
        v4 = 100 * a2 / totalsize;
      else
        v4 = 100;
      srv_patchpercent = v4;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1901C15E
 * CodeView symbol: SrvIsConnected
 * Demangled: SrvIsConnected
 * IDA name: ?SrvIsConnected@@YIHXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x1B15E
 * Code length: 0xA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl SrvIsConnected()
{
  return srv_connected;
}


// ==============================================================================
/*
 * EA: 0x1901C168
 * CodeView symbol: SrvIsWaitingForResponse
 * Demangled: SrvIsWaitingForResponse
 * IDA name: ?SrvIsWaitingForResponse@@YIHXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x1B168
 * Code length: 0x11
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __cdecl SrvIsWaitingForResponse()
{
  return srv_waiting != 0;
}


// ==============================================================================
/*
 * EA: 0x1901C179
 * CodeView symbol: SrvJoinChannel
 * Demangled: SrvJoinChannel
 * IDA name: ?SrvJoinChannel@@YIHPBDH@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B179
 * Code length: 0x74
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvJoinChannel(const char *a1, int a2)
{
  size_t v2; // eax
  _DWORD req[33]; // [esp+8h] [ebp-84h] BYREF

  memset(a1: req, Val: 0, Size: sizeof(req));
  req[0] = a2 != 0 ? 2 : 0;
  SStrCopy(a1: &req[1], a2: a1, a3: 128);
  v2 = strlen(Str: (const char *)&req[1]);
  return SendServerMessage(a1: 0xCu, a2: (const char *)req, databytes: v2 + 5);
}


// ==============================================================================
/*
 * EA: 0x1901C1ED
 * CodeView symbol: SrvLogon
 * Demangled: SrvLogon
 * IDA name: ?SrvLogon@@YIHPBD00PAK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B1ED
 * Code length: 0x15A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvLogon(const char *a1, const char *a2, const char *password, unsigned int *errorcode)
{
  int v4; // eax
  DWORD rec; // [esp+8h] [ebp-140h] BYREF
  int v9; // [esp+Ch] [ebp-13Ch]
  _BYTE v10[20]; // [esp+10h] [ebp-138h] BYREF
  CHAR localpassword[128]; // [esp+24h] [ebp-124h] BYREF
  _DWORD response[2]; // [esp+A4h] [ebp-A4h] BYREF
  _BYTE v13[20]; // [esp+ACh] [ebp-9Ch] BYREF
  _BYTE v14[128]; // [esp+C0h] [ebp-88h] BYREF
  DWORD stringlength; // [esp+140h] [ebp-8h]
  int usernamelength; // [esp+144h] [ebp-4h]

  *errorcode = 0;
  SStrCopy(a1: srv_username, a2: a1, a3: 128);
  SStrCopy(a1: &srv_userdesc, a2, a3: 128);
  SStrCopy(a1: localpassword, a2: password, a3: 128);
  stringlength = SStrLen(a1: localpassword);
  CharLowerBuffA(lpsz: localpassword, cchLength: stringlength);
  rec = GetTickCount();
  v9 = srv_logonchallenge;
  ShaHash(a1: v10, a2: localpassword, bytes: stringlength);
  response[0] = rec;
  response[1] = v9;
  ShaHash(a1: v13, a2: &rec, bytes: 0x1Cu);
  v4 = SStrCopy(a1: v14, a2: a1, a3: 128);
  usernamelength = v4 + 1;
  SendServerMessage(a1: 0x29u, a2: (const char *)response, databytes: v4 + 29);
  if ( WaitForServerResponse(a1: 41, a2: 0) != 0 )
  {
    if ( srv_logonresult == 0 )
      *errorcode = 1;
    return srv_logonresult;
  }
  else
  {
    if ( SErrGetLastError() == 1223 )
      *errorcode = 2;
    else
      *errorcode = 1;
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901C347
 * CodeView symbol: SrvMaintainAds
 * Demangled: SrvMaintainAds
 * IDA name: ?SrvMaintainAds@@YIXXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x1B347
 * Code length: 0x65
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl SrvMaintainAds()
{
  DWORD currtime; // [esp+0h] [ebp-14h]
  _DWORD req[4]; // [esp+4h] [ebp-10h] BYREF

  currtime = GetTickCount();
  if ( currtime - lasttime >= 0x3A98 )
  {
    lasttime = currtime;
    req[0] = 1230518326;
    req[1] = global_programid;
    req[2] = srv_adnumber;
    req[3] = time(Time: nullptr) - srv_addisplaytime;
    SendServerMessage(a1: 0x15u, a2: (const char *)req, databytes: 16);
  }
}


// ==============================================================================
/*
 * EA: 0x1901C3AC
 * CodeView symbol: SrvMaintainLatencies
 * Demangled: SrvMaintainLatencies
 * IDA name: ?SrvMaintainLatencies@@YIXXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x1B3AC
 * Code length: 0x6D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl SrvMaintainLatencies()
{
  _DWORD *curr; // [esp+0h] [ebp-8h]
  DWORD currtime; // [esp+4h] [ebp-4h]

  currtime = GetTickCount();
  CCritSect::Enter(this: &srv_pingcritsect);
  for ( curr = (_DWORD *)srv_pinghead; curr != nullptr; curr = (_DWORD *)curr[7] )
  {
    if ( currtime - curr[5] > 0x7D0 && curr[6] == 0 )
    {
      curr[4] = 2000;
      curr[6] = currtime;
    }
  }
  CCritSect::Leave(this: &srv_pingcritsect);
}


// ==============================================================================
/*
 * EA: 0x1901C419
 * CodeView symbol: SrvNotifyClickAd
 * Demangled: SrvNotifyClickAd
 * IDA name: ?SrvNotifyClickAd@@YIXKH@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B419
 * Code length: 0x2E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvNotifyClickAd(unsigned int a1, int a2)
{
  _DWORD req[2]; // [esp+8h] [ebp-8h] BYREF

  req[0] = a1;
  req[1] = a2 != 0;
  SendServerMessage(a1: 0x16u, a2: (const char *)req, databytes: 8);
}


// ==============================================================================
/*
 * EA: 0x1901C447
 * CodeView symbol: SrvNotifyDisplayAd
 * Demangled: SrvNotifyDisplayAd
 * IDA name: ?SrvNotifyDisplayAd@@YIXKPBD0@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B447
 * Code length: 0xA8
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvNotifyDisplayAd(unsigned int a1, const char *a2, const char *url)
{
  int v3; // eax
  int v4; // eax
  _DWORD req[3]; // [esp+Ch] [ebp-218h] BYREF
  _BYTE v6[520]; // [esp+18h] [ebp-20Ch] BYREF
  _BYTE *curr; // [esp+220h] [ebp-4h]

  req[0] = 1230518326;
  req[1] = global_programid;
  req[2] = a1;
  curr = v6;
  v3 = SStrCopy(a1: v6, a2, a3: 128);
  curr += v3 + 1;
  v4 = SStrCopy(a1: curr, a2: url, a3: 128);
  SendServerMessage(a1: 0x21u, a2: (const char *)req, databytes: &curr[v4 + 13] - v6);
}


// ==============================================================================
/*
 * EA: 0x1901C4EF
 * CodeView symbol: SrvNotifyJoin
 * Demangled: SrvNotifyJoin
 * IDA name: ?SrvNotifyJoin@@YIXPBD0@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B4EF
 * Code length: 0x9F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvNotifyJoin(const char *a1, const char *a2)
{
  int v2; // eax
  int v3; // eax
  _DWORD req[2]; // [esp+Ch] [ebp-10Ch] BYREF
  _BYTE v6[256]; // [esp+14h] [ebp-104h] BYREF
  _BYTE *curr; // [esp+114h] [ebp-4h]

  req[0] = global_programid;
  req[1] = global_versionid;
  curr = v6;
  v2 = SStrCopy(a1: v6, a2: a1, a3: 128);
  curr += v2 + 1;
  v3 = SStrCopy(a1: curr, a2, a3: 128);
  SendServerMessage(a1: 0x22u, a2: (const char *)req, databytes: &curr[v3 + 9] - v6);
}


// ==============================================================================
/*
 * EA: 0x1901C58E
 * CodeView symbol: SrvPingAddress
 * Demangled: SrvPingAddress
 * IDA name: ?SrvPingAddress@@YIXPAU_SNETADDR@@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B58E
 * Code length: 0x26
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvPingAddress(struct _SNETADDR *a1)
{
  unsigned int latency; // [esp+4h] [ebp-4h] BYREF

  if ( !SrvGetLatency(a1, a2: &latency) )
    AddPingUser(a1);
}


// ==============================================================================
/*
 * EA: 0x1901C5B4
 * CodeView symbol: AddPingUser
 * Demangled: AddPingUser
 * IDA name: AddPingUser
 * CodeView module: 6
 * CV address: segment 1 : 0x1B5B4
 * Code length: 0xE8
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall AddPingUser(__int128 *this)
{
  void *v1; // eax
  void *result; // eax
  int newptr; // [esp+8h] [ebp-18h]
  __int128 dataportaddr; // [esp+Ch] [ebp-14h] BYREF
  int request; // [esp+1Ch] [ebp-4h] BYREF

  RemovePingUser(this);
  dataportaddr = *this;
  WORD1(dataportaddr) = htons(hostshort: 0x17E0u);
  memset(a1: (char *)&dataportaddr + 8, Val: 0, Size: 8u);
  v1 = (void *)SMemAlloc(a1: 32, a2: aSrvCpp_31, a3: 293, a4: 0);
  result = operator new(__formal: 32, ptr: v1);
  newptr = (int)result;
  if ( result != nullptr )
  {
    memset(a1: result, Val: 0, Size: 0x20u);
    *(__int128 *)newptr = dataportaddr;
    *(_DWORD *)(newptr + 20) = GetTickCount();
    CCritSect::Enter(this: &srv_pingcritsect);
    *(_DWORD *)(newptr + 28) = srv_pinghead;
    srv_pinghead = newptr;
    CCritSect::Leave(this: &srv_pingcritsect);
    request = 0;
    return (void *)SpiSendSpecial(
                     a1: (const struct sockaddr *)&dataportaddr,
                     a2: 3u,
                     data: &request,
                     databytes: 4u,
                     count: 1u);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901C69C
 * CodeView symbol: SrvProcessClientReq
 * Demangled: SrvProcessClientReq
 * IDA name: ?SrvProcessClientReq@@YIXPAU_SNETADDR@@PAEK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B69C
 * Code length: 0xF7
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvProcessClientReq(const struct sockaddr *a1, unsigned __int8 *a2, unsigned int databytes)
{
  DWORD currtime; // [esp+Ch] [ebp-14h]
  DWORD latency; // [esp+10h] [ebp-10h]
  _DWORD *userptr; // [esp+14h] [ebp-Ch]
  int response; // [esp+18h] [ebp-8h] BYREF
  int req; // [esp+1Ch] [ebp-4h]

  if ( databytes >= 4 )
  {
    req = *(_DWORD *)a2;
    if ( req != 0 )
    {
      if ( req == 1 )
      {
        CCritSect::Enter(this: &srv_pingcritsect);
        for ( userptr = (_DWORD *)srv_pinghead;
              userptr != nullptr && memcmp(Buf1: userptr, Buf2: a1, Size: 0x10u) != 0;
              userptr = (_DWORD *)userptr[7] )
        {
          ;
        }
        if ( userptr != nullptr )
        {
          currtime = GetTickCount();
          latency = currtime - userptr[5];
          if ( userptr[4] != 0 )
            userptr[4] = (latency + 3 * userptr[4]) >> 2;
          else
            userptr[4] = latency;
          userptr[6] = currtime;
        }
        CCritSect::Leave(this: &srv_pingcritsect);
      }
    }
    else
    {
      response = 1;
      SpiSendSpecial(a1, a2: 3u, data: &response, databytes: 4u, count: 1u);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1901C793
 * CodeView symbol: SrvProcessServerPing
 * Demangled: SrvProcessServerPing
 * IDA name: ?SrvProcessServerPing@@YIXPAEK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B793
 * Code length: 0x23
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SrvProcessServerPing(unsigned __int8 *a1, unsigned int a2)
{
  if ( a2 >= 4 )
    srv_udppingdata = *(_DWORD *)a1;
}


// ==============================================================================
/*
 * EA: 0x1901C7B6
 * CodeView symbol: SrvReadLadderData
 * Demangled: SrvReadLadderData
 * IDA name: ?SrvReadLadderData@@YIHKKKKKPAPAU_LADDERREC@@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B7B6
 * Code length: 0xFC
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvReadLadderData(
        unsigned int a1,
        unsigned int a2,
        unsigned int sortcriterion,
        unsigned int firstentry,
        unsigned int numentries,
        struct _LADDERREC **resultarray)
{
  unsigned int loop; // [esp+8h] [ebp-20h]
  _DWORD req[5]; // [esp+Ch] [ebp-1Ch] BYREF
  struct _LADDERREC *curr; // [esp+20h] [ebp-8h]
  unsigned int returnedentries; // [esp+24h] [ebp-4h]

  memset(a1: resultarray, Val: 0, Size: 4 * numentries);
  req[0] = a1;
  req[1] = a2;
  req[2] = sortcriterion;
  req[3] = firstentry;
  req[4] = numentries;
  if ( SendServerMessage(a1: 0x2Eu, a2: (const char *)req, databytes: 20) == 0 )
    return 0;
  if ( WaitForServerResponse(a1: 46, a2: 0) == 0 )
    return 0;
  if ( srv_ladderdata == nullptr || memcmp(Buf1: srv_ladderdata, Buf2: req, Size: 0x10u) != 0 )
    return 0;
  returnedentries = *((_DWORD *)srv_ladderdata + 4);
  curr = (struct _LADDERREC *)((char *)srv_ladderdata + 20);
  for ( loop = 0; loop < returnedentries; ++loop )
  {
    resultarray[loop] = curr;
    curr = (struct _LADDERREC *)((char *)curr + strlen(Str: (const char *)resultarray[loop] + 80) + 81);
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901C8B2
 * CodeView symbol: SrvReadUserData
 * Demangled: SrvReadUserData
 * IDA name: ?SrvReadUserData@@YIHKPAPBDK00@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1B8B2
 * Code length: 0x264
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvReadUserData(
        unsigned int a1,
        const char **a2,
        unsigned int numfields,
        const char **fieldnames,
        const char **fielddata)
{
  int stringbytes; // [esp+8h] [ebp-18h]
  char *request; // [esp+Ch] [ebp-14h]
  char *curr; // [esp+10h] [ebp-10h]
  const char *curra; // [esp+10h] [ebp-10h]
  unsigned int field; // [esp+14h] [ebp-Ch]
  unsigned int fielda; // [esp+14h] [ebp-Ch]
  unsigned int fieldb; // [esp+14h] [ebp-Ch]
  unsigned int user; // [esp+18h] [ebp-8h]
  unsigned int usera; // [esp+18h] [ebp-8h]
  unsigned int userb; // [esp+18h] [ebp-8h]

  stringbytes = 0;
  for ( user = 0; user < a1; ++user )
    stringbytes += SStrLen(a1: a2[user]) + 1;
  for ( field = 0; field < numfields; ++field )
    stringbytes += SStrLen(a1: fieldnames[field]) + 1;
  request = (char *)SMemAlloc(a1: stringbytes + 12, a2: aSrvCpp_32, a3: 3322, a4: 0);
  *(_DWORD *)request = a1;
  *((_DWORD *)request + 1) = numfields;
  *((_DWORD *)request + 2) = GetTickCount();
  curr = request + 12;
  for ( usera = 0; usera < a1; ++usera )
    curr += SStrCopy(a1: curr, a2: a2[usera], a3: 0x7FFFFFFF) + 1;
  for ( fielda = 0; fielda < numfields; ++fielda )
    curr += SStrCopy(a1: curr, a2: fieldnames[fielda], a3: 0x7FFFFFFF) + 1;
  if ( SendServerMessage(a1: 0x26u, a2: request, databytes: curr - request) != 0 )
  {
    if ( WaitForServerResponse(a1: 38, a2: 0) != 0 )
    {
      if ( srv_userdata != nullptr && memcmp(Buf1: srv_userdata, Buf2: request, Size: 0xCu) == 0 )
      {
        SMemFree(a1: request, a2: aSrvCpp_36, a3: 3361, a4: 0);
        curra = (char *)srv_userdata + 12;
        for ( userb = 0; userb < a1; ++userb )
        {
          for ( fieldb = 0; fieldb < numfields; ++fieldb )
          {
            (&fielddata[fieldb])[numfields * userb] = curra;
            curra += SStrLen(a1: curra) + 1;
          }
        }
        return 1;
      }
      else
      {
        SMemFree(a1: request, a2: aSrvCpp_35, a3: 3356, a4: 0);
        return 0;
      }
    }
    else
    {
      SMemFree(a1: request, a2: aSrvCpp_34, a3: 3349, a4: 0);
      return 0;
    }
  }
  else
  {
    SMemFree(a1: request, a2: aSrvCpp_33, a3: 3343, a4: 0);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901CB16
 * CodeView symbol: SrvRequestFile
 * Demangled: SrvRequestFile
 * IDA name: ?SrvRequestFile@@YIHPBDK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1BB16
 * Code length: 0x70
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvRequestFile(const char *a1, unsigned int a2)
{
  int v2; // eax
  _DWORD request[2]; // [esp+8h] [ebp-110h] BYREF
  _BYTE v5[260]; // [esp+10h] [ebp-108h] BYREF
  _BYTE *curr; // [esp+114h] [ebp-4h]

  request[0] = a2;
  request[1] = 0;
  curr = v5;
  v2 = SStrCopy(a1: v5, a2: a1, a3: 260);
  return SendServerMessage(a1: 0x33u, a2: (const char *)request, databytes: &curr[v2 + 1] - (_BYTE *)request);
}


// ==============================================================================
/*
 * EA: 0x1901CB86
 * CodeView symbol: SrvSendChatString
 * Demangled: SrvSendChatString
 * IDA name: ?SrvSendChatString@@YIHPBD@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1BB86
 * Code length: 0x25
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvSendChatString(const char *a1)
{
  size_t v1; // eax

  v1 = strlen(Str: a1);
  return SendServerMessage(a1: 0xEu, a2: a1, databytes: v1 + 1);
}


// ==============================================================================
/*
 * EA: 0x1901CBAB
 * CodeView symbol: SrvSendGameResult
 * Demangled: SrvSendGameResult
 * IDA name: ?SrvSendGameResult@@YIHKKPAPBDPAKPBD2@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1BBAB
 * Code length: 0x1A9
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SrvSendGameResult(
        unsigned int a1,
        unsigned int a2,
        const char **namearray,
        unsigned int *resultarray,
        const char *textgameresult,
        const char *textplayerresult)
{
  int v6; // eax
  size_t v7; // eax
  int v8; // eax
  int v9; // eax
  int stringbytes; // [esp+10h] [ebp-14h]
  size_t stringbytesa; // [esp+10h] [ebp-14h]
  unsigned int name; // [esp+14h] [ebp-10h]
  unsigned int namea; // [esp+14h] [ebp-10h]
  int request; // [esp+18h] [ebp-Ch]
  unsigned int curr; // [esp+1Ch] [ebp-8h]
  int curra; // [esp+1Ch] [ebp-8h]
  int v20; // [esp+20h] [ebp-4h]

  stringbytes = 0;
  for ( name = 0; name < a2; ++name )
  {
    if ( namearray[name] != nullptr )
      v6 = SStrLen(a1: namearray[name]);
    else
      v6 = SStrLen(a1: &unk_19043C08);
    stringbytes += v6 + 1;
  }
  stringbytesa = stringbytes + strlen(Str: textgameresult) + 1;
  v7 = strlen(Str: textplayerresult);
  request = SMemAlloc(a1: stringbytesa + v7 + 1 + 4 * a2 + 8, a2: aSrvCpp_37, a3: 3415, a4: 0);
  *(_DWORD *)request = a1;
  *(_DWORD *)(request + 4) = a2;
  memcpy(a1: (void *)(request + 8), Src: resultarray, Size: 4 * a2);
  curr = request + 4 * a2 + 8;
  for ( namea = 0; namea < a2; ++namea )
  {
    if ( namearray[namea] != nullptr )
      v8 = SStrCopy(a1: curr, a2: namearray[namea], a3: 0x7FFFFFFF);
    else
      v8 = SStrCopy(a1: curr, a2: &unk_19043C0C, a3: 0x7FFFFFFF);
    curr += v8 + 1;
  }
  curra = curr + SStrCopy(a1: curr, a2: textgameresult, a3: 0x7FFFFFFF) + 1;
  v9 = SStrCopy(a1: curra, a2: textplayerresult, a3: 0x7FFFFFFF);
  v20 = SendServerMessage(a1: 0x2Cu, a2: (const char *)request, databytes: curra + v9 + 1 - request);
  SMemFree(a1: request, a2: aSrvCpp_38, a3: 3435, a4: 0);
  return v20;
}


// ==============================================================================
/*
 * EA: 0x1901CD54
 * CodeView symbol: SrvSetBetaId
 * Demangled: SrvSetBetaId
 * IDA name: ?SrvSetBetaId@@YIXK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1BD54
 * Code length: 0xB
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void SrvSetBetaId()
{
  ;
}


// ==============================================================================
/*
 * EA: 0x1901CD5F
 * CodeView symbol: SrvStartAdvertisingGame
 * Demangled: SrvStartAdvertisingGame
 * IDA name: ?SrvStartAdvertisingGame@@YIHPBD00KKKKK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1BD5F
 * Code length: 0x164
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall SrvStartAdvertisingGame(
        const char *a1,
        const char *a2,
        const char *gamedescription,
        unsigned int gamemode,
        unsigned int gameage,
        unsigned int gamecategorybits,
        unsigned int optcategorybits,
        unsigned int ladderid)
{
  int v8; // eax
  int v9; // eax
  int v10; // eax
  struct sockaddr gameaddr; // [esp+Ch] [ebp-1ACh] BYREF
  int length; // [esp+1Ch] [ebp-19Ch]
  _DWORD req[5]; // [esp+20h] [ebp-198h] BYREF
  _BYTE v17[384]; // [esp+34h] [ebp-184h] BYREF
  _BYTE *curr; // [esp+1B4h] [ebp-4h]

  srv_startadvresult = 1;
  SpiGetGameAddress(a1: &gameaddr);
  req[0] = gamemode;
  req[1] = gameage;
  req[2] = gamecategorybits;
  req[3] = optcategorybits;
  req[4] = ladderid;
  curr = v17;
  v8 = SStrCopy(a1: v17, a2: a1, a3: 128);
  curr += v8 + 1;
  v9 = SStrCopy(a1: curr, a2, a3: 128);
  curr += v9 + 1;
  v10 = SStrCopy(a1: curr, a2: gamedescription, a3: 128);
  curr += v10 + 1;
  length = curr + 20 - v17;
  if ( SendServerMessage(a1: 0x1Cu, a2: (const char *)req, databytes: length) == 0 )
    return false;
  if ( (gamemode & 4) != 0 )
    return true;
  if ( WaitForServerResponse(a1: 28, a2: 4) != 0 )
  {
    if ( srv_startadvresult == 1 )
    {
      SErrSetLastError(a1: 183);
    }
    else if ( srv_startadvresult == 2 )
    {
      SErrSetLastError(a1: -2062548859);
    }
    return srv_startadvresult == 0;
  }
  else
  {
    SErrSetLastError(a1: 1232);
    return false;
  }
}


// ==============================================================================
/*
 * EA: 0x1901CEC3
 * CodeView symbol: SrvStopAdvertisingGame
 * Demangled: SrvStopAdvertisingGame
 * IDA name: ?SrvStopAdvertisingGame@@YIHXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x1BEC3
 * Code length: 0x1D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __cdecl SrvStopAdvertisingGame()
{
  return SendServerMessage(a1: 2u, a2: nullptr, databytes: 0) != 0;
}


// ==============================================================================
/*
 * EA: 0x1901CEE0
 * CodeView symbol: SrvWriteUserData
 * Demangled: SrvWriteUserData
 * IDA name: ?SrvWriteUserData@@YGHPBDKPAPBD1@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1BEE0
 * Code length: 0x163
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SrvWriteUserData(
        const char *username,
        unsigned int numfields,
        const char **fieldnames,
        const char **fielddata)
{
  int stringbytes; // [esp+0h] [ebp-14h]
  int stringbytesa; // [esp+0h] [ebp-14h]
  int request; // [esp+4h] [ebp-10h]
  int curr; // [esp+8h] [ebp-Ch]
  unsigned int field; // [esp+Ch] [ebp-8h]
  unsigned int fielda; // [esp+Ch] [ebp-8h]
  unsigned int fieldb; // [esp+Ch] [ebp-8h]
  int v12; // [esp+10h] [ebp-4h]

  stringbytes = SStrLen(a1: username) + 1;
  for ( field = 0; field < numfields; ++field )
  {
    stringbytesa = stringbytes + SStrLen(a1: fieldnames[field]) + 1;
    stringbytes = stringbytesa + SStrLen(a1: fielddata[field]) + 1;
  }
  request = SMemAlloc(a1: stringbytes + 8, a2: aSrvCpp_39, a3: 3524, a4: 0);
  *(_DWORD *)request = 1;
  *(_DWORD *)(request + 4) = numfields;
  curr = request + 8 + SStrCopy(a1: request + 8, a2: username, a3: 0x7FFFFFFF) + 1;
  for ( fielda = 0; fielda < numfields; ++fielda )
    curr += SStrCopy(a1: curr, a2: fieldnames[fielda], a3: 0x7FFFFFFF) + 1;
  for ( fieldb = 0; fieldb < numfields; ++fieldb )
    curr += SStrCopy(a1: curr, a2: fielddata[fieldb], a3: 0x7FFFFFFF) + 1;
  v12 = SendServerMessage(a1: 0x27u, a2: (const char *)request, databytes: curr - request);
  SMemFree(a1: request, a2: aSrvCpp_40, a3: 3544, a4: 0);
  return v12;
}


// ==============================================================================
/*
 * EA: 0x1901D050
 * CodeView symbol: TListAddPtr
 * Demangled: TListAddPtr
 * IDA name: ?TListAddPtr@@YIHPAPAU_CONNECTREC@@PAU1@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x1C050
 * Code length: 0x38
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Srv.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Srv.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAddPtr(struct _CONNECTREC **a1, struct _CONNECTREC *a2)
{
  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  *((_DWORD *)a2 + 2) = *a1;
  *a1 = a2;
  return 1;
}

