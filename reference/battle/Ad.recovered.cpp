/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 10
 */


// ==============================================================================
/*
 * EA: 0x19001BE0
 * CodeView symbol: AdSetInfo
 * Demangled: AdSetInfo
 * IDA name: ?AdSetInfo@@YIXPAU_SNADINFOREC@@@Z
 * CodeView module: 32
 * CV address: segment 1 : 0xBE0
 * Code length: 0x87
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall AdSetInfo(struct _SNADINFOREC *a1)
{
  dword_19041764 = *((_DWORD *)a1 + 1);
  sgNextAdInfo = *(_DWORD *)a1;
  if ( strlen(Str: *((const char **)a1 + 3)) >= 0x400 )
    Destination = 0;
  else
    strcpy(Destination: &Destination, Source: *((const char **)a1 + 3));
  if ( strlen(Str: *((const char **)a1 + 2)) >= 0x104 )
    byte_19041B68 = 0;
  else
    strcpy(Destination: &byte_19041B68, Source: *((const char **)a1 + 2));
}


// ==============================================================================
/*
 * EA: 0x19001C67
 * CodeView symbol: AdDisplay
 * Demangled: AdDisplay
 * IDA name: ?AdDisplay@@YIXPAXK@Z
 * CodeView module: 32
 * CV address: segment 1 : 0xC67
 * Code length: 0x8A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall AdDisplay(void *a1, unsigned int a2)
{
  int bSuccess; // [esp+14h] [ebp-4h]

  if ( sghWndAd != nullptr )
  {
    AdStopAd();
    if ( dword_19041764 == 1802335022 )
    {
      bSuccess = AdDisplaySMK(a1, a2);
    }
    else if ( dword_19041764 == 2019782702 )
    {
      bSuccess = AdDisplayPCX(a1, a2);
    }
    if ( bSuccess != 0 )
    {
      qmemcpy(&sgCurrAdInfo, &sgNextAdInfo, 1292);
      SrvNotifyDisplayAd(a1: sgNextAdInfo, a2: &byte_19041658, a3: &Source);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19001CF1
 * CodeView symbol: AdStopAd
 * Demangled: AdStopAd
 * IDA name: AdStopAd
 * CodeView module: 32
 * CV address: segment 1 : 0xCF1
 * Code length: 0x7E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int AdStopAd()
{
  int result; // eax

  Source = 0;
  byte_19041658 = 0;
  if ( sgAdTimer != 0 )
  {
    result = SDlgKillTimer(a1: sghWndAd, a2: sgAdTimer);
    sgAdTimer = 0;
  }
  if ( sghVideo != 0 )
  {
    result = SVidPlayEnd(a1: sghVideo);
    sghVideo = 0;
  }
  if ( sgpAdAnimation != nullptr )
  {
    result = SMemFree(a1: sgpAdAnimation, a2: aAdCpp, a3: 99, a4: 0);
    sgpAdAnimation = nullptr;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19001D6F
 * CodeView symbol: AdDisplayPCX
 * Demangled: AdDisplayPCX
 * IDA name: AdDisplayPCX
 * CodeView module: 32
 * CV address: segment 1 : 0xD6F
 * Code length: 0x1FD
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall AdDisplayPCX(int a1, int a2)
{
  int v3; // [esp+4h] [ebp-430h]
  struct tagPALETTEENTRY pe[256]; // [esp+10h] [ebp-424h] BYREF
  int dwBufferSize; // [esp+410h] [ebp-24h]
  int nHeight; // [esp+414h] [ebp-20h] BYREF
  int nBitDepth; // [esp+418h] [ebp-1Ch] BYREF
  int nWidth; // [esp+41Ch] [ebp-18h] BYREF
  struct tagRECT r; // [esp+420h] [ebp-14h] BYREF
  unsigned __int8 *pBuffer; // [esp+430h] [ebp-4h]

  if ( sghWndAd == nullptr )
    return 0;
  if ( SBmpDecodeImage(a1: 2, a2: a1, a3: a2, a4: pe, a5: 0, a6: 0, a7: &nWidth, a8: &nHeight, a9: &nBitDepth) == 0 )
    return 0;
  dwBufferSize = nBitDepth * nHeight * nWidth / 8;
  pBuffer = (unsigned __int8 *)SMemAlloc(a1: dwBufferSize, a2: aAdCpp_0, a3: 233, a4: 0);
  if ( pBuffer == nullptr )
    return 0;
  if ( SBmpDecodeImage(
         a1: 2,
         a2: a1,
         a3: a2,
         a4: pe,
         a5: pBuffer,
         a6: dwBufferSize,
         a7: &nWidth,
         a8: &nHeight,
         a9: &nBitDepth) != 0 )
  {
    GetClientRect(hWnd: sghWndAd, lpRect: &r);
    SDlgBltToWindowI(a1: sghWndAd, a2: 0, a3: 0, a4: 0, a5: pBuffer, a6: &r, a7: &r.right, a8: -1, a9: 0, a10: 66);
    if ( UiSetCustomArt(
           a1: nullptr,
           a2: pe,
           nFirstColor: 32,
           nNumColorsUsed: 64,
           bSetPaletteNow: 1,
           data: pBuffer,
           a7: nWidth,
           a8: nHeight) != 0 )
    {
      if ( nHeight >= dword_19041234 )
        v3 = dword_19041234;
      else
        v3 = nHeight;
      if ( nWidth >= sgAdSize )
        SBltROP3(a1: sgpAdData, a2: pBuffer, a3: sgAdSize, a4: v3, a5: sgAdSize, a6: nWidth, a7: 0, a8: 13369376);
      else
        SBltROP3(a1: sgpAdData, a2: pBuffer, a3: nWidth, a4: v3, a5: sgAdSize, a6: nWidth, a7: 0, a8: 13369376);
      InvalidateRect(hWnd: sghWndAd, lpRect: nullptr, bErase: false);
      SMemFree(a1: pBuffer, a2: aAdCpp_3, a3: 292, a4: 0);
      return 1;
    }
    else
    {
      SMemFree(a1: pBuffer, a2: aAdCpp_2, a3: 276, a4: 0);
      return 0;
    }
  }
  else
  {
    SMemFree(a1: pBuffer, a2: aAdCpp_1, a3: 246, a4: 0);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x19001F6C
 * CodeView symbol: AdDisplaySMK
 * Demangled: AdDisplaySMK
 * IDA name: AdDisplaySMK
 * CodeView module: 32
 * CV address: segment 1 : 0xF6C
 * Code length: 0x119
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall AdDisplaySMK(const void *a1, size_t a2)
{
  _DWORD paletteUse[3]; // [esp+8h] [ebp-1Ch] BYREF
  struct tagRECT r; // [esp+14h] [ebp-10h] BYREF

  paletteUse[0] = 12;
  paletteUse[1] = 32;
  paletteUse[2] = 64;
  if ( sghWndAd == nullptr )
    return 0;
  sgpAdAnimation = (void *)SMemAlloc(a1: a2, a2: aAdCpp_4, a3: 318, a4: 0);
  if ( sgpAdAnimation == nullptr )
    return 0;
  memcpy(a1: sgpAdAnimation, Src: a1, Size: a2);
  SetRect(lprc: &r, xLeft: 0, yTop: 0, xRight: sgAdSize - 1, yBottom: dword_19041234 - 1);
  SVidPlayBeginFromMemory(
    a1: sgpAdAnimation,
    a2,
    a3: sgpAdData,
    a4: &r,
    a5: &sgAdSize,
    a6: paletteUse,
    a7: 537133056,
    a8: &sghVideo);
  sgAdTimer = SDlgSetTimer(a1: sghWndAd, a2: 1, a3: 50, a4: AdSMKTimer);
  GetClientRect(hWnd: sghWndAd, lpRect: &r);
  SDlgBltToWindowI(a1: sghWndAd, a2: 0, a3: 0, a4: 0, a5: sgpAdAnimation, a6: &r, a7: &r.right, a8: -1, a9: 0, a10: 66);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19002085
 * CodeView symbol: AdSMKTimer
 * Demangled: AdSMKTimer
 * IDA name: AdSMKTimer
 * CodeView module: 32
 * CV address: segment 1 : 0x1085
 * Code length: 0x42
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall AdSMKTimer(HWND hWnd, char uMsg, char uID, char dwTime)
{
  int result; // eax
  int bUpdated; // [esp+0h] [ebp-4h] BYREF

  bUpdated = 0;
  if ( sghVideo != 0 )
  {
    result = SVidPlayContinueSingle(a1: sghVideo, a2: 0, a3: &bUpdated);
    if ( bUpdated != 0 )
      return InvalidateRect(hWnd, lpRect: nullptr, bErase: false);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190020C7
 * CodeView symbol: AdNavigate
 * Demangled: AdNavigate
 * IDA name: ?AdNavigate@@YIXPAU_SNETUIDATA@@@Z
 * CodeView module: 32
 * CV address: segment 1 : 0x10C7
 * Code length: 0x28C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall AdNavigate(struct _SNETUIDATA *a1)
{
  HWND DesktopWindow; // eax
  HWND FrameWindow; // eax
  HWND v3; // eax
  char szURLSave[1024]; // [esp+4h] [ebp-630h] BYREF
  FILE *fp; // [esp+404h] [ebp-230h]
  char szText[256]; // [esp+408h] [ebp-22Ch] BYREF
  int dwUsedLink; // [esp+508h] [ebp-12Ch] BYREF
  char szTitle[32]; // [esp+50Ch] [ebp-128h] BYREF
  CHAR szBrowserApp[260]; // [esp+52Ch] [ebp-108h] BYREF
  int bAdSuccess; // [esp+630h] [ebp-4h]

  bAdSuccess = 1;
  if ( Source != 0 )
  {
    if ( *((_DWORD *)a1 + 9) != 0 )
      (*((void (__stdcall **)(int, _DWORD, _DWORD))a1 + 9))(a1: 1112425812, a2: 0, a3: 0);
    strcpy(Destination: szURLSave, Source: &Source);
    dwUsedLink = 0;
    SRegLoadValue(a1: aPreferences_0, a2: aClickedLink, a3: 2, a4: &dwUsedLink);
    if ( dwUsedLink != 0
      || (dwUsedLink = 1,
          SRegSaveValue(a1: aPreferences_1, a2: aClickedLink_0, a3: 2, a4: 1),
          LocaleLoadString(a1: 0x410u, a2: szText, a3: 256),
          LocaleLoadString(a1: 0x3F2u, a2: szTitle, a3: 32),
          UiMessageBox(
            a1: *((int (__stdcall **)(HWND, const char *, const char *, unsigned int))a1 + 8),
            a2: *((HWND *)a1 + 2),
            lpText: szText,
            a4: szTitle,
            a5: 1u) == 1) )
    {
      DesktopWindow = GetDesktopWindow();
      SetForegroundWindow(hWnd: DesktopWindow);
      FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
      if ( (unsigned int)ShellExecuteA(
                           hwnd: FrameWindow,
                           lpOperation: Operation,
                           lpFile: szURLSave,
                           lpParameters: nullptr,
                           lpDirectory: nullptr,
                           nShowCmd: 1) <= 0x20 )
      {
        fp = fopen(FileName: FileName, Mode: Mode);
        fclose(Stream: fp);
        if ( (unsigned int)FindExecutableA(lpFile: File, lpDirectory: nullptr, lpResult: szBrowserApp) <= 0x20
          || (v3 = (HWND)SDrawGetFrameWindow(a1: 0),
              (unsigned int)ShellExecuteA(
                              hwnd: v3,
                              lpOperation: aOpen_0,
                              lpFile: szBrowserApp,
                              lpParameters: szURLSave,
                              lpDirectory: nullptr,
                              nShowCmd: 1) <= 0x20) )
        {
          UiRestoreApp();
          LocaleLoadString(a1: 0x412u, a2: szText, a3: 256);
          LocaleLoadString(a1: 0x3F2u, a2: szTitle, a3: 32);
          UiMessageBox(
            a1: *((int (__stdcall **)(HWND, const char *, const char *, unsigned int))a1 + 8),
            a2: *((HWND *)a1 + 2),
            lpText: szText,
            a4: szTitle,
            a5: 0x30u);
          bAdSuccess = 0;
        }
        DeleteFileA(lpFileName: aBattleHtm_1);
      }
      SrvNotifyClickAd(a1: sgCurrAdInfo, a2: bAdSuccess);
    }
    else
    {
      SrvNotifyClickAd(a1: sgCurrAdInfo, a2: 0);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19002353
 * CodeView symbol: AdDialogProc
 * Demangled: AdDialogProc
 * IDA name: ?AdDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 32
 * CV address: segment 1 : 0x1353
 * Code length: 0x181
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall AdDialogProc(HWND window, unsigned int message, unsigned int wparam, struct _SNETUIDATA *lparam)
{
  struct tagRECT testrect; // [esp+4h] [ebp-10h] BYREF

  if ( message <= 0x21 )
  {
    if ( message == 33 )
    {
      SetWindowLongA(hWnd: window, nIndex: 0, dwNewLong: 3);
      return 1;
    }
    if ( message == 2 )
    {
      AdDestroyWindow();
      dword_19041C6C = nullptr;
      DeleteObject(ho: shCursor);
      shCursor = nullptr;
    }
    else if ( message == 32 && sbNavigating == 0 )
    {
      if ( shCursor != nullptr && Source != 0 )
        SetCursor(hCursor: (HCURSOR)shCursor);
      return 1;
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message == 272 )
  {
    GetClientRect(hWnd: window, lpRect: &testrect);
    dword_19041C6C = lparam;
    if ( lparam != nullptr )
    {
      AdInitWindow(a1: window, a2: *((_DWORD *)dword_19041C6C + 3));
      if ( *((_DWORD *)dword_19041C6C + 11) != 0 )
        (*((void (__stdcall **)(int, int, HGDIOBJ *, int, _DWORD))dword_19041C6C + 11))(
          a1: 1112425812,
          a2: 2,
          a3: &shCursor,
          a4: 4,
          a5: 0);
      return 1;
    }
    else
    {
      SDlgEndDialog(a1: window, a2: 0);
      return 0;
    }
  }
  else
  {
    if ( message != 513 )
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    if ( sbNavigating == 0 )
    {
      sbNavigating = 1;
      AdNavigate(a1: dword_19041C6C);
      sbNavigating = 0;
    }
    return 1;
  }
}


// ==============================================================================
/*
 * EA: 0x190024D4
 * CodeView symbol: AdInitWindow
 * Demangled: AdInitWindow
 * IDA name: AdInitWindow
 * CodeView module: 32
 * CV address: segment 1 : 0x14D4
 * Code length: 0x196
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall AdInitWindow(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  LONG cy; // [esp+4h] [ebp-2Ch]
  unsigned __int8 *pTemp; // [esp+10h] [ebp-20h] BYREF
  struct tagRECT r; // [esp+14h] [ebp-1Ch] BYREF
  struct tagSIZE tempSize; // [esp+24h] [ebp-Ch] BYREF
  int nSize; // [esp+2Ch] [ebp-4h]

  pTemp = nullptr;
  sghWndAd = a1;
  if ( a1 == nullptr )
    return 0;
  SetWindowPos(hWnd: sghWndAd, hWndInsertAfter: nullptr, X: 0, Y: 0, cx: 468, cy: 60, uFlags: 0x216u);
  GetClientRect(hWnd: a1, lpRect: &r);
  dword_19041234 = r.bottom;
  sgAdSize = 4 - r.right % 4 + r.right;
  nSize = r.bottom * sgAdSize;
  sgpAdData = SMemAlloc(a1: r.bottom * sgAdSize, a2: aAdCpp_5, a3: 128, a4: 8);
  SDlgSetBitmapI(a1, a2: 0, a3: 0, a4: -1, a5: 1, a6: sgpAdData, a7: 0, a8: sgAdSize, a9: dword_19041234, a10: -1);
  UiLoadCustomArt(
    a1: a2,
    a2: nullptr,
    artid: 0x8000000C,
    nFirstColor: 32,
    nNumColorsUsed: 64,
    bSetPaletteNow: 1,
    a7: &pTemp,
    a8: &tempSize);
  strcpy(Destination: &Source, Source: aHttpWwwBattleN);
  if ( pTemp != nullptr )
  {
    if ( tempSize.cy >= dword_19041234 )
      cy = dword_19041234;
    else
      cy = tempSize.cy;
    if ( tempSize.cx >= sgAdSize )
      SBltROP3(a1: sgpAdData, a2: pTemp, a3: sgAdSize, a4: cy, a5: sgAdSize, a6: tempSize.cx, a7: 0, a8: 13369376);
    else
      SBltROP3(a1: sgpAdData, a2: pTemp, a3: tempSize.cx, a4: cy, a5: sgAdSize, a6: tempSize.cx, a7: 0, a8: 13369376);
    SMemFree(a1: pTemp, a2: aAdCpp_6, a3: 159, a4: 0);
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900266A
 * CodeView symbol: AdDestroyWindow
 * Demangled: AdDestroyWindow
 * IDA name: AdDestroyWindow
 * CodeView module: 32
 * CV address: segment 1 : 0x166A
 * Code length: 0x3E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ad.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ad.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int AdDestroyWindow()
{
  int result; // eax

  result = AdStopAd();
  sghWndAd = nullptr;
  if ( sgpAdData != 0 )
  {
    result = SMemFree(a1: sgpAdData, a2: aAdCpp_7, a3: 170, a4: 0);
    sgpAdData = 0;
  }
  return result;
}

