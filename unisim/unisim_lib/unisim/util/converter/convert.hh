/*
 * convert.hh
 *
 *  Created on: 25 juin 2010
 *      Author: rnouacer
 */

#ifndef CONVERT_HH_
#define CONVERT_HH_

#include <stdint.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <typeinfo>
#include <stdexcept>
#include <map>

#include <unisim/util/endian/endian.hh>

class BadConversion : public std::runtime_error {
public:
	BadConversion(std::string const& s)	: std::runtime_error(s) { }
};

template<typename T>
inline std::string stringify(T const& x) {

	std::ostringstream o;
	if (!(o << x)) {
		throw BadConversion(std::string("stringify(") + typeid(x).name() + ")");
	}

	return (o.str());
}

template<typename T>
inline void convert(std::string const& s, T& x, bool failIfLeftoverChars = true) {

	std::istringstream i(s);
	char c;

	if (!(i >> x) || (failIfLeftoverChars && i.get(c))) {
		throw BadConversion(s);
	}

}

template<typename T>
inline T convertTo(std::string const& s, bool failIfLeftoverChars = true) {

	T x;

	convert(s, x, failIfLeftoverChars);

	return (x);
}

inline bool isHexChar(char ch)
{
	if(ch >= 'a' && ch <= 'f') return (true);
	if(ch >= '0' && ch <= '9') return (true);
	if(ch >= 'A' && ch <= 'F') return (true);
	return (false);
}

inline char nibble2HexChar(uint8_t v)
{
	v = v & 0xf; // keep only 4-bits
	return (v < 10 ? '0' + v : 'a' + v - 10);
}

inline uint8_t hexChar2Nibble(char ch)
{
	if(ch >= 'a' && ch <= 'f') return (ch - 'a' + 10);
	if(ch >= '0' && ch <= '9') return (ch - '0');
	if(ch >= 'A' && ch <= 'F') return (ch - 'A' + 10);
	return (0);
}

inline void number2HexString(uint8_t* value, size_t size, std::string& hex, std::string endian) {

	ssize_t i;
	char c[2];
	c[1] = 0;

	hex.erase();

	if(endian == "big")
	{
//#if BYTE_ORDER == BIG_ENDIAN
//		for(i = 0; i < size; i++)
//#else
//		for(i = size - 1; i >= 0; i--)
//#endif

#if BYTE_ORDER == BIG_ENDIAN
		i = 0;
#else
		i = size - 1;
#endif

		for (size_t j=0; j<size; j++)
		{
			c[0] = nibble2HexChar(value[i] >> 4);
			hex += c;
			c[0] = nibble2HexChar(value[i] & 0xf);
			hex += c;

#if BYTE_ORDER == BIG_ENDIAN
		i = i + 1;
#else
		i = i - 1;
#endif
		}
	}
	else
	{
//#if BYTE_ORDER == LITTLE_ENDIAN
//		for(i = 0; i < size; i++)
//#else
//		for(i = size - 1; i >= 0; i--)
//#endif

#if BYTE_ORDER == LITTLE_ENDIAN
		i = 0;
#else
		i = size - 1;
#endif
		for (size_t j=0; j<size; j++)
		{
			c[0] = nibble2HexChar(value[i] >> 4);
			hex += c;
			c[0] = nibble2HexChar(value[i] & 0xf);
			hex += c;

#if BYTE_ORDER == LITTLE_ENDIAN
		i = i + 1;
#else
		i = i - 1;
#endif
		}
	}

}

inline bool hexString2Number(const std::string& hex, void* value, size_t size, std::string endian) {

	ssize_t i;
	size_t len = hex.length();
	size_t pos = 0;

	if(endian == "big")
	{
//#if BYTE_ORDER == BIG_ENDIAN
//		for(i = 0; i < size && pos < len; i++, pos += 2)
//#else
//		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
//#endif

#if BYTE_ORDER == BIG_ENDIAN
		i = 0;
#else
		i = size - 1;
#endif

		for (size_t j = 0; j < size && pos < len; j++, pos += 2)
		{
			if(!isHexChar(hex[pos]) || !isHexChar(hex[pos + 1])) return (false);

			((uint8_t*) value)[i] = (hexChar2Nibble(hex[pos]) << 4) | hexChar2Nibble(hex[pos + 1]);

#if BYTE_ORDER == BIG_ENDIAN
		i = i + 1;
#else
		i = i - 1;
#endif
		}
	}
	else
	{
//#if BYTE_ORDER == LITTLE_ENDIAN
//		for(i = 0; i < size && pos < len; i++, pos += 2)
//#else
//		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
//#endif

#if BYTE_ORDER == LITTLE_ENDIAN
		i = 0;
#else
		i = size - 1;
#endif
		for(size_t j = 0; j < size && pos < len; j++, pos += 2)
		{
			if(!isHexChar(hex[pos]) || !isHexChar(hex[pos + 1])) return (false);

			((uint8_t*) value)[i] = (hexChar2Nibble(hex[pos]) << 4) | hexChar2Nibble(hex[pos + 1]);

#if BYTE_ORDER == LITTLE_ENDIAN
		i = i + 1;
#else
		i = i - 1;
#endif
		}
	}

	return (true);

}

