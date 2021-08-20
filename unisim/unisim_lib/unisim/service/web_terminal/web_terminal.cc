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

#include <unisim/service/web_terminal/web_terminal.hh>
#include <unisim/util/likely/likely.hh>
#include <set>
#include <cassert>
#include <cstring>

namespace unisim {
namespace service {
namespace web_terminal {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

///////////////////////////////// Enums ///////////////////////////////////////

std::ostream& operator << (std::ostream& os, const Intensity& intensity)
{
	switch(intensity)
	{
		case DECREASED_INTENSITY: os << "decreased intensity"; break;
		case NORMAL_INTENSITY   : os << "normal intensity"; break;
		case INCREASED_INTENSITY: os << "increased intensity"; break;
		default                 : os << "unknown intensity"; break;
	}
	return os;
}

std::ostream& operator << (std::ostream& os, const Blink& blink)
{
	switch(blink)
	{
		case NO_BLINK   : os << "no blink"; break;
		case SLOW_BLINK : os << "slow blink"; break;
		case RAPID_BLINK: os << "rapid blink"; break;
		default         : os << "unknown blink"; break;
	}
	return os;
}

///////////////////////////////////// Theme ///////////////////////////////////

Theme::Theme(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, intensity(NORMAL_INTENSITY)
	, italic(false)
	, underline(false)
	, blink(NO_BLINK)
	, reverse_video(false)
	, crossed_out(false)
	, background_color(0)
	, foreground_color(7)
	, color_table()
	, param_intensity("intensity", this, intensity, "intensity")
	, param_italic("italic", this, italic, "enable/disable italic")
	, param_underline("underline", this, underline, "enable/disable underline")
	, param_blink("blink", this, blink, "enable/disable blink")
	, param_reverse_video("reverse-video", this, reverse_video, "enable/disable reverse video")
	, param_crossed_out("crossed-out", this, crossed_out, "enable/disable crossed-out")
	, param_background_color("background-color", this, background_color, "background color (0-15)")
	, param_foreground_color("foreground-color", this, foreground_color, "foreground color (0-15)")
{
	color_table[0 ] = /*  0: black          */ "rgb(0,0,0)";
	color_table[1 ] = /*  1: red            */ "rgb(170,0,0)";
	color_table[2 ] = /*  2: green          */ "rgb(0,170,0)";
	color_table[3 ] = /*  3: yellow         */ "rgb(170,85,0)";
	color_table[4 ] = /*  4: blue           */ "rgb(0,0,170)";
	color_table[5 ] = /*  5: magenta        */ "rgb(170,0,170)";
	color_table[6 ] = /*  6: cyan           */ "rgb(0,170,170)";
	color_table[7 ] = /*  7: white          */ "rgb(170,170,170)";
	color_table[8 ] = /*  8: bright black   */ "rgb(85,85,85)";
	color_table[9 ] = /*  9: bright red     */ "rgb(255,85,85)";
	color_table[10] = /* 10: bright green   */ "rgb(85,255,85)";
	color_table[11] = /* 11: bright yellow  */ "rgb(255,255,85)";
	color_table[12] = /* 12: bright blue    */ "rgb(85,85,255)";
	color_table[13] = /* 13: bright magenta */ "rgb(255,85,255)";
	color_table[14] = /* 14: bright cyan    */ "rgb(85,255,255)";
	color_table[15] = /* 15: bright white   */ "rgb(255,255,255)";

	param_background_color.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_foreground_color.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	param_color_table = new unisim::kernel::variable::ParameterArray<std::string>("color-table", this, color_table, 16, "color table");
}

Theme::~Theme()
{
	delete param_color_table;
}

GraphicRendition Theme::MakeGraphicRendition() const
{
	GraphicRendition gr;
	
	gr.SetIntensity(intensity);
	gr.SetItalic(italic);
	gr.SetUnderline(underline);
	gr.SetBlink(blink);
	gr.SetReverseVideo(reverse_video);
	gr.SetCrossedOut(crossed_out);
	gr.SetBackgroundColor(background_color);
	gr.SetForegroundColor(foreground_color);
	
	return gr;
}

////////////////////////////// UTF8_Parser ////////////////////////////////////

UTF8_Parser::UTF8_Parser()
	: state(0)
	, n(0)
	, uc()
{
}

UnicodeCharacter *UTF8_Parser::Parse(uint8_t c)
{
	switch(state)
	{
		case 0:
			if(c < 128)
			{
				uc.code_point = c;
				return &uc;
			}

			if((c & 0xe0) == (0xc0))
			{
				// 2-byte UTF-8 character
				uc.code_point = c & 0x1f;
				n = 1;
				state = 1;
			}
			else if((c & 0xf0) == (0xe0))
			{
				// 3-byte UTF-8 character
				uc.code_point = c & 0xf;
				n = 2;
				state = 1;
			}
			else if((c & 0xf8) == (0xf0))
			{
				// 4-byte UTF-8 character
				uc.code_point = c & 0x7;
				n = 3;
				state = 1;
			}
			
			break;
			
		case 1:
			if((c & 0xc0) == 0x80)
			{
				// n-th byte
				uc.code_point = (uc.code_point << 6) | (c & 0x3f);
				if(--n == 0)
				{
					state = 0;
					return &uc;
				}
			}
			else
			{
				state = 0;
			}
			break;
	}
	
	return 0;
}

///////////////////////////// UnicodeCharacter ////////////////////////////////

std::ostream& operator << (std::ostream& os, const UnicodeCharacter& uc)
{
	uint32_t code_point = uc.GetCodePoint();
	if(code_point <= 0x7f)
	{
		// 1-byte UTF-8 character
		os << (char) code_point;
	}
	else if(code_point <= 0x7ff)
	{
		// 2-byte UTF-8 character: xxxxxyyyyyy => 110xxxxx  10yyyyyy
		uint16_t v = 0xc080 | ((code_point << 2) & 0x1f00) | (code_point & 0x3f);
		os << (char)(v >> 8) << (char) v;
	}
	else if(code_point <= 0xffff)
	{
		// 3-byte UTF-8 character: xxxxyyyyyyzzzzzz => 1110xxxx 10yyyyyy 10zzzzzz
		uint32_t v = 0xe08080 | ((code_point << 4) & 0x1f0000) | ((code_point << 2) & 0x3f00) | (code_point & 0x3f);
		os << (char)(v >> 16) << (char)(v >> 8) << (char) v;
	}
	else if(code_point <= 0x10ffff)
	{
		// 4-byte UTF-8 character: wwwxxxxxxyyyyyyzzzzzz => 11110www 10xxxxxx 10yyyyyy 10zzzzzz
		uint32_t v = 0xf0808080 | ((code_point << 6) & 0x7000000) | ((code_point << 4) & 0x3f0000) | ((code_point << 2) & 0x3f00) | (code_point & 0x3f);
		os << (char)(v >> 24) << (char)(v >> 16) << (char)(v >> 8) << (char) v;
	}
	
	return os;
}

///////////////////////////// ScreenBufferLine ////////////////////////////////

ScreenBufferLine::ScreenBufferLine(unsigned int _display_width, const Theme& theme)
	: display_width(_display_width)
	, storage(new ScreenBufferCharacter[display_width])
{
	Erase(theme);
}

ScreenBufferLine::~ScreenBufferLine()
{
	delete[] storage;
}

void ScreenBufferLine::Erase(const Theme& theme)
{
	for(unsigned int i = 0; i < display_width; i++)
	{
		storage[i].Erase(theme);
	}
}

ScreenBufferCharacter& ScreenBufferLine::operator [] (unsigned int colno)
{
	return storage[colno - 1];
}

//////////////////////////////// ScreenBuffer /////////////////////////////////

ScreenBuffer::ScreenBuffer(const char *name, WebTerminal *_web_terminal)
	: unisim::kernel::Object(name, _web_terminal)
	, web_terminal(_web_terminal)
	, logger(*this)
	, display_width(80)
	, display_height(25)
	, height(display_height)
	, cursor_colno(1)
	, cursor_lineno(1)
	, save_cursor_colno(1)
	, save_cursor_lineno(1)
	, save_gr()
	, lines()
	, gr()
	, utf8_parser()
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_display_width("display-width", this, display_width, "Number of displayed columns")
	, param_display_height("display-height", this, display_height, "Number of displayed lines")
	, buffer_height(4 * display_height)
	, param_buffer_height("buffer-height", this, buffer_height, "Number of lines in buffer")
{
	buffer_height = std::max(buffer_height, display_height);
	
	param_display_width.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_display_height.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_buffer_height.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	param_display_width.SetMutable(false);
	param_display_height.SetMutable(false);
	param_buffer_height.SetMutable(false);
	
	lines.reserve(buffer_height);
	const Theme& theme = web_terminal->GetTheme();
	for(unsigned int i = 0; i < height; i++)
	{
		lines.push_back(new ScreenBufferLine(display_width, theme));
	}
	
	SetDefaultGraphicRendition();
}

ScreenBuffer::~ScreenBuffer()
{
	for(unsigned int i = 0; i < height; i++)
	{
		ScreenBufferLine *line = lines[i];
		delete line;
	}
}

ScreenBuffer::const_iterator ScreenBuffer::Begin() const
{
	return lines.begin();
}

ScreenBuffer::iterator ScreenBuffer::Begin()
{
	return lines.begin();
}

ScreenBuffer::const_iterator ScreenBuffer::End() const
{
	return lines.end();
}

ScreenBuffer::iterator ScreenBuffer::End()
{
	return lines.end();
}

unsigned int ScreenBuffer::GetCursorColNo() const
{
	return cursor_colno;
}

unsigned int ScreenBuffer::GetCursorLineNo() const
{
	return cursor_lineno;
}

void ScreenBuffer::SetDefaultGraphicRendition()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Setting default graphic rendition" << EndDebugInfo;
	}
	
