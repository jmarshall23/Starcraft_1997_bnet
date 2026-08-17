/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 *
 * CodeView module object:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 19
 */


// ==============================================================================
/*
 * EA: 0x18008520
 * CodeView symbol: NullCompareNetAddresses
 * Demangled: NullCompareNetAddresses
 * IDA name: ?NullCompareNetAddresses@@YGHPAU_SNETADDR@@0PAK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x7520
 * Code length: 0x55
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullCompareNetAddresses(struct _SNETADDR *addr1, struct _SNETADDR *addr2, unsigned int *diffmagnitude)
{
  if ( diffmagnitude != nullptr )
    *diffmagnitude = 0;
  if ( addr1 != nullptr && addr2 != nullptr )
  {
    if ( diffmagnitude != nullptr )
      *diffmagnitude = memcmp(Buf1: addr1, Buf2: addr2, Size: 0x10u) == 0;
    return 1;
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18008575
 * CodeView symbol: NullDestroy
 * Demangled: NullDestroy
 * IDA name: ?NullDestroy@@YGHXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x7575
 * Code length: 0xA
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullDestroy()
{
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800857F
 * CodeView symbol: NullFree
 * Demangled: NullFree
 * IDA name: ?NullFree@@YGHPAU_SNETADDR@@PAXK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x757F
 * Code length: 0x24
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullFree(struct _SNETADDR *addr, void *data, char databytes)
{
  if ( addr != nullptr && data != nullptr )
    return 1;
  SetLastError(dwErrCode: 0x57u);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x180085A3
 * CodeView symbol: NullFreeExternalMessage
 * Demangled: NullFreeExternalMessage
 * IDA name: ?NullFreeExternalMessage@@YGHPBD00@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x75A3
 * Code length: 0x11
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullFreeExternalMessage(const char *senderpath, const char *sendername, const char *mesage)
{
  SetLastError(dwErrCode: 0x57u);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x180085B4
 * CodeView symbol: NullGetGameInfo
 * Demangled: NullGetGameInfo
 * IDA name: ?NullGetGameInfo@@YGHKPBD0PAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x75B4
 * Code length: 0x14
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullGetGameInfo(
        char gameid,
        const char *gamename,
        const char *gamepassword,
        struct _SNETSPI_GAMELIST *gameinfo)
{
  SetLastError(dwErrCode: 0x85100068);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x180085C8
 * CodeView symbol: NullGetPerformanceData
 * Demangled: NullGetPerformanceData
 * IDA name: ?NullGetPerformanceData@@YGHKPAKPAT_LARGE_INTEGER@@1@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x75C8
 * Code length: 0x1A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullGetPerformanceData(
        unsigned int counterid,
        unsigned int *countervalue,
        union _LARGE_INTEGER *measurementtime,
        union _LARGE_INTEGER *measurementfreq)
{
  return PerfGetPerformanceData(a1: counterid, a2: countervalue, measurementtime, measurementfreq);
}


// ==============================================================================
/*
 * EA: 0x180085E2
 * CodeView symbol: NullInitialize
 * Demangled: NullInitialize
 * IDA name: ?NullInitialize@@YGHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAX@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x75E2
 * Code length: 0x1F
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullInitialize(
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        void *event)
{
  PerfReset();
  return SelectName(a1: programdata, a2: playerdata, interfacedata, versiondata);
}


// ==============================================================================
/*
 * EA: 0x18008601
 * CodeView symbol: NullInitializeDevice
 * Demangled: NullInitializeDevice
 * IDA name: ?NullInitializeDevice@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x7601
 * Code length: 0x9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullInitializeDevice(
        char deviceid,
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata)
{
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1800860A
 * CodeView symbol: NullLockDeviceList
 * Demangled: NullLockDeviceList
 * IDA name: ?NullLockDeviceList@@YGHPAPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x760A
 * Code length: 0x15
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullLockDeviceList(struct _SNETSPI_DEVICELIST **devicelist)
{
  *devicelist = nullptr;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800861F
 * CodeView symbol: NullLockGameList
 * Demangled: NullLockGameList
 * IDA name: ?NullLockGameList@@YGHKKPAPAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x761F
 * Code length: 0x2D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullLockGameList(char categorybits, char categorymask, struct _SNETSPI_GAMELIST **gamelist)
{
  if ( gamelist != nullptr )
  {
    *gamelist = nullptr;
    return 1;
  }
  else
  {
    SetLastError(dwErrCode: 0x57u);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1800864C
 * CodeView symbol: NullReceive
 * Demangled: NullReceive
 * IDA name: ?NullReceive@@YGHPAPAU_SNETADDR@@PAPAXPAK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x764C
 * Code length: 0x41
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullReceive(struct _SNETADDR **addr, void **data, unsigned int *databytes)
{
  if ( addr != nullptr )
    *addr = nullptr;
  if ( data != nullptr )
    *data = nullptr;
  if ( databytes != nullptr )
    *databytes = 0;
  SetLastError(dwErrCode: 0x8510006B);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1800868D
 * CodeView symbol: NullReceiveExternalMessage
 * Demangled: NullReceiveExternalMessage
 * IDA name: ?NullReceiveExternalMessage@@YGHPAPBD00@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x768D
 * Code length: 0x41
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullReceiveExternalMessage(const char **senderpath, const char **sendername, const char **message)
{
  if ( senderpath != nullptr )
    *senderpath = nullptr;
  if ( sendername != nullptr )
    *sendername = nullptr;
  if ( message != nullptr )
    *message = nullptr;
  SetLastError(dwErrCode: 0x8510006B);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x180086CE
 * CodeView symbol: NullSelectGame
 * Demangled: NullSelectGame
 * IDA name: ?NullSelectGame@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x76CE
 * Code length: 0x9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullSelectGame(
        char flags,
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        unsigned int *playerid)
{
  return 0;
}


// ==============================================================================
/*
 * EA: 0x180086D7
 * CodeView symbol: NullSend
 * Demangled: NullSend
 * IDA name: ?NullSend@@YGHKPAPAU_SNETADDR@@PAXK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x76D7
 * Code length: 0x30
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullSend(unsigned int addresses, struct _SNETADDR **addrlist, void *data, unsigned int databytes)
{
  if ( addresses != 0 && addrlist != nullptr && data != nullptr && databytes != 0 )
    return 1;
  SetLastError(dwErrCode: 0x57u);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18008707
 * CodeView symbol: NullSendExternalMessage
 * Demangled: NullSendExternalMessage
 * IDA name: ?NullSendExternalMessage@@YGHPBD0000@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x7707
 * Code length: 0x9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullSendExternalMessage(
        const char *senderpath,
        const char *sendername,
        const char *targetpath,
        const char *targetname,
        const char *message)
{
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18008710
 * CodeView symbol: NullStartAdvertisingGame
 * Demangled: NullStartAdvertisingGame
 * IDA name: ?NullStartAdvertisingGame@@YGHPBD00KKKKKPBXK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x7710
 * Code length: 0x24
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullStartAdvertisingGame(
        const char *gamename,
        const char *gamepassword,
        const char *gamedescription,
        char gamemode,
        char gameage,
        char gamecategorybits,
        char optcategorybits,
        char ladderid,
        const void *clientdata,
        char clientdatabytes)
{
  if ( gamename != nullptr && gamedescription != nullptr )
    return 1;
  SetLastError(dwErrCode: 0x57u);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18008734
 * CodeView symbol: NullStopAdvertisingGame
 * Demangled: NullStopAdvertisingGame
 * IDA name: ?NullStopAdvertisingGame@@YGHXZ
 * CodeView module: 6
 * CV address: segment 1 : 0x7734
 * Code length: 0xA
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullStopAdvertisingGame()
{
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800873E
 * CodeView symbol: NullUnlockDeviceList
 * Demangled: NullUnlockDeviceList
 * IDA name: ?NullUnlockDeviceList@@YGHPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x773E
 * Code length: 0xC
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullUnlockDeviceList(struct _SNETSPI_DEVICELIST *devicelist)
{
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800874A
 * CodeView symbol: NullUnlockGameList
 * Demangled: NullUnlockGameList
 * IDA name: ?NullUnlockGameList@@YGHPAU_SNETSPI_GAMELIST@@PAK@Z
 * CodeView module: 6
 * CV address: segment 1 : 0x774A
 * Code length: 0x1B
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Null.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Null.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall NullUnlockGameList(struct _SNETSPI_GAMELIST *gamelist, unsigned int *hintnextcall)
{
  if ( hintnextcall != nullptr )
    *hintnextcall = 0;
  return 1;
}

