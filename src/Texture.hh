// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// Texture.hh for Blackbox - an X11 Window manager
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

#ifndef TEXTURE_HH
#define TEXTURE_HH

#include "Color.hh"
#include "Util.hh"

#include <string>
using std::string;

// bevel options
#define BImage_Flat		(1l<<1)
#define BImage_Sunken		(1l<<2)
#define BImage_Raised		(1l<<3)

// textures
#define BImage_Solid		(1l<<4)
#define BImage_Gradient		(1l<<5)

// gradients
#define BImage_Horizontal	(1l<<6)
#define BImage_Vertical		(1l<<7)
#define BImage_Diagonal		(1l<<8)
#define BImage_CrossDiagonal	(1l<<9)
#define BImage_Rectangle	(1l<<10)
#define BImage_Pyramid		(1l<<11)
#define BImage_PipeCross	(1l<<12)
#define BImage_Elliptic		(1l<<13)

// bevel types
#define BImage_Bevel1		(1l<<14)
#define BImage_Bevel2		(1l<<15)

// inverted image
#define BImage_Invert		(1l<<16)

// parent relative image
#define BImage_ParentRelative   (1l<<17)

#ifdef    INTERLACE
// fake interlaced image
#  define BImage_Interlaced	(1l<<18)
#endif // INTERLACE


class BTexture {
public:
  BTexture(int scr = -1);
  BTexture(const string &, int scr = -1);
  ~BTexture();

  void setColor(const BColor &);
  void setColorTo(const BColor &cc) { ct = cc; }

  const BColor &color(void) const { return c; }
  const BColor &colorTo(void) const { return ct; }
  const BColor &lightColor(void) const { return lc; }
  const BColor &shadowColor(void) const { return sc; }

  unsigned long texture(void) const { return t; }
  void setTexture(unsigned long tt) { t = tt; }
  void addTexture(unsigned long tt) { t |= tt; }

  BTexture &operator=(const BTexture &tt);
  bool operator==(const BTexture &tt)
  {
    return (c == tt.c  && ct == tt.ct && lc == tt.lc && sc == tt.sc && t == tt.t);
  }
  bool operator!=(const BTexture &tt)
  {
    return (! operator==(tt));
  }

  int screen() const { return scrn; }
  void setScreen(int scr);

  const string &description() const { return descr; }
  void setDescription(const string &);

  Pixmap render(const Size &, Pixmap old = 0);

private:
  BColor c, ct, lc, sc;
  string descr;
  unsigned long t;
  int scrn;
};

#endif // TEXTURE_HH