	const Theme& theme = web_terminal->GetTheme();
	SetIntensity(theme.GetIntensity());
	SetItalic(theme.GetItalic());
	SetUnderline(theme.GetUnderline());
	SetBlink(theme.GetBlink());
	SetReverseVideo(theme.GetReverseVideo());
	SetCrossedOut(theme.GetCrossedOut());
	SetBackgroundColor(theme.GetBackgroundColor());
	SetForegroundColor(theme.GetForegroundColor());
}

void ScreenBuffer::SetIntensity(Intensity intensity)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Setting " << intensity << EndDebugInfo;
	}
	gr.SetIntensity(intensity);
}

void ScreenBuffer::SetItalic(bool italic)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":" << (italic ? "Enabling" : "Disabling") << " italic" << EndDebugInfo;
	}
	gr.SetItalic(italic);
}

void ScreenBuffer::SetUnderline(bool underline)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":" << (underline ? "Enabling" : "Disabling") << " underline" << EndDebugInfo;
	}
	gr.SetUnderline(underline);
}

void ScreenBuffer::SetBlink(Blink blink)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Setting " << blink << EndDebugInfo;
	}
	gr.SetBlink(blink);
}

void ScreenBuffer::SetReverseVideo(bool reverse_video)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":" << (reverse_video ? "Enabling" : "Disabling") << " reverse video" << EndDebugInfo;
	}
	gr.SetReverseVideo(reverse_video);
}

void ScreenBuffer::SetCrossedOut(bool crossed_out)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":" << (crossed_out ? "Enabling" : "Disabling") << " crossed-out" << EndDebugInfo;
	}
	gr.SetCrossedOut(crossed_out);
}

void ScreenBuffer::SetBackgroundColor(unsigned int bg_color)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Setting background color to " << bg_color << EndDebugInfo;
	}
	gr.SetBackgroundColor(bg_color);
}

void ScreenBuffer::SetForegroundColor(unsigned int fg_color)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Setting background color to " << fg_color << EndDebugInfo;
	}
	gr.SetForegroundColor(fg_color);
}

void ScreenBuffer::CarriageReturn()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Carriage return" << EndDebugInfo;
	}
	cursor_colno = 1;
}

void ScreenBuffer::LineFeed()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Line feed" << EndDebugInfo;
	}
	if(cursor_lineno < display_height)
	{
		cursor_lineno++;
	}
	else
	{
		NewLine();
	}
}

void ScreenBuffer::Backspace()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Backspace" << EndDebugInfo;
	}
	
	if(cursor_colno > 1)
	{
		cursor_colno--;
	}
}

void ScreenBuffer::Delete()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Delete" << EndDebugInfo;
	}
	
	ScreenBufferLine& sbl = (*this)[cursor_lineno];
	ScreenBufferCharacter& sbc = sbl[cursor_colno];
	
	sbc.Erase(web_terminal->GetTheme());
}

ScreenBufferLine& ScreenBuffer::GetLine(unsigned int line_index)
{
	assert(line_index < height);
//	std::cerr << "GetLine(" << line_index << ")" << std::endl;
	return *lines[line_index];
}

bool ScreenBuffer::IsAtCursorLinePos(const_iterator it) const
{
	unsigned int cursor_line_index = height - display_height + cursor_lineno - 1;
	return (it - Begin()) == cursor_line_index;
}

ScreenBufferLine& ScreenBuffer::operator [] (unsigned int lineno)
{
	unsigned int line_index = height - display_height + lineno - 1;
	return *lines[line_index];
}

void ScreenBuffer::PutChar(char c)
{
	UnicodeCharacter *uc = utf8_parser.Parse(c);
	if(uc)
	{
// 		{
// 			OStreamContext(std::cerr);
// 			std::cerr << "got U+";
// 			std::cerr.fill('0');
// 			std::cerr.width(4);
// 			std::cerr << std::hex << uc->GetCodePoint() << std::endl;
// 		}
		ScreenBufferLine& sbl = (*this)[cursor_lineno];
		ScreenBufferCharacter& sbc = sbl[cursor_colno];
		sbc.c = *uc;
		sbc.gr = gr;
		cursor_colno++;
		if(cursor_colno > display_width)
		{
			CarriageReturn();
			LineFeed();
		}
	}
}

