/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Profile.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Profile.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 5
 */


// ==============================================================================
/*
 * EA: 0x190123D0
 * CodeView symbol: ProfileInitialize
 * Demangled: ProfileInitialize
 * IDA name: ?ProfileInitialize@@YIXPBD@Z
 * CodeView module: 13
 * CV address: segment 1 : 0x113D0
 * Code length: 0x127
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Profile.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Profile.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ProfileInitialize(const char *a1)
{
  char *Str; // [esp+4h] [ebp-14h]
  void *szFieldNames; // [esp+8h] [ebp-10h]
  const char *pszField; // [esp+Ch] [ebp-Ch]
  int nTotalLen; // [esp+10h] [ebp-8h]
  size_t nTotalLena; // [esp+10h] [ebp-8h]
  int nCount; // [esp+14h] [ebp-4h]
  signed int nCounta; // [esp+14h] [ebp-4h]

  if ( a1 != nullptr )
  {
    nTotalLen = 0;
    nCount = 0;
    pszField = a1;
    while ( *pszField != 0 )
    {
      nTotalLen += strlen(Str: pszField) + 1;
      pszField = &a1[nTotalLen];
      ++nCount;
    }
    if ( nCount != 0 )
    {
      nTotalLena = nTotalLen + 1;
      sgnNumFields = nCount + 1;
      sgszFieldNameTbl = (char **)SMemAlloc(a1: 4 * (nCount + 1), a2: aProfileCpp, a3: 77, a4: 0);
      szFieldNames = (void *)SMemAlloc(a1: nTotalLena, a2: aProfileCpp_0, a3: 81, a4: 0);
      memcpy(a1: szFieldNames, Src: a1, Size: nTotalLena);
      Str = (char *)szFieldNames;
      for ( nCounta = 0; nCounta < (int)sgnNumFields; ++nCounta )
      {
        sgszFieldNameTbl[nCounta] = Str;
        Str += strlen(Str) + 1;
      }
    }
    else
    {
      sgszFieldNameTbl = nullptr;
    }
  }
  else
  {
    sgszFieldNameTbl = nullptr;
  }
}


// ==============================================================================
/*
 * EA: 0x190124F7
 * CodeView symbol: ProfileDestroy
 * Demangled: ProfileDestroy
 * IDA name: ?ProfileDestroy@@YIXXZ
 * CodeView module: 13
 * CV address: segment 1 : 0x114F7
 * Code length: 0x45
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Profile.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Profile.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ProfileDestroy()
{
  if ( sgszFieldNameTbl != nullptr )
  {
    SMemFree(a1: *sgszFieldNameTbl, a2: aProfileCpp_1, a3: 94, a4: 0);
    SMemFree(a1: sgszFieldNameTbl, a2: aProfileCpp_2, a3: 95, a4: 0);
    sgszFieldNameTbl = nullptr;
  }
}


// ==============================================================================
/*
 * EA: 0x1901253C
 * CodeView symbol: ProfileDraw
 * Demangled: ProfileDraw
 * IDA name: ?ProfileDraw@@YIHPAU_UIPARAMS@@HPAPBDPAPAEHH@Z
 * CodeView module: 13
 * CV address: segment 1 : 0x1153C
 * Code length: 0x18B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Profile.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Profile.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ProfileDraw(
        struct _UIPARAMS *a1,
        int a2,
        const char **usertbl,
        unsigned __int8 **bitmaptbl,
        int width,
        int height)
{
  signed int idx; // [esp+8h] [ebp-10h]
  int localdatatbl; // [esp+Ch] [ebp-Ch]
  int success; // [esp+10h] [ebp-8h]
  int index; // [esp+14h] [ebp-4h]

  if ( a2 == 0 && usertbl == nullptr )
    return (*(int (__stdcall **)(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, int, int))(*((_DWORD *)a1 + 3) + 68))(
             a1: *((_DWORD *)a1 + 1),
             a2: *((_DWORD *)a1 + 2),
             a3: *((_DWORD *)a1 + 3),
             a4: *((_DWORD *)a1 + 4),
             a5: 0,
             a6: 0,
             a7: 0,
             a8: 0,
             a9: *bitmaptbl,
             a10: width,
             a11: height);
  if ( *(_DWORD *)(*((_DWORD *)a1 + 3) + 64) == 0 )
    return 0;
  if ( *(_DWORD *)(*((_DWORD *)a1 + 3) + 60) == 0 )
    return 0;
  localdatatbl = GetFieldData(a1: a2, a2: usertbl);
  if ( localdatatbl == 0 )
    return 0;
  success = 1;
  for ( index = 0; index < a2; ++index )
  {
    if ( (*(int (__stdcall **)(_DWORD, _DWORD, _DWORD, _DWORD, const char *, unsigned int, char **, unsigned int, unsigned __int8 *, int, int))(*((_DWORD *)a1 + 3) + 68))(
           a1: *((_DWORD *)a1 + 1),
           a2: *((_DWORD *)a1 + 2),
           a3: *((_DWORD *)a1 + 3),
           a4: *((_DWORD *)a1 + 4),
           a5: usertbl[index],
           a6: sgnNumFields,
           a7: sgszFieldNameTbl,
           a8: localdatatbl + 4 * sgnNumFields * index,
           a9: bitmaptbl[index],
           a10: width,
           a11: height) == 0 )
    {
      success = 0;
      break;
    }
  }
  for ( idx = 0; idx < (int)(sgnNumFields * a2); ++idx )
    SMemFree(a1: *(_DWORD *)(localdatatbl + 4 * idx), a2: aProfileCpp_3, a3: 156, a4: 0);
  SMemFree(a1: localdatatbl, a2: aProfileCpp_4, a3: 157, a4: 0);
  return success;
}


// ==============================================================================
/*
 * EA: 0x190126C7
 * CodeView symbol: GetFieldData
 * Demangled: GetFieldData
 * IDA name: GetFieldData
 * CodeView module: 13
 * CV address: segment 1 : 0x116C7
 * Code length: 0x136
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Profile.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Profile.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall GetFieldData(signed int a1, const char **a2)
{
  int length; // [esp+8h] [ebp-14h]
  signed int idx; // [esp+Ch] [ebp-10h]
  int localdatatbl; // [esp+10h] [ebp-Ch]
  const char **srvdatatbl; // [esp+14h] [ebp-8h]
  signed int user; // [esp+18h] [ebp-4h]

  srvdatatbl = (const char **)SMemAlloc(a1: 4 * sgnNumFields * a1, a2: aProfileCpp_5, a3: 22, a4: 0);
  if ( SrvReadUserData(a1, a2, numfields: sgnNumFields, a4: (const char **)sgszFieldNameTbl, a5: srvdatatbl) != 0 )
  {
    localdatatbl = SMemAlloc(a1: 4 * sgnNumFields * a1, a2: aProfileCpp_7, a3: 35, a4: 8);
    for ( user = 0; user < a1; ++user )
    {
      for ( idx = sgnNumFields * user; idx < (int)(sgnNumFields * (user + 1)); ++idx )
      {
        length = SStrLen(a1: srvdatatbl[idx]) + 1;
        *(_DWORD *)(localdatatbl + 4 * idx) = SMemAlloc(a1: length, a2: aProfileCpp_8, a3: 39, a4: 0);
        SStrCopy(a1: *(_DWORD *)(localdatatbl + 4 * idx), a2: srvdatatbl[idx], a3: length);
      }
    }
    SMemFree(a1: srvdatatbl, a2: aProfileCpp_9, a3: 43, a4: 0);
    return localdatatbl;
  }
  else
  {
    SMemFree(a1: srvdatatbl, a2: aProfileCpp_6, a3: 29, a4: 0);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x190127FD
 * CodeView symbol: ProfileCall
 * Demangled: ProfileCall
 * IDA name: ?ProfileCall@@YIHPAUHWND__@@PAU_UIPARAMS@@PBD@Z
 * CodeView module: 13
 * CV address: segment 1 : 0x117FD
 * Code length: 0x19A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Profile.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Profile.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ProfileCall(HWND a1, struct _UIPARAMS *a2, const char *szUser)
{
  int bResult; // [esp+8h] [ebp-84h]
  _DWORD uidata[20]; // [esp+Ch] [ebp-80h] BYREF
  char szLocalName[16]; // [esp+5Ch] [ebp-30h] BYREF
  int (__stdcall *writeproc)(const char *, unsigned int, const char **, const char **); // [esp+6Ch] [ebp-20h]
  const char *nametbl; // [esp+70h] [ebp-1Ch] BYREF
  _BYTE szProfileName[16]; // [esp+74h] [ebp-18h] BYREF
  int szLocalDataTbl; // [esp+84h] [ebp-8h]
  signed int idx; // [esp+88h] [ebp-4h]

  if ( *(_DWORD *)(*((_DWORD *)a2 + 3) + 64) == 0 )
    return 0;
  if ( *(_DWORD *)(*((_DWORD *)a2 + 3) + 60) == 0 )
    return 0;
  if ( *((_DWORD *)a2 + 3) != 0 )
    memcpy(a1: uidata, Src: *((const void **)a2 + 3), Size: sizeof(uidata));
  else
    memset(a1: uidata, Val: 0, Size: sizeof(uidata));
  uidata[0] = 80;
  uidata[2] = a1;
  writeproc = nullptr;
  SrvGetLocalPlayerName(a1: 1, a2: szLocalName, a3: 0x10u);
  if ( _strnicmp(String1: szUser, String2: szLocalName, MaxCount: 0x10u) == 0 )
  {
    writeproc = SrvWriteUserData;
    szProfileName[0] = 0;
  }
  else
  {
    SStrCopy(a1: szProfileName, a2: szUser, a3: 16);
  }
  nametbl = szProfileName;
  szLocalDataTbl = GetFieldData(a1: 1, a2: &nametbl);
  if ( szLocalDataTbl == 0 )
    return 0;
  bResult = (*(int (__stdcall **)(_DWORD, _DWORD, _DWORD *, _DWORD, const char *, unsigned int, char **, int, int (__stdcall *)(const char *, unsigned int, const char **, const char **)))(*((_DWORD *)a2 + 3) + 60))(
              a1: *((_DWORD *)a2 + 1),
              a2: *((_DWORD *)a2 + 2),
              a3: uidata,
              a4: *((_DWORD *)a2 + 4),
              a5: szUser,
              a6: sgnNumFields,
              a7: sgszFieldNameTbl,
              a8: szLocalDataTbl,
              a9: writeproc);
  for ( idx = 0; idx < (int)sgnNumFields; ++idx )
    SMemFree(a1: *(_DWORD *)(szLocalDataTbl + 4 * idx), a2: aProfileCpp_10, a3: 208, a4: 0);
  SMemFree(a1: szLocalDataTbl, a2: aProfileCpp_11, a3: 209, a4: 0);
  return bResult;
}

