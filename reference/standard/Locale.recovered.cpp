/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Locale.cpp
 *
 * CodeView module object:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Locale.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 6
 */


// ==============================================================================
/*
 * EA: 0x18004600
 * CodeView symbol: LocaleInitialize
 * Demangled: LocaleInitialize
 * IDA name: ?LocaleInitialize@@YIXPAU_SNETPROGRAMDATA@@@Z
 * CodeView module: 8
 * CV address: segment 1 : 0x3600
 * Code length: 0x17
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Locale.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall LocaleInitialize(struct _SNETPROGRAMDATA *a1)
{
  *(_DWORD *)&sgLcid = *((_DWORD *)a1 + 14);
}


// ==============================================================================
/*
 * EA: 0x18004617
 * CodeView symbol: LocaleGetLocale
 * Demangled: LocaleGetLocale
 * IDA name: ?LocaleGetLocale@@YIKXZ
 * CodeView module: 8
 * CV address: segment 1 : 0x3617
 * Code length: 0xA
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Locale.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
unsigned int __cdecl LocaleGetLocale()
{
  return *(_DWORD *)&sgLcid;
}


// ==============================================================================
/*
 * EA: 0x18004621
 * CodeView symbol: LocaleGetDialog
 * Demangled: LocaleGetDialog
 * IDA name: ?LocaleGetDialog@@YIPBUDLGTEMPLATE@@PBD@Z
 * CodeView module: 8
 * CV address: segment 1 : 0x3621
 * Code length: 0x79
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Locale.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Locale.cpp
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
 * EA: 0x1800469A
 * CodeView symbol: LocaleGetDialog
 * Demangled: LocaleGetDialog
 * IDA name: ?LocaleGetDialog@@YIPBUDLGTEMPLATE@@I@Z
 * CodeView module: 8
 * CV address: segment 1 : 0x369A
 * Code length: 0x19
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Locale.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
const struct DLGTEMPLATE *__fastcall LocaleGetDialog(unsigned __int16 a1)
{
  return LocaleGetDialog((const char *)a1);
}


// ==============================================================================
/*
 * EA: 0x180046B3
 * CodeView symbol: LocaleLoadString
 * Demangled: LocaleLoadString
 * IDA name: ?LocaleLoadString@@YIHIPADH@Z
 * CodeView module: 8
 * CV address: segment 1 : 0x36B3
 * Code length: 0x180
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Locale.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Locale.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall LocaleLoadString(UINT a1, char *a2, int buffersize)
{
  HRSRC resource; // [esp+20h] [ebp-Ch]
  unsigned int stringtableid; // [esp+28h] [ebp-4h]

  if ( buffersize > 0 )
    *a2 = 0;
  stringtableid = (a1 >> 4) + 1;
  resource = FindResourceExA(
               hModule: global_hinstance,
               lpType: (LPCSTR)6,
               lpName: (LPCSTR)(unsigned __int16)((a1 >> 4) + 1),
               wLanguage: sgLcid);
  if ( resource == nullptr )
    resource = FindResourceExA(
                 hModule: global_hinstance,
                 lpType: (LPCSTR)6,
                 lpName: (LPCSTR)(unsigned __int16)stringtableid,
                 wLanguage: 0);
  if ( resource != nullptr )
    return LoadStringA(hInstance: global_hinstance, uID: a1, lpBuffer: a2, cchBufferMax: buffersize);
  else
    return 0;
}


// ==============================================================================
/*
 * EA: 0x18004833
 * CodeView symbol: LocaleFindResource
 * Demangled: LocaleFindResource
 * IDA name: ?LocaleFindResource@@YIPAUHRSRC__@@PBD0@Z
 * CodeView module: 8
 * CV address: segment 1 : 0x3833
 * Code length: 0x50
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Locale.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Locale.cpp
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

