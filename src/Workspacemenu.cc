//
// Workspacemenu.cc for Blackbox - an X11 Window manager
// Copyright (c) 1997, 1998 by Brad Hughes, bhughes@tcac.net
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// (See the included file COPYING / GPL-2.0)
//

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
#  include "../config.h"
#endif

#include "blackbox.hh"
#include "Screen.hh"
#include "Toolbar.hh"
#include "Workspacemenu.hh"
#include "Workspace.hh"


Workspacemenu::Workspacemenu(Blackbox *bb, BScreen *scrn) :
  Basemenu(bb, scrn)
{
  screen = scrn;
  
  setTitleVisibility(False);
  setMovable(False);
  setHidable(True);
  setAlignment(Basemenu::MenuAlignBottom);
  defaultMenu();
  
  insert("New Workspace");
  insert("Remove Last");
}


void Workspacemenu::itemSelected(int button, int index) {
  if (button == 1) {
    if (index == 0) {
      screen->addWorkspace();
    } else if (index == 1) {
      screen->removeLastWorkspace();
    } else if ((screen->getCurrentWorkspace()->getWorkspaceID() !=
		(index - 1)) &&
               ((index - 1) < screen->getCount())) {
      screen->changeWorkspaceID(index - 1);
      hide();
    }
  }
}


void Workspacemenu::hide(void) {
  screen->getToolbar()->redrawMenuButton(False, True);

  Basemenu::hide();
}
