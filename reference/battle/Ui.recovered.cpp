/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 29
 */


// ==============================================================================
/*
 * EA: 0x1901D850
 * CodeView symbol: UiRegister
 * Demangled: UiRegister
 * IDA name: ?UiRegister@@YIXXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1C850
 * Code length: 0x14
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl UiRegister()
{
  ComboRegisterClass();
  ScrollbarRegisterClass();
  SliderRegisterClass();
}


// ==============================================================================
/*
 * EA: 0x1901D864
 * CodeView symbol: UiUnregister
 * Demangled: UiUnregister
 * IDA name: ?UiUnregister@@YIXXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1C864
 * Code length: 0x14
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl UiUnregister()
{
  ScrollbarUnregisterClass();
  ComboUnregisterClass();
  SliderUnregisterClass();
}


// ==============================================================================
/*
 * EA: 0x1901D878
 * CodeView symbol: UiInitialize
 * Demangled: UiInitialize
 * IDA name: ?UiInitialize@@YIHPAU_SNETUIDATA@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1C878
 * Code length: 0x3E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall UiInitialize(struct _SNETUIDATA *a1)
{
  if ( *((_DWORD *)a1 + 3) == 0 )
    return 0;
  ScrollbarLoadArtwork(a1: *((int (__stdcall **)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))a1
                           + 3));
  ComboboxLoadArtwork(a1: *((int (__stdcall **)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))a1
                          + 3));
  SliderLoadArtwork(a1: *((int (__stdcall **)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))a1
                        + 3));
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901D8B6
 * CodeView symbol: UiDestroy
 * Demangled: UiDestroy
 * IDA name: ?UiDestroy@@YIXXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1C8B6
 * Code length: 0x2A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl UiDestroy()
{
  if ( ghWndUiMainParent != nullptr )
    SDlgEndDialog(a1: ghWndUiMainParent, a2: 0);
  ScrollbarDestroyArtwork();
  ComboboxDestroyArtwork();
  SliderDestroyArtwork();
}


// ==============================================================================
/*
 * EA: 0x1901D8E0
 * CodeView symbol: RestoreWindowsColors
 * Demangled: RestoreWindowsColors
 * IDA name: ?RestoreWindowsColors@@YIXXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1C8E0
 * Code length: 0x4C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl RestoreWindowsColors()
{
  HPALETTE hPal; // [esp+0h] [ebp-4h]

  hPal = (HPALETTE)GetStockObject(i: 15);
  GetPaletteEntries(hpal: hPal, iStart: 0, cEntries: 0xAu, pPalEntries: &sgFadePal);
  GetPaletteEntries(hpal: hPal, iStart: 0xAu, cEntries: 0xAu, pPalEntries: &pPalEntries);
  SDrawUpdatePalette(a1: 0, a2: 256, a3: &sgFadePal, a4: 1);
}


// ==============================================================================
/*
 * EA: 0x1901D92C
 * CodeView symbol: UiVidFade
 * Demangled: UiVidFade
 * IDA name: ?UiVidFade@@YIXHH@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1C92C
 * Code length: 0xEF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall UiVidFade(int a1, int a2)
{
  int palreg; // [esp+8h] [ebp-4h]

  if ( a1 == a2 )
  {
    memcpy(a1: &sgFadePal, Src: &sgPalette, Size: 0x400u);
    ShowCursor(bShow: true);
  }
  else if ( a2 != 0 )
  {
    for ( palreg = 0; palreg < 256; ++palreg )
    {
      *(&sgFadePal.peRed + 4 * palreg) = a2 * *(&sgPalette.peRed + 4 * palreg) / a1;
      *(&sgFadePal.peGreen + 4 * palreg) = a2 * *(&sgPalette.peGreen + 4 * palreg) / a1;
      *(&sgFadePal.peBlue + 4 * palreg) = a2 * *(&sgPalette.peBlue + 4 * palreg) / a1;
    }
  }
  else
  {
    memcpy(a1: &sgFadePal, Src: &sgPalette, Size: 0x400u);
    UiClearPalette(a1: &sgFadePal);
  }
  SDrawUpdatePalette(a1: 0, a2: 256, a3: &sgFadePal, a4: 1);
}


// ==============================================================================
/*
 * EA: 0x1901DA1B
 * CodeView symbol: UiClearPalette
 * Demangled: UiClearPalette
 * IDA name: UiClearPalette
 * CodeView module: 4
 * CV address: segment 1 : 0x1CA1B
 * Code length: 0x4A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall UiClearPalette(_BYTE *this)
{
  int result; // eax
  int palreg; // [esp+4h] [ebp-4h]

  for ( palreg = 0; palreg < 256; ++palreg )
  {
    *(this + 4 * palreg) = 0;
    *(this + 4 * palreg + 1) = 0;
    *(this + 4 * palreg + 2) = 0;
    result = palreg + 1;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1901DA65
 * CodeView symbol: UiVidFadeOut
 * Demangled: UiVidFadeOut
 * IDA name: ?UiVidFadeOut@@YIXH@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1CA65
 * Code length: 0x123
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall UiVidFadeOut(int a1)
{
  int palreg; // [esp+4h] [ebp-8h]
  int index; // [esp+8h] [ebp-4h]

  ShowCursor(bShow: false);
  memcpy(a1: &sgFadePal, Src: &sgPalette, Size: 0x400u);
  for ( index = 0; index < a1; ++index )
  {
    for ( palreg = 0; palreg < 256; ++palreg )
    {
      *(&sgFadePal.peRed + 4 * palreg) -= *(&sgPalette.peRed + 4 * palreg) / a1;
      *(&sgFadePal.peGreen + 4 * palreg) -= *(&sgPalette.peGreen + 4 * palreg) / a1;
      *(&sgFadePal.peBlue + 4 * palreg) -= *(&sgPalette.peBlue + 4 * palreg) / a1;
    }
    SDrawUpdatePalette(a1: 0, a2: 256, a3: &sgFadePal, a4: 1);
  }
  UiClearPalette(this: &sgFadePal);
  SDrawUpdatePalette(a1: 0, a2: 256, a3: &sgFadePal, a4: 1);
  SDrawClearSurface(a1: 0);
  RestoreWindowsColors();
}


// ==============================================================================
/*
 * EA: 0x1901DB88
 * CodeView symbol: UiLoadCursors
 * Demangled: UiLoadCursors
 * IDA name: ?UiLoadCursors@@YIXPAUHWND__@@PAU_SNETUIDATA@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1CB88
 * Code length: 0x6F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall UiLoadCursors(HWND a1, struct _SNETUIDATA *a2)
{
  HCURSOR CursorA; // eax
  HCURSOR v3; // eax
  int OldArrow; // [esp+8h] [ebp-8h] BYREF
  int OldIBeam; // [esp+Ch] [ebp-4h] BYREF

  OldArrow = 0;
  OldIBeam = 0;
  if ( a2 != nullptr && *((_DWORD *)a2 + 11) != 0 )
  {
    CursorA = LoadCursorA(hInstance: nullptr, lpCursorName: (LPCSTR)0x7F00);
    SDlgSetCursor(a1, a2: CursorA, a3: 32512, a4: &OldArrow);
    v3 = LoadCursorA(hInstance: nullptr, lpCursorName: (LPCSTR)0x7F01);
    SDlgSetCursor(a1, a2: v3, a3: 32513, a4: &OldIBeam);
  }
}


// ==============================================================================
/*
 * EA: 0x1901DBF7
 * CodeView symbol: UiSetFont
 * Demangled: UiSetFont
 * IDA name: ?UiSetFont@@YIPAUHFONT__@@PAUHWND__@@W4eFONTSTYLE@@QAH@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1CBF7
 * Code length: 0x22D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HANDLE __fastcall UiSetFont(HWND a1, int a2, int nControlTbl)
{
  char *v4; // eax
  FILE *fp; // [esp+8h] [ebp-148h]
  CHAR str[256]; // [esp+Ch] [ebp-144h] BYREF
  HANDLE hFont; // [esp+10Ch] [ebp-44h]
  LOGFONTA lFont; // [esp+110h] [ebp-40h] BYREF
  int ctl; // [esp+14Ch] [ebp-4h]

  hFont = (HANDLE)SendMessageA(hWnd: a1, Msg: 0x31u, wParam: 0, lParam: 0);
  if ( hFont == nullptr )
    hFont = GetStockObject(i: 13);
  memset(a1: &lFont, Val: 0, Size: sizeof(lFont));
  if ( GetObjectA(h: hFont, c: 60, pv: &lFont) == 0 )
    return nullptr;
  lFont.lfHeight = -MulDiv(nNumber: *(&StyleTbl + 2 * a2), nNumerator: 96, nDenominator: 72);
  lFont.lfWeight = 400;
  lFont.lfCharSet = 0x80;
  strcpy(Destination: lFont.lfFaceName, Source: (&off_1903DBDC)[2 * a2]);
  fp = fopen(FileName: aBattleSnpDebug, Mode: aAt_1);
  if ( fp == nullptr )
    MessageBoxA(hWnd: nullptr, lpText: Text, lpCaption: Caption, uType: 0);
  _wsprintfA(a1: str, a2: "font Face %s\n", lFont.lfFaceName);
  fputs(Buffer: str, Stream: fp);
  OutputDebugStringA(lpOutputString: str);
  _wsprintfA(a1: str, a2: "font Height %d\n", lFont.lfHeight);
  fputs(Buffer: str, Stream: fp);
  OutputDebugStringA(lpOutputString: str);
  _wsprintfA(a1: str, a2: "font StyleTbl Size %d\n", *(&StyleTbl + 2 * a2));
  fputs(Buffer: str, Stream: fp);
  OutputDebugStringA(lpOutputString: str);
  v4 = setlocale(Category: 0, Locale: nullptr);
  fputs(Buffer: v4, Stream: fp);
  fclose(Stream: fp);
  hFont = CreateFontIndirectA(lplf: &lFont);
  if ( hFont == nullptr )
    return nullptr;
  for ( ctl = 0; *(_DWORD *)(nControlTbl + 4 * ctl) != 0; ++ctl )
    SendDlgItemMessageA(
      hDlg: a1,
      nIDDlgItem: *(_DWORD *)(nControlTbl + 4 * ctl),
      Msg: 0x30u,
      wParam: (WPARAM)hFont,
      lParam: 0);
  return hFont;
}


// ==============================================================================
/*
 * EA: 0x1901DE24
 * CodeView symbol: UiGetData
 * Demangled: UiGetData
 * IDA name: ?UiGetData@@YIHP6GHKKPAXKPAK@ZKPAPAE1@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1CE24
 * Code length: 0xB7
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall UiGetData(
        int (__stdcall *a1)(unsigned int, unsigned int, void *, unsigned int, unsigned int *),
        unsigned int a2,
        unsigned __int8 **data,
        unsigned int *datasize)
{
  unsigned int dwSize; // [esp+8h] [ebp-4h] BYREF

  *data = nullptr;
  if ( a1 == nullptr )
    return 0;
  if ( a1(a1: 1112425812u, a2, a3: nullptr, a4: 0, a5: &dwSize) == 0 )
    return 0;
  *data = (unsigned __int8 *)SMemAlloc(a1: dwSize, a2: aUiCpp, a3: 341, a4: 0);
  if ( *data == nullptr )
    return 0;
  if ( a1(a1: 1112425812u, a2, a3: *data, a4: dwSize, a5: nullptr) != 0 )
  {
    *datasize = dwSize;
    return 1;
  }
  else
  {
    SMemFree(a1: *data, a2: aUiCpp_0, a3: 350, a4: 0);
    *data = nullptr;
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901DEDB
 * CodeView symbol: UiLoadArtwork
 * Demangled: UiLoadArtwork
 * IDA name: ?UiLoadArtwork@@YIHP6GHKKPAUtagPALETTEENTRY@@PAEKPAH22@ZPAUHWND__@@4KPBDKJHHPAPAEPAUtagSIZE@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1CEDB
 * Code length: 0x22F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall UiLoadArtwork(
        int (__stdcall *a1)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *),
        HWND a2,
        HWND hWndParent,
        unsigned int artid,
        const char *controltype,
        unsigned int controlstyle,
        int usageflags,
        int loadpalette,
        int prepfadein,
        unsigned __int8 **data,
        struct tagSIZE *size)
{
  HPALETTE hPal; // [esp+8h] [ebp-414h]
  unsigned int bytes; // [esp+Ch] [ebp-410h]
  LONG height; // [esp+10h] [ebp-40Ch] BYREF
  int bitdepth; // [esp+14h] [ebp-408h] BYREF
  struct tagPALETTEENTRY pe[256]; // [esp+18h] [ebp-404h] BYREF
  LONG width; // [esp+418h] [ebp-4h] BYREF

  *data = nullptr;
  if ( a1 == nullptr )
    return 0;
  width = 0;
  height = 0;
  if ( a1(a1: 1112425812u, a2: artid, a3: nullptr, a4: nullptr, a5: 0, a6: &width, a7: &height, a8: &bitdepth) == 0 )
    return 0;
  if ( size != nullptr )
  {
    size->cx = width;
    size->cy = height;
  }
  bytes = bitdepth * height * width / 8;
  *data = (unsigned __int8 *)SMemAlloc(a1: bytes, a2: aUiCpp_1, a3: 400, a4: 0);
  if ( *data == nullptr )
    return 0;
  if ( a1(a1: 1112425812u, a2: artid, a3: pe, a4: *data, a5: bytes, a6: &width, a7: &height, a8: &bitdepth) != 0 )
  {
    if ( loadpalette != 0 )
    {
      hPal = (HPALETTE)GetStockObject(i: 15);
      GetPaletteEntries(hpal: hPal, iStart: 0, cEntries: 0xAu, pPalEntries: pe);
      GetPaletteEntries(hpal: hPal, iStart: 0xAu, cEntries: 0xAu, pPalEntries: &pe[246]);
      memcpy(a1: &sgPalette, Src: pe, Size: 0x400u);
      IconMap(a1: &sgPalette);
      if ( prepfadein != 0 )
      {
        UiClearPalette(this: pe);
        SDrawUpdatePalette(a1: 0, a2: 256, a3: pe, a4: 1);
        gFadeStep = 0;
      }
      else
      {
        SDrawUpdatePalette(a1: 0, a2: 256, a3: &sgPalette, a4: 1);
      }
    }
    if ( a2 != nullptr || hWndParent != nullptr )
      SDlgSetBitmapI(
        a1: a2,
        a2: hWndParent,
        a3: controltype,
        a4: controlstyle,
        a5: usageflags,
        a6: *data,
        a7: 0,
        a8: width,
        a9: height,
        a10: -1);
    return 1;
  }
  else
  {
    SMemFree(a1: *data, a2: aUiCpp_2, a3: 412, a4: 0);
    *data = nullptr;
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901E10A
 * CodeView symbol: UiSetCustomArt
 * Demangled: UiSetCustomArt
 * IDA name: ?UiSetCustomArt@@YIHPAUHWND__@@PAUtagPALETTEENTRY@@HHHPAEHH@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1D10A
 * Code length: 0x96
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall UiSetCustomArt(
        HWND a1,
        struct tagPALETTEENTRY *a2,
        int nFirstColor,
        int nNumColorsUsed,
        int bSetPaletteNow,
        unsigned __int8 *data,
        int nWidth,
        int nHeight)
{
  if ( nNumColorsUsed != 0 )
    memcpy(a1: &sgPalette + nFirstColor, Src: &a2[nFirstColor], Size: 4 * nNumColorsUsed);
  if ( bSetPaletteNow != 0 )
    SDrawUpdatePalette(a1: nFirstColor, a2: nNumColorsUsed, a3: &sgPalette + nFirstColor, a4: 0);
  if ( a1 != nullptr )
  {
    SDlgSetBitmapI(a1, a2: 0, a3: 0, a4: -1, a5: 1, a6: data, a7: 0, a8: nWidth, a9: nHeight, a10: -1);
    InvalidateRect(hWnd: a1, lpRect: nullptr, bErase: false);
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901E1A0
 * CodeView symbol: UiLoadCustomArt
 * Demangled: UiLoadCustomArt
 * IDA name: ?UiLoadCustomArt@@YIHP6GHKKPAUtagPALETTEENTRY@@PAEKPAH22@ZPAUHWND__@@KHHHPAPAEPAUtagSIZE@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1D1A0
 * Code length: 0x16D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall UiLoadCustomArt(
        int (__stdcall *a1)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *),
        HWND a2,
        unsigned int artid,
        int nFirstColor,
        int nNumColorsUsed,
        int bSetPaletteNow,
        unsigned __int8 **data,
        struct tagSIZE *size)
{
  unsigned int bytes; // [esp+8h] [ebp-410h]
  LONG height; // [esp+Ch] [ebp-40Ch] BYREF
  int bitdepth; // [esp+10h] [ebp-408h] BYREF
  struct tagPALETTEENTRY pe[256]; // [esp+14h] [ebp-404h] BYREF
  LONG width; // [esp+414h] [ebp-4h] BYREF

  *data = nullptr;
  if ( a1 == nullptr )
    return 0;
  width = 0;
  height = 0;
  if ( a1(a1: 1112425812u, a2: artid, a3: nullptr, a4: nullptr, a5: 0, a6: &width, a7: &height, a8: &bitdepth) == 0 )
    return 0;
  if ( size != nullptr )
  {
    size->cx = width;
    size->cy = height;
  }
  bytes = bitdepth * height * width / 8;
  *data = (unsigned __int8 *)SMemAlloc(a1: bytes, a2: aUiCpp_3, a3: 535, a4: 0);
  if ( *data == nullptr )
    return 0;
  if ( a1(a1: 1112425812u, a2: artid, a3: pe, a4: *data, a5: bytes, a6: &width, a7: &height, a8: &bitdepth) != 0 )
  {
    UiSetCustomArt(
      a1: a2,
      a2: pe,
      nFirstColor,
      nNumColorsUsed,
      bSetPaletteNow,
      data: *data,
      nWidth: width,
      nHeight: height);
    return 1;
  }
  else
  {
    SMemFree(a1: *data, a2: aUiCpp_4, a3: 547, a4: 0);
    *data = nullptr;
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901E30D
 * CodeView symbol: UiBeginConnect
 * Demangled: UiBeginConnect
 * IDA name: ?UiBeginConnect@@YIHPAU_SNETPROGRAMDATA@@PAU_SNETUIDATA@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1D30D
 * Code length: 0x178
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall UiBeginConnect(struct _SNETPROGRAMDATA *a1, struct _SNETUIDATA *a2)
{
  HWND Parent; // eax
  HMODULE ModuleHandleA; // eax
  HMODULE v5; // eax
  HMODULE v6; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-24h]
  const struct DLGTEMPLATE *v8; // [esp-10h] [ebp-24h]
  HWND v9; // [esp-Ch] [ebp-20h]
  HWND hWndParent; // [esp+10h] [ebp-4h]

  suiparams = 0;
  dword_19044044 = (int)a1;
  dword_19044048 = 0;
  dword_1904404C = (int)a2;
  dword_19044050 = 0;
  dword_19044054 = 0;
  gbConnectionLost = 0;
  gbConnectionSucks = 0;
  if ( a2 == nullptr )
    return false;
  hWndParent = *((HWND *)a2 + 2);
  if ( (HWND)SDrawGetFrameWindow(a1: 0) != hWndParent )
  {
    Parent = GetParent(hWnd: hWndParent);
    SetActiveWindow(hWnd: Parent);
    ShowWindow(hWnd: hWndParent, nCmdShow: 0);
  }
  SDrawClearSurface(a1: 0);
  Dialog = LocaleGetDialog(a1: 0x69u);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  sghDlgConnect = (HWND)SDlgCreateDialogIndirectParam(
                          a1: ModuleHandleA,
                          a2: Dialog,
                          a3: hWndParent,
                          a4: ConnectDialogProc,
                          a5: &suiparams);
  if ( sghDlgConnect == nullptr )
  {
    v5 = GetModuleHandleA(lpModuleName: nullptr);
    sghDlgConnect = (HWND)SDlgCreateDialogParam(a1: v5, a2: 105, a3: hWndParent, a4: ConnectDialogProc, a5: &suiparams);
  }
  qmemcpy(&sgInterfacedata, a2, 0x50u);
  dword_19044478 = (int)sghDlgConnect;
  if ( sghDlgConnect != nullptr )
  {
    v9 = sghDlgConnect;
    v8 = LocaleGetDialog(a1: 0x6Cu);
    v6 = GetModuleHandleA(lpModuleName: nullptr);
    sghDlgCancel = (HWND)SDlgCreateDialogIndirectParam(
                           a1: v6,
                           a2: v8,
                           a3: v9,
                           a4: ConnectCancelDialogProc,
                           a5: &sgInterfacedata);
    ProtectMinimize(a1: 1);
  }
  return sghDlgConnect != nullptr;
}


// ==============================================================================
/*
 * EA: 0x1901E485
 * CodeView symbol: UiEndConnect
 * Demangled: UiEndConnect
 * IDA name: ?UiEndConnect@@YIXH@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1D485
 * Code length: 0x82
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall UiEndConnect(int a1)
{
  HWND FrameWindow; // esi
  HWND Parent; // eax
  HWND v3; // eax

  if ( sghDlgConnect != nullptr )
  {
    ProtectMinimize(a1: 0);
    FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
    if ( FrameWindow != GetParent(hWnd: sghDlgConnect) && a1 != 0 )
    {
      Parent = GetParent(hWnd: sghDlgConnect);
      v3 = GetParent(hWnd: Parent);
      SetActiveWindow(hWnd: v3);
    }
    DestroyWindow(hWnd: sghDlgConnect);
    sghDlgConnect = nullptr;
    sghDlgCancel = nullptr;
  }
  dword_19044478 = 0;
}


// ==============================================================================
/*
 * EA: 0x1901E507
 * CodeView symbol: UiRestoreApp
 * Demangled: UiRestoreApp
 * IDA name: ?UiRestoreApp@@YIXXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1D507
 * Code length: 0x28
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl UiRestoreApp()
{
  HWND FrameWindow; // eax
  HWND v1; // eax

  FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
  SetActiveWindow(hWnd: FrameWindow);
  v1 = (HWND)SDrawGetFrameWindow(a1: 0);
  ShowWindow(hWnd: v1, nCmdShow: 9);
  UiProcessWindowMessages();
}


// ==============================================================================
/*
 * EA: 0x1901E52F
 * CodeView symbol: UiDisconnect
 * Demangled: UiDisconnect
 * IDA name: ?UiDisconnect@@YIHXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1D52F
 * Code length: 0x19
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl UiDisconnect()
{
  EnumNewRASConnections(
    lpCallback: (int (__stdcall *)(const char *, void *, void *))RasEnumCallback,
    lpContext: &sgInterfacedata);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901E548
 * CodeView symbol: UiConnectionLost
 * Demangled: UiConnectionLost
 * IDA name: ?UiConnectionLost@@YIXPAU_SNETUIDATA@@PAUHWND__@@@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1D548
 * Code length: 0x8A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall UiConnectionLost(int (__stdcall **a1)(HWND, const char *, const char *, unsigned int), HWND a2)
{
  char szText[256]; // [esp+8h] [ebp-124h] BYREF
  char szTitle[32]; // [esp+108h] [ebp-24h] BYREF
  HWND hWnd; // [esp+128h] [ebp-4h]

  hWnd = GetActiveWindow();
  LocaleLoadString(a1: 0x3F2u, a2: szTitle, buffersize: 32);
  LocaleLoadString(a1: 0x3F3u, a2: szText, buffersize: 256);
  UiMessageBox(a1: a1[8], a2: hWnd, lpText: szText, lpCaption: szTitle, uType: 0x10u);
  KillChildWindows(a1: a2, a2: hWnd);
  SDlgEndDialog(a1: a2, a2: 0);
  SErrSetLastError(a1: 1232);
}


// ==============================================================================
/*
 * EA: 0x1901E5D2
 * CodeView symbol: UiProcessWindowMessages
 * Demangled: UiProcessWindowMessages
 * IDA name: ?UiProcessWindowMessages@@YIHXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1D5D2
 * Code length: 0xC0
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl UiProcessWindowMessages()
{
  struct tagMSG message; // [esp+0h] [ebp-1Ch] BYREF

  while ( PeekMessageA(lpMsg: &message, hWnd: nullptr, wMsgFilterMin: 0, wMsgFilterMax: 0, wRemoveMsg: 1u) )
  {
    if ( message.message == 18 )
    {
      PostMessageA(hWnd: message.hwnd, Msg: 0x12u, wParam: message.wParam, lParam: message.lParam);
      return 0;
    }
    if ( (sghDlgConnect == nullptr || !IsDialogMessageA(hDlg: sghDlgConnect, lpMsg: &message))
      && (sghDlgProgress == nullptr || !IsDialogMessageA(hDlg: sghDlgProgress, lpMsg: &message))
      && (sghDlgCancel == nullptr || !IsDialogMessageA(hDlg: sghDlgCancel, lpMsg: &message)) )
    {
      TranslateMessage(lpMsg: &message);
      DispatchMessageA(lpMsg: &message);
    }
  }
  SDlgCheckTimers();
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1901E692
 * CodeView symbol: UiHideConnectCancel
 * Demangled: UiHideConnectCancel
 * IDA name: ?UiHideConnectCancel@@YIXXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1D692
 * Code length: 0x26
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl UiHideConnectCancel()
{
  if ( sghDlgCancel != nullptr )
  {
    DestroyWindow(hWnd: sghDlgCancel);
    sghDlgCancel = nullptr;
  }
}


// ==============================================================================
/*
 * EA: 0x1901E6B8
 * CodeView symbol: UiWSockErrMessage
 * Demangled: UiWSockErrMessage
 * IDA name: ?UiWSockErrMessage@@YIXXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1D6B8
 * Code length: 0x1B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl UiWSockErrMessage()
{
  UiHideConnectCancel();
  UiConnectMsg(flags: 48);
}


// ==============================================================================
/*
 * EA: 0x1901E6D3
 * CodeView symbol: UiConnectMsg
 * Demangled: UiConnectMsg
 * IDA name: UiConnectMsg
 * CodeView module: 4
 * CV address: segment 1 : 0x1D6D3
 * Code length: 0x59
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall UiConnectMsg(int a1, UINT a2, int flags)
{
  char szText[512]; // [esp+8h] [ebp-220h] BYREF
  char szTitle[32]; // [esp+208h] [ebp-20h] BYREF

  LocaleLoadString(a1: 0x3F2u, a2: szTitle, buffersize: 32);
  LocaleLoadString(a1: a2, a2: szText, buffersize: 512);
  return sub_1901E72C(pszTitle: (int)szTitle, flags);
}


// ==============================================================================
/*
 * EA: 0x1901E72C
 * CodeView symbol: UiConnectMsg
 * Demangled: UiConnectMsg
 * IDA name: sub_1901E72C
 * CodeView module: 4
 * CV address: segment 1 : 0x1D72C
 * Code length: 0x5F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall sub_1901E72C(int a1, const char *a2, const char *pszTitle, unsigned int flags)
{
  if ( a1 == 0 )
    return false;
  if ( *(_DWORD *)(a1 + 32) == 0 )
    return false;
  if ( *(_DWORD *)(a1 + 8) != 0 )
    return UiMessageBox(
             a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(a1 + 32),
             a2: *(HWND *)(a1 + 8),
             lpText: a2,
             lpCaption: pszTitle,
             uType: flags) == 1;
  return false;
}


// ==============================================================================
/*
 * EA: 0x1901E78B
 * CodeView symbol: UiUpgradeMessage
 * Demangled: UiUpgradeMessage
 * IDA name: ?UiUpgradeMessage@@YIHXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1D78B
 * Code length: 0x22
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl UiUpgradeMessage()
{
  UiHideConnectCancel();
  ProtectMinimize(a1: 0);
  return UiConnectMsg(a1: (int)&sgInterfacedata, a2: 0x3EBu, flags: 1);
}


// ==============================================================================
/*
 * EA: 0x1901E7AD
 * CodeView symbol: UiNotificationWaiting
 * Demangled: UiNotificationWaiting
 * IDA name: ?UiNotificationWaiting@@YIXXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1D7AD
 * Code length: 0x25
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl UiNotificationWaiting()
{
  if ( ghWndUiMainParent != nullptr )
    PostMessageA(hWnd: ghWndUiMainParent, Msg: 0x469u, wParam: 0, lParam: 0);
}


// ==============================================================================
/*
 * EA: 0x1901E7D2
 * CodeView symbol: UiNotification
 * Demangled: UiNotification
 * IDA name: ?UiNotification@@YIXXZ
 * CodeView module: 4
 * CV address: segment 1 : 0x1D7D2
 * Code length: 0x494
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl UiNotification()
{
  char Buffer[1024]; // [esp+Ch] [ebp-8C8h] BYREF
  char szFmt[1024]; // [esp+40Ch] [ebp-4C8h] BYREF
  char szTitle[32]; // [esp+80Ch] [ebp-C8h] BYREF
  char szName[16]; // [esp+82Ch] [ebp-A8h] BYREF
  char *pdwError; // [esp+83Ch] [ebp-98h]
  UINT nMsgNo; // [esp+840h] [ebp-94h]
  char szText[128]; // [esp+844h] [ebp-90h] BYREF
  char *pDword; // [esp+8C4h] [ebp-10h]
  char *paramdata; // [esp+8C8h] [ebp-Ch] BYREF
  unsigned int notifycode; // [esp+8CCh] [ebp-8h] BYREF
  unsigned int parambytes; // [esp+8D0h] [ebp-4h] BYREF

  while ( SrvGetUiNotification(a1: &notifycode, a2: (void **)&paramdata, &parambytes) != 0 )
  {
    switch ( notifycode )
    {
      case 1u:
        ChatAddChannel(a1: paramdata);
        break;
      case 2u:
        ChatDeleteChannel(a1: paramdata);
        break;
      case 3u:
        ChatJoinChannel(a1: (const char **)paramdata);
        break;
      case 4u:
        ChatAddUser(a1: (const char **)paramdata);
        break;
      case 5u:
        ChatDeleteUser(a1: (const char **)paramdata);
        break;
      case 6u:
        ChatReceiveMsg(a1: (struct _SNDISPLAYSTRINGREC *)paramdata);
        break;
      case 8u:
        pDword = paramdata;
        sgnDownloadProgress = *(_DWORD *)paramdata;
        if ( sghDlgProgress == nullptr )
        {
          LocaleLoadString(a1: 0x3F6u, a2: szText, buffersize: 128);
          sghDlgProgress = (HWND)UiModelessProgressDialog(
                                   a1: (struct _SNETUIDATA *)&sgInterfacedata,
                                   a2: szText,
                                   abortable: 1,
                                   progressfcn: GetDownloadProgress,
                                   callspersec: 20);
        }
        break;
      case 9u:
        if ( sghDlgProgress != nullptr )
        {
          DestroyWindow(hWnd: sghDlgProgress);
          sghDlgProgress = nullptr;
        }
        UiConnectMsg(a1: (int)&sgInterfacedata, a2: 0x3F5u, flags: 16);
        break;
      case 0xAu:
        if ( sghDlgProgress != nullptr )
        {
          DestroyWindow(hWnd: sghDlgProgress);
          sghDlgProgress = nullptr;
        }
        UiConnectMsg(a1: (int)&sgInterfacedata, a2: 0x3ECu, flags: 0);
        break;
      case 0xBu:
        pdwError = paramdata;
        nMsgNo = -1;
        switch ( *(_DWORD *)paramdata )
        {
          case 1:
            nMsgNo = 1009;
            break;
          case 2:
            nMsgNo = 1011;
            break;
          case 3:
            nMsgNo = 1012;
            break;
          case 4:
            nMsgNo = 1032;
            break;
          case 5:
            nMsgNo = 1072;
            break;
          case 6:
            nMsgNo = 1073;
            break;
          case 7:
            nMsgNo = 1074;
            break;
          case 8:
            LocaleLoadString(a1: 0x3F2u, a2: szTitle, buffersize: 32);
            LocaleLoadString(a1: 0x433u, a2: szFmt, buffersize: 1024);
            SrvGetCdKeyUser(a1: szName, a2: 0x10u);
            sprintf(Buffer, Format: szFmt, szName);
            UiHideConnectCancel();
            ProtectMinimize(a1: 0);
            sub_1901E72C(a1: (int)&sgInterfacedata, a2: Buffer, pszTitle: szTitle, flags: 0x10u);
            nMsgNo = -1;
            break;
          case 9:
            nMsgNo = 1076;
            break;
          default:
            nMsgNo = 1009;
            break;
        }
        if ( nMsgNo != -1 )
        {
          UiHideConnectCancel();
          ProtectMinimize(a1: 0);
          UiConnectMsg(a1: (int)&sgInterfacedata, a2: nMsgNo, flags: 16);
        }
        break;
      case 0xCu:
        ChatSetUserName();
        break;
      case 0xDu:
        gbConnectionLost = 1;
        break;
      case 0xEu:
        ChatChangeUserFlags(a1: (struct _SNCHANGEUSERFLAGSREC *)paramdata);
        break;
      case 0x10u:
        ChatChannelFull(a1: paramdata);
        break;
      case 0x11u:
        ChatChannelDoesNotExist(a1: paramdata);
        break;
      case 0x12u:
        ChatChannelRestricted(a1: paramdata);
        break;
      case 0x13u:
        ChatSquelchUser(a1: (const char **)paramdata);
        break;
      case 0x14u:
        ChatUnsquelchUser(a1: (const char **)paramdata);
        break;
      case 0x15u:
        UiConnectMsg(a1: (int)&sgInterfacedata, a2: 0x409u, flags: 16);
        gbConnectionSucks = 1;
        break;
      case 0x16u:
        AdSetInfo(a1: (struct _SNADINFOREC *)paramdata);
        break;
      case 0x17u:
        AdDisplay(a1: paramdata, a2: parambytes);
        break;
      case 0x18u:
        if ( dword_19044490 != nullptr )
          dword_19044490(
            a1: ghWndUiMainParent,
            a2: *(_DWORD *)paramdata,
            a3: *((_DWORD *)paramdata + 1),
            a4: *((_DWORD *)paramdata + 2));
        else
          MessageBoxA(
            hWnd: ghWndUiMainParent,
            lpText: *(LPCSTR *)paramdata,
            lpCaption: *((LPCSTR *)paramdata + 1),
            uType: *((_DWORD *)paramdata + 2));
        break;
      case 0x19u:
        IconLoad();
        break;
      case 0x1Au:
        TOSUpdate(a1: (unsigned __int8 *)paramdata);
        break;
      default:
        continue;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1901EC66
 * CodeView symbol: GetDownloadProgress
 * Demangled: GetDownloadProgress
 * IDA name: GetDownloadProgress
 * CodeView module: 4
 * CV address: segment 1 : 0x1DC66
 * Code length: 0xA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall GetDownloadProgress()
{
  return sgnDownloadProgress;
}


// ==============================================================================
/*
 * EA: 0x1901EC70
 * CodeView symbol: UiSelectGame
 * Demangled: UiSelectGame
 * IDA name: ?UiSelectGame@@YIHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAK@Z
 * CodeView module: 4
 * CV address: segment 1 : 0x1DC70
 * Code length: 0x95
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Ui.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Ui.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall UiSelectGame(
        unsigned int a1,
        struct _SNETPROGRAMDATA *a2,
        struct _SNETPLAYERDATA *playerdata,
        int (__stdcall **interfacedata)(unsigned int, unsigned int, void *, unsigned int, unsigned int *),
        struct _SNETVERSIONDATA *versiondata,
        unsigned int *playerid)
{
  HMODULE ModuleHandleA; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-34h]
  int (__stdcall *v9)(unsigned int, unsigned int, void *, unsigned int, unsigned int *); // [esp-Ch] [ebp-30h]
  _DWORD uiparams[6]; // [esp+8h] [ebp-1Ch] BYREF
  int v11; // [esp+20h] [ebp-4h]

  uiparams[0] = a1;
  uiparams[1] = a2;
  uiparams[2] = playerdata;
  uiparams[3] = interfacedata;
  uiparams[4] = versiondata;
  uiparams[5] = playerid;
  if ( interfacedata == nullptr )
    return false;
  ColorPrefInit(a1: interfacedata[11]);
  v9 = interfacedata[2];
  Dialog = LocaleGetDialog(a1: 0x6Bu);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  v11 = SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: v9, a4: BattleNetDialogProc, a5: uiparams);
  ColorPrefDestroy();
  return v11 == 1;
}