inline void textToHex(const char *s, size_t count, std::string& packet)
{
	size_t i;
	std::stringstream strm;

	for(i = 0; (i<count); i++)
	{
		strm << nibble2HexChar((uint8_t) s[i] >> 4);
		strm << nibble2HexChar((uint8_t) s[i] & 0xf);
	}

	packet = strm.str();
	strm.str(std::string());

}

inline void hexToText(const char *s, size_t count, std::string& packet)
{
	size_t i;
	std::stringstream strm;
	uint8_t c;

	for (i = 0; i < count; i=i+2) {
		c = (hexChar2Nibble((uint8_t) s[i]) << 4) | hexChar2Nibble((uint8_t) s[i+1]);
		strm << c;
	}

	packet = strm.str();
	strm.str(std::string());
}

inline void stringSplit(std::string str, const std::string delim, std::vector<std::string>& results)
{
	size_t cutAt = 0;

	while ((cutAt != str.npos) && !str.empty()) {

		cutAt = str.find_first_of(delim);

		if (cutAt == str.npos) {
			results.push_back(str);
		} else {
			results.push_back(str.substr(0,cutAt));
			str = str.substr(cutAt+1);
		}
	}

}

inline void splitCharStr2Array(char* str, const char * delimiters, int *count, char** *res) {

	char *  p    = strtok (str, delimiters);
	int n_spaces = 0;

	/* split string and append tokens to 'res' */

	while (p) {
	  (*res) = (char**) realloc ((*res), sizeof (char*) * ++n_spaces);

	  if ((*res) == NULL)
	    exit (-1); /* memory allocation failed */

	  (*res)[n_spaces-1] = p;

	  p = strtok (NULL, " ");
	}

//	/* print the result */
//
//	for (i = 0; i < (n_spaces); ++i)
//	  printf ("res[%d] = %s\n", i, (*res)[i]);

	*count = n_spaces;

}

/* Function to get parity of number n. It returns 1
   if n has odd parity, and returns 0 if n has even
   parity */
template <typename T>
inline bool getParity(T n)
{
    bool parity = false;
    while (n)
    {
        parity = !parity;
        n      = n & (n - 1);
    }
    return parity;
}

template <typename T>
inline bool ParseHex(const std::string& s, size_t& pos, T& value)
{
	size_t len = s.length();
	size_t n = 0;

	value = 0;
	while(pos < len && n < 2 * sizeof(T))
	{
		uint8_t nibble;
		if(!isHexChar(s[pos])) break;
		nibble = hexChar2Nibble(s[pos]);
		value <<= 4;
		value |= nibble;
		pos++;
		n++;
	}
	return (n > 0);
}

inline const char* getOsName() {

	#ifndef __OSNAME__
	#if defined(_WIN32) || defined(_WIN64)
			const char* osName = "Windows";
	#else
	#ifdef __linux
			const char* osName = "Linux";
	#else
			const char* osName = "Unknown";
	#endif
	#endif
	#endif

    return (osName);
}

/*
 * Expand environment variables
 * Windows:
 *    - ExpandEnvironmentStrings(%PATH%): Expands environment-variable strings and replaces them with the values defined for the current user
 *    - PathUnExpandEnvStrings: To replace folder names in a fully qualified path with their associated environment-variable strings
 *    - Example Get system informations :
 *      url: http://msdn.microsoft.com/en-us/library/ms724426%28v=vs.85%29.aspx
 *
 * Linux/Posix:
 *   url: http://stackoverflow.com/questions/1902681/expand-file-names-that-have-environment-variables-in-their-path
 *
 */

#include <cstdlib>
#include <string>

inline std::string expand_environment_variables( std::string s ) {
    if( s.find( "$(" ) == std::string::npos ) return s;

    std::string pre  = s.substr( 0, s.find( "$(" ) );
    std::string post = s.substr( s.find( "$(" ) + 2 );

    if( post.find( ')' ) == std::string::npos ) return s;

    std::string variable = post.substr( 0, post.find( ')' ) );
    std::string value    = "";

    post = post.substr( post.find( ')' ) + 1 );

    if( getenv( variable.c_str() ) != NULL ) value = std::string( getenv( variable.c_str() ) );

    return expand_environment_variables( pre + value + post );
}

inline std::string expand_path_variables( std::string s , std::map<std::string, std::string> env_vars) {
    if( s.find( "$(" ) == std::string::npos ) return s;

    std::string pre  = s.substr( 0, s.find( "$(" ) );
    std::string post = s.substr( s.find( "$(" ) + 2 );

    if( post.find( ')' ) == std::string::npos ) return s;

    std::string variable = post.substr( 0, post.find( ')' ) );
    std::string value    = "";

    post = post.substr( post.find( ')' ) + 1 );

	std::map<std::string, std::string>::iterator it = env_vars.find(variable);
	if (it == env_vars.end()) {
		if( getenv( variable.c_str() ) != NULL ) value = std::string( getenv( variable.c_str() ) );
	} else {
		value = it->second;
	}

    return expand_environment_variables( pre + value + post );
}

#endif /* CONVERT_HH_ */
