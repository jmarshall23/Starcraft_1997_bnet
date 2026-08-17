/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 *
 * CodeView module object:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 42
 */


// ==============================================================================
/*
 * EA: 0x180038F0
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: unknown_libname_3
 * CodeView module: 9
 * CV address: segment 1 : 0x28F0
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_3()
{
  sub_180038FF();
  return sub_1800390E();
}


// ==============================================================================
/*
 * EA: 0x180038FF
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: sub_180038FF
 * CodeView module: 9
 * CV address: segment 1 : 0x28FF
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *sub_180038FF()
{
  return CCritSect::CCritSect(this: (CCritSect *)&sgPktCrit);
}


// ==============================================================================
/*
 * EA: 0x1800390E
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: sub_1800390E
 * CodeView module: 9
 * CV address: segment 1 : 0x290E
 * Code length: 0x12
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1800390E()
{
  return atexit(a1: sub_18003920);
}


// ==============================================================================
/*
 * EA: 0x18003920
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: sub_18003920
 * CodeView module: 9
 * CV address: segment 1 : 0x2920
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_18003920()
{
  CCritSect::~CCritSect(this: (CCritSect *)&sgPktCrit);
}


// ==============================================================================
/*
 * EA: 0x1800392F
 * CodeView symbol: $E9
 * Demangled: $E9
 * IDA name: $E9
 * CodeView module: 9
 * CV address: segment 1 : 0x292F
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
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
 * EA: 0x1800393E
 * CodeView symbol: $E6
 * Demangled: $E6
 * IDA name: $E6
 * CodeView module: 9
 * CV address: segment 1 : 0x293E
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E6()
{
  return TSList<TPKT,TSGetLink<TPKT>>::TSList<TPKT,TSGetLink<TPKT>>(a1: &sgPktList);
}


// ==============================================================================
/*
 * EA: 0x1800394D
 * CodeView symbol: $E8
 * Demangled: $E8
 * IDA name: $E8
 * CodeView module: 9
 * CV address: segment 1 : 0x294D
 * Code length: 0x12
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int _E8()
{
  return atexit(a1: _E7);
}


// ==============================================================================
/*
 * EA: 0x1800395F
 * CodeView symbol: $E7
 * Demangled: $E7
 * IDA name: $E7
 * CodeView module: 9
 * CV address: segment 1 : 0x295F
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl _E7()
{
  TSList<TPKT,TSGetLink<TPKT>>::~TSList<TPKT,TSGetLink<TPKT>>(a1: &sgPktList);
}


// ==============================================================================
/*
 * EA: 0x1800396E
 * CodeView symbol: LatencySend
 * Demangled: LatencySend
 * IDA name: ?LatencySend@@YGHKPAPAU_SNETADDR@@PAXK@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x296E
 * Code length: 0x143
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall LatencySend(unsigned int addresses, struct _SNETADDR **addrlist, void *data, size_t databytes)
{
  unsigned int addr; // [esp+0h] [ebp-8h]
  _DWORD *pPkt; // [esp+4h] [ebp-4h]

  if ( addresses != 0 && addrlist != nullptr && data != nullptr && databytes != 0 )
  {
    CCritSect::Enter(this: (CCritSect *)&sgPktCrit);
    pPkt = (_DWORD *)TSList<TPKT,TSGetLink<TPKT>>::NewNode(
                       location: 2,
                       extrabytes: 20 * addresses + databytes,
                       flags: 0);
    pPkt[2] = GetTickCount();
    pPkt[3] = addresses;
    pPkt[4] = (char *)pPkt + databytes + 24;
    pPkt[5] = databytes;
    memcpy(a1: pPkt + 6, Src: data, Size: databytes);
    for ( addr = 0; addr < addresses; ++addr )
    {
      *(_DWORD *)(pPkt[4] + 4 * addr) = (char *)&pPkt[4 * addr + 6 + addresses] + databytes;
      *(_OWORD *)*(_DWORD *)(pPkt[4] + 4 * addr) = *((_OWORD *)*addrlist + addr);
    }
    PacketCorrupt(a1: pPkt);
    pPkt[2] += sub_18003AB1() % (unsigned int)(dword_1802833C - s_testparms) + s_testparms;
    CCritSect::Leave(this: (CCritSect *)&sgPktCrit);
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
 * EA: 0x18003AB1
 * CodeView symbol: PickRandomNumber
 * Demangled: PickRandomNumber
 * IDA name: sub_18003AB1
 * CodeView module: 9
 * CV address: segment 1 : 0x2AB1
 * Code length: 0x72
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_18003AB1()
{
  if ( sgRandomSeed == 0 )
    sgRandomSeed = 123459876;
  sgRandomSeed = 16807 * (sgRandomSeed % 0x1F31Du) - 2836 * (sgRandomSeed / 0x1F31Du);
  sgRandomSeed += sgRandomSeed < 0 ? 0x7FFFFFFF : 0;
  return sgRandomSeed;
}


// ==============================================================================
/*
 * EA: 0x18003B23
 * CodeView symbol: PacketCorrupt
 * Demangled: PacketCorrupt
 * IDA name: PacketCorrupt
 * CodeView module: 9
 * CV address: segment 1 : 0x2B23
 * Code length: 0x1BC
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall PacketCorrupt(_DWORD *this)
{
  char *v2; // edx
  _BYTE *v3; // [esp+0h] [ebp-1Ch]
  unsigned int bit; // [esp+8h] [ebp-14h]
  int corruptiontype; // [esp+14h] [ebp-8h]
  int corrupted; // [esp+18h] [ebp-4h]

  if ( s_shutdown != 0 )
    return 0;
  if ( sub_18003AB1() % 0x64u < dword_18028340 )
  {
    corrupted = 0;
    corruptiontype = sub_18003AB1();
    if ( (s_flags & 1) != 0 && *(this + 5) != 0 && (corruptiontype & 0xF) == 0 )
    {
      *(this + 5) = (unsigned int)sub_18003AB1() % *(this + 5);
      if ( *(this + 5) == 0 )
        *(this + 5) = 1;
      corrupted = 1;
    }
    if ( (s_flags & 2) != 0 )
    {
      if ( (corruptiontype & 0xF0) == 0 )
      {
        memmove(a1: this + 6, Src: (char *)this + 25, Size: *(this + 5) - 1);
        corrupted = 1;
      }
      if ( (corruptiontype & 0xF00) == 0 )
      {
        memmove(a1: (char *)this + 25, Src: this + 6, Size: *(this + 5) - 1);
        corrupted = 1;
      }
    }
    if ( (s_flags & 4) != 0 && (corruptiontype & 0xF000) == 0 )
    {
      v3 = (_BYTE *)(*(this + 4) + 4 * ((unsigned int)sub_18003AB1() % *(this + 3)));
      *v3 += sub_18003AB1();
      corrupted = 1;
    }
    if ( (s_flags & 8) != 0 && *(this + 5) != 0 && (corrupted == 0 || (corruptiontype & 0xF0000) == 0) )
    {
      bit = sub_18003AB1();
      v2 = (char *)this + (bit >> 3) % *(this + 5);
      v2[24] ^= 1 << (bit & 7);
    }
  }
  return *(this + 5);
}


// ==============================================================================
/*
 * EA: 0x18003CDF
 * CodeView symbol: LatencyDestroy
 * Demangled: LatencyDestroy
 * IDA name: ?LatencyDestroy@@YIHXZ
 * CodeView module: 9
 * CV address: segment 1 : 0x2CDF
 * Code length: 0xC6
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl LatencyDestroy()
{
  int pTemp; // [esp+4h] [ebp-8h]

  s_shutdown = 1;
  s_sendfcn = 0;
  WaitForSingleObject(hHandle: s_sendthread, dwMilliseconds: 0x64u);
  CloseHandle(hObject: s_sendthread);
  CCritSect::Enter(this: (CCritSect *)&sgPktCrit);
  for ( pTemp = TSList<TPKT,TSGetLink<TPKT>>::Head(a1: &sgPktList);
        pTemp > 0;
        pTemp = TSList<TPKT,TSGetLink<TPKT>>::DeleteNode(ptr: pTemp) )
  {
    ;
  }
  CCritSect::Leave(this: (CCritSect *)&sgPktCrit);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18003DA5
 * CodeView symbol: LatencyInitialize
 * Demangled: LatencyInitialize
 * IDA name: ?LatencyInitialize@@YIHP6GHKPAPAU_SNETADDR@@PAXK@ZK@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x2DA5
 * Code length: 0x163
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall LatencyInitialize(int (__stdcall *a1)(unsigned int, struct _SNETADDR **, void *, unsigned int), int a2)
{
  int v3; // [esp+0h] [ebp-14h]
  unsigned int id; // [esp+Ch] [ebp-8h] BYREF
  unsigned int dwTemp; // [esp+10h] [ebp-4h] BYREF

  if ( a1 == nullptr )
    return 0;
  s_sendfcn = (int)a1;
  s_flags = a2;
  sgRandomSeed = GetTickCount();
  if ( SRegLoadValue(a1: aLatencytest, a2: aMinpacketdelay, a3: 0, a4: &dwTemp) != 0 )
    s_testparms = dwTemp;
  if ( SRegLoadValue(a1: aLatencytest_0, a2: aMaxpacketdelay, a3: 0, a4: &dwTemp) != 0 )
    dword_1802833C = dwTemp;
  if ( SRegLoadValue(a1: aLatencytest_1, a2: aCorruptpercent, a3: 0, a4: &dwTemp) != 0 )
  {
    if ( dwTemp >= 0x32 )
      v3 = 50;
    else
      v3 = dwTemp;
    dword_18028340 = v3;
  }
  if ( dword_1802833C <= (unsigned int)s_testparms )
    dword_1802833C = s_testparms + 1;
  SRegSaveValue(a1: aLatencytest_2, a2: aMinpacketdelay_0, a3: 0, a4: s_testparms);
  SRegSaveValue(a1: aLatencytest_3, a2: aMaxpacketdelay_0, a3: 0, a4: dword_1802833C);
  SRegSaveValue(a1: aLatencytest_4, a2: aCorruptpercent_0, a3: 0, a4: dword_18028340);
  s_shutdown = 0;
  s_sendthread = (HANDLE)_beginthreadex(
                           Security: nullptr,
                           StackSize: 0,
                           StartAddress: SendThreadProc,
                           ArgList: nullptr,
                           InitFlag: 0,
                           ThrdAddr: &id);
  if ( s_sendthread != nullptr )
  {
    SetThreadPriority(hThread: s_sendthread, nPriority: 2);
    return 1;
  }
  else
  {
    LatencyDestroy();
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18003F08
 * CodeView symbol: SendThreadProc
 * Demangled: SendThreadProc
 * IDA name: SendThreadProc
 * CodeView module: 9
 * CV address: segment 1 : 0x2F08
 * Code length: 0x18D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall __noreturn SendThreadProc(void *param)
{
  void *v1; // [esp+0h] [ebp-28h]
  int v2; // [esp+4h] [ebp-24h]
  TPKT *v3; // [esp+8h] [ebp-20h]
  DWORD lCurrTime; // [esp+18h] [ebp-10h]
  int pScan; // [esp+1Ch] [ebp-Ch]
  int lBestTime; // [esp+20h] [ebp-8h]
  TPKT *pBest; // [esp+24h] [ebp-4h]

  while ( s_shutdown == 0 )
  {
    Sleep(dwMilliseconds: 0x32u);
    if ( TSList<TPKT,TSGetLink<TPKT>>::Head(a1: &sgPktList) != 0 )
    {
      while ( 1 )
      {
        pBest = nullptr;
        CCritSect::Enter(this: (CCritSect *)&sgPktCrit);
        lCurrTime = GetTickCount();
        lBestTime = -1;
        for ( pScan = TSList<TPKT,TSGetLink<TPKT>>::Head(a1: &sgPktList); pScan > 0; v2 = pScan )
        {
          if ( (int)(lCurrTime - *(_DWORD *)(pScan + 8)) > lBestTime )
          {
            lBestTime = lCurrTime - *(_DWORD *)(pScan + 8);
            pBest = (TPKT *)pScan;
          }
          pScan = TSList<TPKT,TSGetLink<TPKT>>::RawNext(ptr: pScan);
        }
        if ( lBestTime == -1 )
          pBest = nullptr;
        else
          TSLinkedNode<TPKT>::Unlink(a1: pBest, a2: v1, a3: v2, a4: v3);
        CCritSect::Leave(this: (CCritSect *)&sgPktCrit);
        if ( pBest == nullptr || s_shutdown != 0 || s_sendfcn == nullptr )
          break;
        s_sendfcn(
          a1: *((_DWORD *)pBest + 3),
          a2: *((_DWORD *)pBest + 4),
          a3: (char *)pBest + 24,
          a4: *((_DWORD *)pBest + 5));
        v3 = pBest;
        v1 = TPKT::`scalar deleting destructor'(this: pBest, a2: 1u);
        Sleep(dwMilliseconds: 1u);
      }
    }
  }
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x180040A0
 * CodeView symbol: TPKT::`scalar deleting destructor'
 * Demangled: TPKT::`scalar deleting destructor'
 * IDA name: ??_GTPKT@@QAEPAXI@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x30A0
 * Code length: 0x2E
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
TPKT *__thiscall TPKT::`scalar deleting destructor'(TPKT *this, char __flags)
{
  TPKT::~TPKT(this);
  if ( (__flags & 1) != 0 )
    operator delete(ptr: this);
  return this;
}


// ==============================================================================
/*
 * EA: 0x180040D0
 * CodeView symbol: TPKT::~TPKT
 * Demangled: TPKT::~TPKT
 * IDA name: ??1TPKT@@QAE@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x30D0
 * Code length: 0x13
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall TPKT::~TPKT(TPKT *this)
{
  TSLinkedNode<TPKT>::~TSLinkedNode<TPKT>(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x180040F0
 * CodeView symbol: TSLinkedNode<TPKT>::~TSLinkedNode<TPKT>
 * Demangled: TSLinkedNode<TPKT>::~TSLinkedNode<TPKT>
 * IDA name: ??1?$TSLinkedNode@UTPKT@@@@QAE@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x30F0
 * Code length: 0x1B
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
/* HEX-RAYS FAILED: ida_hexrays.decompile() returned None */


