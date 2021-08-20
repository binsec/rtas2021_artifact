/***************************************************************************
                                  action.cc
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003-2007 CEA and Universite Paris Sud
    authors              : Gilles Mouchard, Yves Lhuillier
    email                : gilles.mouchard@cea.fr, yves.lhuillier@cea.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include <action.hh>
#include <operation.hh>
#include <sourcecode.hh>
#include <comment.hh>
#include <strtools.hh>
#include <iostream>

/** Constructor
    @param operation an operation object to which belong the action
    @param actionproto an action prototype object which is the prototype of this action
    @param source_code a C source code object which is the implementation of the action
    @param comment_list a C/C++ comment list object containing C/C++ comments relative to the action
    @param filename a filename object where the action was found
    @param lineno a line number where the action was found
*/
Action::Action( ActionProto const* _actionproto, SourceCode* _source_code, Vector<Comment> const& _comments, FileLoc const& _fileloc )
  : m_operation( 0 ), m_actionproto( _actionproto ), m_source_code( _source_code ),
    m_comments( _comments ), m_fileloc( _fileloc )
{}

/** Destructor
*/
Action::~Action() {}

/** Dump an action object into a stream
    @param action an action object to dump
    @param _sink a stream
*/
std::ostream&
operator<<( std::ostream& sink, Action const& action )
{
  sink << action.m_operation->symbol << '.' << action.m_actionproto->m_symbol << " = " << (*action.m_source_code);
  
  return sink;
}


/** Create an action prototype object
    @param type the type of the action prototype (constructor, destructor, static, or common)
    @param symbol a symbol object representing the action prototype
    @param returns a C source code object which is the return type of the action
    @param params a C source code pair object which is the parameters of the action
    @param default_source_code a C source code object which is the default implementation of the action
    @param comment_list a C/C++ comment list object containing C/C++ comments relative to the action prototype
    @param filename a filename object where the action prototype was found
    @param lineno a line number where the action prototype was found
*/
ActionProto::ActionProto( ActionProto::type_t _type, ConstStr _symbol, SourceCode* _returns,
                              Vector<CodePair>& _params, bool _constness, SourceCode* _defaultcode,
                              Vector<Comment>& _comments, FileLoc const& _fileloc )
  : m_type( _type ), m_symbol( _symbol ), m_returns( _returns ),
    m_params( _params ), m_constness( _constness ), m_defaultcode( _defaultcode ),
    m_comments( _comments ), m_fileloc( _fileloc )
{}

/** Dump an action prototype object into a stream
    @param sink a stream
*/
std::ostream&
operator << ( std::ostream& sink, ActionProto const& proto )
{
  switch( proto.m_type ) {
  case ActionProto::Constructor:  sink << "constructor "; break;
  case ActionProto::Static:       sink << "static "; break;
  case ActionProto::Destructor:   sink << "destructor "; break;
  case ActionProto::Common:       /* do nothing */ break;
  }
  
  sink << "action ";
  
  if (proto.m_returns) sink << (*proto.m_returns) << ' ';

  sink << proto.m_symbol << "(";
  char const* sep = "";
  for (Vector<CodePair>::const_iterator codepair = proto.m_params.begin(); codepair < proto.m_params.end(); sep = ", ", ++ codepair)
    sink << sep << *(*codepair);
  
  sink << ") " << (proto.m_constness ? "const " : "")<< (*proto.m_defaultcode) << '\n';
  return sink;
}

ActionProto::~ActionProto() {}

char const*
ActionProto::returntype() const
{
  if (m_returns) return m_returns->content.str();
  return "void";
}
