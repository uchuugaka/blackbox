// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// Color.hh for Blackbox - an X11 Window manager
// Copyright (c) 2001 - 2002 Sean 'Shaleh' Perry <shaleh at debian.org>
// Copyright (c) 1997 - 2000, 2002 Bradley T Hughes <bhughes at trolltech.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef COLOR_HH
#define COLOR_HH

#include <X11/Xlib.h>

#include <map>
#include <string>

class BaseDisplay;
class BGCCache;

class BColor {
public:
  BColor(const BaseDisplay * const _display = 0, unsigned int _screen = ~(0u));
  BColor(int _r, int _g, int _b,
         const BaseDisplay * const _display, unsigned int _screen = ~(0u));
  BColor(const std::string &_name,
         const BaseDisplay * const _display, unsigned int _screen = ~(0u));
  ~BColor();

  inline const std::string &name() const { return colorname; }

  inline const int   red(void) const { return r; }
  inline const int green(void) const { return g; }
  inline const int  blue(void) const { return b; }
  void setRGB(int _r, int _g, int _b)
  {
    deallocate();
    r = _r;
    g = _g;
    b = _b;
  }

  inline const BaseDisplay * const display() const { return dpy; }
  inline const unsigned int screen() const { return scrn; }
  void setDisplay(const BaseDisplay * const _display, unsigned int _screen = ~(0u));

  inline const Bool isAllocated(void) const { return allocated; }

  inline const Bool isValid() const { return r != -1 && g != -1 && b != -1; }

  const unsigned long pixel(void) const;

  // operators
  BColor &operator=(const BColor &c);
  inline Bool operator==(const BColor &c) const
  { return (r == c.r && b == c.b && b == c.b); }
  inline Bool operator!=(const BColor &c) const
  { return (! operator==(c)); }

  static void cleanupColorCache();

private:
  void parseColorName();
  void allocate();
  void deallocate();

  Bool allocated;
  int r, g, b;
  unsigned long p;
  const BaseDisplay *dpy;
  unsigned int scrn;
  std::string colorname;

  // global color allocator/deallocator
  struct RGB {
    const BaseDisplay * const display;
    const unsigned int screen;
    const int r, g, b;

    RGB() : display(0), screen(~(0u)),  r(-1), g(-1), b(-1) { }
    RGB(const BaseDisplay * const a, const unsigned int b,
        const int x, const int y, const int z)
      : display(a), screen(b), r(x), g(y), b(z)
    {
    }
    RGB(const RGB &x)
      : display(x.display), screen(x.screen), r(x.r), g(x.g), b(x.b)
    {
    }
    inline const Bool operator==(const RGB &x) const
    {
      return display == x.display &&
              screen == x.screen &&
                   r == x.r && g == x.g && b == x.b;
    }
    inline const Bool operator<(const RGB &x) const
    {
      unsigned long p1, p2;
      p1 = (screen << 24 | r << 16 | g << 8 | b) & 0x00ffffff;
      p2 = (x.screen << 24 | x.r << 16 | x.g << 8 | x.b) & 0x00ffffff;
      return p1 < p2;
    }
  };
  struct PixelRef {
    const unsigned long p;
    int count;
    inline PixelRef() : p(0), count(0) { }
    inline PixelRef(const unsigned long x) : p(x), count(1) { }
  };
  typedef std::map<RGB,PixelRef> ColorCache;
  static ColorCache colorcache;
  static bool cleancache;
  static void doCacheCleanup();
};

#endif // COLOR_HH
