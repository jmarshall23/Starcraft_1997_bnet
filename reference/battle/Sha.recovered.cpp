/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Sha.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Sha.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 3
 */


// ==============================================================================
/*
 * EA: 0x19014B50
 * CodeView symbol: InitializeHash
 * Demangled: InitializeHash
 * IDA name: ?InitializeHash@@YIXPAU_shainfo@@@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x13B50
 * Code length: 0x3C
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Sha.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Sha.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall InitializeHash(struct _shainfo *a1)
{
  *(_DWORD *)a1 = 1732584193;
  *((_DWORD *)a1 + 1) = -271733879;
  *((_DWORD *)a1 + 2) = -1732584194;
  *((_DWORD *)a1 + 3) = 271733878;
  *((_DWORD *)a1 + 4) = -1009589776;
}


// ==============================================================================
/*
 * EA: 0x19014B8C
 * CodeView symbol: TransformHash
 * Demangled: TransformHash
 * IDA name: ?TransformHash@@YIXPAU_shainfo@@@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x13B8C
 * Code length: 0x3A0
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Sha.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Sha.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall TransformHash(struct _shainfo *a1)
{
  _DWORD v1[14]; // [esp+0h] [ebp-198h]
  struct _shainfo *infoptr; // [esp+38h] [ebp-160h]
  int temp; // [esp+3Ch] [ebp-15Ch]
  _DWORD w[80]; // [esp+40h] [ebp-158h]
  unsigned int loop; // [esp+180h] [ebp-18h]
  int e; // [esp+184h] [ebp-14h]
  int d; // [esp+188h] [ebp-10h]
  int c; // [esp+18Ch] [ebp-Ch]
  unsigned int b; // [esp+190h] [ebp-8h]
  unsigned int a; // [esp+194h] [ebp-4h]

  infoptr = a1;
  for ( loop = 0; loop < 0x10; ++loop )
    w[loop] = *((_DWORD *)infoptr + loop + 5);
  while ( loop < 0x50 )
  {
    w[loop] = v1[loop] ^ v1[loop + 2] ^ v1[loop + 8] ^ v1[loop + 13];
    w[loop] = (1 >> (32 - LOBYTE(w[loop]))) | (1 << w[loop]);
    ++loop;
  }
  a = *(_DWORD *)infoptr;
  b = *((_DWORD *)infoptr + 1);
  c = *((_DWORD *)infoptr + 2);
  d = *((_DWORD *)infoptr + 3);
  e = *((_DWORD *)infoptr + 4);
  for ( loop = 0; loop < 0x14; ++loop )
  {
    temp = ((a >> 27) | (32 * a)) + e + (d & ~b | c & b) + w[loop] + 1518500249;
    e = d;
    d = c;
    c = (b >> 2) | (b << 30);
    b = a;
    a = temp;
  }
  while ( loop < 0x28 )
  {
    temp = ((a >> 27) | (32 * a)) + e + (d ^ c ^ b) + w[loop] + 1859775393;
    e = d;
    d = c;
    c = (b >> 2) | (b << 30);
    b = a;
    a = temp;
    ++loop;
  }
  while ( loop < 0x3C )
  {
    temp = ((a >> 27) | (32 * a)) + e + (d & c | d & b | c & b) + w[loop] - 1894007588;
    e = d;
    d = c;
    c = (b >> 2) | (b << 30);
    b = a;
    a = temp;
    ++loop;
  }
  while ( loop < 0x50 )
  {
    temp = ((a >> 27) | (32 * a)) + e + (d ^ c ^ b) + w[loop] - 899497514;
    e = d;
    d = c;
    c = (b >> 2) | (b << 30);
    b = a;
    a = temp;
    ++loop;
  }
  *(_DWORD *)infoptr += a;
  *((_DWORD *)infoptr + 1) += b;
  *((_DWORD *)infoptr + 2) += c;
  *((_DWORD *)infoptr + 3) += d;
  *((_DWORD *)infoptr + 4) += e;
  for ( loop = 0; loop < 0x50; ++loop )
    w[loop] = 0;
}


// ==============================================================================
/*
 * EA: 0x19014F2C
 * CodeView symbol: ShaHash
 * Demangled: ShaHash
 * IDA name: ?ShaHash@@YIXPAXPBXK@Z
 * CodeView module: 9
 * CV address: segment 1 : 0x13F2C
 * Code length: 0xB8
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Sha.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Sha.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
void __fastcall ShaHash(void *a1, const void *a2, unsigned int bytes)
{
  size_t v3; // [esp+0h] [ebp-68h]
  _BYTE info[20]; // [esp+10h] [ebp-58h] BYREF
  _BYTE v7[64]; // [esp+24h] [ebp-44h] BYREF
  unsigned int offset; // [esp+64h] [ebp-4h]

  InitializeHash(a1: (struct _shainfo *)info);
  for ( offset = 0; offset < bytes; offset += 64 )
  {
    if ( bytes - offset <= 0x40 )
      v3 = bytes - offset;
    else
      v3 = 64;
    memcpy(a1: v7, Src: a2, Size: v3);
    if ( v3 < 0x40 )
      memset(a1: &v7[v3], Val: 0, Size: 64 - v3);
    TransformHash(a1: (struct _shainfo *)info);
  }
  memcpy(a1, Src: info, Size: 0x14u);
}

