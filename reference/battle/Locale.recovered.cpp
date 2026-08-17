/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Locale.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Locale.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 6
 */


// ==============================================================================
/*
 * EA: 0x1900FA30
 * CodeView symbol: LocaleInitialize
 * Demangled: LocaleInitialize
 * IDA name: ?LocaleInitialize@@YIXPAU_SNETPROGRAMDATA@@@Z
 * CodeView module: 17
 * CV address: segment 1 : 0xEA30
 * Code length: 0x9B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Locale.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall LocaleInitialize(struct _SNETPROGRAMDATA *a1)
{
  char *v1; // eax
  FILE *fp; // [esp+4h] [ebp-104h]
  char str[256]; // [esp+8h] [ebp-100h] BYREF

  *(_DWORD *)&sgLcid = *((_DWORD *)a1 + 14);
  fp = fopen(FileName: aLocaleDebugTxt, Mode: aAt_0);
  sprintf(Buffer: str, Format: "Locale ID is [%ld]\n", *(_DWORD *)&sgLcid);
  fputs(Buffer: str, Stream: fp);
  v1 = setlocale(Category: 0, Locale: nullptr);
  fprintf(Stream: fp, Format: "setlocale value [%s]\n", v1);
  fclose(Stream: fp);
}


// ==============================================================================
/*
 * EA: 0x1900FACB
 * CodeView symbol: LocaleGetLocale
 * Demangled: LocaleGetLocale
 * IDA name: ?LocaleGetLocale@@YIKXZ
 * CodeView module: 17
 * CV address: segment 1 : 0xEACB
 * Code length: 0xA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Locale.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
unsigned int __cdecl LocaleGetLocale()
{
  return *(_DWORD *)&sgLcid;
}


// ==============================================================================
/*
 * EA: 0x1900FAD5
 * CodeView symbol: LocaleGetDialog
 * Demangled: LocaleGetDialog
 * IDA name: ?LocaleGetDialog@@YIPBUDLGTEMPLATE@@PBD@Z
 * CodeView module: 17
 * CV address: segment 1 : 0xEAD5
 * Code length: 0x79
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Locale.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
const struct DLGTEMPLATE *__fastcall LocaleGetDialog(const char *a1)
{
  HGLOBAL resourcedata; // [esp+4h] [ebp-8h]
  HRSRC resource; // [esp+8h] [ebp-4h]

  resource = FindResourceExA(hModule: global_hinstance, lpType: (LPCSTR)5, lpName: a1, wLanguage: sgLcid);
  if ( resource == nullptr )
    resource = FindResourceExA(hModule: global_hinstance, lpType: (LPCSTR)5, lpName: a1, wLanguage: 0);
  if ( resource == nullptr )
    return nullptr;
  resourcedata = LoadResource(hModule: global_hinstance, hResInfo: resource);
  if ( resourcedata != nullptr )
    return (const struct DLGTEMPLATE *)LockResource(hResData: resourcedata);
  else
    return nullptr;
}


// ==============================================================================
/*
 * EA: 0x1900FB4E
 * CodeView symbol: LocaleGetDialog
 * Demangled: LocaleGetDialog
 * IDA name: ?LocaleGetDialog@@YIPBUDLGTEMPLATE@@I@Z
 * CodeView module: 17
 * CV address: segment 1 : 0xEB4E
 * Code length: 0x19
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Locale.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
const struct DLGTEMPLATE *__fastcall LocaleGetDialog(unsigned __int16 a1)
{
  return LocaleGetDialog((const char *)a1);
}


// ==============================================================================
/*
 * EA: 0x1900FB67
 * CodeView symbol: LocaleLoadString
 * Demangled: LocaleLoadString
 * IDA name: ?LocaleLoadString@@YIHIPADH@Z
 * CodeView module: 17
 * CV address: segment 1 : 0xEB67
 * Code length: 0x2AA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Locale.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall LocaleLoadString(UINT a1, char *a2, int buffersize)
{
  FILE *fp; // [esp+Ch] [ebp-424h]
  HRSRC resource; // [esp+24h] [ebp-40Ch]
  CHAR str[1024]; // [esp+2Ch] [ebp-404h] BYREF
  unsigned int stringtableid; // [esp+42Ch] [ebp-4h]

  if ( buffersize > 0 )
    *a2 = 0;
  fp = fopen(FileName: aLocaleDebugTxt_0, Mode: aAt);
  stringtableid = (a1 >> 4) + 1;
  resource = FindResourceExA(
               hModule: global_hinstance,
               lpType: (LPCSTR)6,
               lpName: (LPCSTR)(unsigned __int16)((a1 >> 4) + 1),
               wLanguage: sgLcid);
  if ( resource != nullptr )
    fputs(Buffer: aFindresourceex_0, Stream: fp);
  else
    fputs(Buffer: Buffer, Stream: fp);
  if ( resource == nullptr )
    resource = FindResourceExA(
                 hModule: global_hinstance,
                 lpType: (LPCSTR)6,
                 lpName: (LPCSTR)(unsigned __int16)stringtableid,
                 wLanguage: 0);
  if ( resource != nullptr )
    LoadStringA(hInstance: global_hinstance, uID: a1, lpBuffer: str, cchBufferMax: 1024);
  fputs(Buffer: str, Stream: fp);
  fclose(Stream: fp);
  if ( resource != nullptr )
    return LoadStringA(hInstance: global_hinstance, uID: a1, lpBuffer: a2, cchBufferMax: buffersize);
  else
    return 0;
}


// ==============================================================================
/*
 * EA: 0x1900FE11
 * CodeView symbol: LocaleFindResource
 * Demangled: LocaleFindResource
 * IDA name: ?LocaleFindResource@@YIPAUHRSRC__@@PBD0@Z
 * CodeView module: 17
 * CV address: segment 1 : 0xEE11
 * Code length: 0x50
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Locale.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HRSRC __fastcall LocaleFindResource(const char *a1, const char *a2)
{
  HRSRC resource; // [esp+8h] [ebp-4h]

  resource = FindResourceExA(hModule: global_hinstance, lpType: a1, lpName: a2, wLanguage: sgLcid);
  if ( resource == nullptr )
    return FindResourceExA(hModule: global_hinstance, lpType: a1, lpName: a2, wLanguage: 0);
  return resource;
}

