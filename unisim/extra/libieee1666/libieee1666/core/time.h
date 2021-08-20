/*
 *  Copyright (c) 2014,
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

#ifndef __LIBIEEE1666_CORE_TIME_H__
#define __LIBIEEE1666_CORE_TIME_H__

#include "core/fwd.h"
#include <string>
#include <iostream>

namespace sc_core {

class sc_time
{
public:
	inline sc_time() ALWAYS_INLINE;
	sc_time(double, sc_time_unit);
	inline sc_time(const sc_time&) ALWAYS_INLINE;
	inline sc_time& operator = (const sc_time&) ALWAYS_INLINE;
	inline sc_dt::uint64 value() const ALWAYS_INLINE;
	inline double to_double() const ALWAYS_INLINE;
	double to_seconds() const;
	const std::string to_string() const;
	inline bool operator == (const sc_time&) const ALWAYS_INLINE;
	inline bool operator != (const sc_time&) const ALWAYS_INLINE;
	inline bool operator < (const sc_time&) const ALWAYS_INLINE;
	inline bool operator <= (const sc_time&) const ALWAYS_INLINE;
	inline bool operator > (const sc_time&) const ALWAYS_INLINE;
	inline bool operator >= (const sc_time&) const ALWAYS_INLINE;
	inline sc_time& operator += (const sc_time&) ALWAYS_INLINE;
	inline sc_time& operator -= (const sc_time&) ALWAYS_INLINE;
	inline sc_time& operator *= (double) ALWAYS_INLINE;
	inline sc_time& operator /= (double) ALWAYS_INLINE;
	inline sc_time& operator %= (const sc_time&) ALWAYS_INLINE; // This is not part of IEEE1666-2011
	void print(std::ostream& = std::cout) const;
	
	/////////////////////////////
private:
	friend class sc_kernel;
	friend const sc_time operator + (const sc_time&, const sc_time&);
	friend const sc_time operator - (const sc_time&, const sc_time&);
	friend const sc_time operator * (const sc_time&, double);
	friend const sc_time operator * (double, const sc_time&);
	friend const sc_time operator / (const sc_time&, double);
	friend double operator / (const sc_time&, const sc_time&);
	friend const sc_time operator % (const sc_time&, const sc_time&); // This is not part of IEEE1666-2011
	
	explicit sc_time(sc_dt::uint64);
	
	friend std::ostream& operator << (std::ostream&, const sc_time&);

	sc_dt::uint64 discrete_value;
};

inline const sc_time operator + (const sc_time&, const sc_time&) ALWAYS_INLINE;
inline const sc_time operator - (const sc_time&, const sc_time&) ALWAYS_INLINE;
inline const sc_time operator * (const sc_time&, double) ALWAYS_INLINE;
inline const sc_time operator * (double, const sc_time&) ALWAYS_INLINE;
inline const sc_time operator / (const sc_time&, double) ALWAYS_INLINE;
inline const sc_time operator % (const sc_time&, const sc_time&) ALWAYS_INLINE; // This is not part of IEEE1666-2011
inline double operator / ( const sc_time&, const sc_time& ) ALWAYS_INLINE;
std::ostream& operator << (std::ostream&, const sc_time&);
extern const sc_time SC_ZERO_TIME;
void sc_set_time_resolution(double, sc_time_unit);
sc_time sc_get_time_resolution();
const sc_time& sc_max_time();

//////////////////////////////////// sc_time /////////////////////////////////////

inline sc_time::sc_time()
	: discrete_value(0)
{
}

inline sc_time::sc_time(sc_dt::uint64 v)
	: discrete_value(v)
{
}

inline sc_time::sc_time( const sc_time& t)
	: discrete_value(t.discrete_value)
{
}

inline sc_time& sc_time::operator = ( const sc_time& t)
{
	discrete_value = t.discrete_value;
	return *this;
}

inline sc_dt::uint64 sc_time::value() const
{
	return discrete_value;
}

inline double sc_time::to_double() const
{
	return static_cast<double>(discrete_value);
}

inline bool sc_time::operator == (const sc_time& t) const
{
	return discrete_value == t.discrete_value;
}

inline bool sc_time::operator != (const sc_time& t) const
{
	return discrete_value != t.discrete_value;
}

inline bool sc_time::operator < (const sc_time& t) const
{
	return discrete_value < t.discrete_value;
}

inline bool sc_time::operator <= (const sc_time& t) const
{
	return discrete_value <= t.discrete_value;
}

inline bool sc_time::operator > (const sc_time& t) const
{
	return discrete_value > t.discrete_value;
}

inline bool sc_time::operator >= (const sc_time& t) const
{
	return discrete_value >= t.discrete_value;
}

inline sc_time& sc_time::operator += (const sc_time& t)
{
	discrete_value += t.discrete_value;
	return *this;
}

inline sc_time& sc_time::operator -= (const sc_time& t)
{
	discrete_value -= t.discrete_value;
	return *this;
}

inline sc_time& sc_time::operator *= (double d)
{
	discrete_value = static_cast<sc_dt::uint64>((discrete_value * d) + 0.5);
	return *this;
}

inline sc_time& sc_time::operator /= (double d)
{
	discrete_value = static_cast<sc_dt::uint64>((discrete_value / d) + 0.5);
	return *this;
}

inline sc_time& sc_time::operator %= (const sc_time& t)
{
	discrete_value %= t.discrete_value;
	return *this;
}

//////////////////////////////// global functions /////////////////////////////////

inline const sc_time operator + (const sc_time& t1, const sc_time& t2)
{
	return sc_time(t1.discrete_value + t2.discrete_value);
}

inline const sc_time operator - (const sc_time& t1, const sc_time& t2)
{
	return sc_time(t1.discrete_value - t2.discrete_value);
}

inline const sc_time operator * (const sc_time& t, double d)
{
	return sc_time(static_cast<sc_dt::uint64>((t.discrete_value * d) + 0.5));
}

inline const sc_time operator * (double d, const sc_time& t)
{
	return sc_time(static_cast<sc_dt::uint64>((d * t.discrete_value) + 0.5));
}

inline const sc_time operator / (const sc_time& t, double d)
{
	return sc_time(static_cast<sc_dt::uint64>((t.discrete_value / d) + 0.5));
}

inline double operator / (const sc_time& t1, const sc_time& t2)
{
	return (double) t1.discrete_value / t2.discrete_value;
}

inline const sc_time operator % (const sc_time& t1, const sc_time& t2)
{
	return sc_time(t1.discrete_value % t2.discrete_value);
}

} // end of namespace sc_core

#endif
