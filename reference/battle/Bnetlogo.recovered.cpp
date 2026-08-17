/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Bnetlogo.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Bnetlogo.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 6
 */


// ==============================================================================
/*
 * EA: 0x19002770
 * CodeView symbol: KillChildWindows
 * Demangled: KillChildWindows
 * IDA name: ?KillChildWindows@@YIXPAUHWND__@@0@Z
 * CodeView module: 30
 * CV address: segment 1 : 0x1770
 * Code length: 0x71
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Bnetlogo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Bnetlogo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall KillChildWindows(HWND a1, HWND a2)
{
  int bKilledChat; // [esp+8h] [ebp-8h]
  HWND hWnd; // [esp+Ch] [ebp-4h]

  bKilledChat = 0;
  if ( a2 != nullptr )
  {
    for ( hWnd = a2; hWnd != nullptr && hWnd != a1; hWnd = GetParent(hWnd) )
    {
      if ( hWnd == ghWndChat )
        bKilledChat = 1;
      SDlgEndDialog(a1: hWnd, a2: 0);
    }
  }
  if ( bKilledChat == 0 )
    SDlgEndDialog(a1: ghWndChat, a2: 0);
}


// ==============================================================================
/*
 * EA: 0x190027E1
 * CodeView symbol: BattleNetDialogProc
 * Demangled: BattleNetDialogProc
 * IDA name: ?BattleNetDialogProc@@YGHPAUHWND__@@IIJ@Z
 * CodeView module: 30
 * CV address: segment 1 : 0x17E1
 * Code length: 0x42B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Bnetlogo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Bnetlogo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall BattleNetDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
  HMODULE ModuleHandleA; // eax
  int v5; // eax
  HWND DlgItem; // eax
  HWND ActiveWindow; // eax
  HWND Parent; // eax
  HWND FrameWindow; // eax
  HWND LastActivePopup; // eax
  HWND ChildDialog; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-194h]
  char szText[256]; // [esp+10h] [ebp-174h] BYREF
  char szTitle[32]; // [esp+110h] [ebp-74h] BYREF
  HWND hWndAd; // [esp+130h] [ebp-54h]
  _DWORD Interfacedata[20]; // [esp+134h] [ebp-50h] BYREF

  if ( message > 0x111 )
  {
    if ( message > 0x30F )
    {
      if ( message == 1127 )
      {
        LocaleLoadString(a1: 0x3F2u, a2: szTitle, a3: 32);
        LocaleLoadString(a1: 0x3F3u, a2: szText, a3: 256);
        UiMessageBox(
          a1: *(int (__stdcall **)(HWND, const char *, const char *, unsigned int))(*(_DWORD *)(dword_19041C84 + 12) + 32),
          a2: window,
          lpText: szText,
          a4: szTitle,
          a5: 0x10u);
        return 1;
      }
      if ( message == 1129 )
      {
        UiNotification();
        return 1;
      }
    }
    else
    {
      switch ( message )
      {
        case 0x30Fu:
          UpdateWindow(hWnd: window);
          ColorPrefActivate(a1: 1);
          break;
        case 0x112u:
          if ( wparam == 61536 )
          {
            ActiveWindow = GetActiveWindow();
            KillChildWindows(a1: window, a2: ActiveWindow);
            SDlgEndDialog(a1: window, a2: 0);
            Parent = GetParent(hWnd: window);
            PostMessageA(hWnd: Parent, Msg: message, wParam: 0xF060u, lParam: lparam);
            return 0;
          }
          break;
        case 0x113u:
          if ( wparam == 1 )
          {
            DlgItem = GetDlgItem(hDlg: window, nIDDlgItem: 1040);
            LogoAnimate(a1: window, a2: DlgItem);
            if ( gbConnectionLost != 0 )
            {
              gbConnectionLost = 0;
              UiConnectionLost(a1: *(struct _SNETUIDATA **)(dword_19041C84 + 12), a2: window);
            }
          }
          else if ( wparam == 2 )
          {
            SrvMaintainLatencies();
            SrvMaintainAds();
          }
          return 0;
        default:
          break;
      }
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message == 273 )
  {
    if ( (unsigned __int16)wparam == 1 )
    {
      hWndAd = nullptr;
      UpdateWindow(hWnd: window);
      if ( dword_19041C84 != 0 )
      {
        qmemcpy(Interfacedata, *(const void **)(dword_19041C84 + 12), sizeof(Interfacedata));
        Interfacedata[2] = window;
        Dialog = LocaleGetDialog(a1: 0x6Du);
        ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
        hWndAd = (HWND)SDlgCreateDialogIndirectParam(
                         a1: ModuleHandleA,
                         a2: Dialog,
                         a3: window,
                         a4: AdDialogProc,
                         a5: Interfacedata);
      }
      v5 = DoChatRoom(a1: dword_19041C84, a2: window);
      SDlgEndDialog(a1: window, a2: v5);
      if ( hWndAd != nullptr && IsWindow(hWnd: hWndAd) )
        DestroyWindow(hWnd: hWndAd);
      return 1;
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message > 0x1C )
  {
    if ( message >= 0x104 )
    {
      if ( message <= 0x105 )
      {
        FrameWindow = (HWND)SDrawGetFrameWindow(a1: 0);
        SendMessageA(hWnd: FrameWindow, Msg: message, wParam: wparam, lParam: lparam);
      }
      else if ( message == 272 )
      {
        dword_19041C84 = lparam;
        if ( lparam != 0 )
        {
          sub_19002C5A(a1: window, a2: *(_DWORD *)(*(_DWORD *)(dword_19041C84 + 12) + 12));
          LogoSetTimer(
            a1: window,
            a2: 1,
            a3: *(int (__stdcall **)(unsigned int, unsigned int, void *, unsigned int, unsigned int *))(*(_DWORD *)(dword_19041C84 + 12)
                                                                                                  + 44));
          SDlgSetTimer(a1: window, a2: 2, a3: 5000, a4: 0);
          PostMessageA(hWnd: window, Msg: 0x111u, wParam: 1u, lParam: 0);
          ghWndUiMainParent = window;
          UiLoadCursors(a1: window, a2: *(struct _SNETUIDATA **)(dword_19041C84 + 12));
          UiVidFade(a1: 1, a2: 1);
        }
        return 1;
      }
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( message != 28 )
  {
    if ( message == 2 )
    {
      UiVidFadeOut(a1: 10);
      SDlgKillTimer(a1: window, a2: 1);
      SDlgKillTimer(a1: window, a2: 2);
      sub_19002C0C(a1: window);
      ghWndUiMainParent = nullptr;
      return 0;
    }
    if ( message == 7 )
    {
      LastActivePopup = GetLastActivePopup(hWnd: window);
      SetFocus(hWnd: LastActivePopup);
      return 0;
    }
    return SDlgDefDialogProc(a1: window, a2: message, a3: wparam, a4: lparam);
  }
  if ( wparam != 0 )
  {
    ChildDialog = (HWND)GetChildDialog(a1: window);
    SetFocus(hWnd: ChildDialog);
  }
  else
  {
    ColorPrefActivate(a1: 0);
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x19002C0C
 * CodeView symbol: DestroyArtwork
 * Demangled: DestroyArtwork
 * IDA name: sub_19002C0C
 * CodeView module: 30
 * CV address: segment 1 : 0x1C0C
 * Code length: 0x4E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Bnetlogo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Bnetlogo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall sub_19002C0C(HWND this)
{
  HWND DlgItem; // eax

  DlgItem = GetDlgItem(hDlg: this, nIDDlgItem: 1040);
  LogoFramesDestroy(a1: DlgItem);
  if ( bitmap != nullptr )
  {
    SMemFree(a1: bitmap, a2: aBnetlogoCpp, a3: 62, a4: 0);
    bitmap = nullptr;
  }
  ScrollbarDestroyArtwork();
}


// ==============================================================================
/*
 * EA: 0x19002C5A
 * CodeView symbol: LoadArtwork
 * Demangled: LoadArtwork
 * IDA name: sub_19002C5A
 * CodeView module: 30
 * CV address: segment 1 : 0x1C5A
 * Code length: 0x66
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Bnetlogo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Bnetlogo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_19002C5A(
        HWND a1,
        int (__stdcall *a2)(unsigned int, unsigned int, struct tagPALETTEENTRY *, unsigned __int8 *, unsigned int, int *, int *, int *))
{
  HWND DlgItem; // eax
  unsigned __int8 *v4; // [esp-Ch] [ebp-1Ch]
  struct tagSIZE bgSize; // [esp+8h] [ebp-8h] BYREF

  UiLoadArtwork(
    a1: a2,
    a2: a1,
    hWndParent: nullptr,
    artid: 0,
    controltype: byte_19041C88,
    controlstyle: 0xFFFFFFFF,
    usageflags: 1,
    loadpalette: 1,
    prepfadein: 1,
    a10: &bitmap,
    a11: &bgSize);
  v4 = bitmap;
  DlgItem = GetDlgItem(hDlg: a1, nIDDlgItem: 1040);
  LogoInit(a1, a2: DlgItem, bitmap: v4, a4: &bgSize, a5: a2);
  ScrollbarLoadArtwork(a1: a2);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x19002CC0
 * CodeView symbol: DoChatRoom
 * Demangled: DoChatRoom
 * IDA name: DoChatRoom
 * CodeView module: 30
 * CV address: segment 1 : 0x1CC0
 * Code length: 0x80
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Bnetlogo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Bnetlogo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall DoChatRoom(const void *a1, int a2)
{
  HMODULE ModuleHandleA; // eax
  const struct DLGTEMPLATE *Dialog; // [esp-10h] [ebp-8Ch]
  _DWORD interfacedata[20]; // [esp+10h] [ebp-6Ch] BYREF
  unsigned int dialogid; // [esp+60h] [ebp-1Ch]
  const void *UIparams[6]; // [esp+64h] [ebp-18h] BYREF

  qmemcpy(UIparams, a1, sizeof(UIparams));
  qmemcpy(interfacedata, UIparams[3], sizeof(interfacedata));
  interfacedata[2] = a2;
  UIparams[3] = interfacedata;
  if ( (interfacedata[1] & 1) != 0 )
    dialogid = 114;
  else
    dialogid = 100;
  Dialog = LocaleGetDialog(a1: dialogid);
  ModuleHandleA = GetModuleHandleA(lpModuleName: nullptr);
  return SDlgDialogBoxIndirectParam(a1: ModuleHandleA, a2: Dialog, a3: a2, a4: ChatRoomDialogProc, a5: UIparams) == 1;
}


// ==============================================================================
/*
 * EA: 0x19002D40
 * CodeView symbol: GetChildDialog
 * Demangled: GetChildDialog
 * IDA name: GetChildDialog
 * CodeView module: 30
 * CV address: segment 1 : 0x1D40
 * Code length: 0x18
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Bnetlogo.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Bnetlogo.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
HWND GetChildDialog()
{
  return FindWindowA(lpClassName: ClassName, lpWindowName: nullptr);
}

