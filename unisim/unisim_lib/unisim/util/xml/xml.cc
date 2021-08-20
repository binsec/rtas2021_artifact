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
 
#include "unisim/util/xml/xml.hh"
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <stdio.h>

namespace unisim {
namespace util {
namespace xml {

using std::list;
using std::istream;
using std::fstream;
using std::ostream;
using std::endl;
using std::string;
using std::cerr;

Property::Property(const string &_name, const string &_value)
	: name(_name)
	, value(_value)
	, filename("")
	, lineno(0)
{}

Property::Property(const string& name, const string& value, const string& filename, int lineno)
{
	this->name = name;
	this->value = value;
	this->filename = filename;
	this->lineno = lineno;
}

Property::~Property()
{
}

ostream& operator << (ostream& os, const Property& prop)
{
	return os << prop.name << "=" << "\"" << prop.value << "\"";
}

Node::Node(const string &_name)
	: parent(0)
	, name(_name)
	, props(0)
	, childs(0)
	, filename("")
	, lineno(0)
{
	props = new PtrList<Property>();
	childs = new PtrList<Node>();
}
	
Node::Node(const string &_name, PtrList<Property> *_props, PtrList<Node> *_childs)
	: parent(0)
	, name(_name)
	, props(_props)
	, childs(_childs)
	, filename("")
	, lineno(0)
{}
	
Node::Node(const string& name, PtrList<Property> *props, PtrList<Node> *childs, const string& filename, int lineno)
{
	this->name = name;
	this->props = props;
	this->childs = childs;
	this->filename = filename;
	this->lineno = lineno;
	this->parent = 0;
}

Node::~Node()
{
	delete props;
	delete childs;
}

ostream& operator << (ostream& os, const Node& node)
{
	os << "<" << node.name;
	if(node.props)
	{
		PtrList<Property>::iterator prop;
		for(prop = node.props->begin(); prop != node.props->end(); prop++)
		{
			os << " ";
			os << **prop;
		}
	}
	
	if(node.childs->empty())
	{
		os << "/>" << endl;
	}
	else
	{
		os << ">" << endl;
		PtrList<Node>::iterator child;
		for(child = node.childs->begin(); child != node.childs->end(); child++)
		{
			os << **child;
		}
		os << "</" << node.name << ">" << endl;
	}

	return os;
}

Node& Node::operator << (Property& prop)
{
	props->push_back(&prop);
	return *this;
}

Node& Node::operator << (Node& child)
{
	childs->push_back(&child);
	child.parent = this;
	return *this;
}

const int TOK_LESS_SLASH = 256;
const int TOK_SLASH_GREATER = 257;
const int TOK_IDENTIFIER = 258;
const int TOK_STRING = 259;
const int TOK_EOF = 260;

const int PEEK_TOK = 0;
const int GET_TOK = 1;

const int CONTEXT_INITIAL = 0;
const int CONTEXT_COMMENT = 1;

Parser::Parser(std::ostream& _debug_info_stream, std::ostream& _debug_warning_stream, std::ostream& _debug_error_stream)
	: debug_info_stream(_debug_info_stream), debug_warning_stream(_debug_warning_stream), debug_error_stream(_debug_error_stream), root_node(NULL), current_lineno(0), look_ahead_token(0)
{
}

Node *Parser::Parse(const string& filename)
{
	fstream f(filename.c_str(), fstream::in);
	
	if(f.fail())
	{
		Error(filename, 0, "Can't open \"%s\"", filename.c_str());
		return 0;
	}
	
	look_ahead_token = 0;
	current_filename = filename;
	current_lineno = 1;
	Node *node = ParseNode(f);
	if(node)
	{
		string value;
		int token = ReadToken(f, value, GET_TOK);
		if(token && token != TOK_EOF)
		{
			Error(current_filename, current_lineno, "parse error! expect an end of file, got %s", GetTokenName(token));
			std::cout << GetTokenName(token);
			delete node;
			return 0;
		}
	}
	return node;
}

Parser::~Parser()
{
}


const char *Parser::GetTokenName(int token)
{
	static char buffer[4];
	switch(token)
	{
		case 0: return "no token";
		case TOK_LESS_SLASH: return "TOK_LESS_SLASH";
		case TOK_SLASH_GREATER: return "TOK_SLASH_GREATER";
		case TOK_STRING: return "TOK_STRING";
		case TOK_IDENTIFIER: return "TOK_IDENTIFIER";
		case TOK_EOF: return "TOK_EOF";
	}
	buffer[0] = '\'';
	buffer[1] = token;
	buffer[2] = '\'';
	buffer[3] = 0;
	return buffer;
}

void Parser::Error(const string& filename, int lineno, const char *format, ...)
{
	char buffer[2048];
	char s[1024];
	char *buf = buffer;
	va_list args;

	va_start(args, format);
	vsprintf(s, format, args);
	if(!filename.empty())
		buf += sprintf(buf, "%s:", filename.c_str());
	if(lineno)
		buf += sprintf(buf, "%d:", lineno);

	buf += sprintf(buf, "%s\n", s);
	
	debug_error_stream << buffer << std::endl;
	va_end(args);
}

int Parser::ReadToken(istream& is, string& value, int mode)
{
	int state;
	int context = CONTEXT_INITIAL;
	int token = 0;
	value.erase();
	
	if(look_ahead_token)
	{
		if(mode == GET_TOK)
		{
			value = look_ahead_value;
			token = look_ahead_token;
			look_ahead_token = 0;
			//printf("GET %s\n", get_token_name(token));
			return token;
		}
		if(mode == PEEK_TOK)
		{
			value = look_ahead_value;
			//printf("PEEK %s\n", get_token_name(look_ahead_token));
			return look_ahead_token;
		}
		return 0;
	}
	
	
	state = 0;
	char s[2];
	s[1] = 0;
	
	while(!token)
	{
		char c;
		is.get(c);
		
		//printf("state %d, got '%c'\n", state, c);

		if(is.eof()) return TOK_EOF;
		if(is.fail()) return 0;
		s[0] = c;
		
		switch(context)
		{
			case CONTEXT_INITIAL:
				switch(state)
				{
					case 0:
						if(c == '<')
						{
							state = 3;
							break;
						}
						if(c == '/')
						{
							state = 4;
							break;
						}
						
						if(c == '>' || c == '=')
						{
							token = c;
							break;
						}
						if(c == '\n') { current_lineno++; continue; }
						if(c == ' ' || c == '\t') continue;
						if(c == '\"')
						{
							// start of string
							state = 2;
							break;
						}
						if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
						{
							// start of identifier
							state = 1;
							value += s;
							break;
						}
						Error(current_filename, current_lineno, "unexpected character '%c'\n", c);
						return 0;
						
					case 1:
						if((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9') && c != '_' && c != '-')
						{
							// end of identifier
							is.unget();
							token = TOK_IDENTIFIER;
							break;
						}
						value += s;
						break;
					case 2:
						if(c == '\"')
						{
							// end of string
							token = TOK_STRING;
							break;
						}
						if(c == '\n') current_lineno++;
						value += s;
						break;
						
					case 3:
						if(c == '!')
						{
							state = 5;
							break;
						}
						if(c == '/')
						{
							token = TOK_LESS_SLASH;
							break;
						}
						is.unget();
						token = '<';
						break;
						
					case 4:
						if(c == '>')
						{
							token = TOK_SLASH_GREATER;
							break;
						}
						is.unget();
						token = '/';
						break;
						
					case 5:
						if(c == '-')
						{
							state = 6;
							break;
						}
						if(c == '\n') current_lineno++;
						Error(current_filename, current_lineno, "unexpected character '%c' after '!'. Expect a '-'.", c);
						return 0;
					
					case 6:
						if(c == '-')
						{
							context = CONTEXT_COMMENT;
							state = 0;
							break;
						}
						if(c == '\n') current_lineno++;
						Error(current_filename, current_lineno, "unexpected character '%c' after '-'. Expect a '-'.", c);
						return 0;
				}
				break;
			case CONTEXT_COMMENT:
				switch(state)
				{
					case 0:
						if(c == '-')
						{
							state = 1;
							break;
						}
						if(c == '\n') current_lineno++;
						break;
					case 1:
						if(c == '-')
						{
							state = 2;
							break;
						}
						if(c == '\n') current_lineno++;
						state = 0;
						break;
						
					case 2:
						if(c == '>')
						{
							context = CONTEXT_INITIAL;
							state = 0;
							break;
						}
						break;
				}
				break;
		}
	}
	
	if(mode == PEEK_TOK)
	{
		look_ahead_token = token; // save token
		look_ahead_value = value;
		//printf("PEEK %s\n", get_token_name(token));
		return token;
	}
	
	if(mode == GET_TOK)
	{
		//printf("GET %s\n", get_token_name(token));
		return token;
	}
	
	return 0;
}

PtrList<Property> *Parser::ParsePropertyList(istream& is)
{
	Property *prop = 0;
	PtrList<Property> *prop_list;
	int token;
	int prop_lineno;

	prop_list = new PtrList<Property>();
	string dummy_string;
	string prop_name;
	string prop_value;

	do
	{
		token = ReadToken(is, prop_name, GET_TOK);
		if(!token) return 0;

		if(token != TOK_IDENTIFIER)
		{
			Error(current_filename, current_lineno, "parse error! expect an identifier for property");
			return 0;
		}

		prop_lineno = current_lineno;

		token = ReadToken(is, dummy_string, GET_TOK);
		if(!token) return 0;

		if(token != '=')
		{
			Error(current_filename, current_lineno, "parse error! expect an '=' after identifier \"%s\"", prop_name.c_str());
			return 0;
		}

		token = ReadToken(is, prop_value, GET_TOK);
		if(!token) return 0;

		if(token != TOK_STRING)
		{
			Error(current_filename, current_lineno, "parse error! expect a double-quoted string after '=' to define value of \"%s\"", prop_name.c_str());
			return 0;
		}

		prop = new Property(prop_name, prop_value, current_filename, prop_lineno);

		prop_list->push_back(prop);

		token = ReadToken(is, dummy_string, PEEK_TOK);

		if(!token)
		{
			Error(current_filename, current_lineno, "parse error! unexpected end-of-file");
			if(prop_list) delete prop_list;
			return 0;
		}
	} while(token == TOK_IDENTIFIER);
	return prop_list;
}

PtrList<Node> *Parser::ParseNodeList(istream& is)
{
	int token;
	PtrList<Node> *node_list;
	Node *node;
	string dummy_string;

	node_list = new PtrList<Node>();

	do
	{
		node = ParseNode(is);
		if(!node) return 0;

		node_list->push_back(node);

		token = ReadToken(is, dummy_string, PEEK_TOK);
		if(!token)
		{
			delete node_list;
			return 0;
		}

	} while(token != TOK_LESS_SLASH);
	return node_list;
}

Node *Parser::ParseNode(istream& is)
{
	PtrList<Property> *props = 0;
	PtrList<Node> *childs = 0;
	int token;
	int node_lineno;
	string open_tag;
	string close_tag;
	string dummy_string;

	token = ReadToken(is, dummy_string, GET_TOK);
	if(!token) return 0;

	if(token != '<')
	{
		Error(current_filename, current_lineno, "parse error! expect a '<'");
		cerr << "Got a " << GetTokenName(token) << endl; 
		return 0;
	}

	node_lineno = current_lineno;

	token = ReadToken(is, open_tag, GET_TOK);
	if(!token) return 0;

	if(token != TOK_IDENTIFIER)
	{
		Error(current_filename, current_lineno, "parse error! expect an identifier for a tag after '<'");
		return 0;
	}


	token = ReadToken(is, dummy_string, PEEK_TOK);
	if(!token) return 0;

	if(token == TOK_IDENTIFIER)
	{
		props = ParsePropertyList(is);
		if(!props) return 0;
	}
	else
	{
		props = new PtrList<Property>();
	}

	token = ReadToken(is, dummy_string, GET_TOK);
	if(!token) return 0;

	if(token == TOK_SLASH_GREATER)
	{
		return new Node(open_tag, props, new PtrList<Node>(), current_filename, node_lineno);
	}

	if(token != '>')
	{
		Error(current_filename, current_lineno, "parse error! expect a '>' or '/>' to finish tag <%s>", open_tag.c_str());
		if(props) delete props;
		return 0;
	}

	token = ReadToken(is, dummy_string, PEEK_TOK);
	
	if(!token)
	{
		if(props) delete props;
		return 0;
	}

	if(token != TOK_LESS_SLASH)
	{
		childs = ParseNodeList(is);
		if(!childs)
		{
			if(props) delete props;
			return 0;
		}
	}
	else
	{
		childs = new PtrList<Node>();
	}

	token = ReadToken(is, dummy_string, GET_TOK);
	if(!token)
	{
		if(props) delete props;
		if(childs) delete childs;
		return 0;
	}

	if(token != TOK_LESS_SLASH)
	{
		Error(current_filename, current_lineno, "parse error! expect a '</'");
		if(props) delete props;
		if(childs) delete childs;
		return 0;
	}

	token = ReadToken(is, close_tag, GET_TOK);
	if(!token)
	{
		if(props) delete props;
		if(childs) delete childs;
		return 0;
	}

	if(token != TOK_IDENTIFIER)
	{
		Error(current_filename, current_lineno, "parse error! expect an identifier after '</'");
		if(props) delete props;
		if(childs) delete childs;
		return 0;
	}

	token = ReadToken(is, dummy_string, GET_TOK);
	if(!token)
	{
		if(props) delete props;
		if(childs) delete childs;
		return 0;
	}

	if(token != '>')
	{
		Error(current_filename, current_lineno, "parse error! expect a '>' after identifier to finish tag </%s>", close_tag.c_str());
		if(props) delete props;
		if(childs) delete childs;
		return 0;
	}

	if(open_tag != close_tag)
	{
		Error(current_filename, current_lineno, "parse error! open tag <%s> and close tag </%s> dont't match", open_tag.c_str(), close_tag.c_str());
		Error(current_filename, node_lineno, "<%s> is here", open_tag.c_str());
		if(props) delete props;
		if(childs) delete childs;
		return 0;
	}

	return new Node(open_tag, props, childs, current_filename, node_lineno);
}

} // end of namespace xml
} // end of namespace util
} // end of namespace unisim