void ScreenBuffer::PutString(const std::string& s)
{
	std::size_t n = s.length();
	for(std::size_t i = 0; i < n; i++)
	{
		PutChar(s[i]);
	}
}

void ScreenBuffer::MoveCursorUp(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Move cursor up " << n << EndDebugInfo;
	}
	if((n + 1) < cursor_lineno)
	{
		cursor_lineno -= n;
	}
	else
	{
		cursor_lineno = 1;
	}
}

void ScreenBuffer::MoveCursorDown(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Move cursor down " << n << EndDebugInfo;
	}
	if(n < (display_height - cursor_lineno))
	{
		cursor_lineno += n;
	}
	else
	{
		cursor_lineno = display_height;
	}
}

void ScreenBuffer::MoveCursorForward(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Move cursor forward " << n << EndDebugInfo;
	}
	if(n < (display_width - cursor_colno))
	{
		cursor_colno += n;
	}
	else
	{
		cursor_colno = display_width;
	}
}

void ScreenBuffer::MoveCursorBack(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Move cursor back " << n << EndDebugInfo;
	}
	if((n + 1) < cursor_colno)
	{
		cursor_colno -= n;
	}
	else
	{
		cursor_colno = 1;
	}
}

void ScreenBuffer::MoveCursorNextLine(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Move cursor next line" << n << EndDebugInfo;
	}
	MoveCursorDown(n);
	cursor_colno = 1;
}

void ScreenBuffer::MoveCursorPreviousLine(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Move cursor previous line " << n << EndDebugInfo;
	}
	MoveCursorUp(n);
	cursor_colno = 1;
}

void ScreenBuffer::MoveCursorHorizontalAbsolute(unsigned int colno)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Move cursor horizontal absolute " << colno << EndDebugInfo;
	}
	cursor_colno = std::min(colno, display_width);
}

void ScreenBuffer::MoveCursorTo(unsigned int lineno, unsigned int colno)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Move cursor to " << lineno << ":" << colno << EndDebugInfo;
	}
	cursor_colno = std::min(colno, display_width);
	cursor_lineno = std::min(lineno, display_height);
}

void ScreenBuffer::EraseInDisplay(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Erase in display " << n << EndDebugInfo;
	}
	/* Clears part of the screen.
		* If n is 0 (or missing), clear from cursor to end of screen.
		* If n is 1, clear from cursor to beginning of the screen.
		* If n is 2, clear entire screen (and moves cursor to upper left on DOS ANSI.SYS).
		* If n is 3, clear entire screen and delete all lines saved in the scrollback buffer (this feature was added for xterm and is supported by other terminal applications).
		*/ 
	if(n > 3) return;
	if(n == 0) EraseInLine(0);
	else if(n == 1) EraseInLine(1);
	int inc = (n != 1) ? +1 : -1;
	unsigned int start = (n & 2) ? 1 : (cursor_lineno + 1);
	unsigned int end = (n & 2) ? ((n & 1) ? (height + 1) : (display_height + 1)) : ((n & 1) ? 0 : (display_height + 1));
	const Theme& theme = web_terminal->GetTheme();
	for(unsigned int lineno = start; lineno != end; lineno += inc)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Erasing line #" << lineno << EndDebugInfo;
		}
		(*this)[lineno].Erase(theme);
	}
}

void ScreenBuffer::EraseInLine(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Erase in line " << n << EndDebugInfo;
	}
	if(n > 2) return;
	ScreenBufferLine& sbl = (*this)[cursor_lineno];
	/* Erases part of the line.
		* If n is 0 (or missing), clear from cursor to the end of the line.
		* If n is 1, clear from cursor to beginning of the line.
		* If n is 2, clear entire line. Cursor position does not change.
		*/
	unsigned int start = n ? 1 : cursor_colno;
	unsigned int end = (n != 1) ? display_width : cursor_colno;
	const Theme& theme = web_terminal->GetTheme();
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Erasing columns " << start << "-" << end << EndDebugInfo;
	}
	for(unsigned int colno = start; colno <= end; colno++)
	{
		sbl[start].Erase(theme);
	}
}

void ScreenBuffer::NewLine()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":New Line" << EndDebugInfo;
	}
	const Theme& theme = web_terminal->GetTheme();
	if(height < buffer_height)
	{
		lines.push_back(new ScreenBufferLine(display_width, theme));
		height++;
	}
	else
	{
		ScreenBufferLine *new_line = lines[0];
		memmove(&lines[0], &lines[1], (height - 1) * sizeof(ScreenBufferLine *));
		lines[height - 1] = new_line;
		new_line->Erase(theme);
	}
}

void ScreenBuffer::ScrollUp(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Scroll Up " << n << EndDebugInfo;
	}
	const Theme& theme = web_terminal->GetTheme();
	for(unsigned int i = 0; i < n; i++)
	{
		ScreenBufferLine *new_line = lines[0];
		memmove(&lines[0], &lines[1], (height - 1) * sizeof(ScreenBufferLine *));
		lines[height - 1] = new_line;
		new_line->Erase(theme);
	}
}

void ScreenBuffer::ScrollDown(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Scroll Down " << n << EndDebugInfo;
	}
	const Theme& theme = web_terminal->GetTheme();
	for(unsigned int i = 0; i < n; i++)
	{
		ScreenBufferLine *new_line = lines[height - 1];
		memmove(&lines[1], &lines[0], (height - 1) * sizeof(ScreenBufferLine *));
		lines[0] = new_line;
		new_line->Erase(theme);
	}
}

void ScreenBuffer::SelectGraphicsRendition(unsigned int n)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Select Graphic Rendition " << n << EndDebugInfo;
	}
	switch(n)
	{
		case 0:
			SetDefaultGraphicRendition();
			break;
		case 1:
			SetIntensity(INCREASED_INTENSITY);
			break;
		case 2:
			SetIntensity(DECREASED_INTENSITY);
			break;
		case 3:
			SetItalic();
			break;
		case 4:
			SetUnderline();
			break;
		case 5:
			SetBlink(SLOW_BLINK);
			break;
		case 6:
			SetBlink(RAPID_BLINK);
			break;
		case 7:
			SetReverseVideo();
			break;
		case 9:
			SetCrossedOut();
			break;
		case 22:
			SetIntensity(NORMAL_INTENSITY);
			break;
		case 23:
			SetItalic(false);
			break;
		case 24:
			SetUnderline(false);
			break;
		case 25:
			SetBlink(NO_BLINK);
			break;
		case 27:
			SetReverseVideo(false);
			break;
		case 29:
			SetCrossedOut(false);
			break;
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
			SetForegroundColor(n - 30);
			break;
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			SetBackgroundColor(n - 40);
			break;
		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:
			SetForegroundColor(8 + n - 90);
			break;
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
			SetBackgroundColor(8 + n - 100);
			break;
		default:
			logger << DebugWarning << "Unknown ANSI graphic rendition " << n << EndDebugWarning;
			break;
	}
}

