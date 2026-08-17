/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Battle.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Battle.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 3
 */


// ==============================================================================
/*
 * EA: 0x190026B0
 * CodeView symbol: SnpQuery
 * Demangled: SnpQuery
 * IDA name: _SnpQuery@20
 * CodeView module: 31
 * CV address: segment 1 : 0x16B0
 * Code length: 0x57
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Battle.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Battle.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SnpQuery(int index, _DWORD *id, _DWORD *description, _DWORD *requirements, _DWORD *caps)
{
  if ( index != 0 || id == nullptr || description == nullptr || requirements == nullptr || caps == nullptr )
    return 0;
  *id = bn_id;
  *description = bn_desc;
  *requirements = bn_req;
  *caps = &bn_caps;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19002707
 * CodeView symbol: SnpBind
 * Demangled: SnpBind
 * IDA name: _SnpBind@8
 * CodeView module: 31
 * CV address: segment 1 : 0x1707
 * Code length: 0x25
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Battle.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Battle.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SnpBind(int index, _DWORD *spi)
{
  if ( index != 0 || spi == nullptr )
    return 0;
  *spi = &bn_spi;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900272C
 * CodeView symbol: DllMain
 * Demangled: DllMain
 * IDA name: _DllMain@12
 * CodeView module: 31
 * CV address: segment 1 : 0x172C
 * Code length: 0x44
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Battle.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Battle.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  SDlgSetLocale(a1: 0, a2: aJapanese_0);
  if ( fdwReason != 0 )
  {
    if ( fdwReason == 1 )
      UiRegister();
  }
  else
  {
    UiUnregister();
  }
  global_hinstance = hinstDLL;
  return true;
}

