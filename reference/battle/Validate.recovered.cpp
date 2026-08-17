/*
 * Recovered compilation unit
 *
 * Original/reconstructed full source path:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Validate.cpp
 *
 * CodeView module object:
 *   C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Validate.obj
 *
 * Generated from embedded NB11 CodeView + Hex-Rays.
 * This is reconstructed pseudocode, NOT original source code.
 * Functions: 3
 */


// ==============================================================================
/*
 * EA: 0x1901ED10
 * CodeView symbol: FixInvalidString
 * Demangled: FixInvalidString
 * IDA name: ?FixInvalidString@@YIHPAD0@Z
 * CodeView module: 3
 * CV address: segment 1 : 0x1DD10
 * Code length: 0x104
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Validate.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Validate.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall FixInvalidString(char *a1, char *a2)
{
  char c; // [esp+8h] [ebp-Ch]
  char *pbad; // [esp+Ch] [ebp-8h]
  char *pbada; // [esp+Ch] [ebp-8h]
  int v7; // [esp+10h] [ebp-4h]

  v7 = 0;
  while ( 1 )
  {
    c = *a1;
    if ( *a1 == 0 )
      return v7;
    if ( c >= 32 )
    {
      if ( c <= 126 )
      {
        for ( pbad = sgszBasicBad; *pbad != 0; ++pbad )
        {
          if ( c == *pbad )
          {
            *a1 = 45;
            v7 = 1;
            break;
          }
        }
        if ( *pbad == 0 && a2 != nullptr )
        {
          for ( pbada = a2; *pbada != 0; ++pbada )
          {
            if ( c == *pbada )
            {
              *a1 = 45;
              v7 = 1;
              break;
            }
          }
        }
      }
      else
      {
        *a1 = 45;
        v7 = 1;
      }
    }
    else
    {
      *a1 = 45;
      v7 = 1;
    }
    ++a1;
  }
}


// ==============================================================================
/*
 * EA: 0x1901EE14
 * CodeView symbol: InvalidChars
 * Demangled: InvalidChars
 * IDA name: ?InvalidChars@@YIHPBD0@Z
 * CodeView module: 3
 * CV address: segment 1 : 0x1DE14
 * Code length: 0xA2
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Validate.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Validate.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
int __fastcall InvalidChars(const char *a1, const char *a2)
{
  const char *name; // [esp+4h] [ebp-8h]
  unsigned __int8 c; // [esp+8h] [ebp-4h]

  name = a1;
  if ( strpbrk(Str: a1, Control: sgszBasicBad) != nullptr )
    return 1;
  if ( strpbrk(Str: name, Control: a2) != nullptr )
    return 1;
  while ( 1 )
  {
    c = *name;
    if ( *name == 0 )
      break;
    if ( c < 0x20u )
      return 1;
    if ( c > 0x7Eu && c < 0xC0u )
      return 1;
    ++name;
  }
  return 0;
}


// ==============================================================================
/*
 * EA: 0x1901EEB6
 * CodeView symbol: IsInvalid
 * Demangled: IsInvalid
 * IDA name: ?IsInvalid@@YIHD@Z
 * CodeView module: 3
 * CV address: segment 1 : 0x1DEB6
 * Code length: 0x25
 * Original object: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\debug\Validate.obj
 * Source: C:\TEMP\c\sasaki\BATTLE\Battle_build_J\battle\Validate.cpp
 * Source path confidence: reconstructed-from-build-output-parent
 */
// Function has frame pointer omitted
BOOL __fastcall IsInvalid(char a1)
{
  return strchr(Str: sgszBasicBad, Val: a1) != nullptr;
}

