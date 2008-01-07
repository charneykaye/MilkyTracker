/*
 *  ppui/Menu.h
 *
 *  Copyright 2008 Peter Barth
 *
 *  This file is part of Milkytracker.
 *
 *  Milkytracker is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Milkytracker is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Milkytracker.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 *  Menu.h
 *  MilkyTracker
 *
 *  Created by Peter Barth on Fri Mar 04 2005.
 *
 */

#ifndef MENU__H
#define MENU__H

#include "BasicTypes.h"
#include "SimpleVector.h"

class PPFont;
class PPGraphicsAbstract;
class PPContextMenu;

enum MenuCommandIDs
{
	MenuCommandIDNew			= 0,
	MenuCommandIDCut			= 1,
	MenuCommandIDCopy			= 2,
	MenuCommandIDPaste			= 3,
	MenuCommandIDSelectAll		= 4,
	MenuCommandIDSelectNothing	= 5,
	MenuCommandIDUndo			= 7,
	MenuCommandIDRedo			= 8,
	MenuCommandIDHide			= 0x10000
};

struct PPMenu
{
	struct Entry
	{
		PPString name;
		pp_int32 identifier;
		pp_uint32 state;
		PPContextMenu* subMenu;
		
		Entry(const PPString& s, pp_int32 theId, pp_uint32 stat = 0, PPContextMenu* aSubMenu = NULL) :
			name(s),
			identifier(theId),
			state(stat),
			subMenu(aSubMenu)
		{
		}
		
	};
	
	PPSimpleVector<Entry> items;
	PPFont* font;
	
	const PPColor* backColor;
	const PPColor* borderColor;
	const PPColor* selectionColor;
	
	const PPColor* textBrightColor;
	const PPColor* textDarkColor;
	
	bool subMenu;
	PPContextMenu* parentMenu;
	
	PPMenu(PPFont* aFont, const PPColor& selectionColor, const PPColor& bgColor, bool bSubMenu = false) :
		font(aFont),
		backColor(&bgColor),
		borderColor(&bgColor),
		textBrightColor(NULL),
		textDarkColor(NULL),
		subMenu(bSubMenu),
		parentMenu(NULL)
		//borderColor(128, 128, 128)
	{
		this->selectionColor = &selectionColor;
	}
	
	pp_uint32 getMaxWidth();			
	pp_uint32 getEntryHeight();
	PPRect getBoundingRect();
	
	bool setState(pp_int32 theId, pp_uint32 newState)
	{
		bool b = false;
		for (pp_int32 i = 0; i < items.size(); i++)
		{
			if (items.get(i)->identifier == theId)
			{
				items.get(i)->state = newState;
				b = true;
			}
		}
		
		return b;
	}
	
	void setSubMenu(bool bSubMenu) { subMenu = bSubMenu; }
	bool isSubMenu() { return subMenu; }
	
	void setParentMenu(PPContextMenu* parent) { parentMenu = parent; }
	PPContextMenu* getParentMenu() { return parentMenu; }
	
	void paint(PPGraphicsAbstract* g, pp_int32 px, pp_int32 py, pp_int32 menuSelection);

	void setFont(PPFont* font) { this->font = font; }
	
	void setTextBrightColor(const PPColor& color) { textBrightColor = &color; }
	void setTextDarkColor(const PPColor& color) { textDarkColor = &color; }
};

#endif
