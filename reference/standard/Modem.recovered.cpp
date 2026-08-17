/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 *
 * CodeView module object:
 *   C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 65
 */


// ==============================================================================
/*
 * EA: 0x18004890
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: unknown_libname_4
 * CodeView module: 7
 * CV address: segment 1 : 0x3890
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_4()
{
  sub_1800489F();
  return sub_180048AE();
}


// ==============================================================================
/*
 * EA: 0x1800489F
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: sub_1800489F
 * CodeView module: 7
 * CV address: segment 1 : 0x389F
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
CCritSect *sub_1800489F()
{
  return CCritSect::CCritSect(this: (CCritSect *)&modem_critsect);
}


// ==============================================================================
/*
 * EA: 0x180048AE
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: sub_180048AE
 * CodeView module: 7
 * CV address: segment 1 : 0x38AE
 * Code length: 0x12
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_180048AE()
{
  return atexit(a1: sub_180048C0);
}


// ==============================================================================
/*
 * EA: 0x180048C0
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: sub_180048C0
 * CodeView module: 7
 * CV address: segment 1 : 0x38C0
 * Code length: 0xF
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_180048C0()
{
  CCritSect::~CCritSect(this: (CCritSect *)&modem_critsect);
}


// ==============================================================================
/*
 * EA: 0x180048CF
 * CodeView symbol: CancelCall
 * Demangled: CancelCall
 * IDA name: ?CancelCall@@YGHXZ
 * CodeView module: 7
 * CV address: segment 1 : 0x38CF
 * Code length: 0x41
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall CancelCall()
{
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  modem_callstatus = 1223;
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  TraceOut(format: format);
  SetEvent(hEvent: modem_TAPIHangupEvent);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18004910
 * CodeView symbol: RecvThreadProc
 * Demangled: RecvThreadProc
 * IDA name: ?RecvThreadProc@@YGIPAX@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x3910
 * Code length: 0x33A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall __noreturn RecvThreadProc(void *__formal)
{
  void *v1; // eax
  size_t v2; // [esp+0h] [ebp-20h]
  unsigned int bytesleft; // [esp+10h] [ebp-10h] BYREF
  unsigned __int8 *dataptr; // [esp+14h] [ebp-Ch] BYREF
  _DWORD *messageptr; // [esp+18h] [ebp-8h]
  int port; // [esp+1Ch] [ebp-4h]

  while ( modem_state != 1 )
  {
    TraceOut(format: aReadThreadAwak);
    CCritSect::Enter(this: (CCritSect *)&modem_critsect);
    for ( port = 0; port == 0; ++port )
    {
      if ( *(_DWORD *)modem_port[0] != -1 )
      {
        if ( GetOverlappedResult(
               hFile: *(HANDLE *)modem_port[port],
               lpOverlapped: (LPOVERLAPPED)(modem_port[port] + 4),
               lpNumberOfBytesTransferred: (LPDWORD)(modem_port[port] + 88),
               bWait: false) )
        {
          bytesleft = *(_DWORD *)(modem_port[port] + 88);
          dataptr = (unsigned __int8 *)(modem_port[port] + 24);
          while ( bytesleft != 0 )
          {
            messageptr = nullptr;
            if ( *(_DWORD *)(modem_port[port] + 92) != 0 )
            {
              TraceOut(format: aPartialMessage);
              messageptr = *(_DWORD **)(modem_port[port] + 92);
            }
            else if ( ScanForHeader(a1: &bytesleft, a2: &dataptr) != 0 )
            {
              v1 = (void *)SMemAlloc(a1: 220, a2: aModemCpp, a3: 951, a4: 8);
              messageptr = operator new(__formal: 220, ptr: v1);
              messageptr[53] = port;
              InsertHeaderKey(a1: messageptr + 4);
              messageptr[52] = 4;
              *(_DWORD *)(modem_port[port] + 92) = messageptr;
            }
            if ( bytesleft != 0 )
            {
              if ( messageptr[51] != 0 || (messageptr[51] = 4 * *dataptr, messageptr[51] <= 0xBCu) )
              {
                if ( bytesleft >= messageptr[51] - messageptr[52] )
                  v2 = messageptr[51] - messageptr[52];
                else
                  v2 = bytesleft;
                memcpy(a1: (char *)messageptr + messageptr[52] + 16, Src: dataptr, Size: v2);
                messageptr[52] += v2;
                dataptr += v2;
                bytesleft -= v2;
                if ( messageptr[52] >= messageptr[51] )
                {
                  sub_18004C4A(a1: port, a2: messageptr);
                  *(_DWORD *)(modem_port[port] + 92) = 0;
                }
              }
              else
              {
                TraceOut(format: aBogusPacket);
                SMemFree(a1: messageptr, a2: aModemCpp_0, a3: 968, a4: 0);
                *(_DWORD *)(modem_port[port] + 92) = 0;
              }
            }
          }
          ResetEvent(hEvent: *(&modem_event + port));
          ReadFile(
            hFile: *(HANDLE *)modem_port[port],
            lpBuffer: (LPVOID)(modem_port[port] + 24),
            nNumberOfBytesToRead: 0x40u,
            lpNumberOfBytesRead: (LPDWORD)(modem_port[port] + 88),
            lpOverlapped: (LPOVERLAPPED)(modem_port[port] + 4));
        }
        else if ( GetLastError() == 995 )
        {
          ResetEvent(hEvent: *(&modem_event + port));
        }
      }
    }
    CCritSect::Leave(this: (CCritSect *)&modem_critsect);
    WaitForMultipleObjects(nCount: 1u, lpHandles: &modem_event, bWaitAll: false, dwMilliseconds: 0xFFFFFFFF);
  }
  modem_state = 0;
  _endthreadex(ReturnCode: 0);
}


// ==============================================================================
/*
 * EA: 0x18004C4A
 * CodeView symbol: ProcessIncomingMessage
 * Demangled: ProcessIncomingMessage
 * IDA name: sub_18004C4A
 * CodeView module: 7
 * CV address: segment 1 : 0x3C4A
 * Code length: 0x439
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_18004C4A(int a1, int a2)
{
  int result; // eax
  int v5; // [esp+Ch] [ebp-1Ch]
  int localnetworkid; // [esp+10h] [ebp-18h] BYREF
  int routeloopcheck; // [esp+14h] [ebp-14h]
  int networkid; // [esp+18h] [ebp-10h]
  int advinfo; // [esp+1Ch] [ebp-Ch]
  BOOL systemmessage; // [esp+20h] [ebp-8h]
  int origtargetmask; // [esp+24h] [ebp-4h]

  systemmessage = *(unsigned __int8 *)(a2 + 21) == 1;
  LOBYTE(origtargetmask) = *(_BYTE *)(a2 + 22);
  TraceOut(format: aProcessincomin);
  PerfIncrement(a1: 1u);
  PerfAdd(a1: 3u, a2: *(_DWORD *)(a2 + 208));
  if ( a1 != 0 )
    return SMemFree(a1: a2, a2: aModemCpp_1, a3: 513, a4: 0);
  if ( modem_networkid != 0
    && modem_networkidlocked != 0
    && (((1 << modem_networkid) & (unsigned __int8)origtargetmask) == 0
     || !systemmessage && (*(_BYTE *)(a2 + 23) & 0xF) == modem_networkid) )
  {
    return SMemFree(a1: a2, a2: aModemCpp_2, a3: 522, a4: 0);
  }
  if ( systemmessage )
  {
    TraceOut(format: aSystemMessage);
    switch ( *(_BYTE *)(a2 + 23) & 0xF )
    {
      case 1:
        TraceOut(format: aRcvSysQueryid);
        if ( *(_DWORD *)(a2 + 208) >= 0x10u )
        {
          networkid = *(_DWORD *)(a2 + 24);
          routeloopcheck = *(_DWORD *)(a2 + 28);
          if ( routeloopcheck != modem_routeloopcheck )
          {
            if ( modem_networkidlocked != 0 )
            {
              TraceOut(format: aSndSysAssertid);
              localnetworkid = (unsigned __int8)modem_networkid;
              SendDataMessage(data: (int)&localnetworkid, databytes: 4);
            }
            else if ( networkid == (unsigned __int8)modem_networkid )
            {
              modem_networkid = 0;
            }
          }
        }
        break;
      case 2:
        TraceOut(format: aRcvSysAssertid);
        if ( *(_DWORD *)(a2 + 208) >= 0xCu )
        {
          v5 = *(_DWORD *)(a2 + 24);
          *(_BYTE *)(modem_port[0] + 96) |= 1 << v5;
          if ( modem_networkidlocked == 0 && v5 == (unsigned __int8)modem_networkid )
            modem_networkid = 0;
        }
        break;
      case 3:
        TraceOut(format: aRcvSysQuerylin);
        TraceOut(format: aSndSysAssertli);
        SendDataMessage(data: 0, databytes: 0);
        CCritSect::Enter(this: (CCritSect *)&modem_critsect);
        modem_lineconfirmed = 1;
        CCritSect::Leave(this: (CCritSect *)&modem_critsect);
        break;
      case 4:
        TraceOut(format: aRcvSysAssertli);
        CCritSect::Enter(this: (CCritSect *)&modem_critsect);
        modem_lineconfirmed = 1;
        CCritSect::Leave(this: (CCritSect *)&modem_critsect);
        break;
      case 9:
        TraceOut(format: aRcvSysQuerygam);
        if ( modem_gameadvinfo != nullptr )
        {
          TraceOut(format: aSndSysGameinfo);
          SendDataMessage(data: (int)modem_gameadvinfo, databytes: *((_DWORD *)modem_gameadvinfo + 133));
        }
        break;
      case 0xA:
        TraceOut(format: aRcvSysGameinfo);
        sub_180053E0(a1: a2 + 24, a2: *(_DWORD *)(a2 + 208) - 8);
        break;
      case 0xB:
        TraceOut(format: aRcvSysRemove);
        advinfo = a2 + 24;
        if ( (unsigned __int8)modem_networkid != *(_DWORD *)(a2 + 24) && modem_gamelist != nullptr )
        {
          if ( *((_DWORD *)modem_gamelist + 75) != 0 )
            SMemFree(a1: *((_DWORD *)modem_gamelist + 75), a2: aModemCpp_4, a3: 605, a4: 0);
          *((_DWORD *)modem_gamelist + 75) = 0;
          SMemFree(a1: modem_gamelist, a2: aModemCpp_5, a3: 607, a4: 0);
          modem_gamelist = nullptr;
        }
        break;
      default:
        return SMemFree(a1: a2, a2: aModemCpp_6, a3: 612, a4: 0);
    }
    return SMemFree(a1: a2, a2: aModemCpp_6, a3: 612, a4: 0);
  }
  else
  {
    TraceOut(format: aUserMessage);
    if ( modem_networkid != 0 && modem_networkidlocked != 0 )
    {
      *(_BYTE *)a2 = *(_BYTE *)(a2 + 23) & 0xF;
      result = TListAddPtrEnd(a1: &modem_messagehead, (struct _MESSAGEREC *)a2);
      if ( modem_recvevent != nullptr )
        return SetEvent(hEvent: modem_recvevent);
    }
    else
    {
      return SMemFree(a1: a2, a2: aModemCpp_3, a3: 536, a4: 0);
    }
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x18005083
 * CodeView symbol: SendDataMessage
 * Demangled: SendDataMessage
 * IDA name: SendDataMessage
 * CodeView module: 7
 * CV address: segment 1 : 0x4083
 * Code length: 0x14F
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall SendDataMessage(char a1, char a2, const void *data, size_t databytes)
{
  _BYTE messagedata[188]; // [esp+8h] [ebp-C0h] BYREF
  BOOL systemmessage; // [esp+C4h] [ebp-4h]

  systemmessage = a1 != 0;
  if ( a1 != 0 )
    a2 = -1;
  if ( modem_lineestablished == 0 )
    return 0;
  if ( a1 == 0 && (modem_networkid == 0 || modem_networkidlocked == 0) )
    return 0;
  memset(a1: messagedata, Val: 0, Size: sizeof(messagedata));
  InsertHeaderKey(a1: messagedata);
  messagedata[5] = a1 != 0;
  messagedata[4] = (databytes + 11) >> 2;
  messagedata[7] = messagedata[7] & 0xF | 0x20;
  messagedata[6] = a2;
  if ( systemmessage )
    messagedata[7] = a1 & 0xF | messagedata[7] & 0xF0;
  else
    messagedata[7] = modem_networkid & 0xF | messagedata[7] & 0xF0;
  if ( data != nullptr && databytes != 0 )
    memcpy(a1: &messagedata[8], Src: data, Size: databytes);
  sub_180051D2(wait: 1);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x180051D2
 * CodeView symbol: SendFormedMessage
 * Demangled: SendFormedMessage
 * IDA name: sub_180051D2
 * CodeView module: 7
 * CV address: segment 1 : 0x41D2
 * Code length: 0x20E
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_180051D2(unsigned __int8 *a1, int a2, int wait)
{
  int result; // eax
  DWORD v4; // ecx
  int port; // [esp+Ch] [ebp-2Ch]
  HANDLE event; // [esp+10h] [ebp-28h] BYREF
  BOOL bIOResult; // [esp+14h] [ebp-24h]
  _BYTE byteswritten[24]; // [esp+18h] [ebp-20h] BYREF
  BOOL systemmessage; // [esp+30h] [ebp-8h]
  int numsends; // [esp+34h] [ebp-4h]

  systemmessage = a1[5] == 1;
  numsends = 0;
  result = (int)memset(a1: &event, Val: 0, Size: sizeof(event));
  for ( port = 0; port == 0; port = 1 )
  {
    if ( a2 != 0
      && modem_port[0] != 0
      && *(_DWORD *)modem_port[0] != -1
      && (systemmessage || (a1[6] & *(_BYTE *)(modem_port[0] + 96)) != 0) )
    {
      memset(a1: &byteswritten[20 * numsends + 4], Val: 0, Size: 0x14u);
      if ( wait != 0 )
        *(&event + numsends) = CreateEventA(
                                 lpEventAttributes: nullptr,
                                 bManualReset: false,
                                 bInitialState: false,
                                 lpName: nullptr);
      if ( *(&event + numsends) == nullptr )
        SErrDisplayError(a1: -2062548992, a2: aModemCpp_7, a3: 324, a4: aEventNumsends, a5: 0, a6: 1);
      result = numsends;
      if ( *(&event + numsends) == nullptr )
        return result;
      *(_DWORD *)&byteswritten[20 * numsends + 20] = *(&event + numsends);
      bIOResult = WriteFile(
                    hFile: *(HANDLE *)modem_port[0],
                    lpBuffer: a1,
                    nNumberOfBytesToWrite: 4 * a1[4],
                    lpNumberOfBytesWritten: (LPDWORD)&byteswritten[4 * numsends],
                    lpOverlapped: (LPOVERLAPPED)&byteswritten[20 * numsends + 4]);
      if ( bIOResult || GetLastError() == 997 )
      {
        PerfIncrement(a1: 0);
        PerfAdd(a1: 2u, a2: 4 * a1[4]);
        ++numsends;
      }
      else if ( wait != 0 && *(&event + numsends) != nullptr )
      {
        CloseHandle(hObject: *(&event + numsends));
      }
    }
    result = 1;
  }
  if ( numsends != 0 && wait != 0 )
    result = WaitForMultipleObjects(nCount: numsends, lpHandles: &event, bWaitAll: true, dwMilliseconds: 0xFFFFFFFF);
  while ( 1 )
  {
    v4 = numsends--;
    if ( v4 == 0 )
      break;
    result = CloseHandle(hObject: *(&event + numsends));
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x180053E0
 * CodeView symbol: RecvGameInfoPacket
 * Demangled: RecvGameInfoPacket
 * IDA name: sub_180053E0
 * CodeView module: 7
 * CV address: segment 1 : 0x43E0
 * Code length: 0x2DD
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
unsigned int __fastcall sub_180053E0(_DWORD *a1, unsigned int a2)
{
  unsigned int result; // eax
  void *v3; // eax
  int v4; // eax
  int v5; // eax
  unsigned int packetbytes; // [esp+18h] [ebp-1Ch]
  unsigned __int16 checksum; // [esp+1Ch] [ebp-18h]
  unsigned int bytesleft; // [esp+20h] [ebp-14h]
  int byteslefta; // [esp+20h] [ebp-14h]
  unsigned int bytesleftb; // [esp+20h] [ebp-14h]
  unsigned int bytesleftc; // [esp+20h] [ebp-14h]
  signed int bytesleftd; // [esp+20h] [ebp-14h]
  char *currptr; // [esp+24h] [ebp-10h]
  char *currptra; // [esp+24h] [ebp-10h]
  unsigned int *currptrb; // [esp+24h] [ebp-10h]
  unsigned __int16 *currptrc; // [esp+24h] [ebp-10h]
  const void *currptrd; // [esp+24h] [ebp-10h]
  char *temp_gamelist; // [esp+30h] [ebp-4h]

  result = 16;
  if ( a2 >= 0x10 )
  {
    if ( a1[1] != modem_programid )
    {
      modem_versionmismatch = 1;
      return result;
    }
    v3 = (void *)SMemAlloc(a1: 316, a2: aModemCpp_8, a3: 414, a4: 8);
    temp_gamelist = (char *)operator new(__formal: 60, ptr: v3);
    temp_gamelist[12] = *(_BYTE *)a1;
    *(_DWORD *)temp_gamelist = 1;
    *((_DWORD *)temp_gamelist + 78) = a1[2];
    *((_DWORD *)temp_gamelist + 77) = a1[1];
    bytesleft = a2 - 14;
    currptr = (char *)(a1 + 3);
    if ( (int)(a2 - 14) >= 128 )
      v4 = SStrCopy(a1: temp_gamelist + 40, a2: currptr, a3: 128);
    else
      v4 = SStrCopy(a1: temp_gamelist + 40, a2: currptr, a3: bytesleft);
    currptra = &currptr[v4 + 1];
    byteslefta = bytesleft - (v4 + 1);
    if ( byteslefta >= 128 )
      v5 = SStrCopy(a1: temp_gamelist + 168, a2: currptra, a3: 128);
    else
      v5 = SStrCopy(a1: temp_gamelist + 168, a2: currptra, a3: byteslefta);
    currptrb = (unsigned int *)&currptra[v5 + 1];
    result = byteslefta - (v5 + 1);
    if ( result >= 4 )
    {
      if ( result < 6 )
        goto LABEL_22;
      packetbytes = *currptrb;
      currptrc = (unsigned __int16 *)(currptrb + 1);
      bytesleftb = result - 4;
      result = a2 >> 2;
      if ( (packetbytes + 3) >> 2 != a2 >> 2
        || ((bytesleftc = packetbytes - a2 + bytesleftb) & 0x80000000) != 0
        || (checksum = *currptrc,
            *currptrc = 0,
            currptrd = currptrc + 1,
            bytesleftd = bytesleftc - 2,
            result = ComputeChecksum(a1, a2: packetbytes),
            checksum != result) )
      {
LABEL_22:
        if ( temp_gamelist != nullptr )
          return SMemFree(a1: temp_gamelist, a2: aModemCpp_12, a3: 498, a4: 0);
        return result;
      }
      if ( bytesleftd > 0 )
      {
        *((_DWORD *)temp_gamelist + 75) = SMemAlloc(a1: bytesleftd, a2: aModemCpp_9, a3: 482, a4: 0);
        *((_DWORD *)temp_gamelist + 76) = bytesleftd;
        result = (unsigned int)memcpy(a1: *((void **)temp_gamelist + 75), Src: currptrd, Size: bytesleftd);
      }
    }
    if ( modem_gamelist != nullptr )
    {
      if ( *((_DWORD *)modem_gamelist + 75) != 0 )
        SMemFree(a1: *((_DWORD *)modem_gamelist + 75), a2: aModemCpp_10, a3: 490, a4: 0);
      *((_DWORD *)modem_gamelist + 75) = 0;
      result = SMemFree(a1: modem_gamelist, a2: aModemCpp_11, a3: 492, a4: 0);
    }
    modem_gamelist = temp_gamelist;
    temp_gamelist = nullptr;
    goto LABEL_22;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x180056BD
 * CodeView symbol: InsertHeaderKey
 * Demangled: InsertHeaderKey
 * IDA name: InsertHeaderKey
 * CodeView module: 7
 * CV address: segment 1 : 0x46BD
 * Code length: 0x5B
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall InsertHeaderKey(void *this)
{
  void *result; // eax

  if ( this == nullptr )
    result = (void *)SErrDisplayError(a1: -2062548992, a2: aModemCpp_13, a3: 886, a4: aHeader, a5: 0, a6: 1);
  if ( this != nullptr )
    return memcpy(a1: this, Src: &gs_arrHeaderKeys, Size: 4u);
  return result;
}


// ==============================================================================
/*
 * EA: 0x18005718
 * CodeView symbol: ScanForHeader
 * Demangled: ScanForHeader
 * IDA name: ScanForHeader
 * CodeView module: 7
 * CV address: segment 1 : 0x4718
 * Code length: 0xF6
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ScanForHeader(_DWORD *a1, _DWORD *a2)
{
  if ( a1 == nullptr || *a2 == 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_14, a3: 894, a4: aBytesleftDatap, a5: 0, a6: 1);
  if ( a1 == nullptr || *a2 == 0 )
    return 0;
  while ( *a1 != 0 && matchlevel < 4 )
  {
    if ( *(unsigned __int8 *)*a2 == (unsigned __int8)gs_arrHeaderKeys[matchlevel] )
    {
      ++matchlevel;
      goto LABEL_13;
    }
    if ( matchlevel != 0 )
    {
      matchlevel = 0;
      TraceOut(format: aResetMatch);
    }
    else
    {
LABEL_13:
      --*a1;
      ++*a2;
    }
  }
  if ( matchlevel != 4 )
    return 0;
  matchlevel = 0;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800580E
 * CodeView symbol: ModemCompareNetAddresses
 * Demangled: ModemCompareNetAddresses
 * IDA name: ?ModemCompareNetAddresses@@YGHPAU_SNETADDR@@0PAK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x480E
 * Code length: 0x4F
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemCompareNetAddresses(struct _SNETADDR *addr1, struct _SNETADDR *addr2, unsigned int *diffmagnitude)
{
  if ( diffmagnitude != nullptr )
    *diffmagnitude = 0;
  if ( addr1 != nullptr && addr2 != nullptr )
  {
    *diffmagnitude = memcmp(Buf1: addr1, Buf2: addr2, Size: 0x10u) != 0;
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1800585D
 * CodeView symbol: ModemDestroy
 * Demangled: ModemDestroy
 * IDA name: ?ModemDestroy@@YGHXZ
 * CodeView module: 7
 * CV address: segment 1 : 0x485D
 * Code length: 0xA9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemDestroy()
{
  TraceOut(format: aModemdestroy);
  ModemStopAdvertisingGame();
  ShutdownTAPI();
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  TListClear(a1: &modem_messagehead, a2: aModemCpp_15, a3: 1759);
  if ( modem_gamelist != nullptr )
  {
    SMemFree(a1: modem_gamelist, a2: aModemCpp_16, a3: 1763, a4: 0);
    *((_DWORD *)modem_gamelist + 75) = 0;
    SMemFree(a1: modem_gamelist, a2: aModemCpp_17, a3: 1765, a4: 0);
    modem_gamelist = nullptr;
  }
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18005906
 * CodeView symbol: ShutdownTAPI
 * Demangled: ShutdownTAPI
 * IDA name: ShutdownTAPI
 * CodeView module: 7
 * CV address: segment 1 : 0x4906
 * Code length: 0x242
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int ShutdownTAPI()
{
  if ( modem_tapihangupthread != nullptr )
  {
    ResetEvent(hEvent: modem_TAPIHangupNotify);
    SetEvent(hEvent: modem_TAPIHangupEvent);
    WaitForSingleObject(hHandle: modem_TAPIHangupNotify, dwMilliseconds: 0xFFFFFFFF);
  }
  SetEvent(hEvent: modem_TAPITerminateEvent);
  if ( modem_tapithread != nullptr )
    WaitForSingleObject(hHandle: modem_tapithread, dwMilliseconds: 0xFFFFFFFF);
  if ( modem_tapianswerthread != nullptr )
    WaitForSingleObject(hHandle: modem_tapianswerthread, dwMilliseconds: 0xFFFFFFFF);
  if ( modem_tapihangupthread != nullptr )
    WaitForSingleObject(hHandle: modem_tapihangupthread, dwMilliseconds: 0xFFFFFFFF);
  if ( modem_readthread != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_18, a3: 1027, a4: aModemReadthrea, a5: 0, a6: 1);
  if ( modem_TAPITerminateEvent != nullptr )
  {
    CloseHandle(hObject: modem_TAPITerminateEvent);
    modem_TAPITerminateEvent = nullptr;
  }
  if ( modem_TAPIEvent != nullptr )
  {
    CloseHandle(hObject: modem_TAPIEvent);
    modem_TAPIEvent = nullptr;
  }
  if ( modem_TAPINotifyEvent != nullptr )
  {
    CloseHandle(hObject: modem_TAPINotifyEvent);
    modem_TAPINotifyEvent = nullptr;
  }
  if ( modem_TAPIHangupEvent != nullptr )
  {
    CloseHandle(hObject: modem_TAPIHangupEvent);
    modem_TAPIHangupEvent = nullptr;
  }
  if ( modem_TAPIHangupNotify != nullptr )
  {
    CloseHandle(hObject: modem_TAPIHangupNotify);
    modem_TAPIHangupNotify = nullptr;
  }
  if ( modem_TAPIAnswerEvent != nullptr )
  {
    CloseHandle(hObject: modem_TAPIAnswerEvent);
    modem_TAPIAnswerEvent = nullptr;
  }
  if ( modem_TAPICallStateEvent != nullptr )
  {
    CloseHandle(hObject: modem_TAPICallStateEvent);
    modem_TAPICallStateEvent = nullptr;
  }
  if ( modem_TAPILineReplyEvent != nullptr )
  {
    CloseHandle(hObject: modem_TAPILineReplyEvent);
    modem_TAPILineReplyEvent = nullptr;
  }
  if ( modem_tapithread != nullptr )
  {
    CloseHandle(hObject: modem_tapithread);
    modem_tapithread = nullptr;
  }
  if ( modem_tapianswerthread != nullptr )
  {
    CloseHandle(hObject: modem_tapianswerthread);
    modem_tapianswerthread = nullptr;
  }
  if ( modem_tapihangupthread != nullptr )
  {
    CloseHandle(hObject: modem_tapihangupthread);
    modem_tapihangupthread = nullptr;
  }
  if ( g_hLineApp != 0 )
  {
    lineShutdown(hLineApp: g_hLineApp);
    g_hLineApp = 0;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18005B48
 * CodeView symbol: ModemFree
 * Demangled: ModemFree
 * IDA name: ?ModemFree@@YGHPAU_SNETADDR@@PAXK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x4B48
 * Code length: 0x38
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemFree(struct _SNETADDR *addr, void *data, char databytes)
{
  if ( addr != nullptr && data != nullptr )
  {
    SMemFree(a1: addr, a2: aModemCpp_19, a3: 1782, a4: 0);
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18005B80
 * CodeView symbol: ModemFreeExternalMessage
 * Demangled: ModemFreeExternalMessage
 * IDA name: ?ModemFreeExternalMessage@@YGHPBD00@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x4B80
 * Code length: 0x10
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemFreeExternalMessage(const char *senderpath, const char *sendername, const char *mesage)
{
  SErrSetLastError(a1: 87);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18005B90
 * CodeView symbol: ModemGetGameInfo
 * Demangled: ModemGetGameInfo
 * IDA name: ?ModemGetGameInfo@@YGHKPBD0PAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x4B90
 * Code length: 0x17A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall ModemGetGameInfo(
        void *this,
        unsigned int gameid,
        const char *gamename,
        const char *gamepassword,
        struct _SNETSPI_GAMELIST *gameinfo)
{
  if ( gamename == nullptr || gameinfo == nullptr || gameid == 0 && *gamename == 0 )
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
  memset(a1: gameinfo, Val: 0, Size: 0x13Cu);
  if ( modem_lineestablished == 0 )
  {
    UpdateCallStatus(a1: 10, a2: this);
    modem_networkidlocked = 0;
    if ( MakeCall(a1: gamename) == 0 || WaitForCallState(a1: 256, a2: -1) != 0 )
    {
      SetEvent(hEvent: modem_TAPIHangupEvent);
      SErrSetLastError(a1: modem_callstatus);
      return 0;
    }
    this = (void *)GetTickCount();
    while ( modem_gamelist == nullptr && GetTickCount() - (unsigned int)this < 0x3A98 )
      Sleep(dwMilliseconds: 0xAu);
  }
  if ( modem_versionmismatch != 0 )
  {
    SetEvent(hEvent: modem_TAPIHangupEvent);
    SErrSetLastError(a1: -2062548871);
    return 0;
  }
  else
  {
    UpdateCallStatus(a1: 7, a2: this);
    CCritSect::Enter(this: (CCritSect *)&modem_critsect);
    if ( modem_gamelist != nullptr )
      memcpy(a1: gameinfo, Src: modem_gamelist, Size: 0x13Cu);
    CCritSect::Leave(this: (CCritSect *)&modem_critsect);
    if ( *(_DWORD *)gameinfo != 0 )
    {
      return 1;
    }
    else
    {
      SetEvent(hEvent: modem_TAPIHangupEvent);
      SErrSetLastError(a1: -2062548888);
      return 0;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x18005D0A
 * CodeView symbol: UpdateCallStatus
 * Demangled: UpdateCallStatus
 * IDA name: UpdateCallStatus
 * CodeView module: 7
 * CV address: segment 1 : 0x4D0A
 * Code length: 0x5D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall UpdateCallStatus(void *this)
{
  char buf[128]; // [esp+4h] [ebp-80h] BYREF

  if ( modem_status != nullptr )
  {
    if ( LocaleLoadString(a1: (unsigned int)this, a2: buf, a3: 128) == 0 )
      SStrCopy(a1: buf, a2: asc_18028858, a3: 128);
    modem_status(a1: buf, a2: 0, a3: 0, a4: 0, a5: CancelCall);
  }
}


// ==============================================================================
/*
 * EA: 0x18005D67
 * CodeView symbol: WaitForCallState
 * Demangled: WaitForCallState
 * IDA name: WaitForCallState
 * CodeView module: 7
 * CV address: segment 1 : 0x4D67
 * Code length: 0x6B
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall WaitForCallState(int a1, DWORD a2)
{
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  g_dwDesiredCallState = a1;
  ResetEvent(hEvent: modem_TAPICallStateEvent);
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  if ( WaitForSingleObject(hHandle: modem_TAPICallStateEvent, dwMilliseconds: a2) == 258 )
    return 2;
  else
    return g_dwDesiredCallState != a1;
}


// ==============================================================================
/*
 * EA: 0x18005DD2
 * CodeView symbol: MakeCall
 * Demangled: MakeCall
 * IDA name: MakeCall
 * CodeView module: 7
 * CV address: segment 1 : 0x4DD2
 * Code length: 0x1DD
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __thiscall MakeCall(void *this)
{
  struct linecallparams_tag *v2; // eax
  LONG v3; // eax
  HDC v4; // [esp+0h] [ebp-24h]
  int v5; // [esp+4h] [ebp-20h]
  const char *lpDialString; // [esp+Ch] [ebp-18h]
  DWORD dwTAPIVersion; // [esp+10h] [ebp-14h] BYREF
  int lpLineDevCaps; // [esp+14h] [ebp-10h]
  int lpOutput; // [esp+18h] [ebp-Ch]
  LONG lResult; // [esp+1Ch] [ebp-8h]
  struct linecallparams_tag *lpCallParams; // [esp+20h] [ebp-4h]

  lpLineDevCaps = 0;
  lpOutput = 0;
  modem_callstatus = 1204;
  if ( GetDeviceAPIVersion(a1: g_dwDeviceID, a2: &dwTAPIVersion) == 0 )
    return false;
  lpLineDevCaps = GetDeviceCaps(hdc: v4, index: v5);
  if ( lpLineDevCaps == 0 )
    return false;
  if ( (*(_DWORD *)(lpLineDevCaps + 236) & 8) != 0 )
  {
    SMemFree(a1: lpLineDevCaps, a2: aModemCpp_21, a3: 1344, a4: 0);
    CCritSect::Enter(this: (CCritSect *)&modem_critsect);
    lResult = lineOpen(
                hLineApp: g_hLineApp,
                dwDeviceID: g_dwDeviceID,
                lphLine: &g_hLine,
                dwAPIVersion: dwTAPIVersion,
                dwExtVersion: 0,
                dwCallbackInstance: 0,
                dwPrivileges: 1u,
                dwMediaModes: 0x10u,
                lpCallParams: nullptr);
    CCritSect::Leave(this: (CCritSect *)&modem_critsect);
    if ( lResult != 0 )
    {
      return false;
    }
    else
    {
      v2 = (struct linecallparams_tag *)CreateCallParams(a1: this);
      lpCallParams = v2;
      if ( v2 != nullptr )
      {
        lpOutput = TranslateAddress(szAddress: (int)this);
        if ( lpOutput == 0 )
          SErrDisplayError(a1: -2062548992, a2: aModemCpp_22, a3: 1362, a4: aLpoutput, a5: 0, a6: 1);
        if ( lpOutput != 0 )
        {
          lpDialString = (const char *)(*(_DWORD *)(lpOutput + 16) + lpOutput);
          TraceOut(format: "Dialing %s", lpDialString);
          v3 = lineMakeCall(
                 hLine: g_hLine,
                 lphCall: &g_hCall,
                 lpszDestAddress: lpDialString,
                 dwCountryCode: 0,
                 lpCallParams);
          lResult = WaitForReply(a1: v3, a2: -1);
          if ( lpCallParams != nullptr )
            LocalFree(hMem: lpCallParams);
          if ( lpOutput != 0 )
            SMemFree(a1: lpOutput, a2: aModemCpp_23, a3: 1378, a4: 0);
          return lResult == 0;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
  }
  else
  {
    SMemFree(a1: lpLineDevCaps, a2: aModemCpp_20, a3: 1341, a4: 0);
    return false;
  }
}


// ==============================================================================
/*
 * EA: 0x18005FAF
 * CodeView symbol: WaitForReply
 * Demangled: WaitForReply
 * IDA name: WaitForReply
 * CodeView module: 7
 * CV address: segment 1 : 0x4FAF
 * Code length: 0x63
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall WaitForReply(int a1, DWORD a2)
{
  if ( a1 <= 0 )
    return a1;
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  g_lRequestedID = a1;
  g_lAsyncReply = -1;
  ResetEvent(hEvent: modem_TAPILineReplyEvent);
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  WaitForSingleObject(hHandle: modem_TAPILineReplyEvent, dwMilliseconds: a2);
  return g_lAsyncReply;
}


// ==============================================================================
/*
 * EA: 0x18006012
 * CodeView symbol: GetDeviceAPIVersion
 * Demangled: GetDeviceAPIVersion
 * IDA name: GetDeviceAPIVersion
 * CodeView module: 7
 * CV address: segment 1 : 0x5012
 * Code length: 0x75
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall GetDeviceAPIVersion(DWORD a1, DWORD *a2)
{
  struct lineextensionid_tag lineExtId; // [esp+Ch] [ebp-10h] BYREF

  if ( g_hLineApp == 0 || g_dwNumDevices == 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_24, a3: 1232, a4: aGHlineappGDwnu, a5: 0, a6: 1);
  return lineNegotiateAPIVersion(
           hLineApp: g_hLineApp,
           dwDeviceID: a1,
           dwAPILowVersion: 0x10004u,
           dwAPIHighVersion: 0x10004u,
           lpdwAPIVersion: a2,
           lpExtensionID: &lineExtId) == 0;
}


// ==============================================================================
/*
 * EA: 0x18006087
 * CodeView symbol: GetDeviceCaps
 * Demangled: GetDeviceCaps
 * IDA name: GetDeviceCaps
 * CodeView module: 7
 * CV address: segment 1 : 0x5087
 * Code length: 0x107
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall GetDeviceCaps(HDC hdc, int index)
{
  DWORD v2; // edx
  DWORD v3; // ecx
  DWORD dwTAPIVersion; // [esp+4h] [ebp-FCh]
  DWORD id; // [esp+8h] [ebp-F8h]
  struct linedevcaps_tag *lpLineDevCaps; // [esp+Ch] [ebp-F4h]
  int tmpLineDevCaps; // [esp+10h] [ebp-F0h] BYREF
  DWORD v9; // [esp+14h] [ebp-ECh]

  dwTAPIVersion = v2;
  id = v3;
  if ( g_hLineApp == 0 || g_dwNumDevices == 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_25, a3: 1247, a4: aGHlineappGDwnu_0, a5: 0, a6: 1);
  tmpLineDevCaps = 240;
  if ( lineGetDevCaps(
         hLineApp: g_hLineApp,
         dwDeviceID: id,
         dwAPIVersion: dwTAPIVersion,
         dwExtVersion: 0,
         lpLineDevCaps: (LPLINEDEVCAPS)&tmpLineDevCaps) != 0 )
    return 0;
  lpLineDevCaps = (struct linedevcaps_tag *)SMemAlloc(a1: v9, a2: aModemCpp_26, a3: 1253, a4: 0);
  lpLineDevCaps->dwTotalSize = v9;
  if ( lineGetDevCaps(hLineApp: g_hLineApp, dwDeviceID: id, dwAPIVersion: dwTAPIVersion, dwExtVersion: 0, lpLineDevCaps) == 0 )
    return (int)lpLineDevCaps;
  SMemFree(a1: lpLineDevCaps, a2: aModemCpp_27, a3: 1256, a4: 0);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1800618E
 * CodeView symbol: CreateCallParams
 * Demangled: CreateCallParams
 * IDA name: CreateCallParams
 * CodeView module: 7
 * CV address: segment 1 : 0x518E
 * Code length: 0x108
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall CreateCallParams(const char *this)
{
  size_t v1; // eax
  _DWORD *lpCallParams; // [esp+Ch] [ebp-8h]
  size_t dwAddressSize; // [esp+10h] [ebp-4h]

  if ( this == nullptr || *this == 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_28, a3: 1267, a4: aSzaddressSzadd, a5: 0, a6: 1);
  v1 = strlen(Str: this);
  dwAddressSize = v1 + 1;
  lpCallParams = LocalAlloc(uFlags: 0x40u, uBytes: v1 + 113);
  if ( lpCallParams == nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_29, a3: 1273, a4: aLpcallparams, a5: 0, a6: 1);
  *lpCallParams = dwAddressSize + 112;
  lpCallParams[1] = 1;
  lpCallParams[4] = 16;
  lpCallParams[5] = 2;
  lpCallParams[6] = 1;
  lpCallParams[15] = 112;
  lpCallParams[14] = dwAddressSize;
  SStrCopy(a1: lpCallParams + 28, a2: this, a3: dwAddressSize - 1);
  return lpCallParams;
}


// ==============================================================================
/*
 * EA: 0x18006296
 * CodeView symbol: TranslateAddress
 * Demangled: TranslateAddress
 * IDA name: TranslateAddress
 * CodeView module: 7
 * CV address: segment 1 : 0x5296
 * Code length: 0xF9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
struct linetranslateoutput_tag *__fastcall TranslateAddress(DWORD a1, DWORD a2, const CHAR *szAddress)
{
  struct linetranslateoutput_tag *lpOutput; // [esp+Ch] [ebp-Ch]
  unsigned __int64 dwSize; // [esp+10h] [ebp-8h]

  if ( g_hLineApp == 0 || g_dwNumDevices == 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_30, a3: 1299, a4: aGHlineappGDwnu_1, a5: 0, a6: 1);
  LODWORD(dwSize) = 40;
  lpOutput = nullptr;
  do
  {
    if ( lpOutput != nullptr )
    {
      LODWORD(dwSize) = lpOutput->dwNeededSize;
      SMemFree(a1: lpOutput, a2: aModemCpp_31, a3: 1309, a4: 0);
    }
    lpOutput = (struct linetranslateoutput_tag *)SMemAlloc(a1: dwSize, a2: aModemCpp_32, a3: 1311, a4: 0);
    lpOutput->dwTotalSize = dwSize;
    HIDWORD(dwSize) = lineTranslateAddress(
                        hLineApp: g_hLineApp,
                        dwDeviceID: a1,
                        dwAPIVersion: a2,
                        lpszAddressIn: szAddress,
                        dwCard: 0,
                        dwTranslateOptions: 0,
                        lpTranslateOutput: lpOutput);
  }
  while ( dwSize < lpOutput->dwNeededSize );
  if ( HIDWORD(dwSize) == 0 )
    return lpOutput;
  SMemFree(a1: lpOutput, a2: aModemCpp_33, a3: 1318, a4: 0);
  return nullptr;
}


// ==============================================================================
/*
 * EA: 0x1800638F
 * CodeView symbol: ModemGetPerformanceData
 * Demangled: ModemGetPerformanceData
 * IDA name: ?ModemGetPerformanceData@@YGHKPAKPAT_LARGE_INTEGER@@1@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x538F
 * Code length: 0x1A
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemGetPerformanceData(
        unsigned int counterid,
        unsigned int *countervalue,
        union _LARGE_INTEGER *measurementtime,
        union _LARGE_INTEGER *measurementfreq)
{
  return PerfGetPerformanceData(a1: counterid, a2: countervalue, measurementtime, measurementfreq);
}


// ==============================================================================
/*
 * EA: 0x180063A9
 * CodeView symbol: ModemInitialize
 * Demangled: ModemInitialize
 * IDA name: ?ModemInitialize@@YGHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAX@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x53A9
 * Code length: 0x7B
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemInitialize(
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        void *event)
{
  int v6; // [esp+0h] [ebp-4h]

  modem_programid = *((_DWORD *)programdata + 3);
  modem_versionid = *((_DWORD *)programdata + 4);
  if ( *((_DWORD *)programdata + 6) >= 2u )
    v6 = 2;
  else
    v6 = *((_DWORD *)programdata + 6);
  modem_maxplayers = v6;
  modem_recvevent = event;
  PerfReset();
  LocaleInitialize(a1: programdata);
  if ( StartupTAPI() != 0 )
    return SelectName(a1: programdata, a2: playerdata, interfacedata, versiondata);
  else
    return 0;
}


// ==============================================================================
/*
 * EA: 0x18006424
 * CodeView symbol: StartupTAPI
 * Demangled: StartupTAPI
 * IDA name: StartupTAPI
 * CodeView module: 7
 * CV address: segment 1 : 0x5424
 * Code length: 0x59F
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int StartupTAPI()
{
  unsigned int v1; // [esp+44h] [ebp-Ch] BYREF
  unsigned int ThrdAddr; // [esp+48h] [ebp-8h] BYREF
  unsigned int threadid; // [esp+4Ch] [ebp-4h] BYREF

  if ( g_hLineApp != 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_34, a3: 1054, a4: aGHlineapp, a5: 0, a6: 1);
  if ( modem_TAPITerminateEvent != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_35, a3: 1060, a4: aModemTapitermi, a5: 0, a6: 1);
  modem_TAPITerminateEvent = CreateEventA(
                               lpEventAttributes: nullptr,
                               bManualReset: true,
                               bInitialState: false,
                               lpName: nullptr);
  if ( modem_TAPITerminateEvent == nullptr )
    return 0;
  if ( modem_TAPIEvent != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_36, a3: 1061, a4: aModemTapievent, a5: 0, a6: 1);
  modem_TAPIEvent = CreateEventA(lpEventAttributes: nullptr, bManualReset: true, bInitialState: false, lpName: nullptr);
  if ( modem_TAPIEvent == nullptr )
    return 0;
  if ( modem_TAPINotifyEvent != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_37, a3: 1062, a4: aModemTapinotif, a5: 0, a6: 1);
  modem_TAPINotifyEvent = CreateEventA(
                            lpEventAttributes: nullptr,
                            bManualReset: true,
                            bInitialState: false,
                            lpName: nullptr);
  if ( modem_TAPINotifyEvent == nullptr )
    return 0;
  if ( modem_TAPIAnswerEvent != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_38, a3: 1063, a4: aModemTapianswe, a5: 0, a6: 1);
  modem_TAPIAnswerEvent = CreateEventA(
                            lpEventAttributes: nullptr,
                            bManualReset: true,
                            bInitialState: false,
                            lpName: nullptr);
  if ( modem_TAPIAnswerEvent == nullptr )
    return 0;
  if ( modem_TAPIHangupEvent != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_39, a3: 1064, a4: aModemTapihangu, a5: 0, a6: 1);
  modem_TAPIHangupEvent = CreateEventA(
                            lpEventAttributes: nullptr,
                            bManualReset: true,
                            bInitialState: false,
                            lpName: nullptr);
  if ( modem_TAPIHangupEvent == nullptr )
    return 0;
  if ( modem_TAPIHangupNotify != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_40, a3: 1065, a4: aModemTapihangu_0, a5: 0, a6: 1);
  modem_TAPIHangupNotify = CreateEventA(
                             lpEventAttributes: nullptr,
                             bManualReset: true,
                             bInitialState: false,
                             lpName: nullptr);
  if ( modem_TAPIHangupNotify == nullptr )
    return 0;
  if ( modem_TAPICallStateEvent != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_41, a3: 1066, a4: aModemTapicalls, a5: 0, a6: 1);
  modem_TAPICallStateEvent = CreateEventA(
                               lpEventAttributes: nullptr,
                               bManualReset: true,
                               bInitialState: false,
                               lpName: nullptr);
  if ( modem_TAPICallStateEvent == nullptr )
    return 0;
  if ( modem_TAPILineReplyEvent != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_42, a3: 1067, a4: aModemTapiliner, a5: 0, a6: 1);
  modem_TAPILineReplyEvent = CreateEventA(
                               lpEventAttributes: nullptr,
                               bManualReset: true,
                               bInitialState: false,
                               lpName: nullptr);
  if ( modem_TAPILineReplyEvent == nullptr )
    return 0;
  if ( modem_tapithread != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_43, a3: 1073, a4: aModemTapithrea, a5: 0, a6: 1);
  modem_tapithread = (HANDLE)_beginthreadex(
                               Security: nullptr,
                               StackSize: 0,
                               StartAddress: TAPIProc,
                               ArgList: nullptr,
                               InitFlag: 0,
                               ThrdAddr: &threadid);
  if ( modem_tapithread == nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_44, a3: 1078, a4: aModemTapithrea_0, a5: 0, a6: 1);
  if ( modem_tapithread != nullptr )
  {
    WaitForSingleObject(hHandle: modem_TAPINotifyEvent, dwMilliseconds: 0xFFFFFFFF);
    if ( g_hLineApp != 0 )
    {
      if ( modem_tapianswerthread != nullptr )
        SErrDisplayError(a1: -2062548992, a2: aModemCpp_45, a3: 1094, a4: aModemTapianswe_0, a5: 0, a6: 1);
      modem_tapianswerthread = (HANDLE)_beginthreadex(
                                         Security: nullptr,
                                         StackSize: 0,
                                         StartAddress: TAPIAnswerProc,
                                         ArgList: nullptr,
                                         InitFlag: 0,
                                         &ThrdAddr);
      if ( modem_tapianswerthread == nullptr )
        SErrDisplayError(a1: -2062548992, a2: aModemCpp_46, a3: 1099, a4: aModemTapianswe_1, a5: 0, a6: 1);
      if ( modem_tapianswerthread != nullptr )
      {
        if ( modem_TAPIHangupEvent == nullptr )
          SErrDisplayError(a1: -2062548992, a2: aModemCpp_47, a3: 1108, a4: aModemTapihangu_1, a5: 0, a6: 1);
        if ( modem_TAPIHangupNotify == nullptr )
          SErrDisplayError(a1: -2062548992, a2: aModemCpp_48, a3: 1109, a4: aModemTapihangu_2, a5: 0, a6: 1);
        if ( modem_tapihangupthread != nullptr )
          SErrDisplayError(a1: -2062548992, a2: aModemCpp_49, a3: 1110, a4: aModemTapihangu_3, a5: 0, a6: 1);
        modem_tapihangupthread = (HANDLE)_beginthreadex(
                                           Security: nullptr,
                                           StackSize: 0,
                                           StartAddress: TAPIHangupProc,
                                           ArgList: nullptr,
                                           InitFlag: 0,
                                           ThrdAddr: &v1);
        if ( modem_tapihangupthread == nullptr )
          SErrDisplayError(a1: -2062548992, a2: aModemCpp_50, a3: 1115, a4: aModemTapihangu_4, a5: 0, a6: 1);
        if ( modem_tapihangupthread != nullptr )
        {
          return 1;
        }
        else
        {
          ShutdownTAPI();
          SErrSetLastError(a1: 164);
          return 0;
        }
      }
      else
      {
        ShutdownTAPI();
        SErrSetLastError(a1: 164);
        return 0;
      }
    }
    else
    {
      ShutdownTAPI();
      SErrSetLastError(a1: 1204);
      return 0;
    }
  }
  else
  {
    ShutdownTAPI();
    SErrSetLastError(a1: 164);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x180069C3
 * CodeView symbol: TAPIProc
 * Demangled: TAPIProc
 * IDA name: TAPIProc
 * CodeView module: 7
 * CV address: segment 1 : 0x59C3
 * Code length: 0x107
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
unsigned int __stdcall TAPIProc(void *__formal)
{
  DWORD dwEvent; // [esp+4h] [ebp-2Ch]
  struct tagMSG msg; // [esp+8h] [ebp-28h] BYREF
  HANDLE events[2]; // [esp+24h] [ebp-Ch] BYREF
  LONG v5; // [esp+2Ch] [ebp-4h]

  events[0] = modem_TAPITerminateEvent;
  events[1] = modem_TAPIEvent;
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  v5 = lineInitialize(
         lphLineApp: &g_hLineApp,
         hInstance: global_hinstance,
         lpfnCallback: TAPIEventCallback,
         lpszAppName: nullptr,
         lpdwNumDevs: &g_dwNumDevices);
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  if ( v5 != 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_51, a3: 800, a4: aResult, a5: 0, a6: 1);
  SetEvent(hEvent: modem_TAPINotifyEvent);
  do
  {
    if ( PeekMessageA(lpMsg: &msg, hWnd: nullptr, wMsgFilterMin: 0, wMsgFilterMax: 0, wRemoveMsg: 1u) )
    {
      TranslateMessage(lpMsg: &msg);
      DispatchMessageA(lpMsg: &msg);
    }
    ResetEvent(hEvent: modem_TAPIEvent);
    dwEvent = MsgWaitForMultipleObjects(
                nCount: 2u,
                pHandles: events,
                fWaitAll: false,
                dwMilliseconds: 0xFFFFFFFF,
                dwWakeMask: 0xFFu);
    TraceOut(format: aTapiprocAwake);
  }
  while ( dwEvent != -1 && dwEvent != 0 );
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18006ACA
 * CodeView symbol: TAPIEventCallback
 * Demangled: TAPIEventCallback
 * IDA name: TAPIEventCallback
 * CodeView module: 7
 * CV address: segment 1 : 0x5ACA
 * Code length: 0x91
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __stdcall TAPIEventCallback(
        DWORD hDevice,
        DWORD dwMessage,
        DWORD_PTR dwInstance,
        DWORD_PTR dwParam1,
        DWORD_PTR dwParam2,
        DWORD_PTR dwParam3)
{
  switch ( dwMessage )
  {
    case 2u:
      HandleLineCallState(dwCallbackInstance: dwInstance, dwParam1, dwParam2, dwParam3);
      break;
    case 3u:
      TraceOut(format: aLineClose);
      break;
    case 0xCu:
      TraceOut(format: aLineReply);
      CCritSect::Enter(this: (CCritSect *)&modem_critsect);
      if ( g_lRequestedID == dwParam1 )
      {
        g_lAsyncReply = dwParam2;
        SetEvent(hEvent: modem_TAPILineReplyEvent);
      }
      CCritSect::Leave(this: (CCritSect *)&modem_critsect);
      break;
    default:
      break;
  }
}


// ==============================================================================
/*
 * EA: 0x18006B5B
 * CodeView symbol: HandleLineCallState
 * Demangled: HandleLineCallState
 * IDA name: HandleLineCallState
 * CodeView module: 7
 * CV address: segment 1 : 0x5B5B
 * Code length: 0x229
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall HandleLineCallState(
        void *this,
        char dwCallbackInstance,
        unsigned int dwParam1,
        unsigned int dwParam2,
        char dwParam3)
{
  g_dwCallState = dwParam1;
  if ( dwParam1 == g_dwDesiredCallState )
    SetEvent(hEvent: modem_TAPICallStateEvent);
  TraceOut(format: "LINE_CALLSTATE: %d", g_dwCallState);
  if ( dwParam1 <= 0x40 )
  {
    switch ( dwParam1 )
    {
      case 0x40u:
        TraceOut(format: aLinecallstateB);
        break;
      case 1u:
        TraceOut(format: aLinecallstateI);
        break;
      case 2u:
        CCritSect::Enter(this: (CCritSect *)&modem_critsect);
        g_hCall = (HCALL)this;
        CCritSect::Leave(this: (CCritSect *)&modem_critsect);
        SetEvent(hEvent: modem_TAPIAnswerEvent);
        break;
      case 8u:
        UpdateCallStatus(this: (void *)9);
        break;
      default:
        break;
    }
    return;
  }
  switch ( dwParam1 )
  {
    case 0x100u:
      TraceOut(format: aLinecallstateC);
      StartCom();
      UpdateCallStatus(this: (void *)7);
      return;
    case 0x200u:
      UpdateCallStatus(this: (void *)8);
      return;
    case 0x4000u:
      if ( dwParam2 > 0x100 )
      {
        if ( dwParam2 == 512 )
        {
LABEL_27:
          CCritSect::Enter(this: (CCritSect *)&modem_critsect);
          modem_callstatus = 54;
          CCritSect::Leave(this: (CCritSect *)&modem_critsect);
          goto LABEL_31;
        }
        if ( dwParam2 != 2048 && dwParam2 != 4096 )
          goto LABEL_30;
        CCritSect::Enter(this: (CCritSect *)&modem_critsect);
        modem_callstatus = 1222;
        CCritSect::Leave(this: (CCritSect *)&modem_critsect);
      }
      else
      {
        if ( dwParam2 == 256 )
        {
LABEL_29:
          CCritSect::Enter(this: (CCritSect *)&modem_critsect);
          modem_callstatus = 1232;
          CCritSect::Leave(this: (CCritSect *)&modem_critsect);
          goto LABEL_31;
        }
        if ( dwParam2 != 1 )
        {
          if ( dwParam2 != 32 )
          {
            if ( dwParam2 != 128 )
            {
LABEL_30:
              CCritSect::Enter(this: (CCritSect *)&modem_critsect);
              modem_callstatus = 2250;
              CCritSect::Leave(this: (CCritSect *)&modem_critsect);
              goto LABEL_31;
            }
            goto LABEL_29;
          }
          goto LABEL_27;
        }
        TraceOut(format: aLinedisconnect);
      }
LABEL_31:
      TraceOut(format: "LINEDISCONNECTMODE_?, modem_callstatus: %d", modem_callstatus);
      SetEvent(hEvent: modem_TAPIHangupEvent);
      break;
    default:
      break;
  }
}


// ==============================================================================
/*
 * EA: 0x18006D84
 * CodeView symbol: TAPIAnswerProc
 * Demangled: TAPIAnswerProc
 * IDA name: TAPIAnswerProc
 * CodeView module: 7
 * CV address: segment 1 : 0x5D84
 * Code length: 0x78
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
unsigned int __stdcall TAPIAnswerProc(void *__formal)
{
  HCALL v1; // esi
  LONG v2; // eax
  DWORD dwEvent; // [esp+4h] [ebp-Ch]
  HANDLE events[2]; // [esp+8h] [ebp-8h] BYREF

  events[0] = modem_TAPITerminateEvent;
  events[1] = modem_TAPIAnswerEvent;
  while ( 1 )
  {
    ResetEvent(hEvent: modem_TAPIAnswerEvent);
    dwEvent = WaitForMultipleObjects(nCount: 2u, lpHandles: events, bWaitAll: false, dwMilliseconds: 0xFFFFFFFF);
    if ( dwEvent == -1 || dwEvent == 0 )
      break;
    v1 = g_hCall;
    v2 = lineAnswer(hCall: g_hCall, lpsUserUserInfo: nullptr, dwSize: 0);
    WaitForAnswer(a1: v2, a2: v1);
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18006DFC
 * CodeView symbol: WaitForAnswer
 * Demangled: WaitForAnswer
 * IDA name: WaitForAnswer
 * CodeView module: 7
 * CV address: segment 1 : 0x5DFC
 * Code length: 0x62
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall WaitForAnswer(int a1)
{
  if ( a1 <= 0 )
    return a1;
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  g_lRequestedID = a1;
  g_lAsyncReply = -1;
  ResetEvent(hEvent: modem_TAPILineReplyEvent);
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  WaitForSingleObject(hHandle: modem_TAPILineReplyEvent, dwMilliseconds: 0xFFFFFFFF);
  return g_lAsyncReply;
}


// ==============================================================================
/*
 * EA: 0x18006E5E
 * CodeView symbol: TAPIHangupProc
 * Demangled: TAPIHangupProc
 * IDA name: TAPIHangupProc
 * CodeView module: 7
 * CV address: segment 1 : 0x5E5E
 * Code length: 0x77
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
unsigned int __stdcall TAPIHangupProc(void *__formal)
{
  DWORD dwEvent; // [esp+0h] [ebp-Ch]
  HANDLE events[2]; // [esp+4h] [ebp-8h] BYREF

  events[0] = modem_TAPITerminateEvent;
  events[1] = modem_TAPIHangupEvent;
  while ( 1 )
  {
    ResetEvent(hEvent: modem_TAPIHangupEvent);
    dwEvent = WaitForMultipleObjects(nCount: 2u, lpHandles: events, bWaitAll: false, dwMilliseconds: 0xFFFFFFFF);
    if ( dwEvent == -1 || dwEvent == 0 )
      break;
    HangupCall();
    SetEvent(hEvent: modem_TAPIHangupNotify);
    if ( modem_gameadvinfo != nullptr )
      TakeCall();
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18006ED5
 * CodeView symbol: HangupCall
 * Demangled: HangupCall
 * IDA name: HangupCall
 * CodeView module: 7
 * CV address: segment 1 : 0x5ED5
 * Code length: 0x1AA
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int HangupCall()
{
  LONG v0; // eax
  int v2; // [esp+4h] [ebp-Ch]
  int status; // [esp+8h] [ebp-8h] BYREF
  int lineidle; // [esp+Ch] [ebp-4h]

  TraceOut(format: aHangupcall);
  StopCom();
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  g_dwDesiredCallState = 0x4000;
  SetEvent(hEvent: modem_TAPICallStateEvent);
  SetEvent(hEvent: modem_TAPILineReplyEvent);
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  if ( g_hCall != 0 )
  {
    if ( GetCallStatus(a1: g_hCall, a2: &status) != 0 )
      v2 = 0;
    else
      v2 = *(_DWORD *)(status + 12) & 1;
    lineidle = v2;
    SMemFree(a1: status, a2: aModemCpp_52, a3: 1198, a4: 0);
    if ( lineidle == 0 )
    {
      v0 = lineDrop(hCall: g_hCall, lpsUserUserInfo: nullptr, dwSize: 0);
      WaitForReply(a1: v0, a2: 0x2710u);
    }
    CCritSect::Enter(this: (CCritSect *)&modem_critsect);
    lineDeallocateCall(hCall: g_hCall);
    g_hCall = 0;
    CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  }
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  if ( g_hLine != 0 )
  {
    lineClose(hLine: g_hLine);
    g_hLine = 0;
  }
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  if ( modem_gamelist != nullptr )
  {
    if ( *((_DWORD *)modem_gamelist + 75) != 0 )
      SMemFree(a1: *((_DWORD *)modem_gamelist + 75), a2: aModemCpp_53, a3: 1221, a4: 0);
    *((_DWORD *)modem_gamelist + 75) = 0;
    SMemFree(a1: modem_gamelist, a2: aModemCpp_54, a3: 1223, a4: 0);
    modem_gamelist = nullptr;
  }
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1800707F
 * CodeView symbol: GetCallStatus
 * Demangled: GetCallStatus
 * IDA name: GetCallStatus
 * CodeView module: 7
 * CV address: segment 1 : 0x607F
 * Code length: 0xC1
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall GetCallStatus(HCALL a1, struct linecallstatus_tag **a2)
{
  struct linecallstatus_tag *status; // [esp+Ch] [ebp-Ch]
  unsigned __int64 statussize; // [esp+10h] [ebp-8h]

  if ( a2 == nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_55, a3: 1157, a4: aStatusptr, a5: 0, a6: 1);
  status = nullptr;
  LODWORD(statussize) = 36;
  do
  {
    if ( status != nullptr )
    {
      LODWORD(statussize) = status->dwNeededSize;
      SMemFree(a1: status, a2: aModemCpp_56, a3: 1165, a4: 0);
    }
    status = (struct linecallstatus_tag *)SMemAlloc(a1: statussize, a2: aModemCpp_57, a3: 1167, a4: 0);
    status->dwTotalSize = statussize;
    HIDWORD(statussize) = lineGetCallStatus(hCall: a1, lpCallStatus: status);
  }
  while ( statussize < status->dwNeededSize );
  *a2 = status;
  return HIDWORD(statussize);
}


// ==============================================================================
/*
 * EA: 0x18007140
 * CodeView symbol: TakeCall
 * Demangled: TakeCall
 * IDA name: TakeCall
 * CodeView module: 7
 * CV address: segment 1 : 0x6140
 * Code length: 0xB1
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL TakeCall()
{
  DWORD dwTAPIVersion; // [esp+4h] [ebp-Ch] BYREF
  LONG lResult; // [esp+8h] [ebp-8h]

  TraceOut(format: aTakecall);
  if ( !GetDeviceAPIVersion(a1: g_dwDeviceID, a2: &dwTAPIVersion) )
    return false;
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  lResult = lineOpen(
              hLineApp: g_hLineApp,
              dwDeviceID: g_dwDeviceID,
              lphLine: &g_hLine,
              dwAPIVersion: dwTAPIVersion,
              dwExtVersion: 0,
              dwCallbackInstance: 0,
              dwPrivileges: 4u,
              dwMediaModes: 0x10u,
              lpCallParams: nullptr);
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  if ( lResult != 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_58, a3: 1397, a4: aLresult, a5: 0, a6: 1);
  return lResult == 0;
}


// ==============================================================================
/*
 * EA: 0x180071F1
 * CodeView symbol: StopCom
 * Demangled: StopCom
 * IDA name: StopCom
 * CodeView module: 7
 * CV address: segment 1 : 0x61F1
 * Code length: 0x186
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void StopCom()
{
  int port; // [esp+0h] [ebp-4h]
  int porta; // [esp+0h] [ebp-4h]

  TraceOut(format: aStopcom);
  if ( modem_lineestablished != 0 )
  {
    modem_state = 1;
    for ( port = 0; port == 0; port = 1 )
    {
      if ( modem_port[0] != 0 && *(_DWORD *)modem_port[0] != -1 )
        SetEvent(hEvent: *(HANDLE *)(modem_port[0] + 20));
    }
    WaitForSingleObject(hHandle: modem_readthread, dwMilliseconds: 0xFFFFFFFF);
    CloseHandle(hObject: modem_readthread);
    modem_readthread = nullptr;
    for ( porta = 0; porta == 0; porta = 1 )
    {
      if ( modem_event != nullptr )
      {
        CloseHandle(hObject: modem_event);
        modem_event = nullptr;
      }
      if ( modem_port[0] != 0 )
      {
        if ( *(_DWORD *)modem_port[0] != -1 )
          CloseHandle(hObject: *(HANDLE *)modem_port[0]);
        if ( *(_DWORD *)(modem_port[0] + 92) != 0 )
          SMemFree(a1: *(_DWORD *)(modem_port[0] + 92), a2: aModemCpp_59, a3: 1526, a4: 0);
        SMemFree(a1: modem_port[0], a2: aModemCpp_60, a3: 1527, a4: 0);
        modem_port[0] = 0;
      }
    }
    modem_lineconfirmed = 0;
    modem_lineestablished = 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18007377
 * CodeView symbol: StartCom
 * Demangled: StartCom
 * IDA name: StartCom
 * CodeView module: 7
 * CV address: segment 1 : 0x6377
 * Code length: 0x1F7
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int StartCom()
{
  int v1; // [esp+0h] [ebp-14h]
  unsigned int threadid; // [esp+8h] [ebp-Ch] BYREF
  DWORD starttime; // [esp+Ch] [ebp-8h]
  int loop; // [esp+10h] [ebp-4h]

  TraceOut(format: aStartcom);
  if ( modem_lineestablished != 0 )
    return 1;
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  for ( loop = 0; loop == 0; ++loop )
  {
    if ( sub_18007766(a1: 0) == 0 )
    {
      SErrSetLastError(a1: 8);
      SetEvent(hEvent: modem_TAPIHangupEvent);
      return 0;
    }
  }
  if ( modem_readthread != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_61, a3: 1552, a4: aModemReadthrea_0, a5: 0, a6: 1);
  if ( modem_readthread == nullptr )
  {
    modem_readthread = (HANDLE)_beginthreadex(
                                 Security: nullptr,
                                 StackSize: 0,
                                 StartAddress: (_beginthreadex_proc_type)RecvThreadProc,
                                 ArgList: nullptr,
                                 InitFlag: 0,
                                 ThrdAddr: &threadid);
    if ( modem_readthread == nullptr )
      SErrDisplayError(a1: -2062548992, a2: aModemCpp_62, a3: 1561, a4: aModemReadthrea_1, a5: 0, a6: 1);
    v1 = 0;
    if ( modem_readthread == nullptr )
    {
      SetEvent(hEvent: modem_TAPIHangupEvent);
      SErrSetLastError(a1: 164);
      return 0;
    }
    SetThreadPriority(hThread: modem_readthread, nPriority: 2);
  }
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  modem_lineestablished = 1;
  SendDataMessage(a1: 3, a2: 255, data: nullptr, databytes: 0);
  starttime = GetTickCount();
  while ( modem_lineconfirmed == 0 && GetTickCount() - starttime < 0x2710 )
    Sleep(dwMilliseconds: 0xAu);
  if ( modem_lineconfirmed == 0 )
    return 0;
  TraceOut(format: aFindingId);
  if ( sub_1800756E(a1: v1, a2: 0) != 0 )
  {
    TraceOut(format: aSendingSysQuer);
    SendDataMessage(a1: 9, a2: 255, data: nullptr, databytes: 0);
    return 1;
  }
  else
  {
    TraceOut(format: aFindingIdFaile);
    SetEvent(hEvent: modem_TAPIHangupEvent);
    SErrSetLastError(a1: 68);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x1800756E
 * CodeView symbol: FindNetworkId
 * Demangled: FindNetworkId
 * IDA name: sub_1800756E
 * CodeView module: 7
 * CV address: segment 1 : 0x656E
 * Code length: 0x162
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1800756E()
{
  int v0; // eax
  DWORD starttime; // [esp+0h] [ebp-14h]
  _DWORD messagedata[2]; // [esp+4h] [ebp-10h] BYREF
  int localnetworkid; // [esp+Ch] [ebp-8h] BYREF
  int timeout; // [esp+10h] [ebp-4h]

  timeout = 16;
  while ( 1 )
  {
    v0 = timeout--;
    if ( v0 == 0 )
      break;
    for ( modem_routeloopcheck = 0; modem_routeloopcheck == 0; modem_routeloopcheck = sub_180076D0() )
      ;
    if ( modem_networkidlocked == 0 )
    {
      CCritSect::Enter(this: (CCritSect *)&modem_critsect);
      for ( modem_networkid = 0; modem_networkid == 0; modem_networkid = sub_180076D0() % 2u + 1 )
        ;
      CCritSect::Leave(this: (CCritSect *)&modem_critsect);
      messagedata[0] = (unsigned __int8)modem_networkid;
      messagedata[1] = modem_routeloopcheck;
      TraceOut(format: aSndSysQueryid);
      SendDataMessage(a1: 1, a2: 255, data: messagedata, databytes: 8u);
      starttime = GetTickCount();
      while ( modem_networkid != 0 && GetTickCount() - starttime < 0x3E8 )
        Sleep(dwMilliseconds: 0xAu);
      if ( modem_networkid == 0 )
        continue;
    }
    CCritSect::Enter(this: (CCritSect *)&modem_critsect);
    modem_networkidlocked = 1;
    modem_routeloopcheck = 0;
    CCritSect::Leave(this: (CCritSect *)&modem_critsect);
    TraceOut(format: "Snd: SYS_ASSERTID %d", (unsigned __int8)modem_networkid);
    localnetworkid = (unsigned __int8)modem_networkid;
    SendDataMessage(a1: 2, a2: 255, data: &localnetworkid, databytes: 4u);
    return 1;
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x180076D0
 * CodeView symbol: PickRandomNumber
 * Demangled: PickRandomNumber
 * IDA name: sub_180076D0
 * CodeView module: 7
 * CV address: segment 1 : 0x66D0
 * Code length: 0x96
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_180076D0()
{
  LARGE_INTEGER perfcount; // [esp+0h] [ebp-18h] BYREF
  struct tagPOINT pos; // [esp+8h] [ebp-10h] BYREF
  int rand1; // [esp+14h] [ebp-4h]

  QueryPerformanceCounter(lpPerformanceCount: &perfcount);
  GetCursorPos(lpPoint: &pos);
  dword_18028410 ^= pos.y ^ pos.x ^ GetTickCount() ^ perfcount.LowPart;
  dword_18028410 = (125 * dword_18028410 + 3) % 0x2AAAABu;
  rand1 = (unsigned __int16)dword_18028410;
  dword_18028410 = (125 * dword_18028410 + 3) % 0x2AAAABu;
  return (unsigned __int16)dword_18028410 | (rand1 << 16);
}


// ==============================================================================
/*
 * EA: 0x18007766
 * CodeView symbol: InitializePort
 * Demangled: InitializePort
 * IDA name: sub_18007766
 * CodeView module: 7
 * CV address: segment 1 : 0x6766
 * Code length: 0x296
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall sub_18007766(int a1)
{
  void *v1; // eax
  int v3; // edx
  struct _DCB dcb; // [esp+18h] [ebp-34h] BYREF
  struct _COMMTIMEOUTS timeouts; // [esp+34h] [ebp-18h] BYREF
  int lpVarString; // [esp+48h] [ebp-4h]

  if ( modem_port[a1] != 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_63, a3: 1434, a4: aModemPortPort, a5: 0, a6: 1);
  if ( modem_port[a1] == 0 )
  {
    v1 = (void *)SMemAlloc(a1: 100, a2: aModemCpp_64, a3: 1436, a4: 8);
    modem_port[a1] = (int)operator new(__formal: 100, ptr: v1);
    TraceOut(format: "Allocated port %d", a1);
  }
  if ( *(&modem_event + a1) != nullptr )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_65, a3: 1441, a4: aModemEventPort, a5: 0, a6: 1);
  if ( *(&modem_event + a1) == nullptr )
  {
    *(&modem_event + a1) = CreateEventA(
                             lpEventAttributes: nullptr,
                             bManualReset: true,
                             bInitialState: false,
                             lpName: nullptr);
    if ( *(&modem_event + a1) == nullptr )
      return 0;
    *(_DWORD *)(modem_port[a1] + 20) = *(&modem_event + a1);
  }
  lpVarString = GetVarString();
  if ( lpVarString != 0 )
  {
    *(_DWORD *)modem_port[a1] = *(_DWORD *)(lpVarString + *(_DWORD *)(lpVarString + 20));
    if ( *(_DWORD *)modem_port[a1] == 0 )
      SErrDisplayError(a1: -2062548992, a2: aModemCpp_67, a3: 1462, a4: aModemPortPortH, a5: 0, a6: 1);
    if ( lpVarString != 0 )
      SMemFree(a1: lpVarString, a2: aModemCpp_68, a3: 1464, a4: 0);
    if ( *(_DWORD *)modem_port[a1] != -1 )
    {
      memset(a1: &timeouts, Val: 0, Size: sizeof(timeouts));
      timeouts.ReadIntervalTimeout = 25;
      SetCommTimeouts(hFile: *(HANDLE *)modem_port[a1], lpCommTimeouts: &timeouts);
      GetCommState(hFile: *(HANDLE *)modem_port[a1], lpDCB: &dcb);
      v3 = *((_DWORD *)&dcb + 2);
      BYTE1(v3) = *((_BYTE *)&dcb + 9) & 0xBF;
      *((_DWORD *)&dcb + 2) = v3;
      SetCommState(hFile: *(HANDLE *)modem_port[a1], lpDCB: &dcb);
      ReadFile(
        hFile: *(HANDLE *)modem_port[a1],
        lpBuffer: (LPVOID)(modem_port[a1] + 24),
        nNumberOfBytesToRead: 0x40u,
        lpNumberOfBytesRead: (LPDWORD)(modem_port[a1] + 88),
        lpOverlapped: (LPOVERLAPPED)(modem_port[a1] + 4));
    }
    return 1;
  }
  else
  {
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_66, a3: 1452, a4: aLpvarstring, a5: 0, a6: 1);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x180079FC
 * CodeView symbol: GetVarString
 * Demangled: GetVarString
 * IDA name: GetVarString
 * CodeView module: 7
 * CV address: segment 1 : 0x69FC
 * Code length: 0x113
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
LPVARSTRING GetVarString()
{
  struct varstring_tag tmpVarString; // [esp+Ch] [ebp-1Ch] BYREF
  LPVARSTRING lpVarString; // [esp+24h] [ebp-4h]

  lpVarString = nullptr;
  if ( g_hCall == 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_69, a3: 1410, a4: aGHcall, a5: 0, a6: 1);
  if ( g_hCall == 0 )
    return nullptr;
  tmpVarString.dwTotalSize = 24;
  if ( lineGetID(
         hLine: 0,
         dwAddressID: 0,
         hCall: g_hCall,
         dwSelect: 4u,
         lpDeviceID: &tmpVarString,
         lpszDeviceClass: szDeviceClass) != 0 )
  {
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_70, a3: 1417, a4: aLreturn, a5: 0, a6: 1);
    return nullptr;
  }
  else
  {
    lpVarString = (LPVARSTRING)SMemAlloc(a1: tmpVarString.dwNeededSize, a2: aModemCpp_71, a3: 1421, a4: 0);
    lpVarString->dwTotalSize = tmpVarString.dwNeededSize;
    if ( lineGetID(
           hLine: 0,
           dwAddressID: 0,
           hCall: g_hCall,
           dwSelect: 4u,
           lpDeviceID: lpVarString,
           lpszDeviceClass: aCommDatamodem_0) != 0 )
    {
      SMemFree(a1: lpVarString, a2: aModemCpp_72, a3: 1424, a4: 0);
      return nullptr;
    }
    else
    {
      return lpVarString;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x18007B0F
 * CodeView symbol: ModemInitializeDevice
 * Demangled: ModemInitializeDevice
 * IDA name: ?ModemInitializeDevice@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x6B0F
 * Code length: 0x11D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemInitializeDevice(
        DWORD deviceid,
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata)
{
  int lpLineDevCaps; // [esp+8h] [ebp-8h]
  DWORD dwTAPIVersion; // [esp+Ch] [ebp-4h] BYREF

  if ( g_hLineApp == 0 || g_dwNumDevices == 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_73, a3: 1892, a4: aGHlineappGDwnu_2, a5: 0, a6: 1);
  if ( *((_DWORD *)interfacedata + 10) == 0 )
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_74, a3: 1893, a4: aInterfacedataS, a5: 0, a6: 1);
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  modem_status = *((int (__stdcall **)(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD))interfacedata + 10);
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  if ( !GetDeviceAPIVersion(a1: deviceid, a2: &dwTAPIVersion) )
    return 0;
  lpLineDevCaps = GetDeviceCaps(hdc: nullptr, index: 0);
  if ( lpLineDevCaps == 0 )
    return 0;
  if ( (*(_DWORD *)(lpLineDevCaps + 60) & 0x10) != 0 )
  {
    SMemFree(a1: lpLineDevCaps, a2: aModemCpp_75, a3: 1912, a4: 0);
    CCritSect::Enter(this: (CCritSect *)&modem_critsect);
    g_dwDeviceID = deviceid;
    CCritSect::Leave(this: (CCritSect *)&modem_critsect);
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 1204);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18007C2C
 * CodeView symbol: ModemLockDeviceList
 * Demangled: ModemLockDeviceList
 * IDA name: ?ModemLockDeviceList@@YGHPAPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x6C2C
 * Code length: 0x69
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemLockDeviceList(struct _SNETSPI_DEVICELIST **devicelist)
{
  if ( devicelist != nullptr )
  {
    *devicelist = nullptr;
    EnumerateDevices(a1: DevFillDeviceList, a2: devicelist, a3: 0);
    return 1;
  }
  else
  {
    SErrDisplayError(a1: -2062548992, a2: aModemCpp_76, a3: 1922, a4: aDevicelist, a5: 0, a6: 1);
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18007C95
 * CodeView symbol: DevFillDeviceList
 * Demangled: DevFillDeviceList
 * IDA name: DevFillDeviceList
 * CodeView module: 7
 * CV address: segment 1 : 0x6C95
 * Code length: 0x72
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall DevFillDeviceList(int lpLineDevCaps, int dwDevID, struct _SNETSPI_DEVICELIST **appData)
{
  _DWORD device[76]; // [esp+4h] [ebp-130h] BYREF

  memset(a1: device, Val: 0, Size: sizeof(device));
  SStrCopy(a1: &device[10], a2: *(_DWORD *)(lpLineDevCaps + 36) + lpLineDevCaps, a3: 128);
  device[0] = dwDevID;
  TListAdd(a1: appData, a2: (struct _SNETSPI_DEVICELIST *)device, a3: aModemCpp_77, a4: 1606);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18007D07
 * CodeView symbol: EnumerateDevices
 * Demangled: EnumerateDevices
 * IDA name: EnumerateDevices
 * CodeView module: 7
 * CV address: segment 1 : 0x6D07
 * Code length: 0xD5
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall EnumerateDevices(int (__stdcall *a1)(_DWORD *, DWORD, HDC), HDC a2)
{
  HDC appData; // [esp+0h] [ebp-14h]
  int (__stdcall *func)(_DWORD *, DWORD, HDC); // [esp+4h] [ebp-10h]
  _DWORD *lpLineDevCaps; // [esp+8h] [ebp-Ch]
  DWORD dwTAPIVersion; // [esp+Ch] [ebp-8h] BYREF
  DWORD device; // [esp+10h] [ebp-4h]

  appData = a2;
  func = a1;
  for ( device = 0; device < g_dwNumDevices; ++device )
  {
    if ( GetDeviceAPIVersion(a1: device, a2: &dwTAPIVersion) )
    {
      lpLineDevCaps = (_DWORD *)GetDeviceCaps(hdc: appData, index: (int)func);
      if ( lpLineDevCaps != nullptr )
      {
        if ( (lpLineDevCaps[15] & 0x10) != 0 && (lpLineDevCaps[13] & 1) != 0 && (lpLineDevCaps[59] & 8) != 0 )
        {
          if ( func != nullptr && func(a1: lpLineDevCaps, a2: device, a3: appData) == 0 )
            return;
          SMemFree(a1: lpLineDevCaps, a2: aModemCpp_79, a3: 1634, a4: 0);
        }
        else
        {
          SMemFree(a1: lpLineDevCaps, a2: aModemCpp_78, a3: 1628, a4: 0);
        }
      }
    }
  }
}


// ==============================================================================
/*
 * EA: 0x18007DDC
 * CodeView symbol: ModemLockGameList
 * Demangled: ModemLockGameList
 * IDA name: ?ModemLockGameList@@YGHKKPAPAU_SNETSPI_GAMELIST@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x6DDC
 * Code length: 0x52
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemLockGameList(char categorybits, char categorymask, struct _SNETSPI_GAMELIST **gamelist)
{
  if ( gamelist != nullptr )
  {
    if ( modem_lineestablished != 0 )
    {
      CCritSect::Enter(this: (CCritSect *)&modem_critsect);
      *gamelist = (struct _SNETSPI_GAMELIST *)modem_gamelist;
      return 1;
    }
    else
    {
      SErrSetLastError(a1: 1222);
      *gamelist = nullptr;
      return 0;
    }
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18007E2E
 * CodeView symbol: ModemReceive
 * Demangled: ModemReceive
 * IDA name: ?ModemReceive@@YGHPAPAU_SNETADDR@@PAPAXPAK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x6E2E
 * Code length: 0xC8
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemReceive(struct _SNETADDR **addr, void **data, unsigned int *databytes)
{
  if ( addr != nullptr )
    *addr = nullptr;
  if ( data != nullptr )
    *data = nullptr;
  if ( databytes != nullptr )
    *databytes = 0;
  if ( addr != nullptr && data != nullptr && databytes != nullptr )
  {
    if ( modem_messagehead != nullptr )
    {
      CCritSect::Enter(this: (CCritSect *)&modem_critsect);
      *addr = modem_messagehead;
      *data = (char *)modem_messagehead + 24;
      *databytes = *((_DWORD *)modem_messagehead + 52) - 8;
      TListFreePtr(a1: &modem_messagehead, a2: modem_messagehead, a3: aModemCpp_80, a4: 1979);
      CCritSect::Leave(this: (CCritSect *)&modem_critsect);
      return 1;
    }
    else
    {
      SErrSetLastError(a1: -2062548885);
      return 0;
    }
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18007EF6
 * CodeView symbol: ModemReceiveExternalMessage
 * Demangled: ModemReceiveExternalMessage
 * IDA name: ?ModemReceiveExternalMessage@@YGHPAPBD00@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x6EF6
 * Code length: 0x40
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemReceiveExternalMessage(const char **senderpath, const char **sendername, const char **message)
{
  if ( senderpath != nullptr )
    *senderpath = nullptr;
  if ( sendername != nullptr )
    *sendername = nullptr;
  if ( message != nullptr )
    *message = nullptr;
  SErrSetLastError(a1: -2062548885);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x18007F36
 * CodeView symbol: ModemSelectGame
 * Demangled: ModemSelectGame
 * IDA name: ?ModemSelectGame@@YGHKPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x6F36
 * Code length: 0x9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemSelectGame(
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
 * EA: 0x18007F3F
 * CodeView symbol: ModemSend
 * Demangled: ModemSend
 * IDA name: ?ModemSend@@YGHKPAPAU_SNETADDR@@PAXK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x6F3F
 * Code length: 0x73
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemSend(unsigned int addresses, struct _SNETADDR **addrlist, void *data, size_t databytes)
{
  unsigned int v5; // eax
  char targetmask; // [esp+0h] [ebp-4h]

  if ( addresses != 0 && addrlist != nullptr && data != nullptr && databytes != 0 )
  {
    for ( targetmask = 0; ; targetmask |= 1 << *(_BYTE *)addrlist[addresses] )
    {
      v5 = addresses--;
      if ( v5 == 0 )
        break;
    }
    return SendDataMessage(a1: 0, a2: targetmask, data, databytes);
  }
  else
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18007FB2
 * CodeView symbol: ModemSendExternalMessage
 * Demangled: ModemSendExternalMessage
 * IDA name: ?ModemSendExternalMessage@@YGHPBD0000@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x6FB2
 * Code length: 0x9
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemSendExternalMessage(
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
 * EA: 0x18007FBB
 * CodeView symbol: ModemStartAdvertisingGame
 * Demangled: ModemStartAdvertisingGame
 * IDA name: ?ModemStartAdvertisingGame@@YGHPBD00KKKKKPBXK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x6FBB
 * Code length: 0x20E
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemStartAdvertisingGame(
        const char *gamename,
        const char *gamepassword,
        const char *gamedescription,
        char gamemode,
        char gameage,
        char gamecategorybits,
        char optcategorybits,
        char ladderid,
        const void *clientdata,
        size_t clientdatabytes)
{
  void *v11; // eax
  size_t v12; // esi
  unsigned int bytes; // [esp+8h] [ebp-Ch]
  char *currptra; // [esp+Ch] [ebp-8h]
  int currptrb; // [esp+Ch] [ebp-8h]
  int currptrc; // [esp+Ch] [ebp-8h]
  void *currptr; // [esp+Ch] [ebp-8h]
  _WORD *checksumptr; // [esp+10h] [ebp-4h]

  TraceOut(format: aModemstartadve);
  if ( gamename == nullptr || gamedescription == nullptr )
  {
    SErrSetLastError(a1: 87);
    return 0;
  }
  if ( modem_gameadvinfo != nullptr )
    ModemStopAdvertisingGame();
  CCritSect::Enter(this: (CCritSect *)&modem_critsect);
  v11 = (void *)SMemAlloc(a1: 536, a2: aModemCpp_81, a3: 2068, a4: 8);
  modem_gameadvinfo = operator new(__formal: 24, ptr: v11);
  if ( modem_lineestablished == 0 )
  {
    for ( modem_networkid = 0; modem_networkid == 0; modem_networkid = sub_180076D0() % 2u + 1 )
      ;
    modem_networkidlocked = 1;
  }
  *(_DWORD *)modem_gameadvinfo = (unsigned __int8)modem_networkid;
  *((_DWORD *)modem_gameadvinfo + 1) = modem_programid;
  *((_DWORD *)modem_gameadvinfo + 2) = modem_versionid;
  currptra = (char *)modem_gameadvinfo + 12;
  currptrb = (int)&currptra[SStrCopy(a1: (char *)modem_gameadvinfo + 12, a2: gamename, a3: 128) + 1];
  currptrc = currptrb + SStrCopy(a1: currptrb, a2: gamedescription, a3: 128) + 1;
  v12 = strlen(Str: gamename);
  bytes = strlen(Str: gamedescription) + v12 + clientdatabytes + 22;
  *((_DWORD *)modem_gameadvinfo + 133) = bytes;
  *(_DWORD *)currptrc = bytes;
  checksumptr = (_WORD *)(currptrc + 4);
  *(_WORD *)(currptrc + 4) = 0;
  currptr = (void *)(currptrc + 6);
  if ( clientdatabytes != 0 )
    memcpy(a1: currptr, Src: clientdata, Size: clientdatabytes);
  *checksumptr = ComputeChecksum(a1: modem_gameadvinfo, a2: bytes);
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  if ( modem_lineestablished != 0 )
  {
    SendDataMessage(a1: 10, a2: 255, data: modem_gameadvinfo, databytes: *((_DWORD *)modem_gameadvinfo + 133));
  }
  else if ( !TakeCall() )
  {
    SErrSetLastError(a1: 1222);
    return 0;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x180081C9
 * CodeView symbol: ModemStopAdvertisingGame
 * Demangled: ModemStopAdvertisingGame
 * IDA name: ?ModemStopAdvertisingGame@@YGHXZ
 * CodeView module: 7
 * CV address: segment 1 : 0x71C9
 * Code length: 0x89
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemStopAdvertisingGame()
{
  TraceOut(format: aModemstopadver);
  if ( modem_gameadvinfo != nullptr )
  {
    SendDataMessage(a1: 11, a2: 255, data: modem_gameadvinfo, databytes: *((_DWORD *)modem_gameadvinfo + 133));
    CCritSect::Enter(this: (CCritSect *)&modem_critsect);
    if ( modem_gameadvinfo != nullptr )
    {
      SMemFree(a1: modem_gameadvinfo, a2: aModemCpp_82, a3: 2145, a4: 0);
      modem_gameadvinfo = nullptr;
    }
    CCritSect::Leave(this: (CCritSect *)&modem_critsect);
    return 1;
  }
  else
  {
    SErrSetLastError(a1: 288);
    return 0;
  }
}


// ==============================================================================
/*
 * EA: 0x18008252
 * CodeView symbol: ModemUnlockDeviceList
 * Demangled: ModemUnlockDeviceList
 * IDA name: ?ModemUnlockDeviceList@@YGHPAU_SNETSPI_DEVICELIST@@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x7252
 * Code length: 0x24
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemUnlockDeviceList(struct _SNETSPI_DEVICELIST *devicelist)
{
  if ( devicelist != nullptr )
    TListClear(a1: &devicelist, a2: aModemCpp_83, a3: 2155);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18008276
 * CodeView symbol: ModemUnlockGameList
 * Demangled: ModemUnlockGameList
 * IDA name: ?ModemUnlockGameList@@YGHPAU_SNETSPI_GAMELIST@@PAK@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x7276
 * Code length: 0x33
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemUnlockGameList(struct _SNETSPI_GAMELIST *gamelist, unsigned int *hintnextcall)
{
  CCritSect::Leave(this: (CCritSect *)&modem_critsect);
  if ( hintnextcall != nullptr )
    *hintnextcall = 1000;
  SendDataMessage(a1: 9, a2: 255, data: nullptr, databytes: 0);
  return 1;
}


// ==============================================================================
/*
 * EA: 0x180082A9
 * CodeView symbol: ModemDestroy_DEBUG
 * Demangled: ModemDestroy_DEBUG
 * IDA name: ?ModemDestroy_DEBUG@@YGHXZ
 * CodeView module: 7
 * CV address: segment 1 : 0x72A9
 * Code length: 0x2D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __stdcall ModemDestroy_DEBUG()
{
  return ModemDestroy() != 0 && LatencyDestroy() != 0;
}


// ==============================================================================
/*
 * EA: 0x180082D6
 * CodeView symbol: ModemInitialize_DEBUG
 * Demangled: ModemInitialize_DEBUG
 * IDA name: ?ModemInitialize_DEBUG@@YGHPAU_SNETPROGRAMDATA@@PAU_SNETPLAYERDATA@@PAU_SNETUIDATA@@PAU_SNETVERSIONDATA@@PAX@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x72D6
 * Code length: 0x37
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall ModemInitialize_DEBUG(
        struct _SNETPROGRAMDATA *programdata,
        struct _SNETPLAYERDATA *playerdata,
        struct _SNETUIDATA *interfacedata,
        struct _SNETVERSIONDATA *versiondata,
        void *event)
{
  if ( ModemInitialize(programdata, playerdata, interfacedata, versiondata, event) != 0 )
    return LatencyInitialize(a1: ModemSend, a2: 0xBu);
  else
    return 0;
}


// ==============================================================================
/*
 * EA: 0x18008310
 * CodeView symbol: TListAddPtrEnd
 * Demangled: TListAddPtrEnd
 * IDA name: ?TListAddPtrEnd@@YIHPAPAU_MESSAGEREC@@PAU1@@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x7310
 * Code length: 0x58
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAddPtrEnd(struct _MESSAGEREC **a1, struct _MESSAGEREC *a2)
{
  struct _MESSAGEREC **next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  *((_DWORD *)a2 + 54) = 0;
  for ( next = a1; *next != nullptr; next = (struct _MESSAGEREC **)((char *)*next + 216) )
    ;
  *next = a2;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18008370
 * CodeView symbol: TListAdd
 * Demangled: TListAdd
 * IDA name: ?TListAdd@@YIHPAPAU_SNETSPI_DEVICELIST@@PAU1@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x7370
 * Code length: 0x73
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListAdd(
        struct _SNETSPI_DEVICELIST **a1,
        struct _SNETSPI_DEVICELIST *a2,
        const char *filename,
        int linenumber)
{
  _DWORD *newptr; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return 0;
  newptr = (_DWORD *)SMemAlloc(a1: 304, a2: filename, a3: linenumber, a4: 0);
  if ( newptr == nullptr )
    return 0;
  memcpy(a1: newptr, Src: a2, Size: 0x130u);
  newptr[75] = *a1;
  *a1 = (struct _SNETSPI_DEVICELIST *)newptr;
  return 1;
}


// ==============================================================================
/*
 * EA: 0x180083F0
 * CodeView symbol: TListClear
 * Demangled: TListClear
 * IDA name: ?TListClear@@YIHPAPAU_MESSAGEREC@@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x73F0
 * Code length: 0x56
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListClear(struct _MESSAGEREC **a1, const char *a2, int linenumber)
{
  struct _MESSAGEREC *next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr )
    return 0;
  while ( *a1 != nullptr )
  {
    next = *((struct _MESSAGEREC **)*a1 + 54);
    SMemFree(a1: *a1, a2, a3: linenumber, a4: 0);
    *a1 = next;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x18008450
 * CodeView symbol: TListFreePtr
 * Demangled: TListFreePtr
 * IDA name: ?TListFreePtr@@YIHPAPAU_MESSAGEREC@@PAU1@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x7450
 * Code length: 0x6D
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall TListFreePtr(struct _MESSAGEREC **a1, struct _MESSAGEREC *a2, const char *filename, int linenumber)
{
  struct _MESSAGEREC **next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr || a2 == nullptr )
    return false;
  for ( next = a1; *next != nullptr && *next != a2; next = (struct _MESSAGEREC **)((char *)*next + 216) )
    ;
  if ( *next != nullptr )
    *next = *((struct _MESSAGEREC **)*next + 54);
  return *next != nullptr;
}


// ==============================================================================
/*
 * EA: 0x180084C0
 * CodeView symbol: TListClear
 * Demangled: TListClear
 * IDA name: ?TListClear@@YIHPAPAU_SNETSPI_DEVICELIST@@PBDH@Z
 * CodeView module: 7
 * CV address: segment 1 : 0x74C0
 * Code length: 0x56
 * Original object: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\debug\Modem.obj
 * Source: C:\windows\TEMP\c\hosoi\Battle\Standard_build_J\standard\Modem.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall TListClear(struct _SNETSPI_DEVICELIST **a1, const char *a2, int linenumber)
{
  struct _SNETSPI_DEVICELIST *next; // [esp+8h] [ebp-4h]

  if ( a1 == nullptr )
    return 0;
  while ( *a1 != nullptr )
  {
    next = *((struct _SNETSPI_DEVICELIST **)*a1 + 75);
    SMemFree(a1: *a1, a2, a3: linenumber, a4: 0);
    *a1 = next;
  }
  return 1;
}

