/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 44
 */


// ==============================================================================
/*
 * EA: 0x1900B390
 * CodeView symbol: $E4
 * Demangled: $E4
 * IDA name: unknown_libname_2
 * CodeView module: 20
 * CV address: segment 1 : 0xA390
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int unknown_libname_2()
{
  sub_1900B39F();
  return sub_1900B3AE();
}


// ==============================================================================
/*
 * EA: 0x1900B39F
 * CodeView symbol: $E1
 * Demangled: $E1
 * IDA name: sub_1900B39F
 * CodeView module: 20
 * CV address: segment 1 : 0xA39F
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1900B39F()
{
  return TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>(a1: &s_iconlist);
}


// ==============================================================================
/*
 * EA: 0x1900B3AE
 * CodeView symbol: $E3
 * Demangled: $E3
 * IDA name: sub_1900B3AE
 * CodeView module: 20
 * CV address: segment 1 : 0xA3AE
 * Code length: 0x12
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int sub_1900B3AE()
{
  return atexit(a1: sub_1900B3C0);
}


// ==============================================================================
/*
 * EA: 0x1900B3C0
 * CodeView symbol: $E2
 * Demangled: $E2
 * IDA name: sub_1900B3C0
 * CodeView module: 20
 * CV address: segment 1 : 0xA3C0
 * Code length: 0xF
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl sub_1900B3C0()
{
  TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::~TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>(a1: &s_iconlist);
}


// ==============================================================================
/*
 * EA: 0x1900B3CF
 * CodeView symbol: RLEDecompressImage
 * Demangled: RLEDecompressImage
 * IDA name: ?RLEDecompressImage@@YIHPAE0KH@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xA3CF
 * Code length: 0x132
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall RLEDecompressImage(unsigned __int8 *a1, unsigned __int8 *a2, int sourcesize, size_t bytesperpixel)
{
  int v4; // ecx
  unsigned __int8 *sourcea; // [esp+0h] [ebp-14h]
  __int16 wCount; // [esp+Ch] [ebp-8h]
  unsigned __int16 wCounta; // [esp+Ch] [ebp-8h]
  unsigned __int16 wCountb; // [esp+Ch] [ebp-8h]
  unsigned __int16 wCountc; // [esp+Ch] [ebp-8h]

  while ( sourcesize != 0 )
  {
    wCount = *a2;
    sourcea = a2 + 1;
    if ( (wCount & 0x80) != 0 )
    {
      wCounta = (wCount & 0x7F) + 1;
      sourcesize -= wCounta;
      while ( 1 )
      {
        v4 = wCounta--;
        if ( v4 == 0 )
          break;
        memcpy(a1, Src: sourcea, Size: bytesperpixel);
        a1 += bytesperpixel;
      }
      a2 = &sourcea[bytesperpixel];
    }
    else
    {
      wCountb = wCount + 1;
      sourcesize -= wCountb;
      wCountc = bytesperpixel * wCountb;
      memcpy(a1, Src: sourcea, Size: wCountc);
      a2 = &sourcea[wCountc];
      a1 += wCountc;
    }
    if ( sourcesize < 0 )
      return 0;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900B501
 * CodeView symbol: IconGetXSpacing
 * Demangled: IconGetXSpacing
 * IDA name: ?IconGetXSpacing@@YIHXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xA501
 * Code length: 0xA
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl IconGetXSpacing()
{
  return 32;
}


// ==============================================================================
/*
 * EA: 0x1900B50B
 * CodeView symbol: IconGetYSpacing
 * Demangled: IconGetYSpacing
 * IDA name: ?IconGetYSpacing@@YIHXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xA50B
 * Code length: 0x7
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __cdecl IconGetYSpacing()
{
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1900B512
 * CodeView symbol: IconDestroy
 * Demangled: IconDestroy
 * IDA name: ?IconDestroy@@YIXXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xA512
 * Code length: 0x64
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __cdecl IconDestroy()
{
  if ( s_icontarga != 0 )
  {
    SMemFree(a1: s_icontarga, a2: aIconCpp, a3: 328, a4: 0);
    s_icontarga = 0;
  }
  if ( s_iconmappedbitmap != 0 )
  {
    SMemFree(a1: s_iconmappedbitmap, a2: aIconCpp_0, a3: 332, a4: 0);
    s_iconmappedbitmap = 0;
  }
  TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Clear(a1: &s_iconlist);
}


// ==============================================================================
/*
 * EA: 0x1900B576
 * CodeView symbol: IconLoad
 * Demangled: IconLoad
 * IDA name: ?IconLoad@@YIXPAXK@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xA576
 * Code length: 0xEB
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void IconLoad()
{
  int bytes; // [esp+8h] [ebp-10h] BYREF
  int destsize; // [esp+Ch] [ebp-Ch] BYREF
  int imagesize; // [esp+10h] [ebp-8h] BYREF
  int imagedata; // [esp+14h] [ebp-4h] BYREF

  IconDestroy();
  if ( ParseDescription((int)&imagedata, (int)&imagesize) != 0
    && imagesize != 0
    && ParseTarga(
         newdestsize: (int)&destsize,
         source: imagedata,
         sourcesize: imagesize,
         width: 0,
         height: 0,
         bytesperpixel: 0) != 0 )
  {
    s_icontarga = SMemAlloc(a1: destsize, a2: aIconCpp_1, a3: 355, a4: 0);
    if ( ParseTarga(
           newdestsize: (int)&bytes,
           source: imagedata,
           sourcesize: imagesize,
           width: (int)&s_icontargawidth,
           height: (int)&s_icontargaheight,
           bytesperpixel: (int)&s_icontargabytesperpixel) == 0
      || bytes != destsize
      || s_icontargawidth != 28
      || s_icontargaheight != 14 * s_numicons )
    {
      SMemFree(a1: s_icontarga, a2: aIconCpp_2, a3: 369, a4: 0);
      s_icontarga = 0;
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1900B661
 * CodeView symbol: ParseDescription
 * Demangled: ParseDescription
 * IDA name: ParseDescription
 * CodeView module: 20
 * CV address: segment 1 : 0xA661
 * Code length: 0x1EE
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ParseDescription(__int128 *a1, size_t a2, _DWORD *imagedata, size_t *imagesize)
{
  int v5; // eax
  size_t v6; // [esp+0h] [ebp-30h]
  unsigned int datasizea; // [esp+8h] [ebp-28h]
  _DWORD *dataa; // [esp+Ch] [ebp-24h]
  _DWORD *datab; // [esp+Ch] [ebp-24h]
  int id; // [esp+10h] [ebp-20h]
  _DWORD *icon; // [esp+14h] [ebp-1Ch]
  unsigned int index; // [esp+18h] [ebp-18h]
  int offset; // [esp+1Ch] [ebp-14h]
  __int128 header; // [esp+20h] [ebp-10h] BYREF

  if ( a2 <= 0x10 )
    memcpy(a1: &header, Src: a1, Size: a2);
  else
    header = *a1;
  if ( a2 <= 0x10 )
    return 0;
  if ( (_DWORD)header != 16 )
    return 0;
  if ( WORD2(header) != 1 )
    return 0;
  *imagedata = (char *)a1 + HIDWORD(header);
  if ( a2 < HIDWORD(header) )
    v6 = 0;
  else
    v6 = a2 - HIDWORD(header);
  *imagesize = v6;
  dataa = a1 + 1;
  datasizea = a2 - 16;
  s_numicons = 0;
  offset = 0;
  index = 0;
LABEL_14:
  if ( index >= DWORD2(header) )
    return 1;
  icon = (_DWORD *)TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::NewNode(location: 2, extrabytes: 0, flags: 0);
  if ( datasizea >= 4 )
  {
    icon[2] = *dataa;
    datab = dataa + 1;
    icon[3] = *datab++;
    icon[4] = *datab;
    dataa = datab + 1;
    datasizea -= 12;
    id = 0;
    while ( datasizea >= 4 )
    {
      icon[id + 5] = *dataa++;
      datasizea -= 4;
      v5 = icon[id++ + 5];
      if ( v5 == 0 )
      {
        icon[37] = offset;
        offset += icon[4] * icon[3];
        ++s_numicons;
        ++index;
        goto LABEL_14;
      }
    }
  }
  TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::DeleteNode(ptr: (int)icon);
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1900B84F
 * CodeView symbol: ParseTarga
 * Demangled: ParseTarga
 * IDA name: ParseTarga
 * CodeView module: 20
 * CV address: segment 1 : 0xA84F
 * Code length: 0x1DD
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall ParseTarga(
        unsigned __int8 *a1,
        int a2,
        size_t *newdestsize,
        const void *source,
        unsigned int sourcesize,
        _DWORD *width,
        int *height,
        size_t *bytesperpixel)
{
  const void *tmpbuf; // [esp+Ch] [ebp-24h]
  size_t dwBytesPerPixel; // [esp+14h] [ebp-1Ch]
  size_t dwImageSize; // [esp+18h] [ebp-18h]
  _DWORD header[5]; // [esp+1Ch] [ebp-14h] BYREF

  if ( source == nullptr )
    return 0;
  memcpy(a1: header, Src: source, Size: 0x12u);
  if ( LOBYTE(header[4]) < 0x18u || LOBYTE(header[4]) > 0x20u )
    return 0;
  if ( BYTE2(header[0]) != 10 )
    return 0;
  dwBytesPerPixel = (LOBYTE(header[4]) + 7) / 8;
  dwImageSize = dwBytesPerPixel * HIWORD(header[3]) * LOWORD(header[3]);
  if ( width != nullptr )
    *width = LOWORD(header[3]);
  if ( height != nullptr )
    *height = HIWORD(header[3]);
  if ( bytesperpixel != nullptr )
    *bytesperpixel = dwBytesPerPixel;
  if ( newdestsize != nullptr )
    *newdestsize = dwImageSize;
  if ( a1 == nullptr && a2 == 0 )
    return 1;
  if ( LOBYTE(header[0])
     + (HIBYTE(header[1]) + 7) / 8 * (unsigned int)*(unsigned __int16 *)((char *)&header[1] + 1)
     + 18 > sourcesize )
    return 0;
  if ( RLEDecompressImage(
         a1,
         a2: (unsigned __int8 *)source
       + LOBYTE(header[0])
       + (HIBYTE(header[1]) + 7) / 8 * *(unsigned __int16 *)((char *)&header[1] + 1)
       + 18,
         sourcesize: *height * *width,
         bytesperpixel: dwBytesPerPixel) == 0 )
    return 0;
  if ( (header[4] & 0x2000) == 0 )
  {
    tmpbuf = (const void *)SMemAlloc(a1: dwImageSize, a2: aIconCpp_3, a3: 300, a4: 0);
    InvertImage(dwLines: *height, dwBytesPerLine: dwBytesPerPixel * *width);
    memcpy(a1, Src: tmpbuf, Size: dwImageSize);
    SMemFree(a1: tmpbuf, a2: aIconCpp_4, a3: 303, a4: 0);
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900BA2C
 * CodeView symbol: InvertImage
 * Demangled: InvertImage
 * IDA name: InvertImage
 * CodeView module: 20
 * CV address: segment 1 : 0xAA2C
 * Code length: 0x66
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall InvertImage(char *a1, int a2, unsigned int dwLines, size_t dwBytesPerLine)
{
  int nLine; // [esp+8h] [ebp-4h]

  if ( dwLines < 2 )
    return 1;
  for ( nLine = dwLines - 1; nLine >= 0; --nLine )
  {
    memcpy(a1, Src: (const void *)(dwBytesPerLine * nLine + a2), Size: dwBytesPerLine);
    a1 += dwBytesPerLine;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900BA92
 * CodeView symbol: IconMap
 * Demangled: IconMap
 * IDA name: ?IconMap@@YIXQAUtagPALETTEENTRY@@@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xAA92
 * Code length: 0xBE
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall IconMap(struct tagPALETTEENTRY *const a1)
{
  int destbitmap; // [esp+4h] [ebp-4h]

  if ( s_icontarga != 0 )
  {
    destbitmap = SMemAlloc(a1: s_icontargaheight * s_icontargawidth, a2: aIconCpp_5, a3: 381, a4: 0);
    if ( MapTargaToPcx(
           source: s_icontarga,
           sourcesize: s_icontargabytesperpixel * s_icontargaheight * s_icontargawidth,
           palette: (int)a1) != 0 )
    {
      if ( s_iconmappedbitmap != 0 )
      {
        SMemFree(a1: s_iconmappedbitmap, a2: aIconCpp_7, a3: 393, a4: 0);
        s_iconmappedbitmap = 0;
      }
      s_iconmappedbitmap = destbitmap;
    }
    else
    {
      SMemFree(a1: destbitmap, a2: aIconCpp_6, a3: 388, a4: 0);
    }
  }
}


// ==============================================================================
/*
 * EA: 0x1900BB50
 * CodeView symbol: MapTargaToPcx
 * Demangled: MapTargaToPcx
 * IDA name: MapTargaToPcx
 * CodeView module: 20
 * CV address: segment 1 : 0xAB50
 * Code length: 0x15D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall MapTargaToPcx(_BYTE *a1, int a2, unsigned __int8 *source, unsigned int sourcesize, int palette)
{
  int blue; // [esp+10h] [ebp-1Ch]
  unsigned int minscore; // [esp+14h] [ebp-18h]
  int green; // [esp+18h] [ebp-14h]
  int red; // [esp+1Ch] [ebp-10h]
  char mincolor; // [esp+20h] [ebp-Ch]
  int color; // [esp+24h] [ebp-8h]
  int i; // [esp+28h] [ebp-4h]
  unsigned __int8 *sourcea; // [esp+34h] [ebp+8h]

  if ( init == 0 )
  {
    for ( i = -255; i < 256; ++i )
      dword_190425D4[i] = i * i;
    init = 1;
  }
  while ( a2 != 0 )
  {
    if ( sourcesize < 3 )
      return 0;
    blue = *source;
    sourcea = source + 1;
    green = *sourcea++;
    red = *sourcea;
    source = sourcea + 1;
    sourcesize -= 3;
    minscore = -1;
    mincolor = 0;
    for ( color = 0; color < 256; ++color )
    {
      if ( dword_190425D4[blue - *(unsigned __int8 *)(palette + 4 * color + 2)]
         + dword_190425D4[green - *(unsigned __int8 *)(palette + 4 * color + 1)]
         + dword_190425D4[red - *(unsigned __int8 *)(palette + 4 * color)] < minscore )
      {
        minscore = dword_190425D4[blue - *(unsigned __int8 *)(palette + 4 * color + 2)]
                 + dword_190425D4[green - *(unsigned __int8 *)(palette + 4 * color + 1)]
                 + dword_190425D4[red - *(unsigned __int8 *)(palette + 4 * color)];
        mincolor = color;
      }
    }
    *a1++ = mincolor;
    --a2;
  }
  return 1;
}


// ==============================================================================
/*
 * EA: 0x1900BCAD
 * CodeView symbol: IconDraw
 * Demangled: IconDraw
 * IDA name: ?IconDraw@@YIHPAUtagDRAWITEMSTRUCT@@KK@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xACAD
 * Code length: 0x16C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall IconDraw(struct tagDRAWITEMSTRUCT *a1, unsigned int a2, unsigned int programid)
{
  int id; // [esp+Ch] [ebp-28h]
  _DWORD myrect[4]; // [esp+14h] [ebp-20h] BYREF
  _DWORD *icon; // [esp+28h] [ebp-Ch]
  _DWORD *testicon; // [esp+30h] [ebp-4h]

  icon = nullptr;
  for ( testicon = (_DWORD *)TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Head(a1: &s_iconlist);
        (int)testicon > 0;
        testicon = (_DWORD *)TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::RawNext(ptr: (int)testicon) )
  {
    if ( (testicon[2] & a2) != 0 )
    {
      icon = testicon;
      break;
    }
    for ( id = 0; testicon[id + 5] != 0 && id < 32; ++id )
    {
      if ( programid == testicon[id + 5] )
      {
        icon = testicon;
        break;
      }
    }
  }
  if ( icon == nullptr || s_iconmappedbitmap == 0 )
    return 0;
  myrect[0] = 0;
  myrect[1] = 0;
  myrect[2] = icon[3];
  myrect[3] = icon[4];
  return SDlgBltToWindowI(
           a1: a1->hwndItem,
           a2: 0,
           a3: a1->rcItem.left,
           a4: (a1->rcItem.bottom - a1->rcItem.top - 13) / 2 + a1->rcItem.top,
           a5: icon[37] + s_iconmappedbitmap,
           a6: myrect,
           a7: icon + 3,
           a8: -1,
           a9: 0,
           a10: 13369376);
}


// ==============================================================================
/*
 * EA: 0x1900BE20
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >
 * IDA name: ??0?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QAE@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xAE20
 * Code length: 0x21
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
char *__thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>(char *ecx0)
{
  TSLink<_ICONVALUE>::TSLink<_ICONVALUE>(a1: ecx0 + 4);
  TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Constructor(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x1900BE50
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::~TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::~TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >
 * IDA name: ??1?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QAE@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xAE50
 * Code length: 0x1E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::~TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>(char *ecx0)
{
  TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::UnlinkAll(a1: ecx0);
  return TSLink<_ICONVALUE>::~TSLink<_ICONVALUE>(a1: ecx0 + 4);
}


// ==============================================================================
/*
 * EA: 0x1900BE70
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Clear
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Clear
 * IDA name: ?Clear@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QAEXXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xAE70
 * Code length: 0x48
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_ICONVALUE *__thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Clear(void *ecx0)
{
  _ICONVALUE *result; // eax

  while ( 1 )
  {
    result = (_ICONVALUE *)TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Head(a1: ecx0);
    if ( result == nullptr )
      break;
    _ICONVALUE::`scalar deleting destructor'(this: result, a2: 1u);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900BEC0
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::DeleteNode
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::DeleteNode
 * IDA name: ?DeleteNode@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QAEPAU_ICONVALUE@@PAU2@@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xAEC0
 * Code length: 0x49
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __stdcall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::DeleteNode(_ICONVALUE *ptr)
{
  int nextptr; // [esp+10h] [ebp-4h]

  nextptr = TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Next((int)ptr);
  if ( ptr != nullptr )
    _ICONVALUE::`scalar deleting destructor'(this: ptr, a2: 1u);
  return nextptr;
}


// ==============================================================================
/*
 * EA: 0x1900BF10
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Head
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Head
 * IDA name: ?Head@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QBEPAU_ICONVALUE@@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xAF10
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Head(char *this)
{
  return TSLink<_ICONVALUE>::Next(a1: this + 4, a2: this);
}


// ==============================================================================
/*
 * EA: 0x1900BF30
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::NewNode
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::NewNode
 * IDA name: ?NewNode@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QAEPAU_ICONVALUE@@KKK@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xAF30
 * Code length: 0x7D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_ICONVALUE *__stdcall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::NewNode(int location, int extrabytes, int flags)
{
  int v3; // eax
  const char *v4; // eax
  void *v5; // eax
  int v7; // [esp-4h] [ebp-14h]
  _ICONVALUE *v8; // [esp+0h] [ebp-10h]
  _ICONVALUE *v9; // [esp+8h] [ebp-8h]

  v3 = flags;
  LOBYTE(v3) = flags | 8;
  v7 = v3;
  v4 = type_info::internal_raw_name(this: (type_info *)&_ICONVALUE `RTTI Type Descriptor');
  v5 = (void *)SMemAlloc(a1: extrabytes + 152, a2: v4, a3: -2, a4: v7);
  v9 = (_ICONVALUE *)operator new(__formal: 152, ptr: v5);
  if ( v9 != nullptr )
    v8 = _ICONVALUE::_ICONVALUE(this: v9);
  else
    v8 = nullptr;
  if ( location != 0 )
    TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::LinkNode(ptr: (int)v8, linktype: location, existingptr: 0);
  return v8;
}


// ==============================================================================
/*
 * EA: 0x1900BFB0
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::RawNext
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::RawNext
 * IDA name: ?RawNext@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QBEPAU_ICONVALUE@@PBU2@@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xAFB0
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::RawNext(void *ecx0, int ptr)
{
  int v2; // eax

  v2 = TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Link(ptr);
  return TSLink<_ICONVALUE>::RawNext(a1: v2, a2: ecx0);
}


// ==============================================================================
/*
 * EA: 0x1900BFD0
 * CodeView symbol: _ICONVALUE::`scalar deleting destructor'
 * Demangled: _ICONVALUE::`scalar deleting destructor'
 * IDA name: ??_G_ICONVALUE@@QAEPAXI@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xAFD0
 * Code length: 0x2E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_ICONVALUE *__thiscall _ICONVALUE::`scalar deleting destructor'(_ICONVALUE *this, char __flags)
{
  _ICONVALUE::~_ICONVALUE(this);
  if ( (__flags & 1) != 0 )
    operator delete(ptr: this);
  return this;
}


// ==============================================================================
/*
 * EA: 0x1900C000
 * CodeView symbol: _ICONVALUE::_ICONVALUE
 * Demangled: _ICONVALUE::_ICONVALUE
 * IDA name: ??0_ICONVALUE@@QAE@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB000
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_ICONVALUE *__thiscall _ICONVALUE::_ICONVALUE(_ICONVALUE *this)
{
  TSLinkedNode<_ICONVALUE>::TSLinkedNode<_ICONVALUE>(a1: this);
  return this;
}


// ==============================================================================
/*
 * EA: 0x1900C020
 * CodeView symbol: _ICONVALUE::~_ICONVALUE
 * Demangled: _ICONVALUE::~_ICONVALUE
 * IDA name: ??1_ICONVALUE@@QAE@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB020
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __thiscall _ICONVALUE::~_ICONVALUE(_ICONVALUE *this)
{
  TSLinkedNode<_ICONVALUE>::~TSLinkedNode<_ICONVALUE>(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x1900C040
 * CodeView symbol: TSLinkedNode<_ICONVALUE>::TSLinkedNode<_ICONVALUE>
 * Demangled: TSLinkedNode<_ICONVALUE>::TSLinkedNode<_ICONVALUE>
 * IDA name: ??0?$TSLinkedNode@U_ICONVALUE@@@@QAE@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB040
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSLinkedNode<_ICONVALUE>::TSLinkedNode<_ICONVALUE>(void *ecx0)
{
  TSLink<_ICONVALUE>::TSLink<_ICONVALUE>(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x1900C060
 * CodeView symbol: TSLinkedNode<_ICONVALUE>::~TSLinkedNode<_ICONVALUE>
 * Demangled: TSLinkedNode<_ICONVALUE>::~TSLinkedNode<_ICONVALUE>
 * IDA name: ??1?$TSLinkedNode@U_ICONVALUE@@@@QAE@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB060
 * Code length: 0x1B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLinkedNode<_ICONVALUE>::~TSLinkedNode<_ICONVALUE>(void *ecx0)
{
  TSLinkedNode<_ICONVALUE>::Unlink(a1: ecx0);
  return TSLink<_ICONVALUE>::~TSLink<_ICONVALUE>(a1: ecx0);
}


// ==============================================================================
/*
 * EA: 0x1900C080
 * CodeView symbol: TSLink<_ICONVALUE>::TSLink<_ICONVALUE>
 * Demangled: TSLink<_ICONVALUE>::TSLink<_ICONVALUE>
 * IDA name: ??0?$TSLink@U_ICONVALUE@@@@QAE@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB080
 * Code length: 0x16
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSLink<_ICONVALUE>::TSLink<_ICONVALUE>(void *ecx0)
{
  TSLink<_ICONVALUE>::Constructor(a1: ecx0);
  return ecx0;
}


// ==============================================================================
/*
 * EA: 0x1900C0A0
 * CodeView symbol: TSLink<_ICONVALUE>::~TSLink<_ICONVALUE>
 * Demangled: TSLink<_ICONVALUE>::~TSLink<_ICONVALUE>
 * IDA name: ??1?$TSLink@U_ICONVALUE@@@@QAE@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB0A0
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_ICONVALUE>::~TSLink<_ICONVALUE>(void *this)
{
  return TSLink<_ICONVALUE>::Unlink(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x1900C0C0
 * CodeView symbol: TSLink<_ICONVALUE>::Next
 * Demangled: TSLink<_ICONVALUE>::Next
 * IDA name: ?Next@?$TSLink@U_ICONVALUE@@@@QBEPAU_ICONVALUE@@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB0C0
 * Code length: 0x2B
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_ICONVALUE>::Next(_DWORD *this)
{
  if ( (int)*(this + 1) <= 0 )
    return 0;
  else
    return *(this + 1);
}


// ==============================================================================
/*
 * EA: 0x1900C0F0
 * CodeView symbol: TSLink<_ICONVALUE>::RawNext
 * Demangled: TSLink<_ICONVALUE>::RawNext
 * IDA name: ?RawNext@?$TSLink@U_ICONVALUE@@@@QBEPAU_ICONVALUE@@XZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB0F0
 * Code length: 0x11
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_ICONVALUE>::RawNext(_DWORD *this)
{
  return *(this + 1);
}


// ==============================================================================
/*
 * EA: 0x1900C110
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Constructor
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Constructor
 * IDA name: ?Constructor@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@AAEXXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB110
 * Code length: 0x1C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Constructor(_DWORD *this)
{
  *this = 0;
  return TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::InitializeTerminator(a1: this, a2: this);
}


// ==============================================================================
/*
 * EA: 0x1900C130
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Link
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Link
 * IDA name: ?Link@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@ABEPAV?$TSLink@U_ICONVALUE@@@@PBU_ICONVALUE@@@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xB130
 * Code length: 0x1A
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Link(_DWORD *this, int ptr)
{
  return TSGetLink<_ICONVALUE>::Link(a1: ptr, a2: *this, a3: this);
}


// ==============================================================================
/*
 * EA: 0x1900C150
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::LinkNode
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::LinkNode
 * IDA name: ?LinkNode@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QAEXPAU_ICONVALUE@@K0@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xB150
 * Code length: 0xC1
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int *__thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::LinkNode(int *ecx0, int ptr, int linktype, int existingptr)
{
  int *result; // eax
  int prevlink; // [esp+8h] [ebp-Ch]
  int *existinglink; // [esp+Ch] [ebp-8h]
  _DWORD *link; // [esp+10h] [ebp-4h]

  link = (_DWORD *)TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Link(this: ecx0, ptr);
  if ( *link != 0 )
    TSLink<_ICONVALUE>::Unlink(a1: link);
  if ( existingptr != 0 )
  {
    result = (int *)TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Link(this: ecx0, ptr: existingptr);
    existinglink = result;
  }
  else
  {
    result = ecx0 + 1;
    existinglink = ecx0 + 1;
  }
  if ( linktype == 1 )
  {
    *link = existinglink;
    link[1] = existinglink[1];
    *(_DWORD *)TSLink<_ICONVALUE>::NextLink(linkoffset: *ecx0) = link;
    existinglink[1] = ptr;
    return (int *)ptr;
  }
  else if ( linktype == 2 )
  {
    prevlink = *existinglink;
    *link = *existinglink;
    link[1] = *(_DWORD *)(prevlink + 4);
    *(_DWORD *)(prevlink + 4) = ptr;
    *existinglink = (int)link;
    return (int *)ptr;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900C220
 * CodeView symbol: type_info::internal_raw_name
 * Demangled: type_info::internal_raw_name
 * IDA name: ?internal_raw_name@type_info@@QBEPBDXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB220
 * Code length: 0x11
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
const char *__thiscall type_info::internal_raw_name(type_info *this)
{
  return (char *)this + 8;
}


// ==============================================================================
/*
 * EA: 0x1900C240
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Next
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::Next
 * IDA name: ?Next@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QBEPAU_ICONVALUE@@PBU2@@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xB240
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Next(_DWORD *this, int ptr)
{
  _DWORD *v2; // eax

  v2 = (_DWORD *)TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Link(this, ptr);
  return TSLink<_ICONVALUE>::Next(this: v2);
}


// ==============================================================================
/*
 * EA: 0x1900C260
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::UnlinkAll
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::UnlinkAll
 * IDA name: ?UnlinkAll@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QAEXXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB260
 * Code length: 0x2C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::UnlinkAll(char *ecx0)
{
  int result; // eax

  while ( 1 )
  {
    result = TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Head(this: ecx0);
    if ( result == 0 )
      break;
    TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::UnlinkNode(ptr: result);
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900C290
 * CodeView symbol: TSLinkedNode<_ICONVALUE>::Unlink
 * Demangled: TSLinkedNode<_ICONVALUE>::Unlink
 * IDA name: ?Unlink@?$TSLinkedNode@U_ICONVALUE@@@@QAEXXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB290
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLinkedNode<_ICONVALUE>::Unlink(void *this)
{
  return TSLink<_ICONVALUE>::Unlink(a1: this);
}


// ==============================================================================
/*
 * EA: 0x1900C2B0
 * CodeView symbol: TSLink<_ICONVALUE>::Constructor
 * Demangled: TSLink<_ICONVALUE>::Constructor
 * IDA name: ?Constructor@?$TSLink@U_ICONVALUE@@@@AAEXXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB2B0
 * Code length: 0x1E
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSLink<_ICONVALUE>::Constructor(_DWORD *this)
{
  *this = 0;
  *(this + 1) = 0;
  return this;
}


// ==============================================================================
/*
 * EA: 0x1900C2D0
 * CodeView symbol: TSLink<_ICONVALUE>::NextLink
 * Demangled: TSLink<_ICONVALUE>::NextLink
 * IDA name: ?NextLink@?$TSLink@U_ICONVALUE@@@@ABEPAV1@H@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xB2D0
 * Code length: 0x3D
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __thiscall TSLink<_ICONVALUE>::NextLink(_DWORD *this, int linkoffset)
{
  if ( (int)*(this + 1) < 0 )
    return ~*(this + 1);
  if ( linkoffset < 0 )
    linkoffset = (int)this - *(_DWORD *)(*this + 4);
  return *(this + 1) + linkoffset;
}


// ==============================================================================
/*
 * EA: 0x1900C310
 * CodeView symbol: TSLink<_ICONVALUE>::Unlink
 * Demangled: TSLink<_ICONVALUE>::Unlink
 * IDA name: ?Unlink@?$TSLink@U_ICONVALUE@@@@QAEXXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB310
 * Code length: 0x47
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSLink<_ICONVALUE>::Unlink(_DWORD *ecx0)
{
  _DWORD *result; // eax

  result = ecx0;
  if ( *ecx0 != 0 )
  {
    *(_DWORD *)TSLink<_ICONVALUE>::NextLink(this: ecx0, linkoffset: -1) = *ecx0;
    result = (_DWORD *)*(ecx0 + 1);
    *(_DWORD *)(*ecx0 + 4) = result;
    *ecx0 = 0;
    *(ecx0 + 1) = 0;
  }
  return result;
}


// ==============================================================================
/*
 * EA: 0x1900C360
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::InitializeTerminator
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::InitializeTerminator
 * IDA name: ?InitializeTerminator@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@AAEXXZ
 * CodeView module: 20
 * CV address: segment 1 : 0xB360
 * Code length: 0x25
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::InitializeTerminator(_DWORD *this)
{
  *(this + 1) = this + 1;
  *(this + 2) = ~(unsigned int)(this + 1);
  return this;
}


// ==============================================================================
/*
 * EA: 0x1900C390
 * CodeView symbol: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::UnlinkNode
 * Demangled: TSList<_ICONVALUE,TSGetLink<_ICONVALUE> >::UnlinkNode
 * IDA name: ?UnlinkNode@?$TSList@U_ICONVALUE@@V?$TSGetLink@U_ICONVALUE@@@@@@QAEXPAU_ICONVALUE@@@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xB390
 * Code length: 0x20
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
_DWORD *__thiscall TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::UnlinkNode(_DWORD *this, int ptr)
{
  _DWORD *v2; // eax

  v2 = (_DWORD *)TSList<_ICONVALUE,TSGetLink<_ICONVALUE>>::Link(this, ptr);
  return TSLink<_ICONVALUE>::Unlink(ecx0: v2);
}


// ==============================================================================
/*
 * EA: 0x1900C3B0
 * CodeView symbol: TSGetLink<_ICONVALUE>::Link
 * Demangled: TSGetLink<_ICONVALUE>::Link
 * IDA name: ?Link@?$TSGetLink@U_ICONVALUE@@@@SIPAV?$TSLink@U_ICONVALUE@@@@PBV?$TSLinkedNode@U_ICONVALUE@@@@H@Z
 * CodeView module: 20
 * CV address: segment 1 : 0xB3B0
 * Code length: 0x13
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Icon.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Icon.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void *__thiscall TSGetLink<_ICONVALUE>::Link(void *this)
{
  return this;
}

