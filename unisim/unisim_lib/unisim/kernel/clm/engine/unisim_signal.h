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
                         unisim.h -- Main header file
                             -------------------
    begin                : Thu Apr 3 2003
    author               : Gilles Mouchard
    email                : mouchard@lri.fr, gilles.mouchard@cea.fr
                           gracia@lri.fr
 ***************************************************************************/

#ifndef __UNISIM_SIGNAL_H__
#define __UNISIM_SIGNAL_H__

#include "fsc.h"
#include <stdarg.h>

#include <boost/array.hpp>
/**
 * \brief Fake signal class used to connect desactivated ports.
 *
 * Used for optional ports. To support the connection syntax while the port
 * is desactivated.
 */
template <class T>
class unisim_desactivated_signal
{public:
  /**
   * \brief Fake known() method, perform an error.
   */
  bool known()
  { cerr << "eror: known() a desactivated signal !" << endl;
    exit(1);
  }
  /**
  
   * \brief Fake something() method, perform an error.
   */
  bool something()
  { cerr << "eror: something() a desactivated signal !" << endl;
    exit(1);
  }

  /**
   * \brief Fake nothing() method, perform an error.
   */
  bool nothing()
  { cerr << "eror: nothing() a desactivated signal !" << endl;
    exit(1);
  }

  /**
   * \brief Fake access method, perform an error.
   */
  operator T()
  { cerr << "eror: acessing a desactivated signal !" << endl;
    exit(1);
  }
  
  /**
   * \brief Fake value setting method, perform an error.
   */
  unisim_desactivated_signal & operator=(const T &value)
  { cerr << "eror: setting a desactivated signal !" << endl;
    exit(1);
  }
};

/**
 * \brief Class for the 3-signal objects connecting ports together.
 */
template < class T, uint32_t NCONFIG >
class unisim_3_signals
{public:
  unisim_3_signals(): data(), enable(), accept() {}
  fsc_prim_signal < T > data;           ///< Data signal
  fsc_prim_signal < boost::array<bool,NCONFIG> > enable;      ///< Enable signal
  fsc_prim_signal < boost::array<bool,NCONFIG> > accept;      ///< Accept signal
};

/**
 * \brief Class for the 3-signal objects connecting ports together.
 */
template < class T >
class unisim_3_signals<T,1>
{public:
  unisim_3_signals(): data(), enable(), accept() {}

  fsc_prim_signal < T > data;           ///< Data signal
  fsc_prim_signal < bool > enable;      ///< Enable signal
  fsc_prim_signal < bool > accept;      ///< Accept signal
};

#endif
