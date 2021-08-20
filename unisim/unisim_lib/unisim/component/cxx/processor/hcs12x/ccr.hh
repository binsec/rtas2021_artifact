/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */


#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CCR_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CCR_HH__

#include <inttypes.h>
#include <stdlib.h>

#include <unisim/service/interfaces/register.hh>
#include <unisim/kernel/kernel.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

/* I think it's better to declare the CCR as uint16_t and then use mask to set/get each bit */

class CCR_t : public unisim::service::interfaces::Register
{
public:
	static const uint16_t SETC	=0x0001;
	static const uint16_t CLRC=0xFFFE;

	static const uint16_t SETV=0x0002;
	static const uint16_t CLRV=0xFFFD;

	static const uint16_t SETZ=0x0004;
	static const uint16_t CLRZ=0xFFFB;

	static const uint16_t SETN=0x0008;
	static const uint16_t CLRN=0xFFF7;

	static const uint16_t SETI=0x0010;
	static const uint16_t CLRI=0xFFEF;

	static const uint16_t SETH=0x0020;
	static const uint16_t CLRH=0xFFDF;

	static const uint16_t SETX=0x0040;
	static const uint16_t CLRX=0xFFBF;

	static const uint16_t SETS=0x0080;
	static const uint16_t CLRS=0xFF7F;

	static const uint16_t SETIPL=0x0700;
	static const uint16_t CLRIPL=0xF8FF;


	CCR_t(uint16_t* _ccrReg) : ccrReg(_ccrReg) { };
	~CCR_t() { };

	inline uint8_t getC();
	inline void 	setC();
	inline void 	clrC();

	inline uint8_t getV();
	inline void 	setV();
	inline void 	clrV();

	inline uint8_t getZ();
	inline void 	setZ();
	inline void 	clrZ();

	inline uint8_t getN();
	inline void 	setN();
	inline void 	clrN();

	inline uint8_t getI();
	inline void 	setI();
	inline void 	clrI();

	inline uint8_t getH();
	inline void 	setH();
	inline void 	clrH();

	inline uint8_t getX();
	inline void	setX();
	inline void 	clrX();

	inline uint8_t getS();
	inline void 	setS();
	inline void 	clrS();

	inline uint8_t getIPL();
	inline void 	setIPL(uint8_t newIPL);
	inline void 	clrIPL();

	inline uint8_t getCCRLow();
	inline void setCCRLow(uint8_t val);

	inline uint8_t getCCRHigh();
	inline void setCCRHigh(uint8_t val);

	inline uint16_t getCCR();
	inline void setCCR(uint16_t val);

	void reset() { *ccrReg = 0x00D0; /* S=1 X=1 I=1 */ }

	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;

	unisim::service::interfaces::Register *GetLowRegister();
	unisim::service::interfaces::Register *GetHighRegister();

//	uint16_t *ccrReg; // u----ipl(3bits) SXHI NZVC

private:

	uint16_t* ccrReg; // u----ipl(3bits) SXHI NZVC

}; // end class CCR_t


inline uint8_t CCR_t::getC() { return (*ccrReg & SETC);};
inline void 	CCR_t::setC() { *ccrReg |= SETC;};
inline void 	CCR_t::clrC() { *ccrReg &= CLRC;};

inline uint8_t CCR_t::getV() { return ((*ccrReg & SETV) >> 1);};
inline void 	CCR_t::setV() { *ccrReg |= SETV;};
inline void 	CCR_t::clrV() { *ccrReg &= CLRV;};

inline uint8_t CCR_t::getZ() { return ((*ccrReg & SETZ) >> 2);};
inline void 	CCR_t::setZ() { *ccrReg |= SETZ;};
inline void 	CCR_t::clrZ() { *ccrReg &= CLRZ;};

inline uint8_t CCR_t::getN() { return ((*ccrReg & SETN) >> 3);};
inline void 	CCR_t::setN() { *ccrReg |= SETN;};
inline void 	CCR_t::clrN() { *ccrReg &= CLRN;};

inline uint8_t CCR_t::getI() { return ((*ccrReg & SETI) >> 4);};
inline void 	CCR_t::setI() { *ccrReg |= SETI;};
inline void 	CCR_t::clrI() { *ccrReg &= CLRI;};

inline uint8_t CCR_t::getH() { return ((*ccrReg & SETH) >> 5);};
inline void 	CCR_t::setH() { *ccrReg |= SETH;};
inline void 	CCR_t::clrH() { *ccrReg &= CLRH;};

inline uint8_t CCR_t::getX() { return ((*ccrReg & SETX) >> 6);};
inline void	CCR_t::setX() { *ccrReg |= SETX;};
inline void 	CCR_t::clrX() { *ccrReg &= CLRX;};

inline uint8_t CCR_t::getS() { return ((*ccrReg & SETS) >> 7);};
inline void 	CCR_t::setS() { *ccrReg |= SETS;};
inline void 	CCR_t::clrS() { *ccrReg &= CLRS;};

// IPL is 3-bits
inline uint8_t CCR_t::getIPL() { return ((*ccrReg & SETIPL) >> 8);};
inline void 	CCR_t::setIPL(uint8_t newIPL) { clrIPL(); *ccrReg |= (((uint16_t) newIPL) & 0x00FF) << 8;};
inline void 	CCR_t::clrIPL() { *ccrReg &= CLRIPL;};

inline uint8_t CCR_t::getCCRLow() {
	uint8_t val = (uint8_t) (*ccrReg & 0x00FF);

	return (val);
};
inline void CCR_t::setCCRLow(uint8_t val) {
	// before check the X-bit: once cleared it cannot be set by program instructions
	if (getX() == 0) val &= 0xBF;
	*ccrReg = (*ccrReg & 0xFF00) | val;
};

inline uint8_t CCR_t::getCCRHigh() {
	uint8_t val = (uint8_t) ((*ccrReg >> 8) & 0x00FF);

	return (val);
};

inline void CCR_t::setCCRHigh(uint8_t val) {
	*ccrReg = (*ccrReg & 0x00FF) | ((uint16_t) val << 8);
};


inline uint16_t CCR_t::getCCR() {
	return (*ccrReg);
};

inline void CCR_t::setCCR(uint16_t val) {
	// before check the X-bit: once cleared it cannot be set by program instructions
	if (getX() == 0) val &= 0xFFBF;

	*ccrReg = val;
};

// **************************

class TimeBaseRegisterView : public unisim::kernel::VariableBase
{
public:
	typedef enum
	{
		TB_LOW,
		TB_HIGH
	} Type;
	TimeBaseRegisterView(const char *name, unisim::kernel::Object *owner, uint16_t& storage, Type type, const char *description);
	virtual ~TimeBaseRegisterView();
	virtual const char *GetDataTypeName() const;
	virtual unsigned int GetBitSize() const;
	virtual operator bool () const;
	virtual operator long long () const;
	virtual operator unsigned long long () const;
	virtual operator double () const;
	virtual operator std::string () const;
	virtual unisim::kernel::VariableBase& operator = (bool value);
	virtual unisim::kernel::VariableBase& operator = (long long value);
	virtual unisim::kernel::VariableBase& operator = (unsigned long long value);
	virtual unisim::kernel::VariableBase& operator = (double value);
	virtual unisim::kernel::VariableBase& operator = (const char * value);
private:
	uint16_t& storage;
	Type type;
};


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CCR_HH__