void ScreenBuffer::ReportCursorPosition(InputBuffer& input_buffer)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Get cursor position" << EndDebugInfo;
	}
	std::stringstream sstr;
	sstr << "\x1b[" << cursor_lineno << ";" << cursor_colno << "R";
	input_buffer.Push(sstr.str());
}

void ScreenBuffer::SaveCursorPosition()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Save cursor position" << EndDebugInfo;
	}
	save_cursor_colno = cursor_colno;
	save_cursor_lineno = cursor_lineno;
	save_gr = gr;
}

void ScreenBuffer::RestoreCursorPosition()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << cursor_lineno << ":" << cursor_colno << ":Restore cursor position" << EndDebugInfo;
	}
	cursor_colno = save_cursor_colno;
	cursor_lineno = save_cursor_lineno;
	gr = save_gr;
}

void ScreenBuffer::MoveCursorDownScrollUp()
{
	if(cursor_lineno < display_height)
	{
		cursor_lineno++;
	}
	else
	{
		ScrollUp();
	}
}

void ScreenBuffer::MoveCursorUpScrollDown()
{
	if(cursor_lineno > 1)
	{
		cursor_lineno--;
	}
	else
	{
		ScrollDown();
	}
}

///////////////////////////////// InputBuffer /////////////////////////////////

InputBuffer::InputBuffer(unsigned int _size)
	: size(_size)
	, front_index(0)
	, back_index(0)
	, fill_level(0)
	, storage(size)
{
}

void InputBuffer::Push(char c)
{
	if(fill_level != size)
	{
// 			std::cerr << "Pushing " << (unsigned int)(unsigned char) c << std::endl;
		storage[back_index] = c;
		++back_index;
		if(back_index >= size) back_index = 0;
		fill_level++;
	}
}

void InputBuffer::Push(const char *s)
{
	char c = *s++;
	if(c)
	{
		do
		{
			Push(c);
			c = *s++;
		}
		while(c);
	}
}

void InputBuffer::Push(const std::string& s)
{
	std::size_t n = s.length();
	for(std::size_t i = 0; i < n; i++)
	{
		Push(s[i]);
	}
}

bool InputBuffer::Empty() const
{
	return fill_level == 0;
}

bool InputBuffer::Full() const
{
	return fill_level == size;
}

char InputBuffer::Front() const
{
	return storage[front_index];
}

void InputBuffer::Pop()
{
	if(fill_level)
	{
		++front_index;
		if(front_index >= size) front_index = 0;
		fill_level--;
	}
}

///////////////////////////////// WebTerminal /////////////////////////////////

WebTerminal::WebTerminal(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "An ANSI Terminal over HTTP")
	, unisim::kernel::Service<unisim::service::interfaces::CharIO>(name, parent)
	, unisim::kernel::Service<unisim::service::interfaces::HttpServer>(name, parent)
	, char_io_export("char-io-export", this)
	, http_server_export("http-server-export", this)
	, logger(*this)
	, theme("theme", this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, input_buffer_size(256)
	, param_input_buffer_size("input-buffer-size", this, input_buffer_size, "Input buffer size")
	, min_display_refresh_period(0.040)
	, param_min_display_refresh_period("min-display-refresh-period", this, min_display_refresh_period, "Minimum refresh period of display (in seconds)")
	, max_display_refresh_period(0.320)
	, param_max_display_refresh_period("max-display-refresh-period", this, max_display_refresh_period, "Maximum refresh period of display (in seconds)")
	, title(GetName())
	, param_title("title", this, title, "Title of this web terminal instance")
	, implicit_cr_in_every_lf(true)
	, param_implicit_cr_in_every_lf("implicit-cr-in-every-lf", this, implicit_cr_in_every_lf, "Implicit Carriage Return (CR, '\\r') in every Line Feed (LF, '\\n')")
	, activity(false)
	, refresh_period(min_display_refresh_period)
	, str()
	, state(0)
	, got_arg(false)
	, arg(0)
	, args()
	, screen_buffer("screen-buffer", this)
	, alt_screen_buffer("alt-screen-buffer", this)
	, curr_screen_buffer(&screen_buffer)
	, input_buffer(input_buffer_size)
	, kevt2ansi()
	, mutex()
{
	pthread_mutex_init(&mutex, NULL);
	
	param_input_buffer_size.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	kevt2ansi[KeyEvent("Enter")] = "\n";
	kevt2ansi[KeyEvent("Backspace")] = "\x7f";
	kevt2ansi[KeyEvent("Delete")] = "\x1b[3~";
	kevt2ansi[KeyEvent("Tab")] = "\t";
	kevt2ansi[KeyEvent("ArrowUp")] = "\x1b[A";
	kevt2ansi[KeyEvent("ArrowDown")] = "\x1b[B";
	kevt2ansi[KeyEvent("ArrowRight")] = "\x1b[C";
	kevt2ansi[KeyEvent("ArrowLeft")] = "\x1b[D";
	kevt2ansi[KeyEvent("PageUp")] = "\x1b[5~";
	kevt2ansi[KeyEvent("PageDown")] = "\x1b[6~";
	kevt2ansi[KeyEvent("Home")] = "\x1b[1~";// "\x1b[H";
	kevt2ansi[KeyEvent("End")] = "\x1b[4~"; //"\x1b[F";
	kevt2ansi[KeyEvent("Escape")] = "\x1b";
	kevt2ansi[KeyEvent("Insert")] = "\x1b[2~";
	kevt2ansi[KeyEvent("F1")] = "\x1b[OP";
	kevt2ansi[KeyEvent("F2")] = "\x1b[OQ";
	kevt2ansi[KeyEvent("F3")] = "\x1b[OR";
	kevt2ansi[KeyEvent("F4")] = "\x1b[OS";
	kevt2ansi[KeyEvent("F5")] = "\x1b[15~";
	kevt2ansi[KeyEvent("F6")] = "\x1b[17~";
	kevt2ansi[KeyEvent("F7")] = "\x1b[18~";
	kevt2ansi[KeyEvent("F8")] = "\x1b[19~";
	kevt2ansi[KeyEvent("F9")] = "\x1b[20~";
	kevt2ansi[KeyEvent("F10")] = "\x1b[21~";
	kevt2ansi[KeyEvent("F11")] = "\x1b[23~";
	kevt2ansi[KeyEvent("F12")] = "\x1b[24~";
	kevt2ansi[KeyEvent("F13")] = "\x1b[25~";
	kevt2ansi[KeyEvent("F14")] = "\x1b[26~";
	kevt2ansi[KeyEvent("F15")] = "\x1b[28~";
	kevt2ansi[KeyEvent("F16")] = "\x1b[29~";
	kevt2ansi[KeyEvent("F17")] = "\x1b[31~";
	kevt2ansi[KeyEvent("F18")] = "\x1b[32~";
	kevt2ansi[KeyEvent("F19")] = "\x1b[33~";
	kevt2ansi[KeyEvent("F20")] = "\x1b[34~";
	kevt2ansi[KeyEvent("a").CTRL()] = "\x1";
	kevt2ansi[KeyEvent("b").CTRL()] = "\x2";
	kevt2ansi[KeyEvent("c").CTRL()] = "\x3";
	kevt2ansi[KeyEvent("d").CTRL()] = "\x4";
	kevt2ansi[KeyEvent("e").CTRL()] = "\x5";
	kevt2ansi[KeyEvent("f").CTRL()] = "\x6";
	kevt2ansi[KeyEvent("g").CTRL()] = "\x7";
	kevt2ansi[KeyEvent("h").CTRL()] = "\x8";
	kevt2ansi[KeyEvent("i").CTRL()] = "\x9";
	kevt2ansi[KeyEvent("j").CTRL()] = "\xa";
	kevt2ansi[KeyEvent("k").CTRL()] = "\xb";
	kevt2ansi[KeyEvent("l").CTRL()] = "\xc";
	kevt2ansi[KeyEvent("m").CTRL()] = "\n\r";
	kevt2ansi[KeyEvent("n").CTRL()] = "\xe";
	kevt2ansi[KeyEvent("o").CTRL()] = "\xf";
	kevt2ansi[KeyEvent("p").CTRL()] = "\x10";
	kevt2ansi[KeyEvent("q").CTRL()] = "\x11";
	kevt2ansi[KeyEvent("r").CTRL()] = "\x12";
	kevt2ansi[KeyEvent("s").CTRL()] = "\x13";
	kevt2ansi[KeyEvent("t").CTRL()] = "\x14";
	kevt2ansi[KeyEvent("u").CTRL()] = "\x15";
	kevt2ansi[KeyEvent("v").CTRL()] = "\x16";
	kevt2ansi[KeyEvent("w").CTRL()] = "\x17";
	kevt2ansi[KeyEvent("x").CTRL()] = "\x18";
	kevt2ansi[KeyEvent("y").CTRL()] = "\x19";
	kevt2ansi[KeyEvent("z").CTRL()] = "\x1a";

}

bool WebTerminal::BeginSetup()
{
	return true;
}

WebTerminal::~WebTerminal()
{
	pthread_mutex_destroy(&mutex);
}

// unisim::service::interfaces::CharIO
void WebTerminal::ResetCharIO()
{
}

bool WebTerminal::GetChar(char& c)
{
	if(!input_buffer.Empty())
	{
		c = input_buffer.Front();
		input_buffer.Pop();
		
		if(!input_buffer.Empty()) activity = true;
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Getting character ";
			uint8_t v = (uint8_t) c;
			if((v >= 32) && (v < 127))
				logger << "'" << c << "'";
			else
				logger << "0x" << std::hex << (unsigned int) v << std::dec;
			logger << EndDebugInfo;
		}
		return true;
	}
	return false;
}

