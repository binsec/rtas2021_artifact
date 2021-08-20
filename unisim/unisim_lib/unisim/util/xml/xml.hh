/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
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
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_UTIL_XML_HH__
#define __UNISIM_UTIL_XML_HH__

#include <list>
#include <string>
#include <iosfwd>
#include <stdarg.h>

namespace unisim {
namespace util {
namespace xml {

using std::list;
using std::istream;
using std::fstream;
using std::ostream;
using std::string;

class Node;
class Property;

template <class T>
class PtrList : public list<T *>
{
private:
   typedef list<T *> inherited;
public:
	virtual ~PtrList()
	{
		typename PtrList<T>::iterator i;
		for(i = inherited::begin(); i != inherited::end(); i++)
			delete *i;
	}
};

class Property
{
public:
	Property(const string &name, const string &value);
	Property(const string &name, const string& value, const string& filename, int lineno);
	virtual ~Property();
	inline const string& Name() const { return name; }
	inline const string& Value() const { return value; }
	inline const string& Filename() const { return filename; }
	inline int LineNo() const { return lineno; }
	friend ostream& operator << (ostream& os, const Property& prop);
private:
	string name;			/*< property name */
	string value;		/*< property value */
	string filename;		/*< source xml file name */
	int lineno;			/*< line number in xml file */
};

class Node
{
public:
	Node(const string &name);
	Node(const string &name, PtrList<Property> *props, PtrList<Node> *childs);
	Node(const string& name, PtrList<Property> *props, PtrList<Node> *childs, const string& filename, int lineno);
	virtual ~Node();
	inline const string& Name() const { return name; }
	inline const list<Property *> *Properties() const { return props; }
	inline const Property *GetProperty(const string &str) const {
		PtrList<Property>::iterator it;
		for(it = props->begin(); it != props->end(); it++) {
			if((*it)->Name() == str)
				return *it;
		}
		return 0;
	}
	inline list<Node *> *Childs() const { return childs; }
	inline const string& Filename() const { return filename; }
	inline int LineNo() const { return lineno; }
	friend ostream& operator << (ostream& os, const Node& node);
	Node& operator << (Property& prop);
	Node& operator << (Node& child);
	inline Node *GetParent() const { return parent;}
private:
	Node *parent; /*< pointer parent to node */
	string name; /*< node name */
	PtrList<Property> *props; /*< node properties */
	PtrList<Node> *childs;	/*< child nodes */
	string filename;		/*< source xml file name */
	int lineno;			/*< line number in xml file */
};

class Parser
{
public:
	Parser(std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream);
	virtual ~Parser();
	Node *Parse(const string& filename);
private:
	const char *GetTokenName(int token);
	int ReadToken(istream& is, string& value, int mode);
	PtrList<Property> *ParsePropertyList(istream& is);
	PtrList<Node> *ParseNodeList(istream& is);
	Node *ParseNode(istream& is);
	bool ParseComment(istream& is);

	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	Node *root_node;
	int current_lineno;
	string current_filename;
	int look_ahead_token;
	string look_ahead_value;
	
	void Error(const string& filename, int lineno, const char *format, ...);
};

} // end of namespace xml
} // end of namespace util
} // end of namespace unisim

#endif
