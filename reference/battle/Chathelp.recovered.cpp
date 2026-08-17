/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chathelp.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chathelp.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 1
 */


// ==============================================================================
/*
 * EA: 0x19005250
 * CodeView symbol: ChatHelpDialogProc
 * Demangled: ChatHelpDialogProc
 * IDA name: ?ChatHelpDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 26
 * CV address: segment 1 : 0x4250
 * Code length: 0x3A9
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Chathelp.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Chathelp.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ChatHelpDialogProc(HWND window, UINT message, HDC wparam, HWND lparam)
{
  HWND FrameWindow; // eax
  void *pHelpText; // [esp+8h] [ebp-48h]
  HWND hWndEdit; // [esp+Ch] [ebp-44h]
  _DWORD btn_ids[3]; // [esp+10h] [ebp-40h] BYREF
  char szHelpRes[20]; // [esp+1Ch] [ebp-34h] BYREF
  const void *pText; // [esp+30h] [ebp-20h]
  signed int i; // [esp+34h] [ebp-1Ch]
  struct tagSIZE sizeBtns; // [esp+38h] [ebp-18h] BYREF
  HRSRC hRsrc; // [esp+40h] [ebp-10h]
  HWND hWndScroll; // [esp+44h] [ebp-Ch]
  _BYTE *pSearch; // [esp+48h] [ebp-8h]
  signed int nSize; // [esp+4Ch] [ebp-4h]

  if ( message > 0x110 )
  {
    if ( message == 273 )
    {
      if ( (_WORD)wparam != 0 && (unsigned __int16)wparam <= 2u )
      {
        if ( *(_DWORD *)(*(_DWORD *)(dword_19041D2C + 12) + 36) != 0 )
          (*(void (__stdcall **)(int, int, _DWORD))(*(_DWORD *)(dword_19041D2C + 12) + 36))(
            a1: 1112425812,
            a2: 1,
            a3: 0);
        SDlgEndDialog(a1: window, a2: 0);
        return 1;
      }
    }
    else if ( message != 312 )
    {
      return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
    }
    if ( GetWindowLongA(hWnd: lparam, nIndex: -12) == 1030 )
    {
      SetTextColor(hdc: wparam, color: 0xFFFFu);
      return (int)GetStockObject(i: 5);
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message != 272 )
  {
    if ( message > 0x53 )
    {
      if ( message >= 0x104 && message <= 0x105 )
      {
        FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
        SendMessageA(hWnd: FrameWindow, Msg: message, wParam: (WPARAM)wparam, lParam: (LPARAM)lparam);
      }
    }
    else
    {
      if ( message == 83 )
        return 1;
      if ( message == 2 )
      {
        if ( spHelpBmp != nullptr )
        {
          SMemFree(a1: spHelpBmp, a2: aChathelpCpp, a3: 62, a4: 0);
          spHelpBmp = nullptr;
        }
        if ( spHelpBtnsBmp != nullptr )
        {
          SMemFree(a1: spHelpBtnsBmp, a2: aChathelpCpp_0, a3: 67, a4: 0);
          spHelpBtnsBmp = nullptr;
        }
      }
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  btn_ids[0] = 1;
  btn_ids[1] = 2;
  btn_ids[2] = 0;
  dword_19041D2C = (int)lparam;
  UiLoadArtwork(
    a1: *(int (__stdcall **)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))(*((_DWORD *)lparam + 3) + 12),
    a2: window,
    hWndParent: nullptr,
    artid: 3u,
    controltype: byte_19041D38,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &spHelpBmp,
    a11: nullptr);
  UiLoadArtwork(
    a1: *(int (__stdcall **)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))(*(_DWORD *)(dword_19041D2C + 12) + 12),
    a2: nullptr,
    hWndParent: nullptr,
    artid: 5u,
    controltype: aButton_13,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 0,
    prepfadein: 0,
    a10: &spHelpBtnsBmp,
    a11: &sizeBtns);
  SDlgSetControlBitmaps(a1: window, a2: btn_ids, a3: 0, a4: spHelpBtnsBmp, a5: &sizeBtns, a6: 1, a7: -1);
  LocaleLoadString(a1: 0x3E8u, a2: szHelpRes, a3: 20);
  hRsrc = FindResourceA(hModule: global_hinstance, lpName: szHelpRes, lpType: Type);
  hResource = LoadResource(hModule: global_hinstance, hResInfo: hRsrc);
  pText = LockResource(hResData: hResource);
  nSize = SizeofResource(hModule: global_hinstance, hResInfo: hRsrc);
  pHelpText = (void *)SMemAlloc(a1: nSize + 1, a2: aChathelpCpp_1, a3: 132, a4: 0);
  if ( pHelpText == nullptr )
    return 1;
  memcpy(a1: pHelpText, Src: pText, Size: nSize);
  *((_BYTE *)pHelpText + nSize) = 0;
  setlocale(Category: 0, Locale: Locale);
  i = 0;
  pSearch = pHelpText;
  while ( i < nSize )
  {
    if ( isleadbyte(C: (char)*pSearch) != 0 )
    {
      ++i;
      ++pSearch;
    }
    else if ( *pSearch == 92 )
    {
      *pSearch = 0;
      break;
    }
    ++i;
    ++pSearch;
  }
  hWndScroll = GetDlgItem(hDlg: window, nIDDlgItem: 1052);
  hWndEdit = GetDlgItem(hDlg: window, nIDDlgItem: 1022);
  SendMessageA(hWnd: hWndEdit, Msg: 0xCu, wParam: 0, lParam: (LPARAM)pHelpText);
  SMemFree(a1: pHelpText, a2: aChathelpCpp_2, a3: 166, a4: 0);
  ScrollbarLink(a1: hWndEdit, a2: hWndScroll);
  EditUpdateScrollbar(a1: hWndEdit);
  return 1;
}