void WebTerminal::Receive(const KeyEvent& key_event)
{
	activity = true;
	
	KEVT2ANSI::const_iterator it = kevt2ansi.find(key_event);
	if(it != kevt2ansi.end())
	{
		const std::string& sequence = (*it).second;
		
// 		std::cerr << key_event << " => \"";
		for(std::size_t i = 0; i < sequence.length(); i++)
		{
// 			char c = sequence[i];
// 			if((c >= ' ') && (c <= '~'))
// 			{
// 				std::cerr << c;
// 			}
// 			else
// 			{
// 				OStreamContext osc(std::cerr);
// 				std::cerr << "\\x" << std::hex;
// 				std::cerr.fill('0');
// 				std::cerr.width(2);
// 				std::cerr << (unsigned int) c;
// 			}
		}
//		std::cerr << "\"" << std::endl;
		input_buffer.Push(sequence);
		return;
	}
	
	const std::string& key = key_event.GetKey();
	std::size_t n = key.length();
	if((n >= 1) && (n <= 4))
	{
		// maybe ASCII (not extended) or UTF-8
		char c = key[0];
		if((((unsigned char) c < 128) && (n == 1)) || (((unsigned char) c >= 128) && (n > 1)))
		{
			input_buffer.Push(c);
			for(std::size_t i = 1; i < n; i++)
			{
				c = key[i];
				input_buffer.Push(c);
			}
			return;
		}
	}
	
	if(verbose)
	{
		logger << DebugInfo << key_event << " is not recognized" << EndDebugInfo;
	}
}

