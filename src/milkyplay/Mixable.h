/*
 *  milkyplay/Mixable.h
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
 *  Mixable.h
 *  cliplayer
 *
 *  Created by Peter Barth on 14.12.07.
 *
 */

#ifndef __MIXABLE_H__
#define __MIXABLE_H__

#include "MilkyPlayTypes.h"

struct Mixable
{
	virtual void mix(mp_sint32* buffer, mp_uint32 numSamples) = 0;			
};

#endif
