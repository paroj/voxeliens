/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
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

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#include "Utility.h"

#include <cstdint>
#include <iomanip>
#include <sstream>

namespace Thermite
{
	std::string generateUID(const std::string& prefix)
	{
		//This will be incremented each time
		static uint32_t currentID = 0;

		//We'll split it just to make it more readable
		uint16_t lowerBits = currentID & 0x0000FFFF;
		uint16_t upperBits = currentID >> 16;

		std::stringstream ss;
		//hex and uppercase just get set once. Fill and width seem to need to be set twice.
		ss << prefix 
			<< "-" 
			<< std::hex << std::uppercase
			<< std::setw(4) << std::setfill('0') << upperBits 
			<< "-" 
			<< std::setw(4) << std::setfill('0') << lowerBits;

		//Inrement the counter and return the string.
		++currentID;
		return ss.str();
	}
}