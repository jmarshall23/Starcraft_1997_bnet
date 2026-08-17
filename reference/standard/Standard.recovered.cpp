/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Standard.cpp
 *
 * CodeView module object:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Standard.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 4
 */


// ==============================================================================
/*
 * EA: 0x1800A690
 * CodeView symbol: SelectName
 * Demangled: SelectName
 * IDA name: ?SelectName@@YIHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@@Z
 * CodeView module: 3
 * CV address: segment 1 : 0x9690
 * Code length: 0x49
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Standard.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Standard.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SelectName(
        struct _SNETPROGRAMDATA *a1,
        struct _SNETPLAYERDATA *a2,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata)
{
  if ( *((_DWORD *)interfacedata + 18) != 0 )
    return (*((int (__stdcall **)(struct _SNETPROGRAMDATA *, struct _SNETPLAYERDATA *, struct _SNETUIDATA *, struct _SNETVERSIONDATA *, unsigned int, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD))interfacedata
            + 18))(
             a1,
             a2,
             a3: interfacedata,
             a4: versiondata,
             a5: ipx_id,
             a6: 0,
             a7: 0,
             a8: 0,
             a9: 0,
             a10: 0);
  else
    return 1;
}


// ==============================================================================
/*
 * EA: 0x1800A6D9
 * CodeView symbol: SnpQuery
 * Demangled: SnpQuery
 * IDA name: _SnpQuery@20
 * CodeView module: 3
 * CV address: segment 1 : 0x96D9
 * Code length: 0x186
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Standard.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Standard.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SnpQuery(int index, _DWORD *id, _DWORD *description, _DWORD *requirements, _DWORD *caps)
{
  int result; // eax

  if ( id == nullptr || description == nullptr || requirements == nullptr || caps == nullptr )
    return 0;
  switch ( index )
  {
    case 0:
      *id = ipx_id;
      *description = ipx_desc;
      *requirements = ipx_req;
      *caps = &ipx_caps;
      result = 1;
      break;
    case 1:
      *id = serial_id;
      *description = serial_desc;
      *requirements = serial_req;
      *caps = &serial_caps;
      result = 1;
      break;
    case 2:
      *id = null_id;
      *description = null_desc;
      *requirements = null_req;
      *caps = &null_caps;
      result = 1;
      break;
    case 3:
      *id = modem_id;
      *description = modem_desc;
      *requirements = modem_req;
      *caps = &modem_caps;
      result = 1;
      break;
    case 4:
      *id = ipx_latency_id;
      *description = ipx_latency_desc;
      *requirements = ipx_latency_req;
      *caps = &ipx_latency_caps;
      result = 1;
      break;
    case 5:
      *id = modem_latency_id;
      *description = modem_latency_desc;
      *requirements = modem_latency_req;
      *caps = &modem_latency_caps;
      result = 1;
      break;
    default:
      result = 0;
      break;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1800A85F
 * CodeView symbol: SnpBind
 * Demangled: SnpBind
 * IDA name: _SnpBind@8
 * CodeView module: 3
 * CV address: segment 1 : 0x985F
 * Code length: 0xA4
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Standard.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Standard.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall SnpBind(int index, _DWORD *spi)
{
  int result; // eax

  if ( spi == nullptr )
    return 0;
  switch ( index )
  {
    case 0:
      *spi = &ipx_spi;
      result = 1;
      break;
    case 1:
      *spi = &serial_spi;
      result = 1;
      break;
    case 2:
      *spi = &null_spi;
      result = 1;
      break;
    case 3:
      *spi = &modem_spi;
      result = 1;
      break;
    case 4:
      *spi = &ipx_latency_spi;
      result = 1;
      break;
    case 5:
      *spi = &modem_latency_spi;
      result = 1;
      break;
    default:
      result = 0;
      break;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1800A903
 * CodeView symbol: DllMain
 * Demangled: DllMain
 * IDA name: _DllMain@12
 * CodeView module: 3
 * CV address: segment 1 : 0x9903
 * Code length: 0x1A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Standard.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Standard.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if ( fdwReason == 1 )
    global_hinstance = hinstDLL;
  return true;
}

