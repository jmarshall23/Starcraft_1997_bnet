/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Rasmgr.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Rasmgr.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 6
 */


// ==============================================================================
/*
 * EA: 0x19012EF0
 * CodeView symbol: InitRASManager
 * Demangled: InitRASManager
 * IDA name: ?InitRASManager@@YGHXZ
 * CodeView module: 11
 * CV address: segment 1 : 0x11EF0
 * Code length: 0x19F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Rasmgr.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Rasmgr.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall InitRASManager()
{
  unsigned int v1; // [esp+0h] [ebp-18h]
  HMODULE instance; // [esp+8h] [ebp-10h]
  unsigned int i; // [esp+Ch] [ebp-Ch]
  unsigned int dwConnections; // [esp+10h] [ebp-8h] BYREF
  int lpRasConn; // [esp+14h] [ebp-4h]

  lpRasConn = 0;
  if ( sg_hRasDLL != nullptr || (sg_hRasDLL = LoadLibraryA(lpLibFileName: LibFileName), sg_hRasDLL != nullptr) )
  {
    sg_pRasEnumConnections = (int)GetProcAddress(hModule: sg_hRasDLL, lpProcName: ProcName);
    sg_pRasHangUp = (int)GetProcAddress(hModule: sg_hRasDLL, lpProcName: aRashangupa);
    sg_pRasGetConnectStatus = (int)GetProcAddress(hModule: sg_hRasDLL, lpProcName: aRasgetconnects);
    if ( sg_pRasEnumConnections == 0 || sg_pRasHangUp == 0 || sg_pRasGetConnectStatus == 0 )
      SErrDisplayError(a1: -2062548992, a2: aRasmgrCpp, a3: 109, a4: aSgPrasenumconn, a5: 0, a6: 1);
    if ( sg_pRasEnumConnections != 0 && sg_pRasHangUp != 0 && sg_pRasGetConnectStatus != 0 )
    {
      lpRasConn = InternalRasEnumConnections((int)&dwConnections);
      if ( lpRasConn != 0 )
      {
        for ( i = 0; ; ++i )
        {
          v1 = dwConnections >= 0xA ? 10 : dwConnections;
          if ( i >= v1 )
            break;
          sg_ahActiveConn[i] = *(_DWORD *)(lpRasConn + 412 * i + 4);
        }
        if ( i < 0xA )
          sg_ahActiveConn[i] = 0;
        if ( lpRasConn != 0 )
          SMemFree(a1: lpRasConn, a2: aRasmgrCpp_0, a3: 123, a4: 0);
        return 1;
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
    instance = LoadLibraryA(lpLibFileName: aRasapi32_0);
    if ( instance != nullptr )
    {
      FreeLibrary(hLibModule: instance);
      FreeLibrary(hLibModule: instance);
    }
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901308F
 * CodeView symbol: InternalRasEnumConnections
 * Demangled: InternalRasEnumConnections
 * IDA name: InternalRasEnumConnections
 * CodeView module: 11
 * CV address: segment 1 : 0x1208F
 * Code length: 0x9C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Rasmgr.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Rasmgr.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__stdcall InternalRasEnumConnections(int dwConnections)
{
  int dwReturn; // [esp+0h] [ebp-Ch]
  int dwBytes; // [esp+4h] [ebp-8h] BYREF
  _DWORD *lpRasConn; // [esp+8h] [ebp-4h]

  lpRasConn = nullptr;
  dwBytes = 412;
  while ( 1 )
  {
    if ( lpRasConn != nullptr )
      SMemFree(a1: lpRasConn, a2: aRasmgrCpp_1, a3: 44, a4: 0);
    lpRasConn = (_DWORD *)SMemAlloc(a1: dwBytes, a2: aRasmgrCpp_2, a3: 45, a4: 0);
    *lpRasConn = 412;
    dwReturn = sg_pRasEnumConnections(a1: lpRasConn, a2: &dwBytes, a3: dwConnections);
    if ( dwReturn != 0 && dwReturn != 603 )
      break;
    if ( dwReturn != 603 )
      return lpRasConn;
  }
  if ( lpRasConn != nullptr )
    SMemFree(a1: lpRasConn, a2: aRasmgrCpp_3, a3: 52, a4: 0);
  return nullptr;
}


// ==============================================================================
/*
 * EA: 0x1901312B
 * CodeView symbol: GetNumActiveRASConnections
 * Demangled: GetNumActiveRASConnections
 * IDA name: ?GetNumActiveRASConnections@@YGKXZ
 * CodeView module: 11
 * CV address: segment 1 : 0x1212B
 * Code length: 0x85
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Rasmgr.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Rasmgr.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall GetNumActiveRASConnections()
{
  int dwConnections; // [esp+4h] [ebp-8h] BYREF
  _DWORD *lpRasConn; // [esp+8h] [ebp-4h]

  lpRasConn = nullptr;
  if ( sg_hRasDLL == nullptr )
    return 0;
  lpRasConn = InternalRasEnumConnections((int)&dwConnections);
  if ( lpRasConn == nullptr )
    SErrDisplayError(a1: -2062548992, a2: aRasmgrCpp_4, a3: 138, a4: aLprasconn, a5: 0, a6: 1);
  if ( lpRasConn == nullptr )
    return 0;
  SMemFree(a1: lpRasConn, a2: aRasmgrCpp_5, a3: 143, a4: 0);
  return dwConnections;
}


// ==============================================================================
/*
 * EA: 0x190131B0
 * CodeView symbol: EnumNewRASConnections
 * Demangled: EnumNewRASConnections
 * IDA name: ?EnumNewRASConnections@@YGHP6GHPBDPAX1@Z1@Z
 * CodeView module: 11
 * CV address: segment 1 : 0x121B0
 * Code length: 0x14D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Rasmgr.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Rasmgr.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall EnumNewRASConnections(int (__stdcall *lpCallback)(const char *, void *, void *), void *lpContext)
{
  unsigned int j; // [esp+4h] [ebp-14h]
  unsigned int i; // [esp+Ch] [ebp-Ch]
  unsigned int dwConnections; // [esp+10h] [ebp-8h] BYREF
  _DWORD *lpRasConn; // [esp+14h] [ebp-4h]

  lpRasConn = nullptr;
  if ( sg_hRasDLL == nullptr )
    return 0;
  if ( lpCallback != nullptr )
  {
    lpRasConn = InternalRasEnumConnections((int)&dwConnections);
    if ( lpRasConn != nullptr )
    {
      for ( i = 0; i < dwConnections; ++i )
      {
        for ( j = 0; j < 0xA && sg_ahActiveConn[j] != 0 && sg_ahActiveConn[j] != lpRasConn[103 * i + 1]; ++j )
          ;
        if ( (j == 10 || sg_ahActiveConn[j] == 0)
          && lpCallback(a1: (const char *)&lpRasConn[103 * i + 2], a2: (void *)lpRasConn[103 * i + 1], a3: lpContext) != 0 )
        {
          break;
        }
      }
      if ( lpRasConn != nullptr )
        SMemFree(a1: lpRasConn, a2: aRasmgrCpp_7, a3: 184, a4: 0);
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    SErrDisplayError(a1: -2062548992, a2: aRasmgrCpp_6, a3: 158, a4: aLpcallback, a5: 0, a6: 1);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x190132FD
 * CodeView symbol: EnumActiveRASConnections
 * Demangled: EnumActiveRASConnections
 * IDA name: ?EnumActiveRASConnections@@YGHP6GHPBDPAX1@Z1@Z
 * CodeView module: 11
 * CV address: segment 1 : 0x122FD
 * Code length: 0x11C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Rasmgr.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Rasmgr.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall EnumActiveRASConnections(int (__stdcall *lpCallback)(const char *, void *, void *), void *lpContext)
{
  unsigned int i; // [esp+Ch] [ebp-Ch]
  unsigned int dwConnections; // [esp+10h] [ebp-8h] BYREF
  _DWORD *lpRasConn; // [esp+14h] [ebp-4h]

  lpRasConn = nullptr;
  if ( sg_hRasDLL == nullptr )
    return 0;
  if ( lpCallback != nullptr )
  {
    lpRasConn = InternalRasEnumConnections((int)&dwConnections);
    if ( lpRasConn == nullptr )
      SErrDisplayError(a1: -2062548992, a2: aRasmgrCpp_9, a3: 204, a4: aLprasconn_0, a5: 0, a6: 1);
    if ( lpRasConn != nullptr )
    {
      for ( i = 0;
            i < dwConnections
         && lpCallback(a1: (const char *)&lpRasConn[103 * i + 2], a2: (void *)lpRasConn[103 * i + 1], a3: lpContext) == 0;
            ++i )
      {
        ;
      }
      if ( lpRasConn != nullptr )
        SMemFree(a1: lpRasConn, a2: aRasmgrCpp_10, a3: 218, a4: 0);
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    SErrDisplayError(a1: -2062548992, a2: aRasmgrCpp_8, a3: 199, a4: aLpcallback_0, a5: 0, a6: 1);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19013419
 * CodeView symbol: HangupRASConnection
 * Demangled: HangupRASConnection
 * IDA name: ?HangupRASConnection@@YGHPAX@Z
 * CodeView module: 11
 * CV address: segment 1 : 0x12419
 * Code length: 0x149
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Rasmgr.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Rasmgr.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall HangupRASConnection(void *rasconn)
{
  int rcStatus[40]; // [esp+10h] [ebp-A8h] BYREF
  void *hRasConn; // [esp+B0h] [ebp-8h]
  DWORD starttime; // [esp+B4h] [ebp-4h]

  hRasConn = rasconn;
  if ( sg_hRasDLL == nullptr )
    SErrDisplayError(a1: -2062548992, a2: aRasmgrCpp_11, a3: 231, a4: aSgHrasdll, a5: 0, a6: 1);
  if ( sg_hRasDLL == nullptr )
    return 0;
  if ( hRasConn != nullptr )
  {
    if ( sg_pRasHangUp(a1: hRasConn) != 0 )
    {
      SErrDisplayError(a1: -2062548992, a2: aRasmgrCpp_13, a3: 241, a4: aDwreturn, a5: 0, a6: 1);
      return 0;
    }
    else
    {
      starttime = GetTickCount();
      rcStatus[0] = 160;
      do
      {
        if ( sg_pRasGetConnectStatus(a1: hRasConn, a2: rcStatus) != 0 )
          break;
        Sleep(dwMilliseconds: 0);
      }
      while ( GetTickCount() - starttime <= 0x2710 );
      return 1;
    }
  }
  else
  {
    SErrDisplayError(a1: -2062548992, a2: aRasmgrCpp_12, a3: 235, a4: aHrasconn, a5: 0, a6: 1);
    return 0;
  }
}