void WebTerminal::PutChar(char c)
{
	Lock();
	
	if(unlikely(verbose))
	{
		uint8_t v = (uint8_t) c;
		logger << DebugInfo << "Putting character ";
		if((v >= 32) && (v < 127))
			logger << "'" << v << "'";
		else
			logger << "0x" << std::hex << (unsigned int) v << std::dec;
		logger << EndDebugInfo;
	}
	
	str += c;
	
	activity = true;
	switch(state)
	{
		case 1:
			if(c == '[')
			{
				// got Control Sequence Introducer (CSI)
				state = 2;
				arg = 0;
				break;
			}
			
			if(c == 'M')
			{
				str.clear();
				curr_screen_buffer->MoveCursorDownScrollUp();
				break;
			}
			
			if(c == 'D')
			{
				str.clear();
				curr_screen_buffer->MoveCursorUpScrollDown();
				break;
			}

			if(c == 'E')
			{
				str.clear();
				curr_screen_buffer->MoveCursorNextLine();
				break;
			}
			
			// ignore previous ESC, process character as usual
			curr_screen_buffer->PutString(str);
			str.clear();
			state = 0;
			break;
			
		case 0:
			if(c == 0x1b) // ESC ?
			{
				// got ESC
				state = 1;
				break;
			}
			
			if(c == '\r')
			{
				curr_screen_buffer->CarriageReturn();
				break;
			}
			
			if(c == '\n')
			{
				if(implicit_cr_in_every_lf) curr_screen_buffer->CarriageReturn();
				curr_screen_buffer->LineFeed();
				break;
			}
			
			if(c == '\t')
			{
				for(unsigned int i = 0; i < 8; i++)
				{
					curr_screen_buffer->PutChar(0);
				}
				break;
			}
			
			if(c == '\b')
			{
				curr_screen_buffer->Backspace();
				break;
			}
			
			if((uint8_t) c == 127)
			{
				curr_screen_buffer->Delete();
				break;
			}
			
			curr_screen_buffer->PutChar(c);
			break;
			
		case 2:
			if(c == '?')
			{
				state = 3;
				break;
			}
			state = 4;
			
		case 3:
		case 4:
			if((c >= '0') && (c <= '9'))
			{
				arg = (arg * 10) + (c - '0');
				got_arg = true;
			}
			else
			{
				if(got_arg)
				{
					args.push_back(arg);
					arg = 0;
					got_arg = false;
				}
				
				if(c == ';')
				{
					break;
				}
				else
				{
					switch(c)
					{
						case 'A': // Cursor Up
							str.clear();
							curr_screen_buffer->MoveCursorUp((args.size() > 0) ? args[0] : 1);
							break;
						case 'B': // Cursor Down
							str.clear();
							curr_screen_buffer->MoveCursorDown((args.size() > 0) ? args[0] : 1);
							break;
						case 'C': // Cursor Forward
							str.clear();
							curr_screen_buffer->MoveCursorForward((args.size() > 0) ? args[0] : 1);
							break;
						case 'D': // Cursor Back
							str.clear();
							curr_screen_buffer->MoveCursorBack((args.size() > 0) ? args[0] : 1);
							break;
						case 'E': // Cursor Next Line
							str.clear();
							curr_screen_buffer->MoveCursorNextLine((args.size() > 0) ? args[0] : 1);
							break;
						case 'F': // Cursor Previous Line
							str.clear();
							curr_screen_buffer->MoveCursorPreviousLine((args.size() > 0) ? args[0] : 1);
							break;
						case 'G': // Cursor Horizontal Absolute
							str.clear();
							curr_screen_buffer->MoveCursorHorizontalAbsolute((args.size() > 0) ? args[0] : 1);
							break;
						case 'H': // Cursor Position
						case 'f': // Horizontal Vertical Position
							str.clear();
							curr_screen_buffer->MoveCursorTo((args.size() > 0) ? args[0] : 1, (args.size() > 1) ? args[1] : 1);
							break;
						case 'J': // Erase In Display
							str.clear();
							curr_screen_buffer->EraseInDisplay((args.size() > 0) ? args[0] : 0);
							break;
						case 'K': // Erase In Line
							str.clear();
							curr_screen_buffer->EraseInLine((args.size() > 0) ? args[0] : 0);
							break;
						case 'S': // Scroll Up
							str.clear();
							curr_screen_buffer->ScrollUp((args.size() > 0) ? args[0] : 1);
							break;
						case 'T': // Scroll Down
							str.clear();
							curr_screen_buffer->ScrollDown((args.size() > 0) ? args[0] : 1);
							break;
						case 'm': // Select Graphic Rendition
						{
							str.clear();
							for(unsigned int i = 0; i < args.size(); i++)
							{
								curr_screen_buffer->SelectGraphicsRendition(args[i]);
							}
							break;
						}
						case 's': // Save Cursor
							str.clear();
							curr_screen_buffer->SaveCursorPosition();
							break;
						case 'u': // Unsave Cursor
							str.clear();
							curr_screen_buffer->RestoreCursorPosition();
							break;
						case 'h':
							str.clear();
							if(args.size() > 0)
							{
								switch(args[0])
								{
									case 1049: // Enable alternative screen buffer
										if(unlikely(verbose))
										{
											logger << DebugInfo << "Enabling alternative screen buffer" << EndDebugInfo;
										}
										curr_screen_buffer = &alt_screen_buffer;
										break;
								}
							}
							break;
						case 'l':
							str.clear();
							if(args.size() > 0)
							{
								switch(args[0])
								{
									case 1049: // Disable alternative screen buffer
										if(unlikely(verbose))
										{
											logger << DebugInfo << "Disabling alternative screen buffer" << EndDebugInfo;
										}
										curr_screen_buffer = &screen_buffer;
										break;
								}
							}
							break;
							
						case 'n':
							str.clear();
							if(args.size() > 0)
							{
								switch(args[0])
								{
									case 5: // Query device status
										if(unlikely(verbose))
										{
											logger << DebugInfo << "Querying device status" << EndDebugInfo;
										}
										input_buffer.Push("\x1b[0n"); // Report Device OK
										break;
										
									case 6: // Query Cursor Position
										if(unlikely(verbose))
										{
											logger << DebugInfo << "Querying cursor position" << EndDebugInfo;
										}
										curr_screen_buffer->ReportCursorPosition(input_buffer);
										break;
								}
							}
							break;
							
						default:
							curr_screen_buffer->PutString(str);
							logger << DebugWarning << "Unknown ANSI escape code '" << c << "'" << EndDebugWarning;
							break;
					}
					args.clear();
					state = 0;
				}
			}
			break;
	}
	Unlock();
}

void WebTerminal::FlushChars()
{
}

void WebTerminal::GenerateStyle(std::ostream& os, const GraphicRendition& gr, bool under_cursor, const std::string& class_name)
{
	os << "\t\t\t." << class_name << " {";
	switch(gr.GetIntensity())
	{
		case DECREASED_INTENSITY: os << " font-weight:lighter;"; break;
		case NORMAL_INTENSITY: break;
		case INCREASED_INTENSITY: os << " font-weight:bold;"; break;
	}
	if(gr.GetItalic())
	{
		os << " font-style:italic;";
	}
	if(gr.GetUnderline() || gr.GetCrossedOut())
	{
		os << " text-decoration:";
		if(gr.GetUnderline()) os << " underline";
		if(gr.GetCrossedOut()) os << " line-through";
		os << ";";
	}
	Blink blink = gr.GetBlink();
	if(blink || under_cursor)
	{
		os << " animation:";
	}
	switch(blink)
	{
		case NO_BLINK: break;
		case SLOW_BLINK:
			os << "" << class_name << "-blinker 1s step-start infinite";
			break;
		case RAPID_BLINK:
			os << "" << class_name << "-blinker 0.5s step-start infinite";
			break;
	}
	if(under_cursor)
	{
		if(blink)
		{
			os << ",";
		}
		os << "" << class_name << "-cursor-blinker 1s step-start infinite;";
	}
	else if(blink)
	{
		os << ";";
	}
	
	unsigned int fg_color = gr.GetReverseVideo() ? gr.GetBackgroundColor() : gr.GetForegroundColor();
	unsigned int bg_color = gr.GetReverseVideo() ? gr.GetForegroundColor() : gr.GetBackgroundColor();
	os << " color:" << theme.GetColor(fg_color) << ";";
	os << " background-color:" << theme.GetColor(bg_color) << ";";
	if(under_cursor)
	{
		os << " box-shadow:inset 0 -2px " << theme.GetColor(theme.GetForegroundColor()) << ";";
	}
	os << " }" << std::endl;
	if(blink)
	{
		os << "\t\t\t@keyframes " << class_name << "-blinker { 50% { color:" << theme.GetColor(bg_color) << "; } }" << std::endl;
	}
	if(under_cursor)
	{
		os << "\t\t\t@keyframes " << class_name << "-cursor-blinker { 50% { box-shadow:none; } }" << std::endl;
	}
}

