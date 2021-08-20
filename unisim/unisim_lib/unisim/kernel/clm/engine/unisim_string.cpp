/******************************************************************************* 
 *                                   BSD LICENSE 
 *******************************************************************************
 *  Copyright (c) 2006, CEA, INRIA and Universite Paris Sud
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without modification,  
 *  are permitted provided that the following conditions are met: 
 *  
 *   - Redistributions of source code must retain the above copyright notice, this  
 *     list of conditions and the following disclaimer.  
 *  
 *   - Redistributions in binary form must reproduce the above copyright notice,  
 *     this list of conditions and the following disclaimer in the documentation  
 *     and/or other materials provided with the distribution.  
 *  
 *   - Neither the name of CEA, INRIA and Universite Paris Sud nor the names of its 
 *     contributors may be used to endorse or promote products derived from this 
 *     software without specific prior written permission.  
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED  
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 *  DISCLAIMED.  
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY  
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *  
 ****************************************************************************** */ 

/***************************************************************************
                    fsc.cpp  - FraternitéSysC source code
                             -------------------
    begin                : Thu Apr 3 2003
    author               : Gilles Mouchard, Daniel Gracia Pérez
    email                : mouchard@lri.fr, gilles.mouchard@cea.fr
                           gracia@lri.fr
 ***************************************************************************/

#include "fsc.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


fsc_string::fsc_string(const int size)
{
	maxSize = size;
	this->size = 0;
	buffer = new char[maxSize];
	*buffer = 0;
}

fsc_string::fsc_string(const char *s)
{
	size = strlen(s) + 1;
	maxSize = (size + 15) & 0xfffffff0;
	buffer = (char *) memcpy(new char[maxSize], s, size);
}

fsc_string::fsc_string(const char *s, int n)
{
	int length = strlen(s);
	if(length > n) length = n;

	size = length + 1;
	maxSize = (size + 15) & 0xfffffff0;
	buffer = (char *) memcpy(new char[maxSize], s, size);
}

fsc_string::fsc_string(const fsc_string& s)
{
    size = s.size;
	maxSize = s.maxSize;
	buffer = (char *) memcpy(new char[maxSize], s.buffer, size);
}

fsc_string::~fsc_string()
{
	delete[] buffer;
}

fsc_string fsc_string::operator + (const char *s) const
{
	int length = strlen(s);
	fsc_string r(size + length);
	memcpy(r.buffer, buffer, size - 1);
	memcpy(r.buffer + size - 1, s, length + 1);
	r.size = size + length;
	return r;
}

fsc_string fsc_string::operator + (char c) const
{
	fsc_string r(size + 1);
	memcpy(r.buffer, buffer, size - 1);
	r.buffer[size - 1] = c;
	r.buffer[size] = 0;
	r.size = size + 1;
	return r;
}

fsc_string fsc_string::operator + (const fsc_string& s) const
{
	int length = s.size - 1;
	fsc_string r(size + length);
	memcpy(r.buffer, buffer, size - 1);
	memcpy(r.buffer + size - 1, s.buffer, s.size);
	r.size = size + length;
	return r;
}

fsc_string& fsc_string::operator = (const char *s)
{
	size = strlen(s) + 1;
	if(size > maxSize)
	{
		maxSize = (size + 15) % 0xfffffff0;
		delete[] buffer;
		buffer = new char[maxSize];
	}
	memcpy(buffer, s, size);
	return *this;
}

fsc_string& fsc_string::operator = (const fsc_string& s)
{
	size = s.size;
	if(size > maxSize)
	{
		maxSize = (size + 15) % 0xfffffff0;
		delete[] buffer;
		buffer = new char[maxSize];
	}
	memcpy(buffer, s.buffer, size);
	return *this;
}

fsc_string& fsc_string::operator += (const char *s )
{
	int length = strlen(s);
	if(size + length > maxSize)
	{
		maxSize = (size + length + 15) % 0xfffffff0;
		char *oldbuffer = buffer;
		buffer = new char[maxSize];
		memcpy(buffer, oldbuffer, size);
	}
	memcpy(buffer + size - 1, s, length + 1);
	size += length;
	return *this;
}

fsc_string& fsc_string::operator += (char c)
{
	if(size >= maxSize)
	{
		maxSize = (size + 1 + 15) % 0xfffffff0;
		char *oldbuffer = buffer;
		buffer = new char[maxSize];
		memcpy(buffer, oldbuffer, size);
	}
	buffer[size - 1] = 0;
	size ++;
	return *this;
}

fsc_string& fsc_string::operator += (const fsc_string& s)
{
	int length = s.size - 1;
	if(size + length > maxSize)
	{
		maxSize = (size + length + 15) % 0xfffffff0;
		char *oldbuffer = buffer;
		buffer = new char[maxSize];
		memcpy(buffer, oldbuffer, size);
	}
	memcpy(buffer + size - 1, s.buffer, length + 1);
	size += length;
	return *this;
}

bool fsc_string::operator == (const char *s) const
{
	return strcmp(buffer, s) == 0;
}

bool fsc_string::operator == (const fsc_string& s) const
{
	return strcmp(buffer, s.buffer) == 0;
}

bool fsc_string::operator != (const char *s) const
{
	return strcmp(buffer, s) != 0;
}

bool fsc_string::operator != (const fsc_string& s) const
{
	return strcmp(buffer, s.buffer) != 0;
}

bool fsc_string::operator < (const char *s) const
{
	return strcmp(buffer, s) < 0;
}

bool fsc_string::operator < (const fsc_string& s) const
{
	return strcmp(buffer, s.buffer) < 0;
}

bool fsc_string::operator <= (const char *s) const
{
	return strcmp(buffer, s) <= 0;
}

bool fsc_string::operator <= (const fsc_string& s) const
{
	return strcmp(buffer, s.buffer) <= 0;
}

bool fsc_string::operator > (const char *s) const
{
	return strcmp(buffer, s) > 0;
}

bool fsc_string::operator > (const fsc_string& s) const
{
	return strcmp(buffer, s.buffer) > 0;
}

bool fsc_string::operator >= (const char *s) const
{
	return strcmp(buffer, s) >= 0;
}

bool fsc_string::operator >= (const fsc_string& s) const
{
	return strcmp(buffer, s.buffer) >= 0;
}

int fsc_string::length() const
{
	return size - 1;
}

const char *fsc_string::c_str() const
{
	return buffer;
}

char fsc_string::operator [] (int index) const
{
	return buffer[index];
}

char& fsc_string::operator [] (int index)
{
	return buffer[index];
}

fsc_string fsc_string::to_string(const char* format, ...)
{
	static char buffer[4096];

	va_list arg;

	va_start(arg, format);
	vsprintf(buffer, format, arg);
	va_end(arg);
	return fsc_string(buffer);
}

ostream& operator << (ostream& os, const fsc_string& s)
{
	os << s.buffer;
	return os;
}

