/*
 *  ppui/CheckBox.cpp
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

#include "CheckBox.h"
#include "Button.h"
#include "GraphicsAbstract.h"
#include "Event.h"
#include "Screen.h"
#include "Font.h"

static const char* checked = "\xFF";
static const char* notChecked = "\x20";

PPCheckBox::PPCheckBox(pp_int32 id, PPScreen* parentScreen, EventListenerInterface* eventListener, PPPoint location, bool checked /* = true */) :
	PPControl(id, parentScreen, eventListener, location, PPSize(10,10))
{
	button = new PPButton(id, parentScreen, this, location, this->size);
	button->setText(checked ? ::checked : ::notChecked);
}

PPCheckBox::~PPCheckBox()
{
	delete button;
}

bool PPCheckBox::isChecked()
{
	return button->getText().compareTo(checked) == 0;
}

void PPCheckBox::checkIt(bool checked)
{
	button->setText(checked ? ::checked : ::notChecked);
}

// from control
void PPCheckBox::paint(PPGraphicsAbstract* graphics)
{
	if (!isVisible())
		return;
	
	button->paint(graphics);
}
	
pp_int32 PPCheckBox::callEventListener(PPEvent* event)
{
	//if (!visible)
	//	return 0;

	if (event->getID() == eLMouseDown)
	{
		button->callEventListener(event);
	}
	else if (event->getID() == eLMouseUp)
	{
		button->setText(isChecked() ? ::notChecked : ::checked);
		
		button->callEventListener(event);
	}
	
	parentScreen->paintControl(this);

	return 0;
}

pp_int32 PPCheckBox::handleEvent(PPObject* sender, PPEvent* event)
{
	return eventListener->handleEvent(reinterpret_cast<PPObject*>(this), event); 	
}

void PPCheckBox::enable(bool b)
{
	PPControl::enable(b);
	
	button->enable(b);
}

void PPCheckBox::setSize(PPSize size)
{
	PPControl::setSize(size);
	button->setSize(size);
}

void PPCheckBox::setLocation(PPPoint location)
{
	PPControl::setLocation(location);
	button->setLocation(location);
}