// unisim::service::interfaces::HttpServer
bool WebTerminal::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(req.GetPath() == "")
	{
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
			case unisim::util::hypapp::Request::POST:
				Lock();
				break;
				
			default:
				break;
		}
		
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::OPTIONS:
				response.Allow("OPTIONS, GET, HEAD, POST");
				break;
				
			case unisim::util::hypapp::Request::POST:
			{
				struct PropertySetter : public unisim::util::hypapp::Form_URL_Encoded_Decoder
				{
					PropertySetter() : key_event(), text() {}
					
					virtual bool FormAssign(const std::string& name, const std::string& value)
					{
						if(name == "key")
						{
							key_event.KEY(value);
						}
						else if(name == "ctrl")
						{
							key_event.CTRL(value == "1");
						}
						else if(name == "shift")
						{
							key_event.SHIFT(value == "1");
						}
						else if(name == "alt")
						{
							key_event.ALT(value == "1");
						}
						else if(name == "meta")
						{
							key_event.META(value == "1");
						}
						else if(name == "text")
						{
							text = value;
						}
						
						return true;
					}
					
					KeyEvent key_event;
					std::string text;
				};
				
				// Decode POST data
				PropertySetter property_setter;
				if(property_setter.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
				{
					if(!property_setter.text.empty())
					{
						input_buffer.Push(property_setter.text);
					}
					else if(!property_setter.key_event.GetKey().empty())
					{
						// Update input buffer with informations of keydown event in POST data
						Receive(property_setter.key_event);
					}
				}
				
				break;
			}
			
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
			{
				// Compute the set of graphic rendition
				std::set<std::pair<GraphicRendition, bool> > gr_set;
				for(ScreenBuffer::iterator it = curr_screen_buffer->Begin(); it != curr_screen_buffer->End(); ++it)
				{
					ScreenBufferLine& sbl = **it;
					for(unsigned int colno = 1; colno <= curr_screen_buffer->GetDisplayWidth(); colno++)
					{
						ScreenBufferCharacter& sbc = sbl[colno];
						bool under_cursor = curr_screen_buffer->IsAtCursorLinePos(it) && (colno == curr_screen_buffer->GetCursorColNo());
						gr_set.insert(std::pair<GraphicRendition, bool>(sbc.gr, under_cursor));
					}
				}
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html lang=\"en\">" << std::endl;
				response << "\t<head>" << std::endl;
				response << "\t\t<title>" << unisim::util::hypapp::HTML_Encoder::Encode(title) << "</title>" << std::endl;
				response << "\t\t<meta name=\"description\" content=\"terminal emulator over HTTP\">" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
				response << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
				response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/web_terminal/style.css\" type=\"text/css\" />" << std::endl;
				response << "\t\t<style>" << std::endl;
				
				// Generate CSS style for each graphic rendition
				GraphicRendition theme_gr = theme.MakeGraphicRendition();
				
				bool theme_style_class_generated = false;
				unsigned int theme_style_class_num = 0;
				std::map<std::pair<GraphicRendition, bool>, unsigned int> gr_class_map;
				unsigned int class_num = 0;
				for(std::set<std::pair<GraphicRendition, bool> >::const_iterator it = gr_set.begin(); it != gr_set.end(); it++, class_num++)
				{
					const std::pair<GraphicRendition, bool>& pair = *it;
					const GraphicRendition& gr = pair.first;
					bool under_cursor = pair.second;
					gr_class_map[pair] = class_num;
					
					GenerateStyle(response, gr, under_cursor, std::string("c") + to_string(class_num));
					if((theme_gr == gr) && !under_cursor)
					{
						theme_style_class_generated = true;
						theme_style_class_num = class_num;
					}
				}
				if(!theme_style_class_generated)
				{
					GenerateStyle(response, theme_gr, false, std::string("c") + to_string(class_num));
					theme_style_class_generated = true;
					theme_style_class_num = class_num;
				}
				
				response << "\t\t</style>" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';var activity = " << (activity ? "true" : "false") << ";</script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/web_terminal/script.js\"></script>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body class=\"c" << theme_style_class_num << "\" onload=\"gui.auto_reload(" << (unsigned int)(refresh_period * 1000) << ", 'self-refresh-when-active')\">" << std::endl;
				refresh_period = activity ? min_display_refresh_period : std::min(refresh_period * 2.0, max_display_refresh_period);
				activity = false;
				response << "\t\t<div id=\"screen-buffer\" tabindex=\"-999\">" << std::endl;
				
				// Generate screen buffer graphical representation line by line
				GraphicRendition gr;
				bool open = false;
				// Iterate over each line of screen buffer
				for(ScreenBuffer::iterator it = curr_screen_buffer->Begin(); it != curr_screen_buffer->End(); ++it)
				{
					response << "\t\t\t";
					
					ScreenBufferLine& sbl = **it;
					
					// Iterate over each column of screen buffer line
					for(unsigned int colno = 1; colno <= curr_screen_buffer->GetDisplayWidth(); colno++)
					{
						ScreenBufferCharacter& sbc = sbl[colno];
						
						// Detect whether current line and column is under the cursor
						bool under_cursor = curr_screen_buffer->IsAtCursorLinePos(it) && (colno == curr_screen_buffer->GetCursorColNo());
						
						if(!open || (sbc.gr != gr) || under_cursor)
						{
							// close pre tag if it's open and when graphic rendition changes or the current line and column is under the cursor
							if(open)
							{
								response << "</pre>";
								open = false;
							}
							
							gr = sbc.gr; // remember what's the current graphic rendition

							// Find the CSS class corresponding to the current graphic rendition
							std::map<std::pair<GraphicRendition, bool>, unsigned int>::const_iterator gr_class_it = gr_class_map.find(std::pair<GraphicRendition, bool>(gr, under_cursor));
							if(gr_class_it != gr_class_map.end())
							{
								unsigned int class_num = (*gr_class_it).second;
								response << "<pre class=\"c" << class_num << "\">"; // open a pre with the right CSS class corresponding to the current graphic rendition
								open = true;
							}
						}
						
						// emit an UTF-8 character
						const UnicodeCharacter& c = sbc.c;
						if(c == (uint32_t) '<') response << "&lt;"; // print '<'
						else if((c >= 32u) && (c != 127u)) response << c; // printable ASCII/UTF-8 character
						else response << ' '; // unprintable ASCII/UTF-8 character
						if(under_cursor)
						{
							// close pre tag right after when the last emitted line and column is under the cursor
							response << "</pre>";
							open = false;
						}
					}
					if(open)
					{
						// close pre tag at the end of line if not already closed
						response << "</pre>";
						open = false;
					}
					response << "<br>" << std::endl; // insert line break
				}
				response << "\t\t</div>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				
				break;
			}
			
			default:
				logger << DebugWarning << "Method not allowed" << EndDebugWarning;
				response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
				response.Allow("OPTIONS, GET, HEAD, POST");
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html lang=\"en\">" << std::endl;
				response << "\t<head>" << std::endl;
				response << "\t\t<title>Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")</title>" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<meta name=\"description\" content=\"Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")\">" << std::endl;
				response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<style>" << std::endl;
				response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
				response << "\t\t</style>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				response << "\t\t<p>Method not allowed</p>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				break;
		}
		
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
			case unisim::util::hypapp::Request::POST:
				Unlock();
				break;
				
			default:
				break;
		}
	}
	else
	{
		response.SetStatus(unisim::util::hypapp::HttpResponse::NOT_FOUND);
		
		response << "<!DOCTYPE html>" << std::endl;
		response << "<html lang=\"en\">" << std::endl;
		response << "\t<head>" << std::endl;
		response << "\t\t<title>Error " << (unsigned int) unisim::util::hypapp::HttpResponse::NOT_FOUND << " (" << unisim::util::hypapp::HttpResponse::NOT_FOUND << ")</title>" << std::endl;
		response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		response << "\t\t<meta name=\"description\" content=\"Error " << (unsigned int) unisim::util::hypapp::HttpResponse::NOT_FOUND << " (" << unisim::util::hypapp::HttpResponse::NOT_FOUND << ")\">" << std::endl;
		response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
		response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
		response << "\t\t<style>" << std::endl;
		response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
		response << "\t\t</style>" << std::endl;
		response << "\t</head>" << std::endl;
		response << "\t<body>" << std::endl;
		response << "\t\t<p>Unavailable</p>" << std::endl;
		response << "\t</body>" << std::endl;
		response << "</html>" << std::endl;
	}
	
	bool send_status = conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));

	if(send_status)
	{
		if(verbose)
		{
			logger << DebugInfo << "sending HTTP response: done" << EndDebugInfo;
		}
	}
	else
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response" << EndDebugWarning;
	}
	
	
	return send_status;
}

