/*
WDL - wdlutf8.h
Copyright (C) 2005 and later, Cockos Incorporated

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/

#ifndef _WDLUTF8_H_
#define _WDLUTF8_H_

/* todo: handle overlongs?
 * todo: handle multi-byte (make WideStr support UTF-16)
 */

#ifndef WDL_WCHAR
  #ifdef _WIN32
    #define WDL_WCHAR WCHAR
  #else
    #define WDL_WCHAR wchar_t
  #endif
#endif

static int WDL_MBtoWideStr(WDL_WCHAR *dest, const char *src, int destlenbytes)
{
  if (!dest || !src || destlenbytes < 1) return 0;
  const unsigned char *p = (const unsigned char *)src;
  WDL_WCHAR *w = dest, *dest_endp = dest+(size_t)destlenbytes/sizeof(WDL_WCHAR)-1;
  for (; *p && w < dest_endp; ++w)
  {
    if (*p < 0x80)
    {
      *w=*p++;
    }
    else 
    {
      if (!p[1]) break;

      if (*p < 0xE0)
      {
        *w = (*p++&0x1F)<<6;
        *w |= (*p++&0x3F);
      }
      else
      {
        if (!p[2]) break;
        if (*p < 0xF0)
        {
          *w = (*p++&0x0F)<<12;
          *w |= (*p++&0x3F)<<6;
          *w |= (*p++&0x3F);
        }
        else 
        {
          if (!p[3]) break;

          *w='_';
          if (*p < 0xF8)
          {
            p += 4;
          }
          else 
          {
            if (!p[4]) break;
            if (*p < 0xFC) 
            {
              p += 5;
            }
            else 
            {
              if (!p[5]) break;
              p += 6;
            }
          }
        }
      }
    }
  }
  *w=0; 
  return w-dest;
}

static int WDL_WideToMBStr(char *dest, const WDL_WCHAR *src, int destlenbytes)
{
  if (!dest || !src || destlenbytes < 1) return 0;
  unsigned char *p = (unsigned char *)dest;
  unsigned char *dest_endp = p + destlenbytes - 1;
  const WDL_WCHAR* w = src;
  for (w=src; *w && p < dest_endp; ++w)
  {
    if (*w < 0x80) 
    {
      *p++=(*w&0xff);
    }
    else if (*w < 0x800 && p < dest_endp-1)
    {
      *p++=0xC0|((*w>>6)&0x1F);
      *p++=0x80|(*w&0x3F);
    }
    else if (p < dest_endp-2)
    {
      *p++=0xE0|((*w>>12)&0x0F);
      *p++=0x80|((*w>>6)&0x3F);
      *p++=0x80|(*w&0x3F);
    }
    else
    {
      *p++='_';
    }
  }
  *p=0;
  return p-(unsigned char *)dest;
}

#endif
