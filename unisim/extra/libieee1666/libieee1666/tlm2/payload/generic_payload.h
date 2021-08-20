/*
 *  Copyright (c) 2015,
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

#ifndef __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_PAYLOAD_GENERIC_PAYLOAD_H__
#define __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_PAYLOAD_GENERIC_PAYLOAD_H__

#include <systemc>

#if defined(__APPLE_CC__)
#include <sys/types.h>
#elif defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define BIG_ENDIAN __BIG_ENDIAN
#define __BYTE_ORDER __LITTLE_ENDIAN
#define BYTE_ORDER LITTLE_ENDIAN
#else
#include <endian.h>
#endif

namespace tlm {

class tlm_generic_payload;
class tlm_mm_interface
{
public:
	virtual void free(tlm_generic_payload*) = 0;
	virtual ~tlm_mm_interface() {}
};

unsigned int max_num_extensions();
class tlm_extension_base
{
public:
	virtual tlm_extension_base* clone() const = 0;
	virtual void free() { delete this; }
	virtual void copy_from(tlm_extension_base const &) = 0;
protected:
	virtual ~tlm_extension_base() {}
public:
	static unsigned int allocate_extension_id();
};

template <typename T>
class tlm_extension : public tlm_extension_base
{
public:
	virtual tlm_extension_base* clone() const = 0;
	virtual void copy_from(tlm_extension_base const &) = 0;
	virtual ~tlm_extension() {}
	const static unsigned int ID;
};

enum tlm_gp_option
{
	TLM_MIN_PAYLOAD,
	TLM_FULL_PAYLOAD,
	TLM_FULL_PAYLOAD_ACCEPTED
};

enum tlm_command
{
	TLM_READ_COMMAND,
	TLM_WRITE_COMMAND,
	TLM_IGNORE_COMMAND
};

enum tlm_response_status
{
	TLM_OK_RESPONSE = 1,
	TLM_INCOMPLETE_RESPONSE = 0,
	TLM_GENERIC_ERROR_RESPONSE = -1,
	TLM_ADDRESS_ERROR_RESPONSE = -2,
	TLM_COMMAND_ERROR_RESPONSE = -3,
	TLM_BURST_ERROR_RESPONSE = -4,
	TLM_BYTE_ENABLE_ERROR_RESPONSE = -5
};

#define TLM_BYTE_DISABLED 0x0
#define TLM_BYTE_ENABLED 0xff

class tlm_generic_payload
{
public:
	// Constructors and destructor
	tlm_generic_payload();
	explicit tlm_generic_payload( tlm_mm_interface* );
	virtual ~tlm_generic_payload();
private:
	// Disable copy constructor and assignment operator
	tlm_generic_payload( const tlm_generic_payload& );
	tlm_generic_payload& operator= ( const tlm_generic_payload& );
public:
	// Memory management
	void set_mm(tlm_mm_interface *);
	bool has_mm() const;
	void acquire();
	void release();
	int get_ref_count() const;
	void reset();
	void deep_copy_from(const tlm_generic_payload &);
	void update_original_from(const tlm_generic_payload&, bool use_byte_enable_on_read = true);
	void update_extensions_from(const tlm_generic_payload &);
	void free_all_extensions();
	// Access methods
	tlm_gp_option get_gp_option() const;
	void set_gp_option(const tlm_gp_option);
	tlm_command get_command() const;
	void set_command(const tlm_command);
	bool is_read();
	void set_read();
	bool is_write();
	void set_write();
	sc_dt::uint64 get_address() const;
	void set_address( const sc_dt::uint64 );
	unsigned char* get_data_ptr() const;
	void set_data_ptr(unsigned char *);
	unsigned int get_data_length() const;

	void set_data_length(const unsigned int);
	unsigned int get_streaming_width() const;
	void set_streaming_width(const unsigned int);
	unsigned char* get_byte_enable_ptr() const;
	void set_byte_enable_ptr(unsigned char *);
	unsigned int get_byte_enable_length() const;
	void set_byte_enable_length(const unsigned int);
	// DMI hint
	void set_dmi_allowed(bool);
	bool is_dmi_allowed() const;
	tlm_response_status get_response_status() const;
	void set_response_status(const tlm_response_status);
	std::string get_response_string();
	bool is_response_ok();
	bool is_response_error();
	// Extension mechanism
	template <typename T> T *set_extension(T *);
	tlm_extension_base* set_extension(unsigned int, tlm_extension_base *);
	template <typename T> T *set_auto_extension(T *);
	tlm_extension_base *set_auto_extension(unsigned int, tlm_extension_base *);
	template <typename T> void get_extension(T *&) const;
	template <typename T> T* get_extension() const;
	tlm_extension_base *get_extension(unsigned int) const;
	template <typename T> void clear_extension(const T *);
	template <typename T> void clear_extension();
	template <typename T> void release_extension(T *);
	template <typename T> void release_extension();
	void resize_extensions();
private:
	tlm_mm_interface *mm;
	int ref_count;
	tlm_gp_option gp_option;
	tlm_command command;
	sc_dt::int64 address;
	unsigned char *data_ptr;
	unsigned int data_length;
	unsigned int streaming_width;
	unsigned char *byte_enable_ptr;
	unsigned int byte_enable_length;
	bool dmi_allowed;
	tlm_response_status response_status;
	
	struct tlm_extension_slot
	{
		tlm_extension_slot();
		
		tlm_extension_base *extension;
		bool sticky;
	};
	
	std::vector<tlm_extension_slot> extension_slots;
	
	void free_auto_extensions();
};

template <typename T>
const unsigned int tlm_extension<T>::ID = tlm_extension_base::allocate_extension_id();

template <typename T> T *tlm_generic_payload::set_extension(T *extension)
{
	return static_cast<T *>(set_extension(T::ID, extension));
}

template <typename T> T *tlm_generic_payload::set_auto_extension(T *extension)
{
	return static_cast<T *>(set_auto_extension(T::ID, extension));
}

template <typename T> void tlm_generic_payload::get_extension(T *& p_extension) const
{
	p_extension = get_extension<T>();
}

template <typename T> T *tlm_generic_payload::get_extension() const
{
	return static_cast<T *>(get_extension(T::ID));
}

template <typename T> void tlm_generic_payload::clear_extension(const T *extension)
{
	extension_slots[T::ID].extension = 0;
}

template <typename T> void tlm_generic_payload::clear_extension()
{
	extension_slots[T::ID].extension = 0;
}

template <typename T> void tlm_generic_payload::release_extension(T *)
{
	release_extension<T>();
}

template <typename T> void tlm_generic_payload::release_extension()
{
	release_extension(T::ID);
}

enum tlm_endianness
{
	TLM_UNKNOWN_ENDIAN,
	TLM_LITTLE_ENDIAN,
	TLM_BIG_ENDIAN
};

inline tlm_endianness get_host_endianness(void)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	return TLM_LITTLE_ENDIAN;
#elif BYTE_ORDER == BIG_ENDIAN
	return TLM_BIG_ENDIAN;
#else
	return TLM_UNKNOWN_ENDIAN;
#endif
}

inline bool host_has_little_endianness(void)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	return true;
#else
	return false;
#endif
}

inline bool has_host_endianness(tlm_endianness endianness)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	return endianness == TLM_LITTLE_ENDIAN;
#elif BYTE_ORDER == BIG_ENDIAN
	return endianness == TLM_BIG_ENDIAN;
#else
	return false;
#endif
}

template<class DATAWORD>
inline void tlm_to_hostendian_generic(tlm_generic_payload *, unsigned int)
{
}

template<class DATAWORD>
inline void tlm_from_hostendian_generic(tlm_generic_payload *, unsigned int)
{
}

template<class DATAWORD>
inline void tlm_to_hostendian_word(tlm_generic_payload *, unsigned int)
{
}

template<class DATAWORD>
inline void tlm_from_hostendian_word(tlm_generic_payload *, unsigned int)
{
}

template<class DATAWORD>
inline void tlm_to_hostendian_aligned(tlm_generic_payload *, unsigned int)
{
}

template<class DATAWORD>
inline void tlm_from_hostendian_aligned(tlm_generic_payload *, unsigned int)
{
}

template<class DATAWORD>
inline void tlm_to_hostendian_single(tlm_generic_payload *, unsigned int)
{
}

template<class DATAWORD>
inline void tlm_from_hostendian_single(tlm_generic_payload *, unsigned int)
{
}

inline void tlm_from_hostendian(tlm_generic_payload *)
{
}

} // namespace tlm

#endif // __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_PAYLOAD_GENERIC_PAYLOAD_H__