void WebTerminal::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
}

void WebTerminal::Lock()
{
	pthread_mutex_lock(&mutex);
}

void WebTerminal::Unlock()
{
	pthread_mutex_unlock(&mutex);
}

} // end of namespace web_terminal
} // end of namespace service
} // end of namespace unisim

namespace unisim {
namespace kernel {
namespace variable {

using unisim::service::web_terminal::Intensity;
using unisim::service::web_terminal::DECREASED_INTENSITY;
using unisim::service::web_terminal::NORMAL_INTENSITY;
using unisim::service::web_terminal::INCREASED_INTENSITY;

template <> Variable<Intensity>::Variable(const char *_name, Object *_object, Intensity& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
	AddEnumeratedValue("decreased");
	AddEnumeratedValue("normal");
	AddEnumeratedValue("increased");
}

template <>
const char *Variable<Intensity>::GetDataTypeName() const
{
	return "unisim::service::web_terminal::Intensity";
}

template <>
VariableBase::DataType Variable<Intensity>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<Intensity>::GetBitSize() const
{
	return 2;
}

template <> Variable<Intensity>::operator bool () const { return *storage != NORMAL_INTENSITY; }
template <> Variable<Intensity>::operator long long () const { return *storage; }
template <> Variable<Intensity>::operator unsigned long long () const { return *storage; }
template <> Variable<Intensity>::operator double () const { return (double)(*storage); }
template <> Variable<Intensity>::operator std::string () const
{
	switch(*storage)
	{
		case DECREASED_INTENSITY: return std::string("decreased");
		case NORMAL_INTENSITY: return std::string("normal");
		case INCREASED_INTENSITY: return std::string("increased");
	}
	return std::string("?");
}

template <> VariableBase& Variable<Intensity>::operator = (bool value)
{
	if(IsMutable())
	{
		Intensity tmp = *storage;
		switch((int) value)
		{
			case DECREASED_INTENSITY:
			case NORMAL_INTENSITY:
			case INCREASED_INTENSITY:
				tmp = (Intensity)(int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<Intensity>::operator = (long long value)
{
	if(IsMutable())
	{
		Intensity tmp = *storage;
		switch(value)
		{
			case DECREASED_INTENSITY:
			case NORMAL_INTENSITY:
			case INCREASED_INTENSITY:
				tmp = (Intensity) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<Intensity>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		Intensity tmp = *storage;
		switch((long long) value)
		{
			case DECREASED_INTENSITY:
			case NORMAL_INTENSITY:
			case INCREASED_INTENSITY:
				tmp = (Intensity)(long long) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<Intensity>::operator = (double value)
{
	if(IsMutable())
	{
		Intensity tmp = *storage;
		switch((int) value)
		{
			case DECREASED_INTENSITY:
			case NORMAL_INTENSITY:
			case INCREASED_INTENSITY:
				tmp = (Intensity)(int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<Intensity>::operator = (const char *value)
{
	if(IsMutable())
	{
		Intensity tmp = *storage;
		if(std::string(value) == std::string("decreased")) tmp = DECREASED_INTENSITY;
		else if(std::string(value) == std::string("normal")) tmp = NORMAL_INTENSITY;
		else if(std::string(value) == std::string("increased")) tmp = INCREASED_INTENSITY;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<Intensity>;

using unisim::service::web_terminal::Blink;
using unisim::service::web_terminal::NO_BLINK;
using unisim::service::web_terminal::SLOW_BLINK;
using unisim::service::web_terminal::RAPID_BLINK;

template <> Variable<Blink>::Variable(const char *_name, Object *_object, Blink& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
	AddEnumeratedValue("none");
	AddEnumeratedValue("slow");
	AddEnumeratedValue("rapid");
}

template <>
const char *Variable<Blink>::GetDataTypeName() const
{
	return "unisim::service::web_terminal::Blink";
}

template <>
VariableBase::DataType Variable<Blink>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<Blink>::GetBitSize() const
{
	return 2;
}

template <> Variable<Blink>::operator bool () const { return *storage != NO_BLINK; }
template <> Variable<Blink>::operator long long () const { return *storage; }
template <> Variable<Blink>::operator unsigned long long () const { return *storage; }
template <> Variable<Blink>::operator double () const { return (double)(*storage); }
template <> Variable<Blink>::operator std::string () const
{
	switch(*storage)
	{
		case NO_BLINK: return std::string("none");
		case SLOW_BLINK: return std::string("slow");
		case RAPID_BLINK: return std::string("rapid");
	}
	return std::string("?");
}

template <> VariableBase& Variable<Blink>::operator = (bool value)
{
	if(IsMutable())
	{
		Blink tmp = *storage;
		switch((unsigned int) value)
		{
			case NO_BLINK:
			case SLOW_BLINK:
			case RAPID_BLINK:
				tmp = (Blink)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<Blink>::operator = (long long value)
{
	if(IsMutable())
	{
		Blink tmp = *storage;
		switch(value)
		{
			case NO_BLINK:
			case SLOW_BLINK:
			case RAPID_BLINK:
				tmp = (Blink) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<Blink>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		Blink tmp = *storage;
		switch(value)
		{
			case NO_BLINK:
			case SLOW_BLINK:
			case RAPID_BLINK:
				tmp = (Blink) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<Blink>::operator = (double value)
{
	if(IsMutable())
	{
		Blink tmp = *storage;
		switch((unsigned int) value)
		{
			case NO_BLINK:
			case SLOW_BLINK:
			case RAPID_BLINK:
				tmp = (Blink)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<Blink>::operator = (const char *value)
{
	if(IsMutable())
	{
		Blink tmp = *storage;
		if(std::string(value) == std::string("none")) tmp = NO_BLINK;
		else if(std::string(value) == std::string("slow")) tmp = SLOW_BLINK;
		else if(std::string(value) == std::string("rapid")) tmp = RAPID_BLINK;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<Blink>;

} // end of variable namespace
} // end of kernel namespace
} // end of unisim namespace
