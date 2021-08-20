/*
 *  Copyright (c) 2019,
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

#ifndef __UNISIM_SERVICE_WEB_TERMINAL_WEB_TERMINAL_HH__
#define __UNISIM_SERVICE_WEB_TERMINAL_WEB_TERMINAL_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/http_server.hh>
#include <unisim/service/interfaces/char_io.hh>
#include <pthread.h>
#include <string>
#include <vector>

namespace unisim {
namespace service {
namespace web_terminal {

/////////////////////////// Forward declarations //////////////////////////////

struct UTF8_Parser;
struct GraphicRendition;
struct ScreenBuffer;
struct InputBuffer;
struct WebTerminal;

//////////////////////////////// to_string<> //////////////////////////////////

template <typename T>
std::string to_string(const T& v)
{
	std::ostringstream sstr;
	sstr << v;
	return sstr.str();
}

///////////////////////////////// Enums ///////////////////////////////////////

enum Intensity
{
	DECREASED_INTENSITY = -1,
	NORMAL_INTENSITY    =  0,
	INCREASED_INTENSITY = +1
};

std::ostream& operator << (std::ostream& os, const Intensity& intensity);

enum Blink
{
	NO_BLINK    = 0,
	SLOW_BLINK  = 1,
	RAPID_BLINK = 2,
};

std::ostream& operator << (std::ostream& os, const Blink& blink);

///////////////////////////// OStreamContext //////////////////////////////////

class OStreamContext
{
public:
	OStreamContext(std::ostream& _os) : os(_os), fill(os.fill()), flags(os.flags()) {}
	~OStreamContext() { os.fill(fill); os.flags(flags); }
private:
	std::ostream& os;
	std::ostream::char_type fill;
	std::ios_base::fmtflags flags;
};

//////////////////////////////// Field<> //////////////////////////////////////

template <typename T, unsigned int BITPOS, unsigned int BITWIDTH>
struct Field
{
	typedef T TYPE;
	
	template <typename V>
	static TYPE Get(const V& packed_value)
	{
		return static_cast<TYPE>((packed_value >> BITPOS) & ((V(1) << BITWIDTH) - 1));
	}
	
	template <typename V>
	static void Set(V& packed_value, const TYPE& field_value)
	{
		packed_value = (packed_value & ~(((V(1) << BITWIDTH) - 1) << BITPOS)) | ((static_cast<V>(field_value) & ((V(1) << BITWIDTH) - 1)) << BITPOS);
	}
};

///////////////////////////////////// Theme ///////////////////////////////////

struct Theme : unisim::kernel::Object
{
public:
	Theme(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~Theme();
	
	Intensity GetIntensity() const { return intensity; }
	bool GetItalic() const { return italic; }
	bool GetUnderline() const { return underline; }
	Blink GetBlink() const { return blink; }
	bool GetReverseVideo() const { return reverse_video; }
	bool GetCrossedOut() const { return crossed_out; }
	unsigned int GetBackgroundColor() const { return background_color; }
	unsigned int GetForegroundColor() const { return foreground_color; }
	const std::string& GetColor(unsigned int color) { return color_table[color]; }
	
	GraphicRendition MakeGraphicRendition() const;
private:
	Intensity intensity;
	bool italic;
	bool underline;
	Blink blink;
	bool reverse_video;
	bool crossed_out;
	unsigned int background_color;
	unsigned int foreground_color;
	std::string color_table[16];
	
	unisim::kernel::variable::Parameter<Intensity> param_intensity;
	unisim::kernel::variable::Parameter<bool> param_italic;
	unisim::kernel::variable::Parameter<bool> param_underline;
	unisim::kernel::variable::Parameter<Blink> param_blink;
	unisim::kernel::variable::Parameter<bool> param_reverse_video;
	unisim::kernel::variable::Parameter<bool> param_crossed_out;
	unisim::kernel::variable::Parameter<unsigned int> param_background_color;
	unisim::kernel::variable::Parameter<unsigned int> param_foreground_color;
	unisim::kernel::variable::ParameterArray<std::string> *param_color_table;
};

/////////////////////////////// GraphicRendition //////////////////////////////

struct GraphicRendition
{
	GraphicRendition() : packed_value(0) {}
	GraphicRendition(const GraphicRendition& o) : packed_value(o.packed_value) {}
	
	Intensity GetIntensity() const { return Get<INTENSITY>(); }
	bool GetItalic() const { return Get<ITALIC>(); }
	bool GetUnderline() const { return Get<UNDERLINE>(); }
	Blink GetBlink() const { return Get<BLINK>(); }
	bool GetReverseVideo() const { return Get<REVERSE_VIDEO>(); }
	bool GetCrossedOut() const { return Get<CROSSED_OUT>(); }
	unsigned int GetForegroundColor() const { return Get<FOREGROUND_COLOR>(); }
	unsigned int GetBackgroundColor() const { return Get<BACKGROUND_COLOR>(); }
	
	void SetIntensity(Intensity intensity) { Set<INTENSITY>(intensity); }
	void SetItalic(bool v = true) { Set<ITALIC>(v); }
	void SetUnderline(bool v = true) { Set<UNDERLINE>(v); }
	void SetBlink(Blink blink) { Set<BLINK>(blink); }
	void SetReverseVideo(bool v = true) { Set<REVERSE_VIDEO>(v); }
	void SetCrossedOut(bool v = true) { Set<CROSSED_OUT>(v); }
	void SetForegroundColor(unsigned int fg_color) { Set<FOREGROUND_COLOR>(fg_color); }
	void SetBackgroundColor(unsigned int bg_color) { Set<BACKGROUND_COLOR>(bg_color); }
	
	GraphicRendition& operator = (const GraphicRendition& o) { packed_value = o.packed_value; return *this; }
	int operator == (const GraphicRendition& o) const { return packed_value == o.packed_value; }
	int operator != (const GraphicRendition& o) const { return packed_value != o.packed_value; }
	int operator < (const GraphicRendition& o) const { return packed_value < o.packed_value; }
	
	void Reset(const Theme& theme)
	{
		SetIntensity(theme.GetIntensity());
		SetItalic(theme.GetItalic());
		SetUnderline(theme.GetUnderline());
		SetBlink(theme.GetBlink());
		SetReverseVideo(theme.GetReverseVideo());
		SetCrossedOut(theme.GetCrossedOut());
		SetForegroundColor(theme.GetForegroundColor());
		SetBackgroundColor(theme.GetBackgroundColor());
	}
private:
	typedef Field<Intensity, 0, 2>  INTENSITY;
	typedef Field<bool, 2, 1>  ITALIC;
	typedef Field<bool, 3, 1>  UNDERLINE;
	typedef Field<Blink, 4, 2>  BLINK;
	typedef Field<bool, 6, 1>  REVERSE_VIDEO;
	typedef Field<bool, 7, 1>  CROSSED_OUT;
	typedef Field<unsigned int, 8, 4> FOREGROUND_COLOR;
	typedef Field<unsigned int, 12, 4> BACKGROUND_COLOR;
	
	template <typename FIELD>
	typename FIELD::TYPE Get() const
	{
		return FIELD::Get(packed_value);
	}
	
	template <typename FIELD>
	void Set(const typename FIELD::TYPE& field_value)
	{
		FIELD::Set(packed_value, field_value);
	}
	
	uint16_t packed_value;
};

///////////////////////////// UnicodeCharacter ////////////////////////////////

struct UnicodeCharacter
{
	static const uint32_t NAC = 0xDEADBEEF;
	
	UnicodeCharacter() : code_point(NAC) {}
	UnicodeCharacter(const UnicodeCharacter& uc) : code_point(uc.code_point) {}
	UnicodeCharacter(uint32_t _code_point) : code_point(_code_point) {}
	uint32_t GetCodePoint() const { return code_point; }
	
	UnicodeCharacter& operator = (const UnicodeCharacter& uc) { code_point = uc.code_point; return *this; }
	UnicodeCharacter& operator = (uint32_t _code_point) { code_point = _code_point; return *this; }
	UnicodeCharacter& operator = (char c) { code_point = (uint8_t) c; return *this; }
	
	int operator < (const UnicodeCharacter& uc) const { return code_point < uc.code_point; }
	int operator <= (const UnicodeCharacter& uc) const { return code_point <= uc.code_point; }
	int operator == (const UnicodeCharacter& uc) const { return code_point == uc.code_point; }
	int operator >= (const UnicodeCharacter& uc) const { return code_point >= uc.code_point; }
	int operator > (const UnicodeCharacter& uc) const { return code_point > uc.code_point; }
	
	int operator < (uint32_t _code_point) const { return code_point < _code_point; }
	int operator <= (uint32_t _code_point) const { return code_point <= _code_point; }
	int operator == (uint32_t _code_point) const { return code_point == _code_point; }
	int operator >= (uint32_t _code_point) const { return code_point >= _code_point; }
	int operator > (uint32_t _code_point) const { return code_point > _code_point; }
	
	int operator < (char c) const { return code_point < (uint8_t) c; }
	int operator <= (char c) const { return code_point <= (uint8_t) c; }
	int operator == (char c) const { return code_point == (uint8_t) c; }
	int operator >= (char c) const { return code_point >= (uint8_t) c; }
	int operator > (char c) const { return code_point > (uint8_t) c; }

	operator uint32_t () const { return code_point; }
private:
	uint32_t code_point;
	
	friend struct UTF8_Parser;
};

std::ostream& operator << (std::ostream& os, const UnicodeCharacter& uc);

////////////////////////////// UTF8_Parser ////////////////////////////////////

struct UTF8_Parser
{
	UTF8_Parser();
	UnicodeCharacter *Parse(uint8_t c);
private:
	int state;
	int n;
	UnicodeCharacter uc;
};

//////////////////////////// ScreenBufferCharacter ////////////////////////////

struct ScreenBufferCharacter
{
	UnicodeCharacter c;
	GraphicRendition gr;
	
	void Erase(const Theme& theme)
	{
		c = 0u;
		gr.Reset(theme);
	}
};

///////////////////////////// ScreenBufferLine ////////////////////////////////

struct ScreenBufferLine
{
	ScreenBufferLine(unsigned int _display_width, const Theme& theme);
	~ScreenBufferLine();
	void Erase(const Theme& theme);
	ScreenBufferCharacter& operator [] (unsigned int colno);
	
private:
	unsigned int display_width;
	ScreenBufferCharacter *storage;
};

//////////////////////////////// ScreenBuffer /////////////////////////////////

struct ScreenBuffer : unisim::kernel::Object
{
	typedef std::vector<ScreenBufferLine *> ScreenBufferLines;
	typedef ScreenBufferLines::iterator iterator;
	typedef ScreenBufferLines::const_iterator const_iterator;
	
	ScreenBuffer(const char *name, WebTerminal *web_terminal);
	~ScreenBuffer();
	
	const_iterator Begin() const;
	iterator Begin();
	const_iterator End() const;
	iterator End();
	
	unsigned int GetCursorColNo() const;
	unsigned int GetCursorLineNo() const;
	
	unsigned int GetDisplayWidth() const { return display_width; }
	unsigned int GetDisplayHeight() const { return display_height; }
	unsigned int GetBufferHeight() const { return buffer_height; }

	void SetDefaultGraphicRendition();
	void SetIntensity(Intensity intensity);
	void SetItalic(bool italic = true);
	void SetUnderline(bool underline = true);
	void SetBlink(Blink blink);
	void SetReverseVideo(bool reverse_video = true);
	void SetCrossedOut(bool crossed_out = true);
	void SetBackgroundColor(unsigned int bg_color);
	void SetForegroundColor(unsigned int fg_color);
	void CarriageReturn();
	void LineFeed();
	void Backspace();
	void Delete();
	
	ScreenBufferLine& GetLine(unsigned int line_index);
	bool IsAtCursorLinePos(const_iterator it) const;
	ScreenBufferLine& operator [] (unsigned int lineno);
	
	void PutChar(char c);
	void PutString(const std::string& s);
	void MoveCursorUp(unsigned int n = 1);
	void MoveCursorDown(unsigned int n = 1);
	void MoveCursorForward(unsigned int n = 1);
	void MoveCursorBack(unsigned int n = 1);
	void MoveCursorNextLine(unsigned int n = 1);
	void MoveCursorPreviousLine(unsigned int n = 1);
	void MoveCursorHorizontalAbsolute(unsigned int colno);
	void MoveCursorTo(unsigned int lineno = 1, unsigned int colno = 1);
	void EraseInDisplay(unsigned int n = 0);
	void EraseInLine(unsigned int n = 0);
	void NewLine();
	void ScrollUp(unsigned int n = 1);
	void ScrollDown(unsigned int n = 1);
	void SelectGraphicsRendition(unsigned int n);
	void ReportCursorPosition(InputBuffer& input_buffer);
	void SaveCursorPosition();
	void RestoreCursorPosition();
	void MoveCursorDownScrollUp();
	void MoveCursorUpScrollDown();
	
private:
	WebTerminal *web_terminal;
	unisim::kernel::logger::Logger logger;

	unsigned int display_width;
	unsigned int display_height;
	unsigned int height;
	unsigned int cursor_colno;
	unsigned int cursor_lineno;
	unsigned int save_cursor_colno;
	unsigned int save_cursor_lineno;
	GraphicRendition save_gr;
	ScreenBufferLines lines;
	GraphicRendition gr;
	UTF8_Parser utf8_parser;
	
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<unsigned int> param_display_width;
	unisim::kernel::variable::Parameter<unsigned int> param_display_height;
	unsigned int buffer_height;
	unisim::kernel::variable::Parameter<unsigned int> param_buffer_height;
};

///////////////////////////////// InputBuffer /////////////////////////////////

struct InputBuffer
{
	InputBuffer(unsigned int _size);
	
	void Push(char c);
	void Push(const char *s);
	void Push(const std::string& s);
	bool Empty() const;
	bool Full() const;
	char Front() const;
	void Pop();

private:
	unsigned int size;
	unsigned int front_index;
	unsigned int back_index;
	unsigned int fill_level;
	std::vector<char> storage;
};

/////////////////////////////////// KeyEvent //////////////////////////////////

struct KeyEvent
{
	KeyEvent() : key(), csam(0) {}
	
	KeyEvent(const std::string& _key)
		: key(_key)
		, csam(0)
	{
	}
	
	KeyEvent& KEY(const std::string& _key) { key = _key; return *this; }
	KeyEvent& CTRL(bool v = true) { Set<CTRL_FIELD>(v); return *this; }
	KeyEvent& SHIFT(bool v = true) { Set<SHIFT_FIELD>(v); return *this; }
	KeyEvent& ALT(bool v = true) { Set<ALT_FIELD>(v); return *this; }
	KeyEvent& META(bool v = true) { Set<META_FIELD>(v); return *this; }
	
	const std::string& GetKey() const { return key; }
	bool GetCTRL() const { return Get<CTRL_FIELD>(); }
	bool GetSHIFT() const { return Get<SHIFT_FIELD>(); }
	bool GetALT() const { return Get<ALT_FIELD>(); }
	bool GetMETA() const { return Get<META_FIELD>(); }
	
	int operator < (const KeyEvent& o) const { return (key < o.key) || ((key >= o.key) && (csam < o.csam)); }
private:
	std::string key;
	uint8_t csam;
	
	typedef Field<bool, 0, 1>  CTRL_FIELD;
	typedef Field<bool, 1, 1>  SHIFT_FIELD;
	typedef Field<bool, 2, 1>  ALT_FIELD;
	typedef Field<bool, 3, 1>  META_FIELD;

	template <typename FIELD>
	typename FIELD::TYPE Get() const
	{
		return FIELD::Get(csam);
	}

	template <typename FIELD>
	void Set(const typename FIELD::TYPE& field_value)
	{
		FIELD::Set(csam, field_value);
	}
};

inline std::ostream& operator << (std::ostream& os, const KeyEvent& key_event)
{
	return os << "KeyEvent(key=\"" << key_event.GetKey() << "\",ctrl=" << key_event.GetCTRL() << ",shift=" << key_event.GetSHIFT() << ",alt=" << key_event.GetALT() << ",meta=" << key_event.GetMETA() << ")";
}

///////////////////////////////// WebTerminal /////////////////////////////////

struct WebTerminal
	: public unisim::kernel::Service<unisim::service::interfaces::CharIO>
	, public unisim::kernel::Service<unisim::service::interfaces::HttpServer>
{
	unisim::kernel::ServiceExport<unisim::service::interfaces::CharIO> char_io_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> http_server_export;
	
	WebTerminal(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~WebTerminal();
	
	virtual bool BeginSetup();
	
	// unisim::service::interfaces::CharIO
	virtual void ResetCharIO();
	virtual bool GetChar(char& c);
	virtual void PutChar(char c);
	virtual void FlushChars();
	
	// unisim::service::interfaces::HttpServer
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);
	
	const Theme& GetTheme() const { return theme; }
	
private:
	unisim::kernel::logger::Logger logger;

	Theme theme;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unsigned int input_buffer_size;
	unisim::kernel::variable::Parameter<unsigned int> param_input_buffer_size;
	double min_display_refresh_period;
	unisim::kernel::variable::Parameter<double> param_min_display_refresh_period;
	double max_display_refresh_period;
	unisim::kernel::variable::Parameter<double> param_max_display_refresh_period;
	std::string title;
	unisim::kernel::variable::Parameter<std::string> param_title;
	bool implicit_cr_in_every_lf;
	unisim::kernel::variable::Parameter<bool> param_implicit_cr_in_every_lf;
	
	bool activity;
	double refresh_period;
	std::string str;
	int state;
	bool got_arg;
	unsigned int arg;
	std::vector<unsigned int> args;
	ScreenBuffer screen_buffer;
	ScreenBuffer alt_screen_buffer;
	ScreenBuffer *curr_screen_buffer;
	InputBuffer input_buffer;

	typedef std::map<KeyEvent, std::string> KEVT2ANSI;
	KEVT2ANSI kevt2ansi;
	
	pthread_mutex_t mutex;
	
	void Receive(const KeyEvent& key_event);
	void Lock();
	void Unlock();
	void GenerateStyle(std::ostream& os, const GraphicRendition& gr, bool under_cursor, const std::string& class_name);
};

} // end of namespace web_terminal
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_WEB_TERMINAL_WEB_TERMINAL_HH__
