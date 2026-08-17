/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 22
 */


// ==============================================================================
/*
 * EA: 0x19014FF0
 * CodeView symbol: SliderLoadArtwork
 * Demangled: SliderLoadArtwork
 * IDA name: ?SliderLoadArtwork@@YIXP6GHKKPAUtagPALETTEENTRY@@PAEKPAH22@Z@Z
 * CodeView module: 8
 * CV address: segment 1 : 0x13FF0
 * Code length: 0x7B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall SliderLoadArtwork(
        int (__stdcall *a1)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  int v1; // eax
  int tile; // [esp+4h] [ebp-4h]

  v1 = sgnSliderCnt++;
  if ( v1 == 0 )
  {
    for ( tile = 0; tile < 6; ++tile )
      UiLoadArtwork(
        a1,
        a2: nullptr,
        hWndParent: nullptr,
        artid: SLIDER_ARTID[tile],
        controltype: byte_1904321C,
        controlstyle: 0xFFFFFFFF,
        usageflags: 1,
        loadpalette: 0,
        prepfadein: 0,
        a10: &(&sgSliderBmp)[tile],
        a11: &sgSliderSize + tile);
  }
}


// ==============================================================================
/*
 * EA: 0x1901506B
 * CodeView symbol: SliderDestroyArtwork
 * Demangled: SliderDestroyArtwork
 * IDA name: ?SliderDestroyArtwork@@YIXXZ
 * CodeView module: 8
 * CV address: segment 1 : 0x1406B
 * Code length: 0x71
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl SliderDestroyArtwork()
{
  int tile; // [esp+0h] [ebp-4h]

  if ( --sgnSliderCnt == 0 )
  {
    for ( tile = 0; tile < 6; ++tile )
    {
      if ( (&sgSliderBmp)[tile] != nullptr )
      {
        SMemFree(a1: (&sgSliderBmp)[tile], a2: aSliderCpp, a3: 520, a4: 0);
        (&sgSliderBmp)[tile] = nullptr;
      }
    }
  }
}


// ==============================================================================
/*
 * EA: 0x190150DC
 * CodeView symbol: SliderRegisterClass
 * Demangled: SliderRegisterClass
 * IDA name: ?SliderRegisterClass@@YIXXZ
 * CodeView module: 8
 * CV address: segment 1 : 0x140DC
 * Code length: 0x52
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl SliderRegisterClass()
{
  WNDCLASSA wndclass; // [esp+0h] [ebp-28h] BYREF

  memset(a1: &wndclass, Val: 0, Size: sizeof(wndclass));
  wndclass.style = 0x4000;
  wndclass.lpfnWndProc = (WNDPROC)StormSliderWndProc;
  wndclass.hInstance = global_hinstance;
  wndclass.hCursor = LoadCursorA(hInstance: nullptr, lpCursorName: (LPCSTR)0x7F00);
  wndclass.lpszClassName = aStormslider;
  RegisterClassA(lpWndClass: &wndclass);
}


// ==============================================================================
/*
 * EA: 0x1901512E
 * CodeView symbol: StormSliderWndProc
 * Demangled: StormSliderWndProc
 * IDA name: StormSliderWndProc
 * CodeView module: 8
 * CV address: segment 1 : 0x1412E
 * Code length: 0x2F0
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall StormSliderWndProc(HWND window, UINT message, int wparam, LPARAM lparam)
{
  int result; // eax
  HWND FrameWindow; // eax
  struct tagPAINTSTRUCT ps; // [esp+4h] [ebp-44h] BYREF
  HDC dc; // [esp+44h] [ebp-4h]

  if ( message <= 0x105 )
  {
    if ( message >= 0x104 )
    {
      FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
      SendMessageA(hWnd: FrameWindow, Msg: message, wParam: wparam, lParam: lparam);
    }
    else
    {
      if ( message <= 0xA )
      {
        if ( message == 10 )
        {
          SliderSetEnable(a1: window, a2: wparam);
          return 0;
        }
        else
        {
          switch ( message )
          {
            case 1u:
              if ( SliderCreate(a1: window) != 0 )
                SliderDraw(a1: window);
              result = 0;
              break;
            case 2u:
              SliderDestroy(a1: window);
              result = 0;
              break;
            case 7u:
              SliderSetFocus(oldwindow: wparam);
              return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: lparam);
            case 8u:
              SliderSetFocus(oldwindow: 0);
              return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: lparam);
            default:
              return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: lparam);
          }
        }
        return result;
      }
      switch ( message )
      {
        case 0xFu:
          dc = BeginPaint(hWnd: window, lpPaint: &ps);
          SDlgDrawBitmap(a1: window, a2: 1, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0);
          EndPaint(hWnd: window, lpPaint: &ps);
          return 0;
        case 0x87u:
          return 1;
        case 0x100u:
          SliderKeyDown(a1: window, a2: wparam);
          return 0;
        default:
          break;
      }
    }
    return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: lparam);
  }
  if ( message <= 0x400 )
  {
    switch ( message )
    {
      case 0x400u:
        return SliderGetPosition(a1: window);
      case 0x200u:
        if ( GetCapture() == window )
        {
          SliderUpdateTrack(a1: window, a2: lparam);
          return 0;
        }
        break;
      case 0x201u:
        SliderButtonDown(yposition: HIWORD(lparam));
        return 0;
      default:
        if ( message == 514 && GetCapture() == window )
        {
          SliderButtonUp(a1: window, a2: lparam);
          return 0;
        }
        break;
    }
    return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: lparam);
  }
  switch ( message )
  {
    case 0x405u:
      SliderSetPosition(a1: window, a2: lparam);
      if ( wparam != 0 )
      {
        SliderDraw(a1: window);
        InvalidateRect(hWnd: window, lpRect: nullptr, bErase: false);
      }
      return 0;
    case 0x406u:
      SliderSetRange(maximum: HIWORD(lparam));
      if ( wparam != 0 )
      {
        SliderDraw(a1: window);
        InvalidateRect(hWnd: window, lpRect: nullptr, bErase: false);
      }
      return 0;
    case 0x415u:
      SliderSetPageSize(a1: window, a2: lparam);
      return 0;
    default:
      return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: lparam);
  }
}


// ==============================================================================
/*
 * EA: 0x1901541E
 * CodeView symbol: SliderDestroy
 * Demangled: SliderDestroy
 * IDA name: SliderDestroy
 * CodeView module: 8
 * CV address: segment 1 : 0x1441E
 * Code length: 0x79
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall SliderDestroy(HWND this)
{
  HANDLE sliderinfo; // [esp+4h] [ebp-Ch]
  HANDLE slidersize; // [esp+8h] [ebp-8h]
  HANDLE sliderbmp; // [esp+Ch] [ebp-4h]

  sliderinfo = RemovePropA(hWnd: this, lpString: aSliderSliderin);
  SMemFree(a1: sliderinfo, a2: aSliderCpp_0, a3: 58, a4: 0);
  sliderbmp = RemovePropA(hWnd: this, lpString: aSliderBitmap);
  SMemFree(a1: sliderbmp, a2: aSliderCpp_1, a3: 61, a4: 0);
  slidersize = RemovePropA(hWnd: this, lpString: aSliderBitmapsi);
  return SMemFree(a1: slidersize, a2: aSliderCpp_2, a3: 64, a4: 0);
}


// ==============================================================================
/*
 * EA: 0x19015497
 * CodeView symbol: SliderCreate
 * Demangled: SliderCreate
 * IDA name: SliderCreate
 * CodeView module: 8
 * CV address: segment 1 : 0x14497
 * Code length: 0x1AD
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall SliderCreate(HWND this)
{
  LONG *bmpsize; // [esp+4h] [ebp-1Ch]
  struct tagRECT clientrect; // [esp+8h] [ebp-18h] BYREF
  char *sliderinfo; // [esp+18h] [ebp-8h]
  HANDLE sliderbmp; // [esp+1Ch] [ebp-4h]

  GetClientRect(hWnd: this, lpRect: &clientrect);
  clientrect.right -= clientrect.right % 4;
  if ( clientrect.right < dword_190431E0 + sgSliderSize.cx )
    clientrect.right = dword_190431E0 + sgSliderSize.cx;
  clientrect.bottom = sgSliderSize.cy;
  sliderbmp = (HANDLE)SMemAlloc(a1: sgSliderSize.cy * clientrect.right, a2: aSliderCpp_3, a3: 75, a4: 0);
  SetPropA(hWnd: this, lpString: aSliderBitmap_0, hData: sliderbmp);
  SDlgSetBitmapI(
    a1: this,
    a2: 0,
    a3: 0,
    a4: -1,
    a5: 1,
    a6: sliderbmp,
    a7: 0,
    a8: clientrect.right,
    a9: clientrect.bottom,
    a10: -1);
  sliderinfo = (char *)SMemAlloc(a1: 44, a2: aSliderCpp_4, a3: 88, a4: 8);
  *(struct tagRECT *)(sliderinfo + 28) = clientrect;
  *((_DWORD *)sliderinfo + 7) += sgSliderSize.cx - dword_190431E8 / 2;
  *((_DWORD *)sliderinfo + 9) -= dword_190431E0 - dword_190431E8 / 2;
  *((_DWORD *)sliderinfo + 8) += (dword_190431DC - dword_190431EC) / 2;
  *((_DWORD *)sliderinfo + 10) -= (dword_190431DC - dword_190431EC) / 2;
  *((_DWORD *)sliderinfo + 2) = 1;
  SetPropA(hWnd: this, lpString: aSliderSliderin_0, hData: sliderinfo);
  bmpsize = (LONG *)SMemAlloc(a1: 8, a2: aSliderCpp_5, a3: 99, a4: 0);
  *bmpsize = clientrect.right;
  bmpsize[1] = clientrect.bottom;
  SetPropA(hWnd: this, lpString: aSliderBitmapsi_0, hData: bmpsize);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19015644
 * CodeView symbol: SliderDraw
 * Demangled: SliderDraw
 * IDA name: SliderDraw
 * CodeView module: 8
 * CV address: segment 1 : 0x14644
 * Code length: 0x1EE
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall SliderDraw(HWND this)
{
  _DWORD *result; // eax
  _DWORD *dstsize; // [esp+4h] [ebp-48h]
  struct tagRECT clientrect; // [esp+8h] [ebp-44h] BYREF
  _DWORD srcrect[4]; // [esp+18h] [ebp-34h] BYREF
  _DWORD *sliderinfo; // [esp+28h] [ebp-24h]
  _DWORD dstrect[4]; // [esp+2Ch] [ebp-20h] BYREF
  int thumbtile; // [esp+3Ch] [ebp-10h]
  char *dstbmp; // [esp+40h] [ebp-Ch]
  int thumboffsety; // [esp+44h] [ebp-8h]
  int thumboffsetx; // [esp+48h] [ebp-4h]

  sliderinfo = GetPropA(hWnd: this, lpString: aSliderSliderin_1);
  dstbmp = (char *)GetPropA(hWnd: this, lpString: aSliderBitmap_1);
  result = GetPropA(hWnd: this, lpString: aSliderBitmapsi_1);
  dstsize = result;
  if ( sliderinfo != nullptr && dstbmp != nullptr )
  {
    GetClientRect(hWnd: this, lpRect: &clientrect);
    SBltROP3(
      a1: dstbmp,
      a2: sgSliderBmp,
      a3: sgSliderSize.cx,
      a4: sgSliderSize.cy,
      a5: *dstsize,
      a6: sgSliderSize.cx,
      a7: 0,
      a8: 13369376);
    srcrect[0] = 0;
    srcrect[1] = 0;
    srcrect[2] = dword_190431D8;
    srcrect[3] = dword_190431DC;
    dstrect[0] = 0;
    dstrect[1] = 0;
    dstrect[2] = sliderinfo[9] - dword_190431E8;
    dstrect[3] = dword_190431E4;
    SBltROP3Tiled(
      a1: &dstbmp[sgSliderSize.cx],
      a2: dstrect,
      a3: *dstsize,
      a4: dword_19043204,
      a5: srcrect,
      a6: dword_190431D8,
      a7: sgSliderSize.cx,
      a8: 0,
      a9: 0,
      a10: 13369376);
    SBltROP3(
      a1: &dstbmp[clientrect.right - dword_190431E0],
      a2: dword_19043208,
      a3: dword_190431E0,
      a4: dword_190431E4,
      a5: *dstsize,
      a6: dword_190431E0,
      a7: 0,
      a8: 13369376);
    thumboffsetx = sliderinfo[4] + sliderinfo[7];
    thumboffsety = sliderinfo[8];
    thumbtile = 3;
    if ( sliderinfo[5] != 0 )
    {
      thumbtile = 4;
    }
    else if ( sliderinfo[6] != 0 )
    {
      thumbtile = 5;
    }
    return (_DWORD *)SBltROP3(
                       a1: &dstbmp[*dstsize * thumboffsety + thumboffsetx - *(&sgSliderSize.cx + 2 * thumbtile) / 2],
                       a2: (&sgSliderBmp)[thumbtile],
                       a3: *(&sgSliderSize.cx + 2 * thumbtile),
                       a4: *(&sgSliderSize.cy + 2 * thumbtile),
                       a5: *dstsize,
                       a6: *(&sgSliderSize.cx + 2 * thumbtile),
                       a7: 0,
                       a8: 13369376);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015832
 * CodeView symbol: SliderSetRange
 * Demangled: SliderSetRange
 * IDA name: SliderSetRange
 * CodeView module: 8
 * CV address: segment 1 : 0x14832
 * Code length: 0x73
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__fastcall SliderSetRange(HWND a1, int a2, int maximum)
{
  _DWORD *result; // eax

  result = GetPropA(hWnd: a1, lpString: aSliderSliderin_2);
  if ( result != nullptr )
  {
    *result = a2;
    result[1] = maximum;
    if ( result[3] < a2 )
      result[3] = a2;
    if ( result[3] > maximum )
      result[3] = maximum;
    return (_DWORD *)SliderSetTrack(a1: result, a2: result[3]);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190158A5
 * CodeView symbol: SliderSetTrack
 * Demangled: SliderSetTrack
 * IDA name: SliderSetTrack
 * CodeView module: 8
 * CV address: segment 1 : 0x148A5
 * Code length: 0x4A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SliderSetTrack(_DWORD *a1, int a2)
{
  int result; // eax
  int range; // [esp+8h] [ebp-4h]

  range = a1[1] - *a1;
  if ( range != 0 )
  {
    result = a2 * (a1[9] - a1[7]) / range;
    a1[4] = result;
  }
  else
  {
    a1[4] = 0;
    return (int)a1;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190158EF
 * CodeView symbol: SliderSetPageSize
 * Demangled: SliderSetPageSize
 * IDA name: SliderSetPageSize
 * CodeView module: 8
 * CV address: segment 1 : 0x148EF
 * Code length: 0x33
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__fastcall SliderSetPageSize(HWND a1, int a2)
{
  _DWORD *result; // eax

  result = GetPropA(hWnd: a1, lpString: aSliderSliderin_3);
  if ( result != nullptr )
    result[2] = a2;
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015922
 * CodeView symbol: SliderSetPosition
 * Demangled: SliderSetPosition
 * IDA name: SliderSetPosition
 * CodeView module: 8
 * CV address: segment 1 : 0x14922
 * Code length: 0x67
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__fastcall SliderSetPosition(HWND a1, int a2)
{
  int *result; // eax

  result = (int *)GetPropA(hWnd: a1, lpString: aSliderSliderin_4);
  if ( result != nullptr )
  {
    if ( a2 < *result )
      a2 = *result;
    if ( a2 > result[1] )
      a2 = result[1];
    result[3] = a2;
    return (int *)SliderSetTrack(a1: result, a2: result[3]);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015989
 * CodeView symbol: SliderGetPosition
 * Demangled: SliderGetPosition
 * IDA name: SliderGetPosition
 * CodeView module: 8
 * CV address: segment 1 : 0x14989
 * Code length: 0x2F
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall SliderGetPosition(HWND this)
{
  _DWORD *sliderinfo; // [esp+4h] [ebp-4h]

  sliderinfo = GetPropA(hWnd: this, lpString: aSliderSliderin_5);
  if ( sliderinfo != nullptr )
    return sliderinfo[3];
  else
    return 0;
}


// ==============================================================================
/*
 * EA: 0x190159B8
 * CodeView symbol: SliderButtonUp
 * Demangled: SliderButtonUp
 * IDA name: SliderButtonUp
 * CodeView module: 8
 * CV address: segment 1 : 0x149B8
 * Code length: 0x62
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__fastcall SliderButtonUp(HWND a1, __int16 a2)
{
  _DWORD *result; // eax
  _DWORD *sliderinfo; // [esp+8h] [ebp-4h]

  ReleaseCapture();
  result = GetPropA(hWnd: a1, lpString: aSliderSliderin_6);
  sliderinfo = result;
  if ( result != nullptr )
  {
    SliderUpdatePosition(a1, a2: 4, a3: a2);
    SliderSetTrack(a1: sliderinfo, a2: sliderinfo[3]);
    SliderDraw(this: a1);
    return (_DWORD *)InvalidateRect(hWnd: a1, lpRect: nullptr, bErase: false);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015A1A
 * CodeView symbol: SliderUpdatePosition
 * Demangled: SliderUpdatePosition
 * IDA name: SliderUpdatePosition
 * CodeView module: 8
 * CV address: segment 1 : 0x14A1A
 * Code length: 0xA1
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__fastcall SliderUpdatePosition(HWND a1, unsigned __int16 a2)
{
  _DWORD *result; // eax
  HWND Parent; // eax
  int position; // [esp+Ch] [ebp-8h]
  int width; // [esp+10h] [ebp-4h]

  result = GetPropA(hWnd: a1, lpString: aSliderSliderin_7);
  if ( result != nullptr )
  {
    width = result[9] - result[7];
    position = 0;
    if ( width != 0 )
      position = (result[4] * (result[1] - *result) + width / 2) / width;
    if ( result[3] != position )
    {
      result[3] = position;
      Parent = GetParent(hWnd: a1);
      return (_DWORD *)SendMessageA(hWnd: Parent, Msg: 0x114u, wParam: a2, lParam: (LPARAM)a1);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015ABB
 * CodeView symbol: SliderUpdateTrack
 * Demangled: SliderUpdateTrack
 * IDA name: SliderUpdateTrack
 * CodeView module: 8
 * CV address: segment 1 : 0x14ABB
 * Code length: 0xC4
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__fastcall SliderUpdateTrack(HWND a1, __int16 a2)
{
  _DWORD *result; // eax
  int v3; // [esp+0h] [ebp-18h]
  int v4; // [esp+4h] [ebp-14h]
  int v5; // [esp+8h] [ebp-10h]

  result = GetPropA(hWnd: a1, lpString: aSliderSliderin_8);
  if ( result != nullptr )
  {
    if ( result[7] <= a2 )
      v5 = a2;
    else
      v5 = result[7];
    if ( result[9] >= v5 )
    {
      if ( result[7] <= a2 )
        v3 = a2;
      else
        v3 = result[7];
      v4 = v3;
    }
    else
    {
      v4 = result[9];
    }
    result[4] = v4;
    result[4] -= result[7];
    SliderUpdatePosition(a1, a2: 5u);
    SliderDraw(this: a1);
    return (_DWORD *)InvalidateRect(hWnd: a1, lpRect: nullptr, bErase: false);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015B7F
 * CodeView symbol: SliderButtonDown
 * Demangled: SliderButtonDown
 * IDA name: SliderButtonDown
 * CodeView module: 8
 * CV address: segment 1 : 0x14B7F
 * Code length: 0x104
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND __fastcall SliderButtonDown(HWND a1, LONG a2, unsigned int yposition)
{
  HWND result; // eax
  RECT thumbrect; // [esp+10h] [ebp-18h] BYREF
  int sliderinfo; // [esp+20h] [ebp-8h]
  int range; // [esp+24h] [ebp-4h]

  result = (HWND)GetPropA(hWnd: a1, lpString: aSliderSliderin_9);
  sliderinfo = (int)result;
  if ( result != nullptr && *(_DWORD *)(sliderinfo + 20) == 0 )
  {
    thumbrect = *(RECT *)(sliderinfo + 28);
    range = *(_DWORD *)(sliderinfo + 4) - *(_DWORD *)sliderinfo;
    if ( range != 0 )
      thumbrect.left += (*(_DWORD *)(sliderinfo + 12) - *(_DWORD *)sliderinfo)
                      * (*(_DWORD *)(sliderinfo + 36) - *(_DWORD *)(sliderinfo + 28))
                      / range;
    thumbrect.left -= dword_190431E8 / 2;
    thumbrect.right = dword_190431E8 + thumbrect.left;
    result = (HWND)PtInRect(lprc: &thumbrect, pt: (POINT)__PAIR64__(yposition, a2));
    if ( result != nullptr )
    {
      return SetCapture(hWnd: a1);
    }
    else if ( a2 >= thumbrect.left )
    {
      if ( a2 > thumbrect.right )
        return (HWND)SliderPlus(a1);
    }
    else
    {
      return (HWND)SliderMinus(a1);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015C83
 * CodeView symbol: SliderMinus
 * Demangled: SliderMinus
 * IDA name: SliderMinus
 * CodeView module: 8
 * CV address: segment 1 : 0x14C83
 * Code length: 0x87
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall SliderMinus(HWND this)
{
  int *result; // eax
  int v2; // [esp+0h] [ebp-10h]

  result = (int *)GetPropA(hWnd: this, lpString: aSliderSliderin_10);
  if ( result != nullptr )
  {
    if ( result[3] - result[2] <= *result )
      v2 = *result;
    else
      v2 = result[3] - result[2];
    SliderSetTrack(a1: result, a2: v2);
    SliderUpdatePosition(a1: this, a2: 4u);
    SliderDraw(this);
    return (int *)InvalidateRect(hWnd: this, lpRect: nullptr, bErase: false);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015D0A
 * CodeView symbol: SliderPlus
 * Demangled: SliderPlus
 * IDA name: SliderPlus
 * CodeView module: 8
 * CV address: segment 1 : 0x14D0A
 * Code length: 0x89
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall SliderPlus(HWND this)
{
  _DWORD *result; // eax
  int v2; // [esp+0h] [ebp-10h]

  result = GetPropA(hWnd: this, lpString: aSliderSliderin_11);
  if ( result != nullptr )
  {
    if ( result[2] + result[3] >= result[1] )
      v2 = result[1];
    else
      v2 = result[2] + result[3];
    SliderSetTrack(a1: result, a2: v2);
    SliderUpdatePosition(a1: this, a2: 4u);
    SliderDraw(this);
    return (_DWORD *)InvalidateRect(hWnd: this, lpRect: nullptr, bErase: false);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015D93
 * CodeView symbol: SliderSetFocus
 * Demangled: SliderSetFocus
 * IDA name: SliderSetFocus
 * CodeView module: 8
 * CV address: segment 1 : 0x14D93
 * Code length: 0x74
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND __fastcall SliderSetFocus(HWND a1, int a2, HWND oldwindow)
{
  HWND result; // eax
  HWND Parent; // eax
  HWND NextDlgTabItem; // eax

  result = (HWND)GetPropA(hWnd: a1, lpString: aSliderSliderin_12);
  if ( result != nullptr )
  {
    if ( *((_DWORD *)result + 5) != 0 )
    {
      Parent = GetParent(hWnd: a1);
      NextDlgTabItem = GetNextDlgTabItem(hDlg: Parent, hCtl: oldwindow, bPrevious: true);
      return SetFocus(hWnd: NextDlgTabItem);
    }
    else
    {
      *((_DWORD *)result + 6) = a2;
      SliderDraw(this: a1);
      return (HWND)InvalidateRect(hWnd: a1, lpRect: nullptr, bErase: false);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015E07
 * CodeView symbol: SliderSetEnable
 * Demangled: SliderSetEnable
 * IDA name: SliderSetEnable
 * CodeView module: 8
 * CV address: segment 1 : 0x14E07
 * Code length: 0x70
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__fastcall SliderSetEnable(HWND a1, int a2)
{
  _DWORD *result; // eax
  int v3; // [esp+0h] [ebp-10h]

  result = GetPropA(hWnd: a1, lpString: aSliderSliderin_13);
  if ( result != nullptr )
  {
    if ( a2 != 0 )
      v3 = result[6];
    else
      v3 = 0;
    result[6] = v3;
    result[5] = a2 == 0;
    SliderDraw(this: a1);
    return (_DWORD *)InvalidateRect(hWnd: a1, lpRect: nullptr, bErase: false);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015E77
 * CodeView symbol: SliderKeyDown
 * Demangled: SliderKeyDown
 * IDA name: SliderKeyDown
 * CodeView module: 8
 * CV address: segment 1 : 0x14E77
 * Code length: 0xC9
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__fastcall SliderKeyDown(HWND a1, int a2)
{
  int *result; // eax

  result = (int *)GetPropA(hWnd: a1, lpString: aSliderSliderin_14);
  if ( result != nullptr && result[5] == 0 )
  {
    switch ( a2 )
    {
      case '!':
      case '%':
      case '&':
        return SliderMinus(this: a1);
      case '"':
      case '\'':
      case '(':
        return SliderPlus(this: a1);
      case '#':
        result[4] = result[9] - result[7];
        goto LABEL_8;
      case '$':
        result[4] = 0;
        goto LABEL_8;
      default:
LABEL_8:
        SliderUpdatePosition(a1, a2: 4u);
        SliderDraw(this: a1);
        result = (int *)InvalidateRect(hWnd: a1, lpRect: nullptr, bErase: false);
        break;
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19015F40
 * CodeView symbol: SliderUnregisterClass
 * Demangled: SliderUnregisterClass
 * IDA name: ?SliderUnregisterClass@@YIXXZ
 * CodeView module: 8
 * CV address: segment 1 : 0x14F40
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Slider.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Slider.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl SliderUnregisterClass()
{
  UnregisterClassA(lpClassName: aStormslider_0, hInstance: global_hinstance);
}

