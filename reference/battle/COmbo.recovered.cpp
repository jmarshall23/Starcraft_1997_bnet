/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 18
 */


// ==============================================================================
/*
 * EA: 0x19008C40
 * CodeView symbol: ComboboxLoadArtwork
 * Demangled: ComboboxLoadArtwork
 * IDA name: ?ComboboxLoadArtwork@@YIXP6GHKKPAUtagPALETTEENTRY@@PAEKPAH22@Z@Z
 * CodeView module: 23
 * CV address: segment 1 : 0x7C40
 * Code length: 0xE8
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ComboboxLoadArtwork(
        int (__stdcall *a1)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  int v1; // eax
  int pos; // [esp+4h] [ebp-1Ch]
  _WORD grayPattern[8]; // [esp+8h] [ebp-18h] BYREF
  int i; // [esp+18h] [ebp-8h]
  HBITMAP grayBitmap; // [esp+1Ch] [ebp-4h]

  v1 = sgnComboCnt++;
  if ( v1 == 0 )
  {
    for ( pos = 0; pos < 9; ++pos )
      UiLoadArtwork(
        a1,
        a2: nullptr,
        hWndParent: nullptr,
        artid: COMBO_ARTID[pos],
        controltype: byte_19042118,
        controlstyle: 0xFFFFFFFF,
        usageflags: 1,
        loadpalette: 0,
        prepfadein: 0,
        a10: &(&sgComboBmp)[pos],
        a11: &sgComboSize + pos);
    for ( i = 0; i < 8; ++i )
      grayPattern[i] = 21845 << (i & 1);
    grayBitmap = CreateBitmap(nWidth: 8, nHeight: 8, nPlanes: 1u, nBitCount: 1u, lpBits: grayPattern);
    sghHalftoneBrush = CreatePatternBrush(hbm: grayBitmap);
    DeleteObject(ho: grayBitmap);
    sghEraseBrush = CreateSolidBrush(color: 0);
  }
}


// ==============================================================================
/*
 * EA: 0x19008D28
 * CodeView symbol: ComboboxDestroyArtwork
 * Demangled: ComboboxDestroyArtwork
 * IDA name: ?ComboboxDestroyArtwork@@YIXXZ
 * CodeView module: 23
 * CV address: segment 1 : 0x7D28
 * Code length: 0x8A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ComboboxDestroyArtwork()
{
  int pos; // [esp+0h] [ebp-4h]

  if ( --sgnComboCnt == 0 )
  {
    for ( pos = 0; pos < 9; ++pos )
    {
      if ( (&sgComboBmp)[pos] != nullptr )
      {
        SMemFree(a1: (&sgComboBmp)[pos], a2: aComboCpp, a3: 961, a4: 0);
        (&sgComboBmp)[pos] = nullptr;
      }
    }
    DeleteObject(ho: sghHalftoneBrush);
    DeleteObject(ho: sghEraseBrush);
  }
}


// ==============================================================================
/*
 * EA: 0x19008DB2
 * CodeView symbol: ComboRegisterClass
 * Demangled: ComboRegisterClass
 * IDA name: ?ComboRegisterClass@@YIXXZ
 * CodeView module: 23
 * CV address: segment 1 : 0x7DB2
 * Code length: 0xA4
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ComboRegisterClass()
{
  struct tagWNDCLASSA wc; // [esp+0h] [ebp-50h] BYREF
  WNDCLASSA wndclass; // [esp+28h] [ebp-28h] BYREF

  memset(a1: &wndclass, Val: 0, Size: sizeof(wndclass));
  wndclass.style = 0x4000;
  wndclass.lpfnWndProc = (WNDPROC)StormComboWndProc;
  wndclass.hInstance = global_hinstance;
  wndclass.hCursor = LoadCursorA(hInstance: nullptr, lpCursorName: (LPCSTR)0x7F00);
  wndclass.lpszClassName = aStormcombobox;
  RegisterClassA(lpWndClass: &wndclass);
  memset(a1: &wc, Val: 0, Size: sizeof(wc));
  GetClassInfoA(hInstance: nullptr, lpClassName: aStatic, lpWndClass: &wc);
  wc.style = 16512;
  wc.hInstance = global_hinstance;
  wc.hCursor = LoadCursorA(hInstance: nullptr, lpCursorName: (LPCSTR)0x7F00);
  wc.lpszClassName = aCombostaticedi;
  RegisterClassA(lpWndClass: &wc);
}


// ==============================================================================
/*
 * EA: 0x19008E56
 * CodeView symbol: StormComboWndProc
 * Demangled: StormComboWndProc
 * IDA name: StormComboWndProc
 * CodeView module: 23
 * CV address: segment 1 : 0x7E56
 * Code length: 0xA3A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall StormComboWndProc(HWND window, UINT message, HDC wparam, LPARAM lparam)
{
  HWND v4; // eax
  int result; // eax
  HWND v6; // eax
  HANDLE v7; // eax
  HWND v8; // eax
  HWND v9; // eax
  const RECT *v10; // eax
  HWND v11; // eax
  HWND v12; // eax
  HWND v13; // eax
  HWND Parent; // eax
  HWND NextDlgTabItem; // eax
  HWND v16; // eax
  HWND v17; // eax
  HWND v18; // eax
  HWND v19; // eax
  HWND v20; // eax
  HWND v21; // eax
  HWND v22; // eax
  HWND PropA; // eax
  HWND v24; // eax
  HWND v25; // eax
  HWND v26; // eax
  HWND v27; // eax
  POINT v28; // [esp-8h] [ebp-98h]
  HWND v29; // [esp+8h] [ebp-88h]
  HWND v30; // [esp+Ch] [ebp-84h]
  LRESULT nPos; // [esp+10h] [ebp-80h]
  HWND v32; // [esp+14h] [ebp-7Ch]
  HWND v33; // [esp+18h] [ebp-78h]
  HWND hEdit; // [esp+1Ch] [ebp-74h]
  HWND v35; // [esp+20h] [ebp-70h]
  LRESULT v36; // [esp+24h] [ebp-6Ch]
  HWND hWnd; // [esp+28h] [ebp-68h]
  LRESULT v38; // [esp+2Ch] [ebp-64h]
  HWND edit; // [esp+30h] [ebp-60h]
  HWND lbox; // [esp+34h] [ebp-5Ch]
  HWND focus; // [esp+38h] [ebp-58h]
  struct tagPAINTSTRUCT ps; // [esp+48h] [ebp-48h] BYREF
  HDC dc; // [esp+88h] [ebp-8h]
  LRESULT font; // [esp+8Ch] [ebp-4h]
  WPARAM wparama; // [esp+A0h] [ebp+10h]
  WPARAM wparamb; // [esp+A0h] [ebp+10h]

  if ( message <= 0x141 )
  {
    if ( message == 321 )
    {
      PropA = (HWND)GetPropA(hWnd: window, lpString: aComboedit_6);
      return SendMessageA(hWnd: PropA, Msg: 0xC5u, wParam: (WPARAM)wparam, lParam: lparam);
    }
    if ( message <= 0xF )
    {
      if ( message == 15 )
      {
        dc = BeginPaint(hWnd: window, lpPaint: &ps);
        SDlgDrawBitmap(a1: window, a2: 1, a3: 0, a4: 0, a5: 0, a6: 0, a7: 0);
        EndPaint(hWnd: window, lpPaint: &ps);
        return 0;
      }
      else
      {
        switch ( message )
        {
          case 1u:
            font = 0;
            if ( *(_DWORD *)(lparam + 12) != 0 )
              font = SendMessageA(hWnd: *(HWND *)(lparam + 12), Msg: 0x31u, wParam: 0, lParam: 0);
            if ( ComboCreate(a1: window, a2: font) != 0 )
              result = 0;
            else
              result = -1;
            break;
          case 2u:
            ComboDestroy(a1: window);
            result = 0;
            break;
          case 7u:
            v11 = (HWND)GetPropA(hWnd: window, lpString: aComboedit_1);
            SetFocus(hWnd: v11);
            result = 0;
            break;
          case 0xAu:
            if ( wparam == nullptr )
            {
              focus = GetFocus();
              edit = (HWND)GetPropA(hWnd: window, lpString: aComboedit_3);
              lbox = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_2);
              if ( focus == edit || focus == lbox )
              {
                Parent = GetParent(hWnd: window);
                NextDlgTabItem = GetNextDlgTabItem(hDlg: Parent, hCtl: window, bPrevious: false);
                SetFocus(hWnd: NextDlgTabItem);
              }
            }
            return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
          case 0xBu:
            v8 = (HWND)GetPropA(hWnd: window, lpString: aComboedit_0);
            SendMessageA(hWnd: v8, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
            v9 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox);
            SendMessageA(hWnd: v9, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
            if ( wparam != nullptr )
              InvalidateRect(hWnd: window, lpRect: nullptr, bErase: true);
            result = 0;
            break;
          case 0xCu:
            hEdit = (HWND)GetPropA(hWnd: window, lpString: aComboedit_4);
            if ( SendMessageA(hWnd: hEdit, Msg: message, wParam: (WPARAM)wparam, lParam: lparam) != 0 )
              result = 1;
            else
              result = -2;
            break;
          case 0xDu:
          case 0xEu:
            v18 = (HWND)GetPropA(hWnd: window, lpString: aComboedit_5);
            result = SendMessageA(hWnd: v18, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
            break;
          default:
            return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
        }
      }
      return result;
    }
    if ( message > 0x111 )
    {
      if ( message >= 0x133 && (message <= 0x134 || message == 312) )
      {
        SetTextColor(hdc: wparam, color: 0xFFFFFFu);
        SetBkColor(hdc: wparam, color: 0);
        SetBkMode(hdc: wparam, mode: 2);
        return (int)GetStockObject(i: 4);
      }
      return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
    }
    switch ( message )
    {
      case 0x111u:
        if ( (unsigned __int16)wparam != 7173 )
        {
          if ( (unsigned __int16)wparam == 7174 && HIWORD(wparam) == 1 )
          {
            v7 = GetPropA(hWnd: window, lpString: aComboedit);
            UpdateSelection(a1: lparam, a2: v7);
            return 0;
          }
          return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
        }
        if ( HIWORD(wparam) == 768 && sgbPopup == 0 )
        {
          wparama = (unsigned __int16)GetWindowLongA(hWnd: window, nIndex: -12) | 0x50000;
          v4 = GetParent(hWnd: window);
          SendMessageA(hWnd: v4, Msg: message, wParam: wparama, lParam: (LPARAM)window);
          return 0;
        }
        if ( HIWORD(wparam) == 1024 && sgbPopup == 0 )
        {
          wparamb = (unsigned __int16)GetWindowLongA(hWnd: window, nIndex: -12) | 0x60000;
          v6 = GetParent(hWnd: window);
          SendMessageA(hWnd: v6, Msg: message, wParam: wparamb, lParam: (LPARAM)window);
          return 0;
        }
        if ( HIWORD(wparam) == 3056 )
          goto LABEL_27;
        if ( HIWORD(wparam) != 3057 )
          return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
        break;
      case 0x30u:
        if ( wparam != nullptr )
        {
          ComboDestroy(a1: window);
          ComboCreate(a1: window, a2: wparam);
          v12 = (HWND)GetPropA(hWnd: window, lpString: aComboedit_2);
          SendMessageA(hWnd: v12, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
          v13 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_1);
          SendMessageA(hWnd: v13, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
        }
        return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
      case 0x100u:
        if ( wparam == (HDC)27 )
        {
          ClosePopup(a1: window);
          return 0;
        }
        if ( wparam != (HDC)38 )
        {
          if ( wparam != (HDC)40 )
            return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
LABEL_27:
          ScrollDown(a1: window);
          return 0;
        }
        break;
      default:
        if ( message == 257 && wparam == (HDC)44 )
          SDrawCaptureScreen(a1: 0);
        return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
    }
    ScrollUp(a1: window);
    return 0;
  }
  switch ( message )
  {
    case 0x142u:
      v24 = (HWND)GetPropA(hWnd: window, lpString: aComboedit_9);
      result = SendMessageA(hWnd: v24, Msg: 0xB1u, wParam: (WPARAM)wparam, lParam: lparam);
      break;
    case 0x143u:
      hWnd = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_3);
      v38 = SendMessageA(hWnd, Msg: 0x180u, wParam: (WPARAM)wparam, lParam: lparam);
      ListUpdateScrollbar(a1: hWnd);
      v16 = (HWND)GetPropA(hWnd: window, lpString: aLboxscroll);
      ShowWindow(hWnd: v16, nCmdShow: 0);
      result = v38;
      break;
    case 0x146u:
      v19 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_5);
      result = SendMessageA(hWnd: v19, Msg: 0x18Bu, wParam: (WPARAM)wparam, lParam: lparam);
      break;
    case 0x147u:
      v20 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_6);
      result = SendMessageA(hWnd: v20, Msg: 0x188u, wParam: (WPARAM)wparam, lParam: lparam);
      break;
    case 0x148u:
      v22 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_8);
      result = SendMessageA(hWnd: v22, Msg: 0x189u, wParam: (WPARAM)wparam, lParam: lparam);
      break;
    case 0x14Au:
      v35 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_4);
      v36 = SendMessageA(hWnd: v35, Msg: 0x181u, wParam: (WPARAM)wparam, lParam: lparam);
      ListUpdateScrollbar(a1: v35);
      v17 = (HWND)GetPropA(hWnd: window, lpString: aLboxscroll_0);
      ShowWindow(hWnd: v17, nCmdShow: 0);
      result = v36;
      break;
    case 0x14Bu:
      v26 = (HWND)GetPropA(hWnd: window, lpString: aComboedit_10);
      SendMessageA(hWnd: v26, Msg: 0xCu, wParam: 0, lParam: (LPARAM)&unk_1904211C);
      v27 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_12);
      result = SendMessageA(hWnd: v27, Msg: 0x184u, wParam: (WPARAM)wparam, lParam: lparam);
      break;
    case 0x14Du:
      v33 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_9);
      v32 = (HWND)GetPropA(hWnd: window, lpString: aComboedit_7);
      nPos = SendMessageA(hWnd: v33, Msg: 0x18Cu, wParam: (WPARAM)wparam, lParam: lparam);
      if ( nPos != -1 )
        UpdateSelection(a1: v33, a2: v32);
      SetFocus(hWnd: v32);
      result = nPos;
      break;
    case 0x14Eu:
      SetFocus(hWnd: nullptr);
      v30 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_10);
      v29 = (HWND)GetPropA(hWnd: window, lpString: aComboedit_8);
      SendMessageA(hWnd: v30, Msg: 0x186u, wParam: (WPARAM)wparam, lParam: lparam);
      UpdateSelection(a1: v30, a2: v29);
      SetFocus(hWnd: v29);
      result = (int)wparam;
      break;
    case 0x150u:
      v21 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_7);
      result = SendMessageA(hWnd: v21, Msg: 0x199u, wParam: (WPARAM)wparam, lParam: lparam);
      break;
    case 0x151u:
      v25 = (HWND)GetPropA(hWnd: window, lpString: aCombolbox_11);
      result = SendMessageA(hWnd: v25, Msg: 0x19Au, wParam: (WPARAM)wparam, lParam: lparam);
      break;
    case 0x201u:
    case 0x203u:
      SetCapture(hWnd: window);
      result = 0;
      break;
    case 0x202u:
      if ( window != GetCapture() )
        return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
      ReleaseCapture();
      if ( ClosePopup(a1: window) != 0 )
      {
        result = 0;
      }
      else
      {
        GetPropA(hWnd: window, lpString: aCombolbox_0);
        v28.y = SHIWORD(lparam);
        v28.x = (__int16)lparam;
        v10 = (const RECT *)GetPropA(hWnd: window, lpString: aComboarrowrct);
        if ( PtInRect(lprc: v10, pt: v28) )
          OpenPopup(a1: window);
        result = 0;
      }
      break;
    default:
      return DefWindowProcA(hWnd: window, Msg: message, wParam: (WPARAM)wparam, lParam: lparam);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19009890
 * CodeView symbol: ClosePopup
 * Demangled: ClosePopup
 * IDA name: ClosePopup
 * CodeView module: 23
 * CV address: segment 1 : 0x8890
 * Code length: 0x162
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall ClosePopup(HWND this)
{
  HWND PropA; // eax
  HANDLE v3; // esi
  HWND Parent; // eax
  WPARAM v5; // [esp-8h] [ebp-48h]
  struct tagRECT invalidrect; // [esp+8h] [ebp-38h] BYREF
  struct tagRECT lboxRect; // [esp+18h] [ebp-28h] BYREF
  HWND hEdit; // [esp+28h] [ebp-18h]
  struct tagRECT comboRect; // [esp+2Ch] [ebp-14h] BYREF
  HWND hLbox; // [esp+3Ch] [ebp-4h]

  if ( sgbPopup == 0 )
    return 0;
  sgbPopup = 0;
  SetCapture(hWnd: nullptr);
  hLbox = (HWND)GetPropA(hWnd: this, lpString: aCombolbox_13);
  GetWindowRect(hWnd: this, lpRect: &comboRect);
  GetWindowRect(hWnd: hLbox, lpRect: &lboxRect);
  ShowWindow(hWnd: hLbox, nCmdShow: 0);
  PropA = (HWND)GetPropA(hWnd: this, lpString: aLboxscroll_1);
  ShowWindow(hWnd: PropA, nCmdShow: 0);
  SetWindowPos(
    hWnd: this,
    hWndInsertAfter: nullptr,
    X: 0,
    Y: 0,
    cx: comboRect.right - comboRect.left,
    cy: comboRect.bottom - comboRect.top - (lboxRect.bottom - lboxRect.top + 6),
    uFlags: 6u);
  v3 = (HANDLE)SendMessageA(hWnd: hLbox, Msg: 0x188u, wParam: 0, lParam: 0);
  if ( v3 != GetPropA(hWnd: this, lpString: aLboxselection) )
  {
    v5 = (unsigned __int16)GetWindowLongA(hWnd: this, nIndex: -12) | 0x10000;
    Parent = GetParent(hWnd: this);
    SendMessageA(hWnd: Parent, Msg: 0x111u, wParam: v5, lParam: (LPARAM)this);
  }
  hEdit = (HWND)GetPropA(hWnd: this, lpString: aComboedit_11);
  GetWindowRect(hWnd: hEdit, lpRect: &invalidrect);
  ScreenToClient(hWnd: hEdit, lpPoint: (LPPOINT)&invalidrect);
  ScreenToClient(hWnd: hEdit, lpPoint: (LPPOINT)&invalidrect.right);
  InvalidateRect(hWnd: hEdit, lpRect: &invalidrect, bErase: true);
  SetFocus(hWnd: hEdit);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x190099F2
 * CodeView symbol: OpenPopup
 * Demangled: OpenPopup
 * IDA name: OpenPopup
 * CodeView module: 23
 * CV address: segment 1 : 0x89F2
 * Code length: 0xE5
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall OpenPopup(HWND this)
{
  HWND PropA; // eax
  struct tagRECT lboxRect; // [esp+4h] [ebp-28h] BYREF
  HANDLE nPos; // [esp+14h] [ebp-18h]
  struct tagRECT comboRect; // [esp+18h] [ebp-14h] BYREF
  HWND hLbox; // [esp+28h] [ebp-4h]

  if ( sgbPopup != 0 )
    return 0;
  sgbPopup = 1;
  hLbox = (HWND)GetPropA(hWnd: this, lpString: aCombolbox_14);
  GetWindowRect(hWnd: this, lpRect: &comboRect);
  GetWindowRect(hWnd: hLbox, lpRect: &lboxRect);
  SetWindowPos(
    hWnd: this,
    hWndInsertAfter: nullptr,
    X: 0,
    Y: 0,
    cx: comboRect.right - comboRect.left,
    cy: comboRect.bottom - comboRect.top + lboxRect.bottom - lboxRect.top + 6,
    uFlags: 6u);
  ShowWindow(hWnd: hLbox, nCmdShow: 1);
  PropA = (HWND)GetPropA(hWnd: this, lpString: aLboxscroll_2);
  ShowWindow(hWnd: PropA, nCmdShow: 1);
  nPos = (HANDLE)SendMessageA(hWnd: hLbox, Msg: 0x188u, wParam: 0, lParam: 0);
  SetPropA(hWnd: this, lpString: aLboxselection_0, hData: nPos);
  SetFocus(hWnd: hLbox);
  SetCapture(hWnd: hLbox);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x19009AD7
 * CodeView symbol: UpdateSelection
 * Demangled: UpdateSelection
 * IDA name: UpdateSelection
 * CodeView module: 23
 * CV address: segment 1 : 0x8AD7
 * Code length: 0x7A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall UpdateSelection(HWND a1, HWND a2)
{
  _BYTE szBuf[256]; // [esp+8h] [ebp-104h] BYREF
  LRESULT idx; // [esp+108h] [ebp-4h]

  idx = SendMessageA(hWnd: a1, Msg: 0x188u, wParam: 0, lParam: 0);
  if ( idx != -1 )
  {
    SendMessageA(hWnd: a1, Msg: 0x189u, wParam: idx, lParam: (LPARAM)szBuf);
    SendMessageA(hWnd: a2, Msg: 0xCu, wParam: 0, lParam: (LPARAM)szBuf);
    ListUpdateScrollbar(a1);
  }
}


// ==============================================================================
/*
 * EA: 0x19009B51
 * CodeView symbol: ScrollDown
 * Demangled: ScrollDown
 * IDA name: ScrollDown
 * CodeView module: 23
 * CV address: segment 1 : 0x8B51
 * Code length: 0xB9
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __thiscall ScrollDown(HWND this)
{
  LRESULT result; // eax
  HWND Parent; // eax
  WPARAM v3; // [esp-8h] [ebp-18h]
  HWND hEdit; // [esp+4h] [ebp-Ch]
  HWND hLbox; // [esp+8h] [ebp-8h]
  LRESULT nSel; // [esp+Ch] [ebp-4h]

  hLbox = (HWND)GetPropA(hWnd: this, lpString: aCombolbox_15);
  nSel = SendMessageA(hWnd: hLbox, Msg: 0x188u, wParam: 0, lParam: 0);
  if ( nSel == -1 )
    nSel = -1;
  result = SendMessageA(hWnd: hLbox, Msg: 0x186u, wParam: nSel + 1, lParam: 0);
  if ( result != -1 )
  {
    hEdit = (HWND)GetPropA(hWnd: this, lpString: aComboedit_12);
    UpdateSelection(a1: hLbox, a2: hEdit);
    SetFocus(hWnd: hEdit);
    v3 = (unsigned __int16)GetWindowLongA(hWnd: this, nIndex: -12) | 0x10000;
    Parent = GetParent(hWnd: this);
    return SendMessageA(hWnd: Parent, Msg: 0x111u, wParam: v3, lParam: (LPARAM)this);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19009C0A
 * CodeView symbol: ScrollUp
 * Demangled: ScrollUp
 * IDA name: ScrollUp
 * CodeView module: 23
 * CV address: segment 1 : 0x8C0A
 * Code length: 0xBF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __thiscall ScrollUp(HWND this)
{
  LRESULT result; // eax
  HWND Parent; // eax
  WPARAM v3; // [esp-8h] [ebp-18h]
  HWND hEdit; // [esp+4h] [ebp-Ch]
  HWND hLbox; // [esp+8h] [ebp-8h]
  int nSel; // [esp+Ch] [ebp-4h]

  hLbox = (HWND)GetPropA(hWnd: this, lpString: aCombolbox_16);
  result = SendMessageA(hWnd: hLbox, Msg: 0x188u, wParam: 0, lParam: 0);
  nSel = result;
  if ( result == -1 )
    nSel = 1;
  if ( nSel != 0 )
  {
    result = SendMessageA(hWnd: hLbox, Msg: 0x186u, wParam: nSel - 1, lParam: 0);
    if ( result != -1 )
    {
      hEdit = (HWND)GetPropA(hWnd: this, lpString: aComboedit_13);
      UpdateSelection(a1: hLbox, a2: hEdit);
      SetFocus(hWnd: hEdit);
      v3 = (unsigned __int16)GetWindowLongA(hWnd: this, nIndex: -12) | 0x10000;
      Parent = GetParent(hWnd: this);
      return SendMessageA(hWnd: Parent, Msg: 0x111u, wParam: v3, lParam: (LPARAM)this);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x19009CC9
 * CodeView symbol: ComboCreate
 * Demangled: ComboCreate
 * IDA name: ComboCreate
 * CodeView module: 23
 * CV address: segment 1 : 0x8CC9
 * Code length: 0x4AA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ComboCreate(HWND a1, WPARAM a2)
{
  HWND Parent; // eax
  HWND v4; // eax
  int v5; // esi
  int Width; // eax
  int v7; // eax
  int v8; // esi
  int v9; // [esp-14h] [ebp-A4h]
  HINSTANCE v10; // [esp-8h] [ebp-98h]
  int v11; // [esp-4h] [ebp-94h]
  int xLeft; // [esp+4h] [ebp-8Ch]
  int v13; // [esp+8h] [ebp-88h]
  int heighttrim; // [esp+18h] [ebp-78h]
  struct tagRECT parentrect; // [esp+1Ch] [ebp-74h] BYREF
  LONG editHgt; // [esp+2Ch] [ebp-64h]
  struct tagRECT lboxRect; // [esp+30h] [ebp-60h] BYREF
  int edityoffset; // [esp+40h] [ebp-50h]
  int fontheight; // [esp+44h] [ebp-4Ch]
  HWND hEdit; // [esp+48h] [ebp-48h]
  int i; // [esp+4Ch] [ebp-44h]
  struct tagRECT comboRect; // [esp+50h] [ebp-40h] BYREF
  struct tagRECT *pDropRect; // [esp+60h] [ebp-30h]
  HWND hLbox; // [esp+64h] [ebp-2Ch]
  HANDLE oldProc; // [esp+68h] [ebp-28h]
  struct tagRECT comboRelRect; // [esp+6Ch] [ebp-24h] BYREF
  struct tagRECT dlgRect; // [esp+7Ch] [ebp-14h] BYREF
  HWND hScroll; // [esp+8Ch] [ebp-4h]

  if ( a2 == 0 || (fontheight = sub_1900A9A1(a1: a2)) == -1 )
    fontheight = dword_190420BC;
  for ( i = 0; i < 9; ++i )
  {
    if ( *(&sgComboSize.cx + 2 * i) == 0 || *(&sgComboSize.cy + 2 * i) == 0 )
      return 0;
  }
  Parent = GetParent(hWnd: a1);
  GetWindowRect(hWnd: Parent, lpRect: &dlgRect);
  GetWindowRect(hWnd: a1, lpRect: &comboRect);
  GetClientRect(hWnd: a1, lpRect: &comboRelRect);
  comboRect.bottom += 10 * dword_190420BC;
  comboRelRect.bottom += 10 * dword_190420BC;
  v4 = GetParent(hWnd: a1);
  GetWindowRect(hWnd: v4, lpRect: &parentrect);
  heighttrim = parentrect.bottom - comboRect.bottom;
  if ( parentrect.bottom - comboRect.bottom < 0 )
  {
    comboRect.bottom = parentrect.bottom - 1;
    comboRelRect.bottom = comboRelRect.bottom + heighttrim - 1;
  }
  comboRelRect.bottom -= dword_190420D4 + sgComboSize.cy;
  if ( comboRelRect.bottom < 0 )
    return 0;
  comboRelRect.right -= comboRelRect.right % sgComboSize.cx;
  comboRelRect.bottom -= comboRelRect.bottom % dword_190420BC;
  comboRelRect.bottom += dword_190420D4 + sgComboSize.cy;
  SetWindowPos(
    hWnd: a1,
    hWndInsertAfter: nullptr,
    X: 0,
    Y: 0,
    cx: comboRelRect.right,
    cy: comboRelRect.bottom,
    uFlags: 6u);
  ComboDraw(a1);
  editHgt = sgComboSize.cy;
  SetWindowPos(hWnd: a1, hWndInsertAfter: nullptr, X: 0, Y: 0, cx: comboRelRect.right, cy: sgComboSize.cy, uFlags: 6u);
  if ( (editHgt - 7 - fontheight) / 2 >= 1 )
    v13 = (editHgt - 7 - fontheight) / 2;
  else
    v13 = 1;
  edityoffset = v13;
  if ( (GetWindowLongA(hWnd: a1, nIndex: -16) & 3) != 0 )
    hEdit = CreateWindowExA(
              dwExStyle: 0,
              lpClassName: aCombostaticedi_0,
              lpWindowName: nullptr,
              dwStyle: 0x50010000u,
              X: 7,
              Y: edityoffset + 3,
              nWidth: comboRelRect.right - dword_190420B0 - 6,
              nHeight: editHgt - 6 - edityoffset - 4,
              hWndParent: a1,
              hMenu: (HMENU)0x1C05,
              hInstance: global_hinstance,
              lpParam: nullptr);
  else
    hEdit = CreateWindowExA(
              dwExStyle: 0,
              lpClassName: aEdit,
              lpWindowName: nullptr,
              dwStyle: 0x50010000u,
              X: 7,
              Y: edityoffset + 3,
              nWidth: comboRelRect.right - dword_190420B0 - 6,
              nHeight: editHgt - 6 - edityoffset - 4,
              hWndParent: a1,
              hMenu: (HMENU)0x1C05,
              hInstance: global_hinstance,
              lpParam: nullptr);
  if ( hEdit == nullptr )
    return 0;
  oldProc = (HANDLE)SetWindowLongA(hWnd: hEdit, nIndex: -4, dwNewLong: (LONG)ComboEditWndProc);
  SetPropA(hWnd: hEdit, lpString: aOldproc, hData: oldProc);
  SetPropA(hWnd: a1, lpString: aComboedit_14, hData: hEdit);
  SendMessageA(hWnd: hEdit, Msg: 0x30u, wParam: a2, lParam: 0);
  v11 = comboRelRect.bottom - 3;
  v5 = comboRelRect.right - 3;
  Width = ScrollbarGetWidth();
  SetRect(
    lprc: &lboxRect,
    xLeft: comboRelRect.left + 3,
    yTop: editHgt + comboRelRect.top,
    xRight: v5 - Width,
    yBottom: v11);
  hLbox = CreateWindowExA(
            dwExStyle: 0,
            lpClassName: aListbox,
            lpWindowName: nullptr,
            dwStyle: 0x44010101u,
            X: lboxRect.left,
            Y: lboxRect.top,
            nWidth: lboxRect.right - lboxRect.left,
            nHeight: lboxRect.bottom - lboxRect.top,
            hWndParent: a1,
            hMenu: (HMENU)0x1C06,
            hInstance: global_hinstance,
            lpParam: nullptr);
  if ( hLbox == nullptr )
    return 0;
  SetPropA(hWnd: a1, lpString: aCombolbox_17, hData: hLbox);
  oldProc = (HANDLE)SetWindowLongA(hWnd: hLbox, nIndex: -4, dwNewLong: (LONG)ComboLboxWndProc);
  SetPropA(hWnd: hLbox, lpString: aOldproc_0, hData: oldProc);
  SendMessageA(hWnd: hLbox, Msg: 0x30u, wParam: a2, lParam: 0);
  v10 = global_hinstance;
  v9 = lboxRect.bottom - lboxRect.top;
  v7 = ScrollbarGetWidth();
  hScroll = CreateWindowExA(
              dwExStyle: 0,
              lpClassName: aStormscrollbar_1,
              lpWindowName: nullptr,
              dwStyle: 0x40000000u,
              X: lboxRect.right,
              Y: lboxRect.top,
              nWidth: v7,
              nHeight: v9,
              hWndParent: a1,
              hMenu: (HMENU)0x1C07,
              hInstance: v10,
              lpParam: nullptr);
  if ( hScroll == nullptr )
    return 0;
  ScrollbarLink(a1: hLbox, a2: hScroll);
  SetPropA(hWnd: a1, lpString: aLboxscroll_3, hData: hScroll);
  pDropRect = (struct tagRECT *)SMemAlloc(a1: 16, a2: aComboCpp_0, a3: 641, a4: 0);
  if ( (GetWindowLongA(hWnd: a1, nIndex: -16) & 3) != 0 )
  {
    SetRect(
      lprc: pDropRect,
      xLeft: comboRelRect.left + 3,
      yTop: comboRelRect.top + 3,
      xRight: comboRelRect.right - 3,
      yBottom: comboRelRect.top + editHgt - 3);
  }
  else
  {
    v8 = comboRelRect.right - 3;
    xLeft = v8 - ScrollbarGetWidth();
    SetRect(
      lprc: pDropRect,
      xLeft,
      yTop: comboRelRect.top + 3,
      xRight: comboRelRect.right - 3,
      yBottom: comboRelRect.top + editHgt - 3);
  }
  SetPropA(hWnd: a1, lpString: aComboarrowrct_0, hData: pDropRect);
  sgbPopup = 0;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900A173
 * CodeView symbol: ComboEditWndProc
 * Demangled: ComboEditWndProc
 * IDA name: ComboEditWndProc
 * CodeView module: 23
 * CV address: segment 1 : 0x9173
 * Code length: 0x24B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ComboEditWndProc(HWND hEdit, UINT message, WPARAM wparam, LPARAM lparam)
{
  HWND v4; // eax
  int result; // eax
  HWND Parent; // eax
  HWND FrameWindow; // eax
  LRESULT dwLen; // [esp+4h] [ebp-Ch]
  HWND hCombo; // [esp+8h] [ebp-8h]
  LRESULT (__stdcall *oldProc)(HWND, UINT, WPARAM, LPARAM); // [esp+Ch] [ebp-4h]

  oldProc = (LRESULT (__stdcall *)(HWND, UINT, WPARAM, LPARAM))GetPropA(hWnd: hEdit, lpString: aOldproc_1);
  if ( message > 0x87 )
  {
    if ( message > 0x105 )
    {
      if ( message >= 0x201 && message <= 0x203 )
      {
        hCombo = GetParent(hWnd: hEdit);
        if ( (GetWindowLongA(hWnd: hCombo, nIndex: -16) & 3) != 0 )
          return SendMessageA(hWnd: hCombo, Msg: message, wParam: wparam, lParam: lparam);
      }
    }
    else if ( message >= 0x104 )
    {
      FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
      SendMessageA(hWnd: FrameWindow, Msg: message, wParam: wparam, lParam: lparam);
    }
    else if ( message == 256 )
    {
      if ( wparam == 40 )
      {
        Parent = GetParent(hWnd: hEdit);
        OpenPopup(this: Parent);
        return 0;
      }
    }
    else if ( message == 257 && wparam == 44 )
    {
      SDrawCaptureScreen(a1: 0);
      SetFocus(hWnd: hEdit);
    }
    return CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hEdit, Msg: message, wParam: wparam, lParam: lparam);
  }
  if ( message == 135 )
  {
    v4 = GetParent(hWnd: hEdit);
    if ( (GetWindowLongA(hWnd: v4, nIndex: -16) & 3) != 0 )
      return 1;
    return CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hEdit, Msg: message, wParam: wparam, lParam: lparam);
  }
  switch ( message )
  {
    case 1u:
      sbFocus = 0;
      return CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hEdit, Msg: message, wParam: wparam, lParam: lparam);
    case 7u:
      sbFocus = 1;
      EditDrawFocus(a1: hEdit, a2: 1);
      dwLen = SendMessageA(hWnd: hEdit, Msg: 0xEu, wParam: 0, lParam: 0);
      SendMessageA(hWnd: hEdit, Msg: 0xB1u, wParam: 0, lParam: dwLen);
      return CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hEdit, Msg: message, wParam: wparam, lParam: lparam);
    case 8u:
      sbFocus = 0;
      EditDrawFocus(a1: hEdit, a2: 0);
      return CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hEdit, Msg: message, wParam: wparam, lParam: lparam);
    case 0xFu:
      CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hEdit, Msg: message, wParam: wparam, lParam: lparam);
      if ( GetFocus() == hEdit )
        EditDrawFocus(a1: hEdit, a2: sbFocus);
      result = 0;
      break;
    default:
      return CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hEdit, Msg: message, wParam: wparam, lParam: lparam);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900A3BE
 * CodeView symbol: EditDrawFocus
 * Demangled: EditDrawFocus
 * IDA name: EditDrawFocus
 * CodeView module: 23
 * CV address: segment 1 : 0x93BE
 * Code length: 0xCA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall EditDrawFocus(HWND a1, int a2)
{
  HWND Parent; // eax
  int result; // eax
  struct tagRECT clientrect; // [esp+Ch] [ebp-18h] BYREF
  HDC hDc; // [esp+1Ch] [ebp-8h]
  HRGN clipregion; // [esp+20h] [ebp-4h]

  Parent = GetParent(hWnd: a1);
  result = GetWindowLongA(hWnd: Parent, nIndex: -16) & 3;
  if ( result != 0 )
  {
    GetClientRect(hWnd: a1, lpRect: &clientrect);
    clientrect.left -= 3;
    clientrect.right -= 2;
    clipregion = CreateRectRgn(
                   x1: clientrect.left - 1,
                   y1: clientrect.top,
                   x2: clientrect.right,
                   y2: --clientrect.bottom);
    hDc = GetDCEx(hWnd: a1, hrgnClip: clipregion, flags: 0x20u);
    if ( a2 != 0 )
      FrameRect(hDC: hDc, lprc: &clientrect, hbr: (HBRUSH)sghHalftoneBrush);
    else
      FrameRect(hDC: hDc, lprc: &clientrect, hbr: (HBRUSH)sghEraseBrush);
    ReleaseDC(hWnd: a1, hDC: hDc);
    return DeleteObject(ho: clipregion);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900A488
 * CodeView symbol: ComboLboxWndProc
 * Demangled: ComboLboxWndProc
 * IDA name: ComboLboxWndProc
 * CodeView module: 23
 * CV address: segment 1 : 0x9488
 * Code length: 0x270
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LRESULT __stdcall ComboLboxWndProc(HWND hLbox, UINT message, WPARAM wparam, LPARAM lparam)
{
  HWND v5; // eax
  HWND v6; // eax
  HWND Parent; // eax
  HWND v8; // eax
  HWND v9; // eax
  HWND v10; // eax
  HWND NextDlgTabItem; // eax
  HWND FrameWindow; // eax
  HWND v13; // eax
  POINT v14; // [esp-8h] [ebp-44h]
  HWND v15; // [esp-8h] [ebp-44h]
  BOOL v16; // [esp-4h] [ebp-40h]
  LRESULT v17; // [esp+4h] [ebp-38h]
  struct tagRECT lboxRect; // [esp+Ch] [ebp-30h] BYREF
  POINT pt; // [esp+1Ch] [ebp-20h]
  struct tagRECT scrollRect; // [esp+24h] [ebp-18h] BYREF
  HWND hScroll; // [esp+34h] [ebp-8h]
  WNDPROC oldProc; // [esp+38h] [ebp-4h]

  oldProc = (WNDPROC)GetPropA(hWnd: hLbox, lpString: aOldproc_2);
  if ( message > 0x105 )
  {
    switch ( message )
    {
      case 0x201u:
        goto LABEL_12;
      case 0x202u:
        v17 = CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hLbox, Msg: 0x202u, wParam: wparam, lParam: lparam);
        ReleaseCapture();
        Parent = GetParent(hWnd: hLbox);
        ClosePopup(this: Parent);
        return v17;
      case 0x203u:
LABEL_12:
        v5 = GetParent(hWnd: hLbox);
        hScroll = (HWND)GetPropA(hWnd: v5, lpString: aLboxscroll_4);
        GetClientRect(hWnd: hLbox, lpRect: &lboxRect);
        GetClientRect(hWnd: hScroll, lpRect: &scrollRect);
        scrollRect.left += lboxRect.right;
        scrollRect.right += lboxRect.right;
        pt.x = (__int16)lparam;
        pt.y = SHIWORD(lparam);
        v14.y = SHIWORD(lparam);
        v14.x = (__int16)lparam;
        if ( PtInRect(lprc: &scrollRect, pt: v14) )
          return SendMessageA(
                   hWnd: hScroll,
                   Msg: message,
                   wParam: wparam,
                   lParam: ((unsigned __int16)(LOWORD(pt.y) - LOWORD(lboxRect.top)) << 16)
                 | (unsigned __int16)(LOWORD(pt.x) - LOWORD(lboxRect.right)));
        if ( !PtInRect(lprc: &lboxRect, pt) )
        {
          ReleaseCapture();
          v6 = GetParent(hWnd: hLbox);
          ClosePopup(this: v6);
          return 0;
        }
        break;
      default:
        break;
    }
  }
  else if ( message >= 0x104 )
  {
    FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
    SendMessageA(hWnd: FrameWindow, Msg: message, wParam: wparam, lParam: lparam);
  }
  else
  {
    switch ( message )
    {
      case 8u:
        ReleaseCapture();
        v13 = GetParent(hWnd: hLbox);
        ClosePopup(this: v13);
        return 0;
      case 0x87u:
        return 4;
      case 0x100u:
        if ( wparam == 27 || wparam == 13 )
        {
          v8 = GetParent(hWnd: hLbox);
          ClosePopup(this: v8);
          return 0;
        }
        if ( wparam == 9 )
        {
          v16 = GetKeyState(nVirtKey: 16) < 0;
          v15 = GetParent(hWnd: hLbox);
          v9 = GetParent(hWnd: hLbox);
          v10 = GetParent(hWnd: v9);
          NextDlgTabItem = GetNextDlgTabItem(hDlg: v10, hCtl: v15, bPrevious: v16);
          SetFocus(hWnd: NextDlgTabItem);
          return 0;
        }
        break;
      default:
        break;
    }
  }
  return CallWindowProcA(lpPrevWndFunc: oldProc, hWnd: hLbox, Msg: message, wParam: wparam, lParam: lparam);
}


// ==============================================================================
/*
 * EA: 0x1900A6F8
 * CodeView symbol: ComboDraw
 * Demangled: ComboDraw
 * IDA name: ComboDraw
 * CodeView module: 23
 * CV address: segment 1 : 0x96F8
 * Code length: 0x197
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall ComboDraw(HWND this)
{
  int v1; // eax
  HWND Parent; // eax
  int v3; // [esp-14h] [ebp-38h]
  int v4; // [esp-Ch] [ebp-30h]
  int v5; // [esp-8h] [ebp-2Ch]
  int pDsta; // [esp+4h] [ebp-20h]
  int pDst; // [esp+4h] [ebp-20h]
  struct tagRECT comborect; // [esp+8h] [ebp-1Ch] BYREF
  int test; // [esp+18h] [ebp-Ch]
  int *pComboBmp; // [esp+1Ch] [ebp-8h]
  int rows; // [esp+20h] [ebp-4h]

  for ( test = 0; test < 9; ++test )
  {
    if ( *(&sgComboSize.cy + 2 * test) == 0 || *(&sgComboSize.cx + 2 * test) == 0 )
      return;
  }
  GetClientRect(hWnd: this, lpRect: &comborect);
  pComboBmp = (int *)SMemAlloc(a1: 12, a2: aComboCpp_1, a3: 418, a4: 0);
  pComboBmp[1] = comborect.right;
  pComboBmp[2] = comborect.bottom;
  v1 = SMemAlloc(a1: comborect.bottom * comborect.right, a2: aComboCpp_2, a3: 421, a4: 0);
  *pComboBmp = v1;
  SetPropA(hWnd: this, lpString: aComboeditbkg, hData: pComboBmp);
  pDsta = *pComboBmp;
  Draw3Tiled(dstSize: pComboBmp[1], a2: pComboBmp[2], srcSize: (int)&sgComboSize);
  pDst = pComboBmp[1] * sgComboSize.cy + pDsta;
  for ( rows = (comborect.bottom - sgComboSize.cy - dword_190420D4) / dword_190420BC; rows != 0; --rows )
  {
    Draw3Tiled(dstSize: pComboBmp[1], a2: pComboBmp[2], srcSize: (int)&unk_190420B8);
    pDst += pComboBmp[1] * dword_190420BC;
  }
  Draw3Tiled(dstSize: pComboBmp[1], a2: pComboBmp[2], srcSize: (int)&unk_190420D0);
  v5 = pComboBmp[2];
  v4 = pComboBmp[1];
  v3 = *pComboBmp;
  Parent = GetParent(hWnd: this);
  SDlgSetBitmapI(a1: this, a2: Parent, a3: aCombobox, a4: -1, a5: 1, a6: v3, a7: 0, a8: v4, a9: v5, a10: -1);
}


// ==============================================================================
/*
 * EA: 0x1900A88F
 * CodeView symbol: Draw3Tiled
 * Demangled: Draw3Tiled
 * IDA name: Draw3Tiled
 * CodeView module: 23
 * CV address: segment 1 : 0x988F
 * Code length: 0x112
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall Draw3Tiled(int a1, _DWORD *a2, int dstSize, int a4, _DWORD *srcSize)
{
  int result; // eax
  int pDsta; // [esp+4h] [ebp-10h]
  int tiles; // [esp+8h] [ebp-Ch]
  int test; // [esp+Ch] [ebp-8h]

  for ( test = 0; test < 3; ++test )
  {
    if ( srcSize[2 * test] == 0 )
      return result;
    result = test + 1;
  }
  SBltROP3(a1, a2: *a2, a3: *srcSize, a4: srcSize[1], a5: dstSize, a6: *srcSize, a7: 0, a8: 13369376);
  pDsta = *srcSize + a1;
  for ( tiles = (dstSize - *srcSize - srcSize[4]) / srcSize[2]; tiles != 0; --tiles )
  {
    SBltROP3(a1: pDsta, a2: a2[1], a3: srcSize[2], a4: srcSize[3], a5: dstSize, a6: srcSize[2], a7: 0, a8: 13369376);
    pDsta += srcSize[2];
  }
  return SBltROP3(
           a1: pDsta,
           a2: a2[2],
           a3: srcSize[4],
           a4: srcSize[5],
           a5: dstSize,
           a6: srcSize[4],
           a7: 0,
           a8: 13369376);
}


// ==============================================================================
/*
 * EA: 0x1900A9A1
 * CodeView symbol: GetFontHeight
 * Demangled: GetFontHeight
 * IDA name: sub_1900A9A1
 * CodeView module: 23
 * CV address: segment 1 : 0x99A1
 * Code length: 0x86
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LONG __thiscall sub_1900A9A1(void *this)
{
  HWND DesktopWindow; // eax
  HWND v2; // eax
  LONG height; // [esp+4h] [ebp-48h]
  HGDIOBJ oldfont; // [esp+8h] [ebp-44h]
  HDC screendc; // [esp+Ch] [ebp-40h]
  HDC memdc; // [esp+10h] [ebp-3Ch]
  struct tagTEXTMETRICA tm; // [esp+14h] [ebp-38h] BYREF

  DesktopWindow = GetDesktopWindow();
  screendc = GetDC(hWnd: DesktopWindow);
  memdc = CreateCompatibleDC(hdc: screendc);
  oldfont = SelectObject(hdc: memdc, h: this);
  height = -1;
  if ( GetTextMetricsA(hdc: memdc, lptm: &tm) )
    height = tm.tmHeight;
  SelectObject(hdc: memdc, h: oldfont);
  DeleteDC(hdc: memdc);
  v2 = GetDesktopWindow();
  ReleaseDC(hWnd: v2, hDC: screendc);
  return height;
}


// ==============================================================================
/*
 * EA: 0x1900AA27
 * CodeView symbol: ComboDestroy
 * Demangled: ComboDestroy
 * IDA name: ComboDestroy
 * CodeView module: 23
 * CV address: segment 1 : 0x9A27
 * Code length: 0xB2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HANDLE __thiscall ComboDestroy(HWND this)
{
  HWND v1; // eax
  HWND v2; // eax
  HANDLE result; // eax
  _DWORD *pBmp; // [esp+4h] [ebp-8h]

  pBmp = RemovePropA(hWnd: this, lpString: aComboeditbkg_0);
  if ( pBmp != nullptr )
  {
    if ( *pBmp != 0 )
      SMemFree(a1: *pBmp, a2: aComboCpp_3, a3: 660, a4: 0);
    SMemFree(a1: pBmp, a2: aComboCpp_4, a3: 661, a4: 0);
  }
  v1 = (HWND)RemovePropA(hWnd: this, lpString: aComboedit_15);
  DestroyWindow(hWnd: v1);
  v2 = (HWND)RemovePropA(hWnd: this, lpString: aCombolbox_18);
  DestroyWindow(hWnd: v2);
  result = RemovePropA(hWnd: this, lpString: aComboarrowrct_1);
  if ( result != nullptr )
    return (HANDLE)SMemFree(a1: result, a2: aComboCpp_5, a3: 668, a4: 0);
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900AAD9
 * CodeView symbol: ComboUnregisterClass
 * Demangled: ComboUnregisterClass
 * IDA name: ?ComboUnregisterClass@@YIXXZ
 * CodeView module: 23
 * CV address: segment 1 : 0x9AD9
 * Code length: 0x28
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\COmbo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\COmbo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl ComboUnregisterClass()
{
  UnregisterClassA(lpClassName: aStormcombobox_0, hInstance: global_hinstance);
  UnregisterClassA(lpClassName: aCombostaticedi_1, hInstance: global_hinstance);
}

