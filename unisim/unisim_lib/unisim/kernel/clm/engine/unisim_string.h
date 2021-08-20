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
                       fsc.h  -  FraternitéSysC header
                             -------------------
    begin                : Thu Apr 3 2003
    copyright            : (C) 2003 CEA and Universite Paris Sud
    author               : Gilles Mouchard
    email                : mouchard@lri.fr, gilles.mouchard@cea.fr
                           gracia@lri.fr
 ***************************************************************************/

#ifndef UNISIM_STRING_H
#define UNISIM_STRING_H

class fsc_string
{
public:
	fsc_string(const int size = 16);
	fsc_string(const char *s);
	fsc_string(const char *s, int n);
	fsc_string(const fsc_string& s);

	~fsc_string();

	fsc_string operator + (const char *s) const;
	fsc_string operator + (char c) const;
	fsc_string operator + (const fsc_string& s) const;

    fsc_string& operator =(const char *s);
    fsc_string& operator =(const fsc_string& s);

    fsc_string& operator +=(const char *s );
    fsc_string& operator +=(char c);
    fsc_string& operator +=(const fsc_string& s);

	bool operator == (const char *s) const;
	bool operator == (const fsc_string& s) const;
	bool operator != (const char *s) const;
	bool operator != (const fsc_string& s) const;
	bool operator < (const char *s) const;
	bool operator < (const fsc_string& s) const;
	bool operator <= (const char *s) const;
	bool operator <= (const fsc_string& s) const;
	bool operator > (const char *s) const;
	bool operator > (const fsc_string& s) const;
	bool operator >= (const char *s) const;
	bool operator >= (const fsc_string& s) const;

    int length() const;
	const char *c_str() const;
    char operator [] (int index) const;
	char& operator [] (int index);

    static fsc_string to_string(const char* format, ...);

    friend ostream& operator << (ostream& os, const fsc_string& s);

private:
	int size;
	int maxSize;
	char *buffer;
};

#endif
