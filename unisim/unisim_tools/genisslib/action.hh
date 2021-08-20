/***************************************************************************
                                 action.hh
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

#ifndef __ACTION_HH__
#define __ACTION_HH__

#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>
#include <errtools.hh>
#include <vect.hh>
#include <iosfwd>

/** An action implementation object */
struct Action : virtual ReferenceCounter
{
  Operation const*       m_operation;    /**< The associated operation  */
  ActionProto const*     m_actionproto;  /**< The associated action prototype */
  Ptr<SourceCode>        m_source_code;  /**< The C implementation of the action */
  Vector<Comment>        m_comments;     /**< The list of the C comment associated with the action */
  FileLoc                m_fileloc;      /**< File location of the declaration */
  //  Action*            m_base;

  Action( ActionProto const* _actionproto, SourceCode* _source_code,
            Vector<Comment> const& _comments, FileLoc const& _fileloc );
  ~Action();
};

std::ostream& operator<<( std::ostream& sink, Action const& _act );

/** An action prototype object */
struct ActionProto : virtual ReferenceCounter
{
  enum type_t { Constructor, Destructor, Static, Common };
  
  type_t                 m_type;                /**< The type of the action prototype */
  ConstStr               m_symbol;              /**< The associated symbol */
  Ptr<SourceCode>        m_returns;             /**< The C return type of the action */
  Vector<CodePair>       m_params;              /**< The C parameters of the action */
  bool                   m_constness;           /**< The constness of the action */
  Ptr<SourceCode>        m_defaultcode;         /**< The default C implementation of the action */
  Vector<Comment>        m_comments;            /**< The list of the C comment associated with the action prototype */
  FileLoc                m_fileloc;             /**< The file location where the action prototype was declared */
  
  ActionProto( type_t _type, ConstStr _symbol, SourceCode* _returns, Vector<CodePair>& _params,
               bool _constness, SourceCode* _defaultcode, Vector<Comment>& _comments, FileLoc const& _fileloc );
  ~ActionProto();
  
  char const*              returntype() const;
};

std::ostream& operator<<( std::ostream& sink, ActionProto const& _ap );

#endif // __ACTION_HH__
