/***************************************************************************
                            referencecounting.cc
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

#include <referencecounting.hh>
#include <stdexcept>

void
ReferenceCounter::const_check() const
{
  if (m_count > 1) {
    asm volatile( "shared_object_modification:" );
    throw std::logic_error("Modifying a shared object...");
  }
}
