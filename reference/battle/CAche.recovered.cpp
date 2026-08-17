/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 20
 */


// ==============================================================================
/*
 * EA: 0x19002D60
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: unknown_libname_1
 * CodeView module: 29
 * CV address: segment 1 : 0x1D60
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_1()
{
  sub_19002D6F();
  return sub_19002D7E();
}


// ==============================================================================
/*
 * EA: 0x19002D6F
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: sub_19002D6F
 * CodeView module: 29
 * CV address: segment 1 : 0x1D6F
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *sub_19002D6F()
{
  return CCritSect::CCritSect(this: (CCritSect *)&cache_critsect);
}


// ==============================================================================
/*
 * EA: 0x19002D7E
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: sub_19002D7E
 * CodeView module: 29
 * CV address: segment 1 : 0x1D7E
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_19002D7E()
{
  return atexit(a1: sub_19002D90);
}


// ==============================================================================
/*
 * EA: 0x19002D90
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: sub_19002D90
 * CodeView module: 29
 * CV address: segment 1 : 0x1D90
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_19002D90()
{
  CCritSect::~CCritSect(this: (CCritSect *)&cache_critsect);
}


// ==============================================================================
/*
 * EA: 0x19002D9F
 * CodeView symbol: CacheDestroy
 * Demangled: CacheDestroy
 * IDA name: ?CacheDestroy@@YIXXZ
 * CodeView module: 29
 * CV address: segment 1 : 0x1D9F
 * Code length: 0xED
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl CacheDestroy()
{
  CCritSect::Enter(this: (CCritSect *)&cache_critsect);
  cache_initialized = 0;
  if ( cache_blocktable != nullptr )
  {
    SMemFree(a1: cache_blocktable, a2: aCacheCpp, a3: 176, a4: 0);
    cache_blocktable = nullptr;
  }
  if ( cache_hashtable != nullptr )
  {
    SMemFree(a1: cache_hashtable, a2: aCacheCpp_0, a3: 180, a4: 0);
    cache_hashtable = nullptr;
  }
  if ( cache_archiveheader != nullptr )
  {
    SMemFree(a1: cache_archiveheader, a2: aCacheCpp_1, a3: 184, a4: 0);
    cache_archiveheader = nullptr;
  }
  if ( cache_archivefile != nullptr )
  {
    CloseHandle(hObject: cache_archivefile);
    cache_archivefile = (HANDLE)-1;
  }
  if ( cache_hashsource != 0 )
  {
    SMemFree(a1: cache_hashsource, a2: aCacheCpp_2, a3: 192, a4: 0);
    cache_hashsource = 0;
  }
  CCritSect::Leave(this: (CCritSect *)&cache_critsect);
}


// ==============================================================================
/*
 * EA: 0x19002E8C
 * CodeView symbol: CacheFree
 * Demangled: CacheFree
 * IDA name: ?CacheFree@@YIHPAXK@Z
 * CodeView module: 29
 * CV address: segment 1 : 0x1E8C
 * Code length: 0x34
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall CacheFree(void *a1, unsigned int a2)
{
  if ( a1 == nullptr )
    return 0;
  SMemFree(a1, a2: aCacheCpp_3, a3: 202, a4: 0);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19002EC0
 * CodeView symbol: CacheInitialize
 * Demangled: CacheInitialize
 * IDA name: ?CacheInitialize@@YIHXZ
 * CodeView module: 29
 * CV address: segment 1 : 0x1EC0
 * Code length: 0x54C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall CacheInitialize()
{
  HMODULE ModuleHandleA; // eax
  void *v1; // eax
  struct _INFORMATIONCARD_CRYPTO_HANDLE *v2; // eax
  struct _INFORMATIONCARD_CRYPTO_HANDLE *v3; // eax
  BOOL v5; // [esp+0h] [ebp-158h]
  DWORD v6; // [esp+4h] [ebp-154h]
  BYTE *v7; // [esp+8h] [ebp-150h]
  DWORD *v8; // [esp+Ch] [ebp-14Ch]
  int v9; // [esp+Ch] [ebp-14Ch]
  PBYTE *v10; // [esp+10h] [ebp-148h]
  PBYTE *v11; // [esp+10h] [ebp-148h]
  int v12; // [esp+14h] [ebp-144h]
  int v13; // [esp+18h] [ebp-140h]
  DWORD v14; // [esp+20h] [ebp-138h] BYREF
  unsigned int loop; // [esp+24h] [ebp-134h]
  DWORD NumberOfBytesRead; // [esp+28h] [ebp-130h] BYREF
  DWORD bytesread; // [esp+2Ch] [ebp-12Ch] BYREF
  char *separator; // [esp+30h] [ebp-128h]
  PBYTE *blockentries; // [esp+34h] [ebp-124h]
  int hashentries; // [esp+38h] [ebp-120h]
  _BYTE archivefilename[260]; // [esp+3Ch] [ebp-11Ch] BYREF
  CPPEH_RECORD ms_exc; // [esp+140h] [ebp-18h]

  CCritSect::Enter(this: (CCritSect *)&cache_critsect);
  if ( cache_initialized != 0 )
    CacheDestroy();
  ms_exc.registration.TryLevel = 0;
  cache_hashsource = SMemAlloc(a1: 5120, a2: aCacheCpp_4, a3: 214, a4: 0);
  if ( cache_hashsource != 0 )
  {
    InitializeHashSource();
    archivefilename[0] = byte_19041CCC;
    memset(&archivefilename[1], 0, 259);
    ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
    GetModuleFileNameA(hModule: ModuleHandleA, lpFilename: archivefilename, nSize: 0x104u);
    for ( separator = strchr(Str: archivefilename, Val: 92);
          separator != nullptr && strchr(Str: separator + 1, Val: 92) != nullptr;
          separator = strchr(Str: separator + 1, Val: 92) )
    {
      ;
    }
    if ( separator != nullptr )
      *separator = 0;
    strcat(Destination: archivefilename, Source: aBncacheDat);
    cache_archivefile = CreateFileA(
                          lpFileName: archivefilename,
                          dwDesiredAccess: 0xC0000000,
                          dwShareMode: 0,
                          lpSecurityAttributes: nullptr,
                          dwCreationDisposition: 4u,
                          dwFlagsAndAttributes: 0x80u,
                          hTemplateFile: nullptr);
    if ( cache_archivefile != (HANDLE)-1 )
    {
      v1 = (void *)SMemAlloc(a1: 32, a2: aCacheCpp_5, a3: 244, a4: 8);
      cache_archiveheader = operator new(__formal: 32, ptr: v1);
      if ( cache_archiveheader != nullptr )
      {
        bytesread = 0;
        ReadFile(
          hFile: cache_archivefile,
          lpBuffer: (LPVOID)cache_archiveheader,
          nNumberOfBytesToRead: 0x20u,
          lpNumberOfBytesRead: &bytesread,
          lpOverlapped: nullptr);
        if ( *(_DWORD *)cache_archiveheader != 439504450 )
        {
          memset(a1: (void *)cache_archiveheader, Val: 0, Size: 0x20u);
          *(_DWORD *)cache_archiveheader = 439504450;
          *((_DWORD *)cache_archiveheader + 1) = 32;
        }
        if ( *((_DWORD *)cache_archiveheader + 6) >= 0x400u )
          v13 = *((_DWORD *)cache_archiveheader + 6);
        else
          v13 = 1024;
        hashentries = v13;
        cache_hashtable = (void *)SMemAlloc(a1: 16 * v13, a2: aCacheCpp_6, a3: 259, a4: 0);
        if ( cache_hashtable != nullptr )
        {
          memset(a1: cache_hashtable, Val: 0, Size: 16 * hashentries);
          if ( *((_DWORD *)cache_archiveheader + 4) != 0 || *((_DWORD *)cache_archiveheader + 6) != 0 )
          {
            SetFilePointer(
              hFile: cache_archivefile,
              lDistanceToMove: *((_DWORD *)cache_archiveheader + 4),
              lpDistanceToMoveHigh: nullptr,
              dwMoveMethod: 0);
            ReadFile(
              hFile: cache_archivefile,
              lpBuffer: cache_hashtable,
              nNumberOfBytesToRead: 16 * hashentries,
              lpNumberOfBytesRead: &NumberOfBytesRead,
              lpOverlapped: nullptr);
            v2 = (struct _INFORMATIONCARD_CRYPTO_HANDLE *)Hash(a1: aHashTable, a2: 3);
            Decrypt(hCrypto: v2, fOAEP: v5, cbInData: v6, pInData: v7, pcbOutData: v8, ppOutData: v10);
          }
          else
          {
            *((_DWORD *)cache_archiveheader + 4) = 32;
            *((_DWORD *)cache_archiveheader + 6) = 1024;
            v12 = *((_DWORD *)cache_archiveheader + 2) <= (unsigned int)(*((_DWORD *)cache_archiveheader + 4) + 0x4000)
                ? *((_DWORD *)cache_archiveheader + 4) + 0x4000
                : *((_DWORD *)cache_archiveheader + 2);
            *((_DWORD *)cache_archiveheader + 2) = v12;
            for ( loop = 0; loop < 0x400; ++loop )
              *((_DWORD *)cache_hashtable + 4 * loop + 3) = -1;
          }
          v11 = *((_DWORD *)cache_archiveheader + 7) >= 0x400u ? *((PBYTE **)cache_archiveheader + 7) : (PBYTE *)1024;
          blockentries = v11;
          cache_blocktable = (void *)SMemAlloc(a1: 16 * (_DWORD)v11, a2: aCacheCpp_7, a3: 282, a4: 0);
          if ( cache_blocktable != nullptr )
          {
            memset(a1: cache_blocktable, Val: 0, Size: 0x4000u);
            if ( *((_DWORD *)cache_archiveheader + 5) != 0 || *((_DWORD *)cache_archiveheader + 7) != 0 )
            {
              SetFilePointer(
                hFile: cache_archivefile,
                lDistanceToMove: *((_DWORD *)cache_archiveheader + 5),
                lpDistanceToMoveHigh: nullptr,
                dwMoveMethod: 0);
              ReadFile(
                hFile: cache_archivefile,
                lpBuffer: cache_blocktable,
                nNumberOfBytesToRead: 16 * (_DWORD)blockentries,
                lpNumberOfBytesRead: &v14,
                lpOverlapped: nullptr);
              v3 = (struct _INFORMATIONCARD_CRYPTO_HANDLE *)Hash(a1: aBlockTable, a2: 3);
              Decrypt(hCrypto: v3, fOAEP: v5, cbInData: v6, pInData: v7, pcbOutData: v8, ppOutData: v11);
            }
            else
            {
              *((_DWORD *)cache_archiveheader + 5) = 16416;
              if ( *((_DWORD *)cache_archiveheader + 2) <= (unsigned int)(*((_DWORD *)cache_archiveheader + 5) + 0x4000) )
                v9 = *((_DWORD *)cache_archiveheader + 5) + 0x4000;
              else
                v9 = *((_DWORD *)cache_archiveheader + 2);
              *((_DWORD *)cache_archiveheader + 2) = v9;
            }
          }
        }
      }
    }
  }
  ms_exc.registration.TryLevel = -1;
  if ( cache_archiveheader != nullptr && cache_blocktable != nullptr && cache_hashtable != nullptr )
    cache_initialized = 1;
  else
    CacheDestroy();
  CCritSect::Leave(this: (CCritSect *)&cache_critsect);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900340C
 * CodeView symbol: Decrypt
 * Demangled: Decrypt
 * IDA name: Decrypt
 * CodeView module: 29
 * CV address: segment 1 : 0x240C
 * Code length: 0x9E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HRESULT __stdcall Decrypt(
        PINFORMATIONCARD_CRYPTO_HANDLE hCrypto,
        BOOL fOAEP,
        DWORD cbInData,
        PBYTE pInData,
        DWORD *pcbOutData,
        PBYTE *ppOutData)
{
  unsigned int v6; // edx
  _DWORD *v7; // ecx
  HRESULT result; // eax
  int v9; // ecx
  _DWORD *data; // [esp+4h] [ebp-Ch]
  unsigned int iter; // [esp+8h] [ebp-8h]
  int adjust; // [esp+Ch] [ebp-4h]
  int adjusta; // [esp+Ch] [ebp-4h]

  data = v7;
  adjust = -286331154;
  result = v6 >> 2;
  iter = v6 >> 2;
  while ( 1 )
  {
    v9 = iter--;
    if ( v9 == 0 )
      break;
    adjusta = *(_DWORD *)(cache_hashsource + 4 * (unsigned __int8)hCrypto + 4096) + adjust;
    *data ^= (unsigned int)hCrypto + adjusta;
    adjust = adjusta + 32 * adjusta + *data + 3;
    result = (HRESULT)++data;
    hCrypto = (PINFORMATIONCARD_CRYPTO_HANDLE)(((((_DWORD)hCrypto << 21) ^ 0xFFE00000) + 286331153)
                                             | ((unsigned __int64)(unsigned int)hCrypto >> 11));
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190034AA
 * CodeView symbol: Hash
 * Demangled: Hash
 * IDA name: Hash
 * CodeView module: 29
 * CV address: segment 1 : 0x24AA
 * Code length: 0x8A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall Hash(_BYTE *a1, int a2)
{
  int v3; // [esp-4h] [ebp-1Ch]
  char origchar; // [esp+Ch] [ebp-Ch]
  int adjust; // [esp+10h] [ebp-8h]
  int v8; // [esp+14h] [ebp-4h]

  v8 = 2146271213;
  adjust = -286331154;
  while ( a1 != nullptr && *a1 != 0 )
  {
    v3 = (char)*a1++;
    origchar = toupper(C: v3);
    v8 = *(_DWORD *)(cache_hashsource + (a2 << 10) + 4 * origchar) ^ (adjust + v8);
    adjust = 33 * adjust + v8 + origchar + 3;
  }
  return v8;
}


// ==============================================================================
/*
 * EA: 0x19003534
 * CodeView symbol: InitializeHashSource
 * Demangled: InitializeHashSource
 * IDA name: InitializeHashSource
 * CodeView module: 29
 * CV address: segment 1 : 0x2534
 * Code length: 0xB8
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int InitializeHashSource()
{
  int result; // eax
  int rand1; // [esp+4h] [ebp-10h]
  int loop2; // [esp+8h] [ebp-Ch]
  unsigned int seed; // [esp+Ch] [ebp-8h]
  unsigned int seeda; // [esp+Ch] [ebp-8h]
  int loop1; // [esp+10h] [ebp-4h]

  if ( cache_hashsource != 0 )
  {
    seed = 1048577;
    for ( loop1 = 0; loop1 < 256; ++loop1 )
    {
      for ( loop2 = 0; loop2 < 5; ++loop2 )
      {
        seeda = (125 * seed + 3) % 0x2AAAAB;
        rand1 = (unsigned __int16)seeda;
        seed = (125 * seeda + 3) % 0x2AAAAB;
        *(_DWORD *)(cache_hashsource + (loop2 << 10) + 4 * loop1) = (unsigned __int16)seed | (rand1 << 16);
      }
      result = loop1 + 1;
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190035EC
 * CodeView symbol: CacheLoadFile
 * Demangled: CacheLoadFile
 * IDA name: ?CacheLoadFile@@YIHPBDPAU_FILETIME@@PAPAXPAK@Z
 * CodeView module: 29
 * CV address: segment 1 : 0x25EC
 * Code length: 0x382
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall CacheLoadFile(const char *a1, struct _FILETIME *a2, void **data, unsigned int *databytes)
{
  const FILETIME *CurrentFileTime; // eax
  DWORD v6; // [esp+4h] [ebp-158h]
  DWORD byteswritten; // [esp+10h] [ebp-14Ch] BYREF
  DWORD bytesread; // [esp+14h] [ebp-148h] BYREF
  int block; // [esp+18h] [ebp-144h]
  int index; // [esp+1Ch] [ebp-140h]
  DWORD fileheader[79]; // [esp+20h] [ebp-13Ch] BYREF

  if ( a2 != nullptr )
    memset(a1: a2, Val: 0, Size: sizeof(struct _FILETIME));
  if ( data != nullptr )
    *data = nullptr;
  if ( databytes != nullptr )
    *databytes = 0;
  if ( a1 == nullptr || *a1 == 0 || data == nullptr || databytes == nullptr || cache_initialized == 0 )
    return 0;
  CCritSect::Enter(this: (CCritSect *)&cache_critsect);
  index = SearchHashTable(a1, a2: 0);
  if ( index == -1
    || (unsigned int)(block = *((_DWORD *)cache_hashtable + 4 * index + 3)) >= *((_DWORD *)cache_archiveheader + 7) )
  {
    CCritSect::Leave(this: (CCritSect *)&cache_critsect);
    return 0;
  }
  else
  {
    memset(a1: fileheader, Val: 0, Size: sizeof(fileheader));
    if ( (*((_DWORD *)cache_blocktable + 4 * block + 3) & 0xC0000000) != 0 )
    {
      SetFilePointer(
        hFile: cache_archivefile,
        lDistanceToMove: *((_DWORD *)cache_blocktable + 4 * block),
        lpDistanceToMoveHigh: nullptr,
        dwMoveMethod: 0);
      ReadFile(
        hFile: cache_archivefile,
        lpBuffer: fileheader,
        nNumberOfBytesToRead: 4u,
        lpNumberOfBytesRead: &bytesread,
        lpOverlapped: nullptr);
      if ( fileheader[0] >= 0x13C )
        v6 = 316;
      else
        v6 = fileheader[0];
      ReadFile(
        hFile: cache_archivefile,
        lpBuffer: &fileheader[1],
        nNumberOfBytesToRead: v6 - 4,
        lpNumberOfBytesRead: &bytesread,
        lpOverlapped: nullptr);
    }
    if ( _stricmp(String1: a1, String2: (const char *)&fileheader[14]) == 0
      && (CurrentFileTime = (const FILETIME *)GetCurrentFileTime(a1: 0),
          CompareFileTime(lpFileTime1: CurrentFileTime, lpFileTime2: (const FILETIME *)&fileheader[12]) <= 0)
      && *((_DWORD *)cache_blocktable + 4 * block + 2) <= 0x1000000u )
    {
      if ( a2 != nullptr )
        *a2 = *(struct _FILETIME *)&fileheader[6];
      *(_QWORD *)&fileheader[4] = *(_QWORD *)GetCurrentFileTime(a1: 0);
      SetFilePointer(
        hFile: cache_archivefile,
        lDistanceToMove: *((_DWORD *)cache_blocktable + 4 * block),
        lpDistanceToMoveHigh: nullptr,
        dwMoveMethod: 0);
      if ( fileheader[0] <= 0x13C )
        WriteFile(
          hFile: cache_archivefile,
          lpBuffer: fileheader,
          nNumberOfBytesToWrite: fileheader[0],
          lpNumberOfBytesWritten: &byteswritten,
          lpOverlapped: nullptr);
      else
        WriteFile(
          hFile: cache_archivefile,
          lpBuffer: fileheader,
          nNumberOfBytesToWrite: 0x13Cu,
          lpNumberOfBytesWritten: &byteswritten,
          lpOverlapped: nullptr);
      *data = (void *)SMemAlloc(a1: *((_DWORD *)cache_blocktable + 4 * block + 2), a2: aCacheCpp_8, a3: 395, a4: 0);
      if ( *data != nullptr )
      {
        SetFilePointer(
          hFile: cache_archivefile,
          lDistanceToMove: fileheader[0] + *((_DWORD *)cache_blocktable + 4 * block),
          lpDistanceToMoveHigh: nullptr,
          dwMoveMethod: 0);
        ReadFile(
          hFile: cache_archivefile,
          lpBuffer: *data,
          nNumberOfBytesToRead: *((_DWORD *)cache_blocktable + 4 * block + 2),
          lpNumberOfBytesRead: databytes,
          lpOverlapped: nullptr);
        CCritSect::Leave(this: (CCritSect *)&cache_critsect);
        return 1;
      }
      else
      {
        CCritSect::Leave(this: (CCritSect *)&cache_critsect);
        return 0;
      }
    }
    else
    {
      CCritSect::Leave(this: (CCritSect *)&cache_critsect);
      return 0;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1900396E
 * CodeView symbol: GetCurrentFileTime
 * Demangled: GetCurrentFileTime
 * IDA name: GetCurrentFileTime
 * CodeView module: 29
 * CV address: segment 1 : 0x296E
 * Code length: 0x9D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
struct _FILETIME *__fastcall GetCurrentFileTime(int a1)
{
  struct _SYSTEMTIME systime; // [esp+Ch] [ebp-10h] BYREF

  if ( a1 != lastoffset || GetTickCount() != lastcall )
  {
    GetSystemTime(lpSystemTime: &systime);
    SystemTimeToFileTime(lpSystemTime: &systime, lpFileTime: &filetime);
    *(_QWORD *)&filetime += 10000000LL * (unsigned int)a1;
    lastoffset = a1;
    lastcall = GetTickCount();
  }
  return &filetime;
}


// ==============================================================================
/*
 * EA: 0x19003A0B
 * CodeView symbol: SearchHashTable
 * Demangled: SearchHashTable
 * IDA name: SearchHashTable
 * CodeView module: 29
 * CV address: segment 1 : 0x2A0B
 * Code length: 0x109
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SearchHashTable(_BYTE *a1, int a2)
{
  int found; // [esp+8h] [ebp-18h]
  int hashindex; // [esp+Ch] [ebp-14h]
  int hashcheck1; // [esp+10h] [ebp-10h]
  int entry; // [esp+14h] [ebp-Ch]
  int hashcheck0; // [esp+18h] [ebp-8h]

  hashindex = Hash(a1, a2: 0);
  hashcheck0 = Hash(a1, a2: 1);
  hashcheck1 = Hash(a1, a2: 2);
  entry = (*((_DWORD *)cache_archiveheader + 6) - 1) & hashindex;
  found = -1;
  do
  {
    if ( *((_DWORD *)cache_hashtable + 4 * entry + 3) == -1 )
      break;
    if ( *((_DWORD *)cache_hashtable + 4 * entry) == hashcheck0
      && *((_DWORD *)cache_hashtable + 4 * entry + 1) == hashcheck1
      && *((_DWORD *)cache_hashtable + 4 * entry + 3) != -2 )
    {
      if ( *((_DWORD *)cache_hashtable + 4 * entry + 2) == a2 )
        return entry;
      if ( *((_DWORD *)cache_hashtable + 4 * entry + 2) == 0 )
        found = entry;
    }
    entry = (*((_DWORD *)cache_archiveheader + 6) - 1) & (entry + 1);
  }
  while ( entry != ((*((_DWORD *)cache_archiveheader + 6) - 1) & hashindex) );
  return found;
}


// ==============================================================================
/*
 * EA: 0x19003B14
 * CodeView symbol: CacheSaveFile
 * Demangled: CacheSaveFile
 * IDA name: ?CacheSaveFile@@YIHPBDPAXKPAU_FILETIME@@KK@Z
 * CodeView module: 29
 * CV address: segment 1 : 0x2B14
 * Code length: 0x839
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall CacheSaveFile(
        char *a1,
        void *a2,
        DWORD databytes,
        struct _FILETIME *lastwritetime,
        int minexpireseconds,
        int maxexpireseconds)
{
  struct _FILETIME *CurrentFileTime; // eax
  void *v8; // eax
  struct _INFORMATIONCARD_CRYPTO_HANDLE *v9; // eax
  struct _INFORMATIONCARD_CRYPTO_HANDLE *v10; // eax
  int v11; // [esp+0h] [ebp-2D4h]
  BOOL v12; // [esp+0h] [ebp-2D4h]
  DWORD v13; // [esp+4h] [ebp-2D0h]
  DWORD v14; // [esp+4h] [ebp-2D0h]
  int v15; // [esp+8h] [ebp-2CCh]
  PBYTE v16; // [esp+8h] [ebp-2CCh]
  DWORD *v17; // [esp+Ch] [ebp-2C8h]
  DWORD *v18; // [esp+Ch] [ebp-2C8h]
  PBYTE *dataa; // [esp+10h] [ebp-2C4h]
  size_t blocksize; // [esp+18h] [ebp-2BCh]
  void *buffer; // [esp+1Ch] [ebp-2B8h]
  size_t hashsize; // [esp+20h] [ebp-2B4h]
  DWORD NumberOfBytesWritten; // [esp+24h] [ebp-2B0h] BYREF
  DWORD byteswritten; // [esp+28h] [ebp-2ACh] BYREF
  int found; // [esp+2Ch] [ebp-2A8h]
  int hashindex; // [esp+30h] [ebp-2A4h]
  int hashcheck1; // [esp+34h] [ebp-2A0h]
  int entry; // [esp+38h] [ebp-29Ch]
  int hashcheck0; // [esp+3Ch] [ebp-298h]
  int firstentry; // [esp+40h] [ebp-294h]
  unsigned int i; // [esp+44h] [ebp-290h]
  _DWORD Buffer[79]; // [esp+48h] [ebp-28Ch] BYREF
  DWORD bytesread; // [esp+184h] [ebp-150h] BYREF
  unsigned int v36; // [esp+188h] [ebp-14Ch]
  int index; // [esp+18Ch] [ebp-148h]
  unsigned int block; // [esp+190h] [ebp-144h]
  DWORD minalloc; // [esp+194h] [ebp-140h]
  _DWORD fileheader[79]; // [esp+198h] [ebp-13Ch] BYREF

  if ( a1 == nullptr
    || *a1 == 0
    || a2 == nullptr
    || databytes == 0
    || lastwritetime == nullptr
    || maxexpireseconds == 0
    || cache_initialized == 0 )
  {
    return 0;
  }
  CCritSect::Enter(this: (CCritSect *)&cache_critsect);
  for ( index = SearchHashTable(a1, a2: 0); index != -1; index = SearchHashTable(a1, a2: 0) )
  {
    v36 = *((_DWORD *)cache_hashtable + 4 * index + 3);
    if ( v36 < *((_DWORD *)cache_archiveheader + 7) )
    {
      *((_DWORD *)cache_blocktable + 4 * v36 + 2) = 0;
      *((_DWORD *)cache_blocktable + 4 * v36 + 3) = 0;
    }
    *((_DWORD *)cache_hashtable + 4 * index + 3) = -2;
  }
  minalloc = databytes + 316;
  for ( block = 0;
        block < *((_DWORD *)cache_archiveheader + 7)
     && (*((_DWORD *)cache_blocktable + 4 * block + 3) != 0 || *((_DWORD *)cache_blocktable + 4 * block + 1) < minalloc);
        ++block )
  {
    ;
  }
  if ( block >= *((_DWORD *)cache_archiveheader + 7) )
  {
    for ( block = 0; block < *((_DWORD *)cache_archiveheader + 7); ++block )
    {
      if ( *((_DWORD *)cache_blocktable + 4 * block + 1) >= minalloc )
      {
        SetFilePointer(
          hFile: cache_archivefile,
          lDistanceToMove: *((_DWORD *)cache_blocktable + 4 * block),
          lpDistanceToMoveHigh: nullptr,
          dwMoveMethod: 0);
        memset(a1: Buffer, Val: 0, Size: sizeof(Buffer));
        ReadFile(
          hFile: cache_archivefile,
          lpBuffer: Buffer,
          nNumberOfBytesToRead: 4u,
          lpNumberOfBytesRead: &bytesread,
          lpOverlapped: nullptr);
        v17 = (DWORD *)(Buffer[0] >= 0x13Cu ? 316 : Buffer[0]);
        ReadFile(
          hFile: cache_archivefile,
          lpBuffer: &Buffer[1],
          nNumberOfBytesToRead: (DWORD)(v17 - 1),
          lpNumberOfBytesRead: &bytesread,
          lpOverlapped: nullptr);
        CurrentFileTime = GetCurrentFileTime(a1: 0);
        if ( CompareFileTime(lpFileTime1: CurrentFileTime, lpFileTime2: (const FILETIME *)&Buffer[10]) > 0 )
          break;
      }
    }
  }
  if ( block >= *((_DWORD *)cache_archiveheader + 7) )
  {
    if ( block >= 0x400 )
      return 0;
    *((_DWORD *)cache_blocktable + 4 * block) = *((_DWORD *)cache_archiveheader + 2);
    *((_DWORD *)cache_blocktable + 4 * block + 1) = minalloc;
    *((_DWORD *)cache_archiveheader + 2) += minalloc;
    *((_DWORD *)cache_archiveheader + 7) = block + 1;
  }
  else
  {
    for ( i = 0; i < *((_DWORD *)cache_archiveheader + 6); ++i )
    {
      if ( *((_DWORD *)cache_hashtable + 4 * i + 3) == block )
        *((_DWORD *)cache_hashtable + 4 * i + 3) = -2;
    }
  }
  hashindex = Hash(a1, a2: 0);
  hashcheck0 = Hash(a1, a2: 1);
  hashcheck1 = Hash(a1, a2: 2);
  entry = (*((_DWORD *)cache_archiveheader + 6) - 1) & hashindex;
  firstentry = entry;
  found = -1;
  do
  {
    if ( *((_DWORD *)cache_hashtable + 4 * entry + 3) == -1 )
      break;
    if ( *((_DWORD *)cache_hashtable + 4 * entry + 3) == -2 )
      break;
    entry = (*((_DWORD *)cache_archiveheader + 6) - 1) & (entry + 1);
  }
  while ( entry != firstentry );
  if ( *((_DWORD *)cache_hashtable + 4 * entry + 3) < 0xFFFFFFFE )
  {
    CCritSect::Leave(this: (CCritSect *)&cache_critsect);
    return 0;
  }
  else
  {
    *((_DWORD *)cache_hashtable + 4 * entry) = hashcheck0;
    *((_DWORD *)cache_hashtable + 4 * entry + 1) = hashcheck1;
    *((_DWORD *)cache_hashtable + 4 * entry + 2) = 0;
    *((_DWORD *)cache_hashtable + 4 * entry + 3) = block;
    *((_DWORD *)cache_blocktable + 4 * block + 2) = databytes;
    *((_DWORD *)cache_blocktable + 4 * block + 3) = -1073741824;
    memset(a1: fileheader, Val: 0, Size: sizeof(fileheader));
    fileheader[0] = 316;
    *(struct _FILETIME *)&fileheader[8] = *GetCurrentFileTime(a1: 0);
    *(struct _FILETIME *)&fileheader[6] = *lastwritetime;
    *(struct _FILETIME *)&fileheader[10] = *GetCurrentFileTime(a1: minexpireseconds);
    *(struct _FILETIME *)&fileheader[12] = *GetCurrentFileTime(a1: maxexpireseconds);
    *(struct _FILETIME *)&fileheader[4] = *GetCurrentFileTime(a1: 0);
    strcpy(Destination: (char *)&fileheader[14], Source: a1);
    SetFilePointer(
      hFile: cache_archivefile,
      lDistanceToMove: *((_DWORD *)cache_blocktable + 4 * block),
      lpDistanceToMoveHigh: nullptr,
      dwMoveMethod: 0);
    WriteFile(
      hFile: cache_archivefile,
      lpBuffer: fileheader,
      nNumberOfBytesToWrite: 0x13Cu,
      lpNumberOfBytesWritten: &byteswritten,
      lpOverlapped: nullptr);
    WriteFile(
      hFile: cache_archivefile,
      lpBuffer: a2,
      nNumberOfBytesToWrite: databytes,
      lpNumberOfBytesWritten: &byteswritten,
      lpOverlapped: nullptr);
    SetFilePointer(hFile: cache_archivefile, lDistanceToMove: 0, lpDistanceToMoveHigh: nullptr, dwMoveMethod: 0);
    WriteFile(
      hFile: cache_archivefile,
      lpBuffer: cache_archiveheader,
      nNumberOfBytesToWrite: 0x20u,
      lpNumberOfBytesWritten: &NumberOfBytesWritten,
      lpOverlapped: nullptr);
    if ( *((_DWORD *)cache_archiveheader + 6) >= 0x400u )
      v15 = *((_DWORD *)cache_archiveheader + 6);
    else
      v15 = 1024;
    hashsize = 16 * v15;
    if ( *((_DWORD *)cache_archiveheader + 7) >= 0x400u )
      v13 = *((_DWORD *)cache_archiveheader + 7);
    else
      v13 = 1024;
    blocksize = 16 * v13;
    if ( hashsize <= 16 * v13 )
    {
      v11 = 16 * v13;
      v8 = (void *)SMemAlloc(a1: blocksize, a2: aCacheCpp_9, a3: 569, a4: 0);
    }
    else
    {
      v11 = 16 * v15;
      v8 = (void *)SMemAlloc(a1: hashsize, a2: aCacheCpp_9, a3: 569, a4: 0);
    }
    buffer = v8;
    if ( v8 != nullptr )
    {
      SetFilePointer(
        hFile: cache_archivefile,
        lDistanceToMove: *((_DWORD *)cache_archiveheader + 4),
        lpDistanceToMoveHigh: nullptr,
        dwMoveMethod: 0);
      memcpy(a1: buffer, Src: cache_hashtable, Size: hashsize);
      v9 = (struct _INFORMATIONCARD_CRYPTO_HANDLE *)Hash(a1: aHashTable_0, a2: 3);
      Encrypt(hCrypto: v9, fOAEP: v11, cbInData: v13, pInData: (PBYTE)v15, pcbOutData: v17, ppOutData: (PBYTE *)a2);
      WriteFile(
        hFile: cache_archivefile,
        lpBuffer: buffer,
        nNumberOfBytesToWrite: hashsize,
        lpNumberOfBytesWritten: &NumberOfBytesWritten,
        lpOverlapped: nullptr);
      SetFilePointer(
        hFile: cache_archivefile,
        lDistanceToMove: *((_DWORD *)cache_archiveheader + 5),
        lpDistanceToMoveHigh: nullptr,
        dwMoveMethod: 0);
      memcpy(a1: buffer, Src: cache_blocktable, Size: blocksize);
      v10 = (struct _INFORMATIONCARD_CRYPTO_HANDLE *)Hash(a1: aBlockTable_0, a2: 3);
      Encrypt(hCrypto: v10, fOAEP: v12, cbInData: v14, pInData: v16, pcbOutData: v18, ppOutData: dataa);
      WriteFile(
        hFile: cache_archivefile,
        lpBuffer: buffer,
        nNumberOfBytesToWrite: blocksize,
        lpNumberOfBytesWritten: &NumberOfBytesWritten,
        lpOverlapped: nullptr);
      SMemFree(a1: buffer, a2: aCacheCpp_10, a3: 587, a4: 0);
    }
    CCritSect::Leave(this: (CCritSect *)&cache_critsect);
    return 1;
  }
}


// ==============================================================================
/*
 * EA: 0x1900434D
 * CodeView symbol: Encrypt
 * Demangled: Encrypt
 * IDA name: Encrypt
 * CodeView module: 29
 * CV address: segment 1 : 0x334D
 * Code length: 0xA2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HRESULT __stdcall Encrypt(
        PINFORMATIONCARD_CRYPTO_HANDLE hCrypto,
        BOOL fOAEP,
        DWORD cbInData,
        PBYTE pInData,
        DWORD *pcbOutData,
        PBYTE *ppOutData)
{
  unsigned int v6; // edx
  int *v7; // ecx
  HRESULT result; // eax
  int v9; // ecx
  int *data; // [esp+4h] [ebp-10h]
  int origdata; // [esp+8h] [ebp-Ch]
  unsigned int iter; // [esp+Ch] [ebp-8h]
  HRESULT adjust; // [esp+10h] [ebp-4h]
  int adjusta; // [esp+10h] [ebp-4h]

  data = v7;
  adjust = -286331154;
  result = v6 >> 2;
  iter = v6 >> 2;
  while ( 1 )
  {
    v9 = iter--;
    if ( v9 == 0 )
      break;
    origdata = *data;
    adjusta = *(_DWORD *)(cache_hashsource + 4 * (unsigned __int8)hCrypto + 4096) + adjust;
    *data++ ^= (unsigned int)hCrypto + adjusta;
    result = adjusta + 32 * adjusta + origdata + 3;
    adjust = result;
    hCrypto = (PINFORMATIONCARD_CRYPTO_HANDLE)(((((_DWORD)hCrypto << 21) ^ 0xFFE00000) + 286331153)
                                             | ((unsigned __int64)(unsigned int)hCrypto >> 11));
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190043F0
 * CodeView symbol: operator new
 * Demangled: operator new
 * IDA name: ??2@YAPAXIPAX@Z
 * CodeView module: 29
 * CV address: segment 1 : 0x33F0
 * Code length: 0x8
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__cdecl operator new(char __formal, void *ptr)
{
  return ptr;
}


// ==============================================================================
/*
 * EA: 0x19004400
 * CodeView symbol: CCritSect::CCritSect
 * Demangled: CCritSect::CCritSect
 * IDA name: ??0CCritSect@@QAE@XZ
 * CodeView module: 29
 * CV address: segment 1 : 0x3400
 * Code length: 0x18
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *__thiscall CCritSect::CCritSect(CCritSect *this)
{
  InitializeCriticalSection(lpCriticalSection: (LPCRITICAL_SECTION)this);
  return this;
}


// ==============================================================================
/*
 * EA: 0x19004420
 * CodeView symbol: CCritSect::~CCritSect
 * Demangled: CCritSect::~CCritSect
 * IDA name: ??1CCritSect@@QAE@XZ
 * CodeView module: 29
 * CV address: segment 1 : 0x3420
 * Code length: 0x15
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall CCritSect::~CCritSect(struct _RTL_CRITICAL_SECTION *this)
{
  DeleteCriticalSection(lpCriticalSection: this);
}


// ==============================================================================
/*
 * EA: 0x19004440
 * CodeView symbol: CCritSect::Enter
 * Demangled: CCritSect::Enter
 * IDA name: ?Enter@CCritSect@@QAEXXZ
 * CodeView module: 29
 * CV address: segment 1 : 0x3440
 * Code length: 0x15
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall CCritSect::Enter(struct _RTL_CRITICAL_SECTION *this)
{
  EnterCriticalSection(lpCriticalSection: this);
}


// ==============================================================================
/*
 * EA: 0x19004460
 * CodeView symbol: CCritSect::Leave
 * Demangled: CCritSect::Leave
 * IDA name: ?Leave@CCritSect@@QAEXXZ
 * CodeView module: 29
 * CV address: segment 1 : 0x3460
 * Code length: 0x15
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CAche.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CAche.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall CCritSect::Leave(struct _RTL_CRITICAL_SECTION *this)
{
  LeaveCriticalSection(lpCriticalSection: this);
}

