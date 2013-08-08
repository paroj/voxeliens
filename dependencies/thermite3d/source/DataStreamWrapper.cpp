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

#include "DataStreamWrapper.h"

namespace Thermite
{
	DataStreamWrapper::DataStreamWrapper(const Ogre::DataStreamPtr &dsp)
		:m_Dsp(dsp)
	{
	}

	std::streamsize DataStreamWrapper::showmanyc (void)
	{
			return -1;
	}

	std::streamsize DataStreamWrapper::xsgetn(char* s, std::streamsize n)
	{
			return m_Dsp->read(s,n);
	}	

	std::streamsize DataStreamWrapper::xsputn(const char_type*, std::streamsize)
	{
		throw std::ios::failure("Cannot write to an Ogre::DataStream");
		return -1;
	}

	std::streamsize DataStreamWrapper::_Xsgetn_s(char *s, size_t size, std::streamsize n)
	{
			return m_Dsp->read(s,n);
	}
}