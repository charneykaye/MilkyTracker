/*
 *  ppui/TransparentContainer.cpp
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

#include "GraphicsAbstract.h"
#include "TransparentContainer.h"
#include "BasicTypes.h"

PPTransparentContainer::PPTransparentContainer(pp_int32 id, PPScreen* parentScreen, EventListenerInterface* eventListener, PPPoint location, PPSize size) :
	PPContainer(id, parentScreen, eventListener, location, size)
{
}

PPTransparentContainer::~PPTransparentContainer()
{
}

void PPTransparentContainer::paint(PPGraphicsAbstract* g)
{
	if (!isVisible())
		return;
	
	paintControls(g);
}
