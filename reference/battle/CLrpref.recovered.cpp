/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CLrpref.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CLrpref.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 5
 */


// ==============================================================================
/*
 * EA: 0x190089F0
 * CodeView symbol: ColorPrefDestroy
 * Demangled: ColorPrefDestroy
 * IDA name: ?ColorPrefDestroy@@YIXXZ
 * CodeView module: 24
 * CV address: segment 1 : 0x79F0
 * Code length: 0x8B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CLrpref.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CLrpref.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ColorPrefDestroy()
{
  ColorPrefRestoreWindowsColors();
  if ( lpaElements != nullptr )
  {
    SMemFree(a1: lpaElements, a2: aClrprefCpp, a3: 50, a4: 0);
    lpaElements = nullptr;
  }
  if ( dword_19042090 != nullptr )
  {
    SMemFree(a1: dword_19042090, a2: aClrprefCpp_0, a3: 55, a4: 0);
    dword_19042090 = nullptr;
  }
  if ( lpaRgbValues != nullptr )
  {
    SMemFree(a1: lpaRgbValues, a2: aClrprefCpp_1, a3: 60, a4: 0);
    lpaRgbValues = nullptr;
  }
  sgPrefColors = 0;
}


// ==============================================================================
/*
 * EA: 0x19008A7B
 * CodeView symbol: ColorPrefRestoreWindowsColors
 * Demangled: ColorPrefRestoreWindowsColors
 * IDA name: ColorPrefRestoreWindowsColors
 * CodeView module: 24
 * CV address: segment 1 : 0x7A7B
 * Code length: 0x2A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CLrpref.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CLrpref.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL ColorPrefRestoreWindowsColors()
{
  BOOL result; // eax

  if ( sgPrefColors != 0 )
    return SetSysColors(cElements: sgPrefColors, lpaElements: lpaElements, lpaRgbValues: lpaRgbValues);
  return result;
}


// ==============================================================================
/*
 * EA: 0x19008AA5
 * CodeView symbol: ColorPrefInit
 * Demangled: ColorPrefInit
 * IDA name: ?ColorPrefInit@@YIHP6GHKKPAXKPAK@Z@Z
 * CodeView module: 24
 * CV address: segment 1 : 0x7AA5
 * Code length: 0x154
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CLrpref.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CLrpref.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ColorPrefInit(int (__stdcall *a1)(unsigned int, unsigned int, void *, unsigned int, unsigned int *))
{
  DWORD SysColor; // eax
  unsigned int dwBytes; // [esp+8h] [ebp-10h] BYREF
  signed int i; // [esp+Ch] [ebp-Ch]
  unsigned __int8 *pSysColorTbl; // [esp+10h] [ebp-8h] BYREF
  signed int nEntries; // [esp+14h] [ebp-4h]

  if ( UiGetData(a1, a2: 1u, a3: &pSysColorTbl, a4: &dwBytes) == 0 )
    return 0;
  nEntries = dwBytes >> 3;
  if ( dwBytes >> 3 == 0 )
    return 0;
  lpaElements = (INT *)SMemAlloc(a1: 4 * nEntries, a2: aClrprefCpp_2, a3: 81, a4: 0);
  dword_19042090 = (COLORREF *)SMemAlloc(a1: 4 * nEntries, a2: aClrprefCpp_3, a3: 82, a4: 0);
  lpaRgbValues = (COLORREF *)SMemAlloc(a1: 4 * nEntries, a2: aClrprefCpp_4, a3: 83, a4: 0);
  if ( lpaElements != nullptr && dword_19042090 != nullptr && lpaRgbValues != nullptr )
  {
    sgPrefColors = nEntries;
    for ( i = 0; i < nEntries; ++i )
    {
      lpaElements[i] = *(_DWORD *)&pSysColorTbl[8 * i];
      dword_19042090[i] = *(_DWORD *)&pSysColorTbl[8 * i + 4];
      SysColor = GetSysColor(nIndex: *(_DWORD *)&pSysColorTbl[8 * i]);
      lpaRgbValues[i] = SysColor;
    }
    SMemFree(a1: pSysColorTbl, a2: aClrprefCpp_6, a3: 102, a4: 0);
    ColorPrefSetAppColors();
    return 1;
  }
  else
  {
    ColorPrefDestroy();
    SMemFree(a1: pSysColorTbl, a2: aClrprefCpp_5, a3: 89, a4: 0);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19008BF9
 * CodeView symbol: ColorPrefSetAppColors
 * Demangled: ColorPrefSetAppColors
 * IDA name: ColorPrefSetAppColors
 * CodeView module: 24
 * CV address: segment 1 : 0x7BF9
 * Code length: 0x2A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CLrpref.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CLrpref.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL ColorPrefSetAppColors()
{
  BOOL result; // eax

  if ( sgPrefColors != 0 )
    return SetSysColors(cElements: sgPrefColors, lpaElements: lpaElements, lpaRgbValues: dword_19042090);
  return result;
}


// ==============================================================================
/*
 * EA: 0x19008C23
 * CodeView symbol: ColorPrefActivate
 * Demangled: ColorPrefActivate
 * IDA name: ?ColorPrefActivate@@YIXH@Z
 * CodeView module: 24
 * CV address: segment 1 : 0x7C23
 * Code length: 0x1D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\CLrpref.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\CLrpref.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ColorPrefActivate(int a1)
{
  if ( a1 != 0 )
    ColorPrefSetAppColors();
  else
    ColorPrefRestoreWindowsColors();
}

