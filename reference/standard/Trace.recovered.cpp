/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 *
 * CodeView module object:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 10
 */


// ==============================================================================
/*
 * EA: 0x18001000
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: unknown_libname_1
 * CodeView module: 11
 * CV address: segment 1 : 0x0
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_1()
{
  sub_1800100F();
  return sub_1800101E();
}


// ==============================================================================
/*
 * EA: 0x1800100F
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: sub_1800100F
 * CodeView module: 11
 * CV address: segment 1 : 0xF
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *sub_1800100F()
{
  return CCritSect::CCritSect(this: (CCritSect *)&trace_critsect);
}


// ==============================================================================
/*
 * EA: 0x1800101E
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: sub_1800101E
 * CodeView module: 11
 * CV address: segment 1 : 0x1E
 * Code length: 0x12
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1800101E()
{
  return atexit(a1: sub_18001030);
}


// ==============================================================================
/*
 * EA: 0x18001030
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: sub_18001030
 * CodeView module: 11
 * CV address: segment 1 : 0x30
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_18001030()
{
  CCritSect::~CCritSect(this: (CCritSect *)&trace_critsect);
}


// ==============================================================================
/*
 * EA: 0x1800103F
 * CodeView symbol: TraceOut
 * Demangled: TraceOut
 * IDA name: ?TraceOut@@YAXPBDZZ
 * CodeView module: 11
 * CV address: segment 1 : 0x3F
 * Code length: 0x5
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void TraceOut()
{
  ;
}


// ==============================================================================
/*
 * EA: 0x18001044
 * CodeView symbol: TracePend
 * Demangled: TracePend
 * IDA name: ?TracePend@@YAXPBDZZ
 * CodeView module: 11
 * CV address: segment 1 : 0x44
 * Code length: 0x5
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void TracePend()
{
  ;
}


// ==============================================================================
/*
 * EA: 0x18001050
 * CodeView symbol: operator delete
 * Demangled: operator delete
 * IDA name: ??3@YAXPAX@Z
 * CodeView module: 11
 * CV address: segment 1 : 0x50
 * Code length: 0x20
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl operator delete(void *ptr)
{
  if ( ptr != nullptr )
    SMemFree(a1: ptr, a2: `string', a3: 695, a4: 0);
}


// ==============================================================================
/*
 * EA: 0x18001070
 * CodeView symbol: operator new
 * Demangled: operator new
 * IDA name: ??2@YAPAXI@Z
 * CodeView module: 11
 * CV address: segment 1 : 0x70
 * Code length: 0x1A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__cdecl operator new(unsigned int bytes)
{
  return (void *)SMemAlloc(a1: bytes, a2: `string', a3: 699, a4: 0);
}


// ==============================================================================
/*
 * EA: 0x18001090
 * CodeView symbol: CCritSect::CCritSect
 * Demangled: CCritSect::CCritSect
 * IDA name: ??0CCritSect@@QAE@XZ
 * CodeView module: 11
 * CV address: segment 1 : 0x90
 * Code length: 0x18
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
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
 * EA: 0x180010B0
 * CodeView symbol: CCritSect::~CCritSect
 * Demangled: CCritSect::~CCritSect
 * IDA name: ??1CCritSect@@QAE@XZ
 * CodeView module: 11
 * CV address: segment 1 : 0xB0
 * Code length: 0x15
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Trace.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Trace.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall CCritSect::~CCritSect(struct _RTL_CRITICAL_SECTION *this)
{
  DeleteCriticalSection(lpCriticalSection: this);
}

