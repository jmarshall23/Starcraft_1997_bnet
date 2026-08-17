/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 22
 */


// ==============================================================================
/*
 * EA: 0x19013570
 * CodeView symbol: ScrollbarLoadArtwork
 * Demangled: ScrollbarLoadArtwork
 * IDA name: ?ScrollbarLoadArtwork@@YIXP6GHKKPAUtagPALETTEENTRY@@PAEKPAH22@Z@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x12570
 * Code length: 0xBF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ScrollbarLoadArtwork(
        int (__stdcall *a1)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  int v1; // eax

  v1 = sgnCreateCnt++;
  if ( v1 == 0 )
  {
    UiLoadArtwork(
      a1,
      a2: nullptr,
      hWndParent: nullptr,
      artid: 0xDu,
      controltype: byte_190431C4,
      controlstyle: 0xFFFFFFFF,
      usageflags: 1,
      loadpalette: 0,
      prepfadein: 0,
      a10: &sgBmpArrows,
      a11: &sgSizeArrows);
    UiLoadArtwork(
      a1,
      a2: nullptr,
      hWndParent: nullptr,
      artid: 0xEu,
      controltype: byte_190431C8,
      controlstyle: 0xFFFFFFFF,
      usageflags: 1,
      loadpalette: 0,
      prepfadein: 0,
      a10: &sgBmpThumb,
      a11: &sgSizeThumb);
    UiLoadArtwork(
      a1,
      a2: nullptr,
      hWndParent: nullptr,
      artid: 0xFu,
      controltype: byte_190431CC,
      controlstyle: 0xFFFFFFFF,
      usageflags: 1,
      loadpalette: 0,
      prepfadein: 0,
      a10: &sgBmpBar,
      a11: &sgSizeBar);
    sgnThumbHgt = sgSizeThumb.cy;
    sgnArrowHgt = sgSizeArrows.cy / 4;
    sgnBarHgt = sgSizeBar.cy;
  }
}


// ==============================================================================
/*
 * EA: 0x1901362F
 * CodeView symbol: ScrollbarDestroyArtwork
 * Demangled: ScrollbarDestroyArtwork
 * IDA name: ?ScrollbarDestroyArtwork@@YIXXZ
 * CodeView module: 10
 * CV address: segment 1 : 0x1262F
 * Code length: 0xA0
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ScrollbarDestroyArtwork()
{
  if ( --sgnCreateCnt == 0 )
  {
    if ( sgBmpArrows != nullptr )
    {
      SMemFree(a1: sgBmpArrows, a2: aScrllbarCpp, a3: 140, a4: 0);
      sgBmpArrows = nullptr;
    }
    if ( sgBmpThumb != nullptr )
    {
      SMemFree(a1: sgBmpThumb, a2: aScrllbarCpp_0, a3: 145, a4: 0);
      sgBmpThumb = nullptr;
    }
    if ( sgBmpBar != nullptr )
    {
      SMemFree(a1: sgBmpBar, a2: aScrllbarCpp_1, a3: 150, a4: 0);
      sgBmpBar = nullptr;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x190136CF
 * CodeView symbol: MouseToScrollPos
 * Demangled: MouseToScrollPos
 * IDA name: ?MouseToScrollPos@@YIHPAUtagPOINT@@PAU_ScrollRects@@PAUtagSCROLLINFO@@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x126CF
 * Code length: 0xAA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall MouseToScrollPos(struct tagPOINT *a1, struct _ScrollRects *a2, struct tagSCROLLINFO *pScrollInfo)
{
  int nScrollPos; // [esp+8h] [ebp-10h]
  LONG y; // [esp+Ch] [ebp-Ch]
  int nScrollRange; // [esp+10h] [ebp-8h]

  nScrollRange = pScrollInfo->nMax - pScrollInfo->nMin;
  if ( nScrollRange == 0 )
    return 0;
  y = a1->y;
  if ( y < *((_DWORD *)a2 + 13) )
    y = *((_DWORD *)a2 + 13);
  nScrollPos = pScrollInfo->nMin
             + nScrollRange * (y - *((_DWORD *)a2 + 13)) / (*((_DWORD *)a2 + 15) - *((_DWORD *)a2 + 13) - sgnThumbHgt);
  if ( nScrollPos < pScrollInfo->nMin )
    nScrollPos = pScrollInfo->nMin;
  if ( nScrollPos > pScrollInfo->nMax )
    return pScrollInfo->nMax;
  return nScrollPos;
}


// ==============================================================================
/*
 * EA: 0x19013779
 * CodeView symbol: ScrollDragThumb
 * Demangled: ScrollDragThumb
 * IDA name: ?ScrollDragThumb@@YIXPAUHWND__@@PAUtagPOINT@@PAU_ScrollRects@@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x12779
 * Code length: 0xA0
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ScrollDragThumb(HWND a1, struct tagPOINT *a2, struct tagRECT *pTRects)
{
  LONG nTarget; // [esp+8h] [ebp-4h]

  a2->y -= sgnThumbHgt / 2;
  if ( a2->y >= pTRects[3].top )
  {
    if ( a2->y <= pTRects[3].bottom - sgnThumbHgt )
      nTarget = a2->y;
    else
      nTarget = pTRects[3].bottom - sgnThumbHgt;
  }
  else
  {
    nTarget = pTRects[3].top;
  }
  OffsetRect(lprc: pTRects + 2, dx: 0, dy: nTarget - pTRects[2].top);
  ScrollbarDraw(a1);
  InvalidateRect(hWnd: a1, lpRect: nullptr, bErase: false);
}


// ==============================================================================
/*
 * EA: 0x19013819
 * CodeView symbol: ScrollbarDraw
 * Demangled: ScrollbarDraw
 * IDA name: ScrollbarDraw
 * CodeView module: 10
 * CV address: segment 1 : 0x12819
 * Code length: 0x218
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall ScrollbarDraw(HWND this)
{
  _DWORD *result; // eax
  LONG cy; // [esp+4h] [ebp-40h]
  _DWORD *pTBmp; // [esp+Ch] [ebp-38h]
  _DWORD *pTRects; // [esp+10h] [ebp-34h]
  struct tagRECT rSrc; // [esp+14h] [ebp-30h] BYREF
  int r; // [esp+24h] [ebp-20h]
  int v8; // [esp+28h] [ebp-1Ch]
  int v9; // [esp+2Ch] [ebp-18h]
  int v10; // [esp+30h] [ebp-14h]
  __int128 rDst; // [esp+34h] [ebp-10h] BYREF

  pTBmp = GetPropA(hWnd: this, lpString: aScrollbitmap);
  result = GetPropA(hWnd: this, lpString: aScrollrects);
  pTRects = result;
  if ( pTBmp != nullptr && result != nullptr )
  {
    r = *result;
    v8 = result[1];
    v9 = result[2];
    v10 = result[3];
    SBltROP3(
      a1: r + pTBmp[1] * v8 + *pTBmp,
      a2: sgBmpArrows,
      a3: v9 - r,
      a4: v10 - v8,
      a5: pTBmp[1],
      a6: sgSizeArrows.cx,
      a7: 0,
      a8: 13369376);
    r = pTRects[4];
    v8 = pTRects[5];
    v9 = pTRects[6];
    v10 = pTRects[7];
    SBltROP3(
      a1: r + pTBmp[1] * v8 + *pTBmp,
      a2: &sgBmpArrows[sgSizeArrows.cx * sgnArrowHgt],
      a3: v9 - r,
      a4: v10 - v8,
      a5: pTBmp[1],
      a6: sgSizeArrows.cx,
      a7: 0,
      a8: 13369376);
    SetRect(lprc: &rSrc, xLeft: 0, yTop: 0, xRight: pTBmp[1], yBottom: sgSizeBar.cy);
    rDst = *((_OWORD *)pTRects + 3);
    SBltROP3Tiled(
      a1: *pTBmp,
      a2: &rDst,
      a3: pTBmp[1],
      a4: sgBmpBar,
      a5: &rSrc,
      a6: sgSizeBar.cx,
      a7: 0,
      a8: 0,
      a9: 0,
      a10: 13369376);
    r = pTRects[8];
    v8 = pTRects[9];
    v9 = pTRects[10];
    v10 = pTRects[11];
    if ( v10 - v8 >= sgSizeThumb.cy )
      cy = sgSizeThumb.cy;
    else
      cy = v10 - v8;
    if ( v9 - r >= sgSizeThumb.cx )
      return (_DWORD *)SBltROP3(
                         a1: r + pTBmp[1] * v8 + *pTBmp,
                         a2: sgBmpThumb,
                         a3: sgSizeThumb.cx,
                         a4: cy,
                         a5: pTBmp[1],
                         a6: sgSizeThumb.cx,
                         a7: 0,
                         a8: 13369376);
    else
      return (_DWORD *)SBltROP3(
                         a1: r + pTBmp[1] * v8 + *pTBmp,
                         a2: sgBmpThumb,
                         a3: v9 - r,
                         a4: cy,
                         a5: pTBmp[1],
                         a6: sgSizeThumb.cx,
                         a7: 0,
                         a8: 13369376);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19013A31
 * CodeView symbol: ScrollbarGetWidth
 * Demangled: ScrollbarGetWidth
 * IDA name: ?ScrollbarGetWidth@@YIHXZ
 * CodeView module: 10
 * CV address: segment 1 : 0x12A31
 * Code length: 0xA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LONG __cdecl ScrollbarGetWidth()
{
  return sgSizeArrows.cx;
}


// ==============================================================================
/*
 * EA: 0x19013A3B
 * CodeView symbol: ScrollbarLink
 * Demangled: ScrollbarLink
 * IDA name: ?ScrollbarLink@@YIXPAUHWND__@@0@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x12A3B
 * Code length: 0x3B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ScrollbarLink(HWND a1, HWND a2)
{
  if ( a1 != nullptr )
  {
    SendMessageA(hWnd: a2, Msg: 0x46Au, wParam: 0, lParam: (LPARAM)a1);
    SetWindowLongA(hWnd: a1, nIndex: -21, dwNewLong: (LONG)a2);
  }
}


// ==============================================================================
/*
 * EA: 0x19013A76
 * CodeView symbol: ListUpdateScrollbar
 * Demangled: ListUpdateScrollbar
 * IDA name: ?ListUpdateScrollbar@@YIXPAUHWND__@@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x12A76
 * Code length: 0x10A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ListUpdateScrollbar(HWND a1)
{
  LRESULT nPixHt; // [esp+8h] [ebp-40h]
  LPARAM lParam[7]; // [esp+Ch] [ebp-3Ch] BYREF
  LRESULT nItems; // [esp+28h] [ebp-20h]
  LPARAM max; // [esp+2Ch] [ebp-1Ch]
  int nItemsInWindow; // [esp+30h] [ebp-18h]
  struct tagRECT r; // [esp+34h] [ebp-14h] BYREF
  HWND hWndScroll; // [esp+44h] [ebp-4h]

  hWndScroll = (HWND)GetWindowLongA(hWnd: a1, nIndex: -21);
  if ( hWndScroll != nullptr )
  {
    GetClientRect(hWnd: a1, lpRect: &r);
    nPixHt = SendMessageA(hWnd: a1, Msg: 0x1A1u, wParam: 0, lParam: 0);
    nItems = SendMessageA(hWnd: a1, Msg: 0x18Bu, wParam: 0, lParam: 0);
    if ( nPixHt != 0 )
    {
      nItemsInWindow = r.bottom / nPixHt;
      GetClientRect(hWnd: a1, lpRect: &r);
      max = 0;
      if ( nPixHt == -1 || nItems == -1 )
      {
        max = 0;
      }
      else
      {
        max = nItems - nItemsInWindow;
        if ( nItems - nItemsInWindow < 0 )
          max = 0;
      }
      lParam[0] = 28;
      lParam[1] = 7;
      lParam[5] = SendMessageA(hWnd: a1, Msg: 0x18Eu, wParam: 0, lParam: 0);
      lParam[2] = 0;
      lParam[3] = max;
      lParam[4] = nItemsInWindow - 1;
      SendMessageA(hWnd: hWndScroll, Msg: 0xE9u, wParam: 1u, (LPARAM)lParam);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x19013B80
 * CodeView symbol: EditUpdateScrollbar
 * Demangled: EditUpdateScrollbar
 * IDA name: ?EditUpdateScrollbar@@YIXPAUHWND__@@@Z
 * CodeView module: 10
 * CV address: segment 1 : 0x12B80
 * Code length: 0x96
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall EditUpdateScrollbar(HWND a1)
{
  LPARAM lParam[7]; // [esp+8h] [ebp-28h] BYREF
  LRESULT nItems; // [esp+24h] [ebp-Ch]
  int max; // [esp+28h] [ebp-8h]
  HWND hWndScroll; // [esp+2Ch] [ebp-4h]

  hWndScroll = (HWND)GetWindowLongA(hWnd: a1, nIndex: -21);
  if ( hWndScroll != nullptr )
  {
    nItems = SendMessageA(hWnd: a1, Msg: 0xBAu, wParam: 0, lParam: 0);
    max = nItems - 1;
    lParam[0] = 28;
    lParam[1] = 7;
    lParam[5] = SendMessageA(hWnd: a1, Msg: 0xCEu, wParam: 0, lParam: 0);
    lParam[2] = 0;
    lParam[3] = nItems - 1;
    lParam[4] = 10;
    SendMessageA(hWnd: hWndScroll, Msg: 0xE9u, wParam: 1u, (LPARAM)lParam);
  }
}


// ==============================================================================
/*
 * EA: 0x19013C16
 * CodeView symbol: ScrollbarRegisterClass
 * Demangled: ScrollbarRegisterClass
 * IDA name: ?ScrollbarRegisterClass@@YIXXZ
 * CodeView module: 10
 * CV address: segment 1 : 0x12C16
 * Code length: 0x52
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ScrollbarRegisterClass()
{
  WNDCLASSA wndclass; // [esp+0h] [ebp-28h] BYREF

  memset(a1: &wndclass, Val: 0, Size: sizeof(wndclass));
  wndclass.style = 0x4000;
  wndclass.lpfnWndProc = (WNDPROC)StormScrollbarWndProc;
  wndclass.hInstance = global_hinstance;
  wndclass.hCursor = LoadCursorA(hInstance: nullptr, lpCursorName: (LPCSTR)0x7F00);
  wndclass.lpszClassName = aStormscrollbar;
  RegisterClassA(lpWndClass: &wndclass);
}


// ==============================================================================
/*
 * EA: 0x19013C68
 * CodeView symbol: StormScrollbarWndProc
 * Demangled: StormScrollbarWndProc
 * IDA name: StormScrollbarWndProc
 * CodeView module: 10
 * CV address: segment 1 : 0x12C68
 * Code length: 0x385
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall StormScrollbarWndProc(HWND window, UINT message, WPARAM wparam, HWND lparam)
{
  _DWORD *pScrollDrag; // [esp+4h] [ebp-10h]
  struct tagPOINT pt; // [esp+Ch] [ebp-8h] BYREF

  if ( message <= 0xEA )
  {
    if ( message == 234 )
      return 0;
    switch ( message )
    {
      case 1u:
        if ( ScrollbarCreate(a1: window) == 0 )
          return -1;
        ScrollbarDraw(this: window);
        break;
      case 2u:
        ScrollbarDestroy(a1: window);
        return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: (LPARAM)lparam);
      case 0xFu:
        ScrollbarPaint(a1: window);
        return 0;
      case 0xE0u:
        return ScrollbarSetPos(bRedraw: (char)lparam);
      case 0xE2u:
      case 0xE6u:
        return ScrollbarSetRange(nMax: (int)lparam);
      case 0xE9u:
        return ScrollbarSetInfo(a1: window, a2: lparam);
      default:
        return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: (LPARAM)lparam);
    }
    return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: (LPARAM)lparam);
  }
  if ( message > 0x201 )
  {
    if ( message == 514 )
    {
      ScrollbarBtnUp(y: SHIWORD(lparam));
      return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: (LPARAM)lparam);
    }
    if ( message != 515 )
    {
      if ( message == 1130 )
      {
        SetPropA(hWnd: window, lpString: aScrollparent, hData: lparam);
        ShowWindow(hWnd: window, nCmdShow: 0);
        return 0;
      }
      return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: (LPARAM)lparam);
    }
LABEL_21:
    ScrollbarBtnDown(y: SHIWORD(lparam));
    return 0;
  }
  if ( message == 513 )
    goto LABEL_21;
  if ( message == 273 )
  {
    if ( HIWORD(wparam) == 1 )
    {
      ListUpdateScrollbar(a1: lparam);
      return 0;
    }
    return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: (LPARAM)lparam);
  }
  if ( message != 275 )
  {
    if ( message == 512 && window == GetCapture() )
    {
      pScrollDrag = GetPropA(hWnd: window, lpString: aScrolldrag);
      if ( pScrollDrag != nullptr && (*pScrollDrag & 0x10) != 0 )
        ScrollbarBtnDown(y: SHIWORD(lparam));
    }
    return DefWindowProcA(hWnd: window, Msg: message, wParam: wparam, lParam: (LPARAM)lparam);
  }
  if ( window == GetCapture() )
  {
    GetCursorPos(lpPoint: &pt);
    ScreenToClient(hWnd: window, lpPoint: &pt);
    ScrollbarBtnDown(y: pt.y);
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x19013FED
 * CodeView symbol: ScrollbarCreate
 * Demangled: ScrollbarCreate
 * IDA name: ScrollbarCreate
 * CodeView module: 10
 * CV address: segment 1 : 0x12FED
 * Code length: 0x261
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall ScrollbarCreate(HWND this)
{
  _DWORD *pScrollInfo; // [esp+4h] [ebp-20h]
  void *pScrollDrag; // [esp+8h] [ebp-1Ch]
  _DWORD *pTBmp; // [esp+Ch] [ebp-18h]
  struct tagRECT *pTRects; // [esp+10h] [ebp-14h]
  struct tagRECT rWnd; // [esp+14h] [ebp-10h] BYREF

  if ( sgBmpArrows == nullptr || sgBmpBar == nullptr || sgBmpThumb == nullptr )
    return 0;
  GetClientRect(hWnd: this, lpRect: &rWnd);
  pTBmp = (_DWORD *)SMemAlloc(a1: 12, a2: aScrllbarCpp_2, a3: 168, a4: 0);
  if ( pTBmp == nullptr )
    return 0;
  *pTBmp = SMemAlloc(a1: rWnd.bottom * rWnd.right, a2: aScrllbarCpp_3, a3: 172, a4: 8);
  if ( *pTBmp != 0 )
  {
    pTBmp[1] = rWnd.right;
    pTBmp[2] = rWnd.bottom;
    SetPropA(hWnd: this, lpString: aScrollbitmap_0, hData: pTBmp);
    SDlgSetBitmapI(a1: this, a2: 0, a3: 0, a4: -1, a5: 1, a6: *pTBmp, a7: 0, a8: rWnd.right, a9: rWnd.bottom, a10: -1);
    pTRects = (struct tagRECT *)SMemAlloc(a1: 64, a2: aScrllbarCpp_5, a3: 195, a4: 0);
    if ( pTRects != nullptr )
    {
      SetRect(lprc: pTRects, xLeft: 0, yTop: 0, xRight: rWnd.right, yBottom: sgnArrowHgt);
      SetRect(lprc: pTRects + 1, xLeft: 0, yTop: rWnd.bottom - sgnArrowHgt, xRight: rWnd.right, yBottom: rWnd.bottom);
      SetRect(lprc: pTRects + 2, xLeft: 0, yTop: sgnArrowHgt, xRight: rWnd.right, yBottom: sgnThumbHgt + sgnArrowHgt);
      SetRect(lprc: pTRects + 3, xLeft: 0, yTop: sgnArrowHgt, xRight: rWnd.right, yBottom: rWnd.bottom - sgnArrowHgt);
      SetPropA(hWnd: this, lpString: aScrollrects_0, hData: pTRects);
      pScrollInfo = (_DWORD *)SMemAlloc(a1: 28, a2: aScrllbarCpp_6, a3: 206, a4: 0);
      if ( pScrollInfo != nullptr )
      {
        memset(a1: pScrollInfo, Val: 0, Size: 0x1Cu);
        *pScrollInfo = 28;
        SetPropA(hWnd: this, lpString: aScrollinfo, hData: pScrollInfo);
        pScrollDrag = (void *)SMemAlloc(a1: 4, a2: aScrllbarCpp_7, a3: 214, a4: 0);
        if ( pScrollDrag != nullptr )
        {
          memset(a1: pScrollDrag, Val: 0, Size: 4u);
          SetPropA(hWnd: this, lpString: aScrolldrag_0, hData: pScrollDrag);
          return 1;
        }
        else
        {
          return 0;
        }
      }
      else
      {
        return 0;
      }
    }
    else
    {
      return 0;
    }
  }
  else
  {
    SMemFree(a1: pTBmp, a2: aScrllbarCpp_4, a3: 174, a4: 0);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1901424E
 * CodeView symbol: ScrollbarDestroy
 * Demangled: ScrollbarDestroy
 * IDA name: ScrollbarDestroy
 * CodeView module: 10
 * CV address: segment 1 : 0x1324E
 * Code length: 0xEF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HANDLE __thiscall ScrollbarDestroy(HWND this)
{
  HANDLE pScrollInfo; // [esp+4h] [ebp-10h]
  HANDLE pScrollDrag; // [esp+8h] [ebp-Ch]
  _DWORD *pTBmp; // [esp+Ch] [ebp-8h]
  HANDLE pTRects; // [esp+10h] [ebp-4h]

  pTBmp = RemovePropA(hWnd: this, lpString: aScrollbitmap_1);
  if ( pTBmp != nullptr )
  {
    if ( *pTBmp != 0 )
      SMemFree(a1: *pTBmp, a2: aScrllbarCpp_8, a3: 236, a4: 0);
    SMemFree(a1: pTBmp, a2: aScrllbarCpp_9, a3: 238, a4: 0);
  }
  pTRects = RemovePropA(hWnd: this, lpString: aScrollrects_1);
  if ( pTRects != nullptr )
    SMemFree(a1: pTRects, a2: aScrllbarCpp_10, a3: 243, a4: 0);
  pScrollInfo = RemovePropA(hWnd: this, lpString: aScrollinfo_0);
  if ( pScrollInfo != nullptr )
    SMemFree(a1: pScrollInfo, a2: aScrllbarCpp_11, a3: 247, a4: 0);
  pScrollDrag = RemovePropA(hWnd: this, lpString: aScrolldrag_1);
  if ( pScrollDrag != nullptr )
    SMemFree(a1: pScrollDrag, a2: aScrllbarCpp_12, a3: 251, a4: 0);
  return RemovePropA(hWnd: this, lpString: aScrollparent_0);
}


// ==============================================================================
/*
 * EA: 0x1901433D
 * CodeView symbol: ScrollbarPaint
 * Demangled: ScrollbarPaint
 * IDA name: ScrollbarPaint
 * CodeView module: 10
 * CV address: segment 1 : 0x1333D
 * Code length: 0x41
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __thiscall ScrollbarPaint(HWND this)
{
  struct tagPAINTSTRUCT ps; // [esp+4h] [ebp-44h] BYREF
  HDC dc; // [esp+44h] [ebp-4h]

  dc = BeginPaint(hWnd: this, lpPaint: &ps);
  SDlgDrawBitmap(a1: this, a2: 1, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0);
  return EndPaint(hWnd: this, lpPaint: &ps);
}


// ==============================================================================
/*
 * EA: 0x1901437E
 * CodeView symbol: ScrollbarSetPos
 * Demangled: ScrollbarSetPos
 * IDA name: ScrollbarSetPos
 * CodeView module: 10
 * CV address: segment 1 : 0x1337E
 * Code length: 0x8E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ScrollbarSetPos(HWND a1, int a2, char bRedraw)
{
  _DWORD *pScrollInfo; // [esp+8h] [ebp-8h]
  int nPosSave; // [esp+Ch] [ebp-4h]

  pScrollInfo = GetPropA(hWnd: a1, lpString: aScrollinfo_1);
  if ( pScrollInfo == nullptr )
    return 0;
  nPosSave = pScrollInfo[5];
  pScrollInfo[5] = a2;
  if ( pScrollInfo[5] < pScrollInfo[2] )
    pScrollInfo[5] = pScrollInfo[2];
  if ( pScrollInfo[5] > pScrollInfo[3] )
    pScrollInfo[5] = pScrollInfo[3];
  if ( nPosSave == pScrollInfo[5] )
    return 0;
  ScrollbarUpdate(a1);
  return nPosSave;
}


// ==============================================================================
/*
 * EA: 0x1901440C
 * CodeView symbol: ScrollbarUpdate
 * Demangled: ScrollbarUpdate
 * IDA name: ScrollbarUpdate
 * CodeView module: 10
 * CV address: segment 1 : 0x1340C
 * Code length: 0xD3
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
struct tagRECT *__thiscall ScrollbarUpdate(HWND this)
{
  struct tagRECT *result; // eax
  _DWORD *pScrollInfo; // [esp+4h] [ebp-14h]
  struct tagRECT *pTRects; // [esp+8h] [ebp-10h]
  int nScrollRange; // [esp+Ch] [ebp-Ch]
  int nNewPos; // [esp+14h] [ebp-4h]

  pScrollInfo = GetPropA(hWnd: this, lpString: aScrollinfo_2);
  result = (struct tagRECT *)GetPropA(hWnd: this, lpString: aScrollrects_2);
  pTRects = result;
  if ( pScrollInfo != nullptr )
  {
    nScrollRange = pScrollInfo[3] - pScrollInfo[2];
    if ( nScrollRange != 0 )
    {
      result = (struct tagRECT *)(result[3].top
                                + (result[3].bottom - result[3].top - sgnThumbHgt)
                                * (pScrollInfo[5] - pScrollInfo[2])
                                / nScrollRange);
      nNewPos = pTRects[3].top
              + (pTRects[3].bottom - pTRects[3].top - sgnThumbHgt) * (pScrollInfo[5] - pScrollInfo[2]) / nScrollRange;
      if ( nNewPos != pTRects[2].top )
      {
        OffsetRect(lprc: pTRects + 2, dx: 0, dy: nNewPos - pTRects[2].top);
        ScrollbarDraw(this);
        return (struct tagRECT *)InvalidateRect(hWnd: this, lpRect: nullptr, bErase: false);
      }
    }
    else
    {
      return (struct tagRECT *)ShowWindow(hWnd: this, nCmdShow: 0);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x190144DF
 * CodeView symbol: ScrollbarSetRange
 * Demangled: ScrollbarSetRange
 * IDA name: ScrollbarSetRange
 * CodeView module: 10
 * CV address: segment 1 : 0x134DF
 * Code length: 0x115
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ScrollbarSetRange(HWND a1, int a2, int nMax)
{
  _DWORD *pScrollInfo; // [esp+8h] [ebp-8h]
  int nSavePos; // [esp+Ch] [ebp-4h]

  pScrollInfo = GetPropA(hWnd: a1, lpString: aScrollinfo_3);
  if ( pScrollInfo == nullptr )
    return 0;
  if ( pScrollInfo[2] == a2 && pScrollInfo[3] == nMax )
    return 0;
  if ( a2 != nMax || pScrollInfo[2] == pScrollInfo[3] )
  {
    if ( a2 != nMax && pScrollInfo[2] == pScrollInfo[3] )
    {
      EnableWindow(hWnd: a1, bEnable: true);
      ShowWindow(hWnd: a1, nCmdShow: 5);
    }
  }
  else if ( (pScrollInfo[1] & 8) != 0 )
  {
    EnableWindow(hWnd: a1, bEnable: false);
  }
  else
  {
    ShowWindow(hWnd: a1, nCmdShow: 0);
  }
  nSavePos = pScrollInfo[5];
  pScrollInfo[2] = a2;
  pScrollInfo[3] = nMax;
  if ( pScrollInfo[5] < pScrollInfo[2] )
    pScrollInfo[5] = pScrollInfo[2];
  if ( pScrollInfo[5] > pScrollInfo[3] )
    pScrollInfo[5] = pScrollInfo[3];
  ScrollbarUpdate(this: a1);
  return nSavePos;
}


// ==============================================================================
/*
 * EA: 0x190145F4
 * CodeView symbol: ScrollbarSetInfo
 * Demangled: ScrollbarSetInfo
 * IDA name: ScrollbarSetInfo
 * CodeView module: 10
 * CV address: segment 1 : 0x135F4
 * Code length: 0x8A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ScrollbarSetInfo(HWND a1, _DWORD *a2)
{
  _DWORD *pScrollInfo; // [esp+8h] [ebp-4h]

  pScrollInfo = GetPropA(hWnd: a1, lpString: aScrollinfo_4);
  if ( pScrollInfo == nullptr )
    return 0;
  if ( (a2[1] & 2) != 0 )
    pScrollInfo[4] = a2[4];
  if ( (a2[1] & 1) != 0 )
    ScrollbarSetRange(a1, a2: a2[2], nMax: a2[3]);
  if ( (a2[1] & 4) != 0 )
    ScrollbarSetPos(a1, a2: a2[5], bRedraw: 1);
  return pScrollInfo[5];
}


// ==============================================================================
/*
 * EA: 0x1901467E
 * CodeView symbol: ScrollbarBtnDown
 * Demangled: ScrollbarBtnDown
 * IDA name: ScrollbarBtnDown
 * CodeView module: 10
 * CV address: segment 1 : 0x1367E
 * Code length: 0x31B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ScrollbarBtnDown(HWND a1, LONG a2, LONG y)
{
  int result; // eax
  WPARAM v4; // eax
  int v5; // eax
  int v6; // eax
  struct tagSCROLLINFO *pScrollInfo; // [esp+8h] [ebp-1Ch]
  int *pScrollDrag; // [esp+Ch] [ebp-18h]
  POINT pt; // [esp+10h] [ebp-14h] BYREF
  WPARAM nNotify; // [esp+18h] [ebp-Ch]
  RECT *pTRects; // [esp+1Ch] [ebp-8h]
  HWND hWndParent; // [esp+20h] [ebp-4h]

  pTRects = (RECT *)GetPropA(hWnd: a1, lpString: aScrollrects_3);
  hWndParent = (HWND)GetPropA(hWnd: a1, lpString: aScrollparent_1);
  pScrollInfo = (struct tagSCROLLINFO *)GetPropA(hWnd: a1, lpString: aScrollinfo_5);
  result = (int)GetPropA(hWnd: a1, lpString: aScrolldrag_2);
  pScrollDrag = (int *)result;
  if ( pTRects != nullptr && hWndParent != nullptr && pScrollInfo != nullptr && result != 0 )
  {
    pt.x = a2;
    pt.y = y;
    if ( a1 == GetCapture() )
    {
      if ( (*pScrollDrag & 0x10) != 0 )
      {
        ScrollDragThumb(a1, a2: &pt, pTRects);
        v4 = (unsigned __int16)MouseToScrollPos(a1: &pt, a2: (struct _ScrollRects *)pTRects, pScrollInfo) << 16;
        LOBYTE(v4) = v4 | 5;
        return SendMessageA(hWnd: hWndParent, Msg: 0x115u, wParam: v4, lParam: (LPARAM)a1);
      }
    }
    else
    {
      sghPrevCapture = SetCapture(hWnd: a1);
    }
    if ( PtInRect(lprc: pTRects, pt) )
    {
      if ( *pScrollDrag != 0 )
      {
        result = *pScrollDrag & 1;
        if ( result == 0 )
          return result;
      }
      if ( (*pScrollDrag & 1) == 0 )
      {
        SDlgSetTimer(a1, a2: 0, a3: 100, a4: 0);
        *pScrollDrag |= 1u;
      }
      ScrollbarScroll(a1, a2: -1);
      nNotify = 0;
    }
    else if ( PtInRect(lprc: pTRects + 1, pt) )
    {
      if ( *pScrollDrag != 0 )
      {
        result = (*pScrollDrag & 2) != 0;
        if ( (*pScrollDrag & 2) == 0 )
          return result;
      }
      if ( (*pScrollDrag & 2) == 0 )
      {
        SDlgSetTimer(a1, a2: 0, a3: 100, a4: 0);
        *pScrollDrag |= 2u;
      }
      ScrollbarScroll(a1, a2: 1);
      nNotify = 1;
    }
    else
    {
      result = PtInRect(lprc: pTRects + 3, pt);
      if ( result == 0 )
        return result;
      if ( PtInRect(lprc: pTRects + 2, pt) )
      {
        if ( *pScrollDrag == 0 )
        {
          result = *pScrollDrag;
          LOBYTE(result) = *pScrollDrag | 0x10;
          *pScrollDrag = result;
          return result;
        }
      }
      else if ( pt.y >= pTRects[2].top )
      {
        if ( *pScrollDrag != 0 )
        {
          result = (*pScrollDrag & 8) != 0;
          if ( (*pScrollDrag & 8) == 0 )
            return result;
        }
        if ( (*pScrollDrag & 8) == 0 )
        {
          SDlgSetTimer(a1, a2: 0, a3: 100, a4: 0);
          *pScrollDrag |= 8u;
        }
        ScrollbarScroll(a1, a2: pScrollInfo->nPage);
        v6 = LOWORD(pScrollInfo->nPos) << 16;
        LOBYTE(v6) = 4;
        nNotify = v6;
      }
      else
      {
        if ( *pScrollDrag != 0 )
        {
          result = (*pScrollDrag & 4) != 0;
          if ( (*pScrollDrag & 4) == 0 )
            return result;
        }
        if ( (*pScrollDrag & 4) == 0 )
        {
          SDlgSetTimer(a1, a2: 0, a3: 100, a4: 0);
          *pScrollDrag |= 4u;
        }
        ScrollbarScroll(a1, a2: -pScrollInfo->nPage);
        v5 = LOWORD(pScrollInfo->nPos) << 16;
        LOBYTE(v5) = 4;
        nNotify = v5;
      }
    }
    return SendMessageA(hWnd: hWndParent, Msg: 0x115u, wParam: nNotify, lParam: (LPARAM)a1);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19014999
 * CodeView symbol: ScrollbarScroll
 * Demangled: ScrollbarScroll
 * IDA name: ScrollbarScroll
 * CodeView module: 10
 * CV address: segment 1 : 0x13999
 * Code length: 0x89
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
struct tagRECT *__fastcall ScrollbarScroll(HWND a1, int a2)
{
  struct tagRECT *result; // eax
  LONG nPosSave; // [esp+Ch] [ebp-4h]

  result = (struct tagRECT *)GetPropA(hWnd: a1, lpString: aScrollinfo_6);
  if ( result != nullptr )
  {
    nPosSave = result[1].top;
    result[1].top = a2 + nPosSave;
    if ( result[1].top < result->right )
      result[1].top = result->right;
    if ( result[1].top > result->bottom )
      result[1].top = result->bottom;
    if ( nPosSave != result[1].top )
      return ScrollbarUpdate(this: a1);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19014A22
 * CodeView symbol: ScrollbarBtnUp
 * Demangled: ScrollbarBtnUp
 * IDA name: ScrollbarBtnUp
 * CodeView module: 10
 * CV address: segment 1 : 0x13A22
 * Code length: 0x115
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND __fastcall ScrollbarBtnUp(HWND a1, LONG a2, LONG y)
{
  HWND result; // eax
  int v4; // eax
  struct tagPOINT pt; // [esp+8h] [ebp-18h] BYREF
  struct tagSCROLLINFO *pScrollInfo; // [esp+10h] [ebp-10h]
  _DWORD *pScrollDrag; // [esp+14h] [ebp-Ch]
  struct tagRECT *pTRects; // [esp+18h] [ebp-8h]
  HWND hWndParent; // [esp+1Ch] [ebp-4h]

  result = GetCapture();
  if ( a1 == result )
  {
    pScrollDrag = GetPropA(hWnd: a1, lpString: aScrolldrag_3);
    pScrollInfo = (struct tagSCROLLINFO *)GetPropA(hWnd: a1, lpString: aScrollinfo_7);
    pTRects = (struct tagRECT *)GetPropA(hWnd: a1, lpString: aScrollrects_4);
    hWndParent = (HWND)GetPropA(hWnd: a1, lpString: aScrollparent_2);
    ReleaseCapture();
    if ( sghPrevCapture != nullptr )
      SetCapture(hWnd: sghPrevCapture);
    result = (HWND)SDlgKillTimer(a1, a2: 0);
    if ( pScrollDrag != nullptr && pScrollInfo != nullptr && pTRects != nullptr && hWndParent != nullptr )
    {
      if ( (*pScrollDrag & 0x10) != 0 )
      {
        pt.x = a2;
        pt.y = y;
        ScrollDragThumb(a1, a2: &pt, pTRects);
        v4 = MouseToScrollPos(a1: &pt, a2: (struct _ScrollRects *)pTRects, pScrollInfo);
        pScrollInfo->nPos = v4;
        SendMessageA(hWnd: hWndParent, Msg: 0x115u, wParam: (LOWORD(pScrollInfo->nPos) << 16) | 8, lParam: (LPARAM)a1);
      }
      result = (HWND)pScrollDrag;
      *pScrollDrag = 0;
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19014B37
 * CodeView symbol: ScrollbarUnregisterClass
 * Demangled: ScrollbarUnregisterClass
 * IDA name: ?ScrollbarUnregisterClass@@YIXXZ
 * CodeView module: 10
 * CV address: segment 1 : 0x13B37
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Scrllbar.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Scrllbar.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ScrollbarUnregisterClass()
{
  UnregisterClassA(lpClassName: aStormscrollbar_0, hInstance: global_hinstance);
}

