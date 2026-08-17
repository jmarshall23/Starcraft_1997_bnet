/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Perf.cpp
 *
 * CodeView module object:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Perf.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 4
 */


// ==============================================================================
/*
 * EA: 0x18008770
 * CodeView symbol: PerfAdd
 * Demangled: PerfAdd
 * IDA name: ?PerfAdd@@YIXKK@Z
 * CodeView module: 5
 * CV address: segment 1 : 0x7770
 * Code length: 0x2D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Perf.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Perf.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall PerfAdd(unsigned int a1, unsigned int a2)
{
  if ( a1 < 4 )
    *(&perfdata + a1) += a2;
}


// ==============================================================================
/*
 * EA: 0x1800879D
 * CodeView symbol: PerfGetPerformanceData
 * Demangled: PerfGetPerformanceData
 * IDA name: ?PerfGetPerformanceData@@YIHKPAKPAT_LARGE_INTEGER@@1@Z
 * CodeView module: 5
 * CV address: segment 1 : 0x779D
 * Code length: 0x8A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Perf.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Perf.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall PerfGetPerformanceData(
        unsigned int a1,
        unsigned int *a2,
        union _LARGE_INTEGER *measurementtime,
        union _LARGE_INTEGER *measurementfreq)
{
  int result; // eax

  switch ( a1 )
  {
    case 0xCu:
      *a2 = perfdata;
      result = 1;
      break;
    case 0xDu:
      *a2 = dword_1802CB48;
      result = 1;
      break;
    case 0xEu:
      *a2 = dword_1802CB4C;
      result = 1;
      break;
    case 0xFu:
      *a2 = dword_1802CB50;
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
 * EA: 0x18008827
 * CodeView symbol: PerfIncrement
 * Demangled: PerfIncrement
 * IDA name: ?PerfIncrement@@YIXK@Z
 * CodeView module: 5
 * CV address: segment 1 : 0x7827
 * Code length: 0x22
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Perf.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Perf.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall PerfIncrement(unsigned int a1)
{
  if ( a1 < 4 )
    InterlockedIncrement(lpAddend: &perfdata + a1);
}


// ==============================================================================
/*
 * EA: 0x18008849
 * CodeView symbol: PerfReset
 * Demangled: PerfReset
 * IDA name: ?PerfReset@@YIXXZ
 * CodeView module: 5
 * CV address: segment 1 : 0x7849
 * Code length: 0x16
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Perf.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Perf.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl PerfReset()
{
  memset(a1: &perfdata, Val: 0, Size: 0x10u);
}