// ==============================================================================
/*
 * EA: 0x18004110
 * CodeView symbol: TSLinkedNode<TPKT>::Unlink
 * Demangled: TSLinkedNode<TPKT>::Unlink
 * IDA name: ?Unlink@?$TSLinkedNode@UTPKT@@@@QAEXXZ
 * CodeView module: 9
 * CV address: segment 1 : 0x3110
 * Code length: 0x13
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLinkedNode<TPKT>::Unlink(void *this)
{
  return TSLink<TPKT>::Unlink(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x18004130
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::TSList<TPKT,TSGetLink<TPKT> >
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::TSList<TPKT,TSGetLink<TPKT> >
 * IDA name: ??0?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QAE@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x3130
 * Code length: 0x21
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
char *__thiscall TSList<TPKT,TSGetLink<TPKT>>::TSList<TPKT,TSGetLink<TPKT>>(char *ecx0)
{
  TSLink<TPKT>::TSLink<TPKT>(a1: ecx0 + 4);
  TSList<TPKT,TSGetLink<TPKT>>::Constructor(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x18004160
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::~TSList<TPKT,TSGetLink<TPKT> >
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::~TSList<TPKT,TSGetLink<TPKT> >
 * IDA name: ??1?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QAE@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x3160
 * Code length: 0x1E
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<TPKT,TSGetLink<TPKT>>::~TSList<TPKT,TSGetLink<TPKT>>(char *ecx0)
{
  TSList<TPKT,TSGetLink<TPKT>>::UnlinkAll(a1: ecx0);
  return TSLink<TPKT>::~TSLink<TPKT>(a1: ecx0 + 4);
}


// ==============================================================================
/*
 * EA: 0x18004180
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::DeleteNode
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::DeleteNode
 * IDA name: ?DeleteNode@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QAEPAUTPKT@@PAU2@@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x3180
 * Code length: 0x49
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall TSList<TPKT,TSGetLink<TPKT>>::DeleteNode(TPKT *ptr)
{
  int nextptr; // [esp+10h] [ebp-4h]

  nextptr = TSList<TPKT,TSGetLink<TPKT>>::Next((int)ptr);
  if ( ptr != nullptr )
    TPKT::`scalar deleting destructor'(this: ptr, __flags: 1);
  return nextptr;
}


// ==============================================================================
/*
 * EA: 0x180041D0
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::Head
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::Head
 * IDA name: ?Head@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QBEPAUTPKT@@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x31D0
 * Code length: 0x16
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<TPKT,TSGetLink<TPKT>>::Head(char *this)
{
  return TSLink<TPKT>::Next(a1: this + 4, a2: this);
}


// ==============================================================================
/*
 * EA: 0x180041F0
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::NewNode
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::NewNode
 * IDA name: ?NewNode@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QAEPAUTPKT@@KKK@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x31F0
 * Code length: 0x77
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
TPKT *__stdcall TSList<TPKT,TSGetLink<TPKT>>::NewNode(int location, int extrabytes, int flags)
{
  int v3; // eax
  const char *v4; // eax
  void *v5; // eax
  int v7; // [esp-4h] [ebp-14h]
  TPKT *v8; // [esp+0h] [ebp-10h]
  TPKT *v9; // [esp+8h] [ebp-8h]

  v3 = flags;
  LOBYTE(v3) = flags | 8;
  v7 = v3;
  v4 = type_info::internal_raw_name(this: (type_info *)&TPKT `RTTI Type Descriptor');
  v5 = (void *)SMemAlloc(a1: extrabytes + 24, a2: v4, a3: -2, a4: v7);
  v9 = (TPKT *)operator new(__formal: 24, ptr: v5);
  if ( v9 != nullptr )
    v8 = TPKT::TPKT(this: v9);
  else
    v8 = nullptr;
  if ( location != 0 )
    TSList<TPKT,TSGetLink<TPKT>>::LinkNode(ptr: (int)v8, linktype: location, existingptr: 0);
  return v8;
}


// ==============================================================================
/*
 * EA: 0x18004270
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::RawNext
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::RawNext
 * IDA name: ?RawNext@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QBEPAUTPKT@@PBU2@@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x3270
 * Code length: 0x20
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<TPKT,TSGetLink<TPKT>>::RawNext(void *ecx0, int ptr)
{
  int v2; // eax

  v2 = TSList<TPKT,TSGetLink<TPKT>>::Link(ptr);
  return TSLink<TPKT>::RawNext(a1: v2, a2: ecx0);
}


// ==============================================================================
/*
 * EA: 0x18004290
 * CodeView symbol: TPKT::TPKT
 * Demangled: TPKT::TPKT
 * IDA name: ??0TPKT@@QAE@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x3290
 * Code length: 0x16
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
TPKT *__thiscall TPKT::TPKT(TPKT *this)
{
  TSLinkedNode<TPKT>::TSLinkedNode<TPKT>(a1: this);
  return this;
}


// ==============================================================================
/*
 * EA: 0x180042B0
 * CodeView symbol: TSLinkedNode<TPKT>::TSLinkedNode<TPKT>
 * Demangled: TSLinkedNode<TPKT>::TSLinkedNode<TPKT>
 * IDA name: ??0?$TSLinkedNode@UTPKT@@@@QAE@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x32B0
 * Code length: 0x16
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSLinkedNode<TPKT>::TSLinkedNode<TPKT>(void *ecx0)
{
  TSLink<TPKT>::TSLink<TPKT>(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x180042D0
 * CodeView symbol: TSLink<TPKT>::TSLink<TPKT>
 * Demangled: TSLink<TPKT>::TSLink<TPKT>
 * IDA name: ??0?$TSLink@UTPKT@@@@QAE@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x32D0
 * Code length: 0x16
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSLink<TPKT>::TSLink<TPKT>(void *ecx0)
{
  TSLink<TPKT>::Constructor(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x180042F0
 * CodeView symbol: TSLink<TPKT>::~TSLink<TPKT>
 * Demangled: TSLink<TPKT>::~TSLink<TPKT>
 * IDA name: ??1?$TSLink@UTPKT@@@@QAE@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x32F0
 * Code length: 0x13
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<TPKT>::~TSLink<TPKT>(void *this)
{
  return TSLink<TPKT>::Unlink(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x18004310
 * CodeView symbol: TSLink<TPKT>::Next
 * Demangled: TSLink<TPKT>::Next
 * IDA name: ?Next@?$TSLink@UTPKT@@@@QBEPAUTPKT@@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x3310
 * Code length: 0x2B
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<TPKT>::Next(_DWORD *this)
{
  if ( (int)*(this + 1) <= 0 )
    return 0;
  else
    return *(this + 1);
}


// ==============================================================================
/*
 * EA: 0x18004340
 * CodeView symbol: TSLink<TPKT>::RawNext
 * Demangled: TSLink<TPKT>::RawNext
 * IDA name: ?RawNext@?$TSLink@UTPKT@@@@QBEPAUTPKT@@XZ
 * CodeView module: 9
 * CV address: segment 1 : 0x3340
 * Code length: 0x11
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<TPKT>::RawNext(_DWORD *this)
{
  return *(this + 1);
}


// ==============================================================================
/*
 * EA: 0x18004360
 * CodeView symbol: TSLink<TPKT>::Unlink
 * Demangled: TSLink<TPKT>::Unlink
 * IDA name: ?Unlink@?$TSLink@UTPKT@@@@QAEXXZ
 * CodeView module: 9
 * CV address: segment 1 : 0x3360
 * Code length: 0x47
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSLink<TPKT>::Unlink(_DWORD *ecx0)
{
  _DWORD *result; // eax

  result = ecx0;
  if ( *ecx0 != 0 )
  {
    *(_DWORD *)TSLink<TPKT>::NextLink(linkoffset: -1) = *ecx0;
    result = (_DWORD *)*(ecx0 + 1);
    *(_DWORD *)(*ecx0 + 4) = result;
    *ecx0 = 0;
    *(ecx0 + 1) = 0;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x180043B0
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::Constructor
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::Constructor
 * IDA name: ?Constructor@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@AAEXXZ
 * CodeView module: 9
 * CV address: segment 1 : 0x33B0
 * Code length: 0x1C
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<TPKT,TSGetLink<TPKT>>::Constructor(_DWORD *this)
{
  *this = 0;
  return TSList<TPKT,TSGetLink<TPKT>>::InitializeTerminator(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x180043D0
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::Link
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::Link
 * IDA name: ?Link@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@ABEPAV?$TSLink@UTPKT@@@@PBUTPKT@@@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x33D0
 * Code length: 0x1A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<TPKT,TSGetLink<TPKT>>::Link(_DWORD *this, int ptr)
{
  return TSGetLink<TPKT>::Link(a1: ptr, a2: *this, a3: this);
}


// ==============================================================================
/*
 * EA: 0x180043F0
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::LinkNode
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::LinkNode
 * IDA name: ?LinkNode@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QAEXPAUTPKT@@K0@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x33F0
 * Code length: 0xC1
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall TSList<TPKT,TSGetLink<TPKT>>::LinkNode(int *ecx0, int ptr, int linktype, int existingptr)
{
  int *result; // eax
  int prevlink; // [esp+8h] [ebp-Ch]
  int *existinglink; // [esp+Ch] [ebp-8h]
  _DWORD *link; // [esp+10h] [ebp-4h]

  link = (_DWORD *)TSList<TPKT,TSGetLink<TPKT>>::Link(this: ecx0, ptr);
  if ( *link != 0 )
    TSLink<TPKT>::Unlink(ecx0: link);
  if ( existingptr != 0 )
  {
    result = (int *)TSList<TPKT,TSGetLink<TPKT>>::Link(this: ecx0, ptr: existingptr);
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
    *(_DWORD *)TSLink<TPKT>::NextLink(linkoffset: *ecx0) = link;
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
 * EA: 0x180044C0
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::Next
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::Next
 * IDA name: ?Next@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QBEPAUTPKT@@PBU2@@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x34C0
 * Code length: 0x20
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<TPKT,TSGetLink<TPKT>>::Next(_DWORD *this, int ptr)
{
  _DWORD *v2; // eax

  v2 = (_DWORD *)TSList<TPKT,TSGetLink<TPKT>>::Link(this, ptr);
  return TSLink<TPKT>::Next(this: v2);
}


// ==============================================================================
/*
 * EA: 0x180044E0
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::UnlinkAll
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::UnlinkAll
 * IDA name: ?UnlinkAll@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QAEXXZ
 * CodeView module: 9
 * CV address: segment 1 : 0x34E0
 * Code length: 0x2C
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<TPKT,TSGetLink<TPKT>>::UnlinkAll(char *ecx0)
{
  int result; // eax

  while ( 1 )
  {
    result = TSList<TPKT,TSGetLink<TPKT>>::Head(this: ecx0);
    if ( result == 0 )
      break;
    TSList<TPKT,TSGetLink<TPKT>>::UnlinkNode(ptr: result);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x18004510
 * CodeView symbol: TSLink<TPKT>::Constructor
 * Demangled: TSLink<TPKT>::Constructor
 * IDA name: ?Constructor@?$TSLink@UTPKT@@@@AAEXXZ
 * CodeView module: 9
 * CV address: segment 1 : 0x3510
 * Code length: 0x1E
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSLink<TPKT>::Constructor(_DWORD *this)
{
  *this = 0;
  *(this + 1) = 0;
  return this;
}


// ==============================================================================
/*
 * EA: 0x18004530
 * CodeView symbol: TSLink<TPKT>::NextLink
 * Demangled: TSLink<TPKT>::NextLink
 * IDA name: ?NextLink@?$TSLink@UTPKT@@@@ABEPAV1@H@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x3530
 * Code length: 0x3D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<TPKT>::NextLink(_DWORD *this, int linkoffset)
{
  if ( (int)*(this + 1) < 0 )
    return ~*(this + 1);
  if ( linkoffset < 0 )
    linkoffset = (int)this - *(_DWORD *)(*this + 4);
  return *(this + 1) + linkoffset;
}


// ==============================================================================
/*
 * EA: 0x18004570
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::InitializeTerminator
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::InitializeTerminator
 * IDA name: ?InitializeTerminator@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@AAEXXZ
 * CodeView module: 9
 * CV address: segment 1 : 0x3570
 * Code length: 0x25
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSList<TPKT,TSGetLink<TPKT>>::InitializeTerminator(_DWORD *this)
{
  *(this + 1) = this + 1;
  *(this + 2) = ~(unsigned int)(this + 1);
  return this;
}


// ==============================================================================
/*
 * EA: 0x180045A0
 * CodeView symbol: TSList<TPKT,TSGetLink<TPKT> >::UnlinkNode
 * Demangled: TSList<TPKT,TSGetLink<TPKT> >::UnlinkNode
 * IDA name: ?UnlinkNode@?$TSList@UTPKT@@V?$TSGetLink@UTPKT@@@@@@QAEXPAUTPKT@@@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x35A0
 * Code length: 0x20
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSList<TPKT,TSGetLink<TPKT>>::UnlinkNode(_DWORD *this, int ptr)
{
  _DWORD *v2; // eax

  v2 = (_DWORD *)TSList<TPKT,TSGetLink<TPKT>>::Link(this, ptr);
  return TSLink<TPKT>::Unlink(ecx0: v2);
}


// ==============================================================================
/*
 * EA: 0x180045C0
 * CodeView symbol: TSGetLink<TPKT>::Link
 * Demangled: TSGetLink<TPKT>::Link
 * IDA name: ?Link@?$TSGetLink@UTPKT@@@@SIPAV?$TSLink@UTPKT@@@@PBV?$TSLinkedNode@UTPKT@@@@H@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x35C0
 * Code length: 0x13
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSGetLink<TPKT>::Link(void *this)
{
  return this;
}


// ==============================================================================
/*
 * EA: 0x180045E0
 * CodeView symbol: type_info::internal_raw_name
 * Demangled: type_info::internal_raw_name
 * IDA name: ?internal_raw_name@type_info@@QBEPBDXZ
 * CodeView module: 9
 * CV address: segment 1 : 0x35E0
 * Code length: 0x11
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Latency.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Latency.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
const char *__thiscall type_info::internal_raw_name(type_info *this)
{
  return (char *)this + 8;
}

