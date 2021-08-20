/***************************************************************************
                                  comment.cc
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

#include <comment.hh>

/** Constructor
    @param _content the C String containing the C comment
    @param _filename the filename object where the C/C++ comment was found
    @param _lineno the line number where the C/C++ comment was found
*/
Comment::Comment( ConstStr _content, FileLoc const& _fileloc )
  : content( _content ), fileloc( _fileloc )
{}

Comment::Comment( Comment const& _comment )
  : content( _comment.content ), fileloc( _comment.fileloc )
{}


/** Destructor
*/
Comment::~Comment() {}

