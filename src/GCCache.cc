#ifdef HAVE_CONFIG_H
#  include "../config.h"
#endif // HAVE_CONFIG_H

#include "GCCache.hh"
#include "BaseDisplay.hh"
#include "Color.hh"

#include <stdio.h>


void BGCCache::Context::set( const BColor &color, XFontStruct *font, int func, int subw )
{
  BaseDisplay *display = BaseDisplay::instance();

  XGCValues gcv;
  pixel = gcv.foreground = color.pixel();
  function = gcv.function = func;
  subwindow = gcv.subwindow_mode = subw;
  unsigned long mask = GCForeground | GCFunction | GCSubwindowMode;

  if ( font ) {
    fontid = gcv.font = font->fid;
    mask |= GCFont;
  } else
    fontid = 0;

  XChangeGC( *display, gc, mask, &gcv );
}

void BGCCache::Context::set( XFontStruct *font )
{
  if ( ! font ) {
    fontid = 0;
    return;
  }

  BaseDisplay *display = BaseDisplay::instance();
  XGCValues gcv;
  fontid = gcv.font = font->fid;
  XChangeGC( *display, gc, GCFont, &gcv );
}


static BGCCache *INSTANCE = 0;

BGCCache::BGCCache()
  : context_count( 128 ), cache_size( 16 ), cache_buckets( 8 )
{
  INSTANCE = this;

  contexts = new Context[ context_count ];

  cache = new Item*[ cache_size * cache_buckets ];
  int i, b, s;
  for ( i = 0, s = 0; i < cache_size; i++ ) {
    for ( b = 0; b < cache_buckets; b++ )
      cache[ s++ ] = new Item;
  }
}

BGCCache::~BGCCache()
{
  int i, b, s;
  for ( i = 0, s = 0; i < cache_size; i++ ) {
    for ( b = 0; b < cache_buckets; b++ )
      delete cache[ s++ ];
  }
  delete [] cache;
  delete [] contexts;

  INSTANCE = 0;
}

BGCCache *BGCCache::instance()
{
  return INSTANCE;
}

BGCCache::Context *BGCCache::nextContext( int scr )
{
  BaseDisplay *display = BaseDisplay::instance();
  Window hd = display->screenInfo( scr )->rootWindow();

  register Context *c = contexts;
  register int i = context_count;
  while ( i-- ) {
    if ( ! c->gc ) {
      c->gc = XCreateGC( *display, hd, 0, 0 );
      c->used = false;
      c->screen = scr;
    }
    if ( ! c->used && c->screen == scr ) {
      c->used = true;
      return c;
    }
    c++;
  }

  fprintf( stderr, "BGCCache: context fault!\n" );
  exit( 1 );
  return 0; // shutup
}

void BGCCache::release( Context *ctx )
{
  ctx->used = false;
}

BGCCache::Item &BGCCache::find( const BColor &color, XFontStruct *font,
				int function, int subwindow )
{
  unsigned long pixel = color.pixel();
  unsigned long fontid = font ? font->fid : 0;
  int screen = color.screen();
  int key = color.red() ^ color.green() ^ color.blue();
  int k = (key % cache_size) * cache_buckets;
  Item *c = cache[ k ], *prev = 0;

#define NOMATCH ( c->ctx && ( c->ctx->pixel != pixel || \
                              c->ctx->function != function || \
                              c->ctx->subwindow != subwindow || \
                              c->ctx->screen != screen ) )

  if ( NOMATCH ) {
    prev = c;
    c = cache[ ++k ];
    if ( NOMATCH ) {
      prev = c;
      c = cache[ ++k ];
      if ( NOMATCH ) {
        prev = c;
        c = cache[ ++k ];
        if ( NOMATCH ) {
          prev = c;
          c = cache[ ++k ];
          if ( NOMATCH ) {
            prev = c;
            c = cache[ ++k ];
            if ( NOMATCH ) {
              prev = c;
              c = cache[ ++k ];
              if ( NOMATCH ) {
                prev = c;
                c = cache[ ++k ];
                if ( NOMATCH ) {
                  if ( c->count == 0 && c->ctx->screen == screen ) {
                    // use this cache item
                    c->ctx->set( color, font, function, subwindow );
                    c->ctx->used = true;
                    c->count = 1;
                    c->hits = 1;
                    return *c;
                  } else {
                    // cache fault!
                    fprintf( stderr, "BGCCache: cache fault\n" );
                    abort();
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  if ( c->ctx ) {
    // reuse existing context
    if ( fontid && fontid != c->ctx->fontid )
      c->ctx->set( font );
    c->count++;
    c->hits++;
    if ( prev && c->hits > prev->hits ) {
      cache[ k     ] = prev;
      cache[ k - 1 ] = c;
    }
    return *c;
  }

  c->ctx = nextContext( screen );
  c->ctx->set( color, font, function, subwindow );
  c->ctx->used = true;
  c->count = 1;
  c->hits = 1;
  return *c;
}

void BGCCache::release( Item &item )
{
  item.count--;
}

void BGCCache::purge()
{
  // fprintf(stderr, "BGCCache::purge\n");
  int i, b, s;
  for ( i = 0, s = 0; i < cache_size; i++ ) {
    for ( b = 0; b < cache_buckets; b++ ) {
      Item *d = cache[ s++ ];

      if ( d->ctx ) {
        // fprintf( stderr, "  cache %4d,%2d=%2d: count %4d",
        // i, b, s - 1, d->count );
        // fprintf( stderr, " pixel %6lx f %d sub %d screen %d",
        // d->ctx->pixel, d->ctx->function,
        // d->ctx->subwindow, d->ctx->screen );

        if ( d->count == 0 ) {
          release( d->ctx );
          d->ctx = 0;
          // fprintf( stderr, " released\n" );
          // } else {
          // fprintf( stderr, "\n" );
        }
      }
    }
  }
}
