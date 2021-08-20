/*
 *  Copyright (c) 2007,
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

#ifndef __UNISIM_SERVICE_SDL_SDL_HH__
#define __UNISIM_SERVICE_SDL_SDL_HH__

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/service/interfaces/video.hh>
#include <unisim/service/interfaces/keyboard.hh>
#include <unisim/service/interfaces/mouse.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>

#include <inttypes.h>

#if defined(HAVE_SDL)
#include <SDL.h>
#include <SDL_thread.h>
#endif

#include <list>
#include <string>
#include <map>

namespace unisim {
namespace service {
namespace sdl {

using std::list;
using std::string;
using std::map;
using namespace unisim::util::endian;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceImport;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Video;
using unisim::service::interfaces::Keyboard;
using unisim::service::interfaces::Mouse;

#if defined(HAVE_SDL)
template <class ADDRESS> class VideoMode;
#endif

template <class ADDRESS>
class SDL :
	public Service<Video<ADDRESS> >,
	public Client<Memory<ADDRESS> >,
	public Service<Keyboard>,
	public Service<Mouse>
{
public:
	ServiceExport<Video<ADDRESS> > video_export;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceExport<Keyboard> keyboard_export;
	ServiceExport<Mouse> mouse_export;
	
	SDL(const char *name, Object *parent = 0);
	virtual ~SDL();

	virtual void OnDisconnect();
	virtual bool Setup(ServiceExportBase *srv_export);

	virtual bool SetVideoMode(ADDRESS fb_addr, uint32_t width, uint32_t height, uint32_t depth, uint32_t fb_bytes_per_line);
	virtual void RefreshDisplay();
	virtual void ResetKeyboard();
	virtual void SetTypematicDelay(unsigned int delay_us, unsigned int interval_us);
	virtual bool GetKeyAction(unisim::service::interfaces::Keyboard::KeyAction& key_action);
	virtual void ResetMouse();
	virtual bool GetMouseState(unisim::service::interfaces::Mouse::MouseState& mouse_state);
	
private:
	unisim::kernel::logger::Logger logger;
	bool verbose_setup;
	bool verbose_run;
	list<unisim::service::interfaces::Keyboard::KeyAction> kbd_key_action_fifo;
	unisim::service::interfaces::Mouse::MouseState mouse_state;
	bool mouse_state_updated;
	Parameter<bool> param_verbose_setup;
	Parameter<bool> param_verbose_run;

	void PushKeyAction(const unisim::service::interfaces::Keyboard::KeyAction& key_action);

	// Setup SDL
	bool SetupSDL();
	
	bool alive;
	unsigned int typematic_delay_us;
	unsigned int typematic_interval_us;
	
#if defined(HAVE_SDL)
	enum
	{
		EV_NOTHING = 0,
		EV_SET_VIDEO_MODE = 1,
		EV_BLIT = 2
	};

	SDL_Surface *surface;
	SDL_Surface *screen;
	SDL_Thread *refresh_thread;
	SDL_Thread *event_handling_thread;
	SDL_mutex *sdl_mutex;
	SDL_mutex *logger_mutex;
	SDL_TimerID refresh_timer;
	bool host_key_down;
	bool host_cmd;
	bool grab_input;
	bool full_screen;
	SDLKey host_key;
	bool mode_set;
	SDL_cond *video_subsystem_initialized_cond;
	SDL_mutex *video_subsystem_initialized_mutex;
	bool refresh;
	bool force_refresh;
	VideoMode<ADDRESS> video_mode;
	unsigned int bmp_out_file_number;
	string learn_keymap_filename;
	map<string, SDLKey> sdlk_string_map;
	map<SDLKey, uint8_t> keymap;
	string bmp_out_filename;
	string keymap_filename;
	string host_key_name;
	uint32_t refresh_period;
	Parameter<uint32_t> param_refresh_period;
	Parameter<string> param_bmp_out_filename;
	Parameter<string> param_keymap_filename;
	Parameter<string> param_host_key_name;
	Parameter<bool> param_force_refresh;
	bool work_around_sdl_mouse_motion_coordinates_bug;
	Parameter<bool> param_work_around_sdl_mouse_motion_coordinates_bug;

	// Keyboard learning is executed in main thread
	void LearnKeyboard();

	// Event loop thread entry point
	static int EventHandlingThread(SDL<ADDRESS> *sdl);

	// Refresh timer hook
	static Uint32 RefreshTimer(Uint32 interval, void *param);
	void Blit();
	
	// Methods below can be run only within event handling thread
	void ProcessKeyboardEvent(SDL_KeyboardEvent& key);
	void ProcessMouseButtonEvent(SDL_MouseButtonEvent& mouse_button_ev);
	void ProcessMouseMotionEvent(SDL_MouseMotionEvent& mouse_motion_ev);
	void EventLoop();
	bool HandleSetVideoMode(const VideoMode<ADDRESS> *video_mode = 0);
	void HandleBlit();
	void GrabInput();
	void UngrabInput();
	void ToggleGrabInput();
	void UpdateWindowCaption();
	void ToggleFullScreen();
#endif
};

#if defined(HAVE_SDL)
template <class ADDRESS>
class VideoMode
{
public:
	VideoMode();
	VideoMode(ADDRESS fb_addr, uint32_t width, uint32_t height, uint32_t depth, uint32_t fb_bytes_per_line);

	ADDRESS fb_addr;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t fb_bytes_per_line;
};
#endif

} // end of namespace sdl
} // end of namespace service
} // end of namespace unisim

#endif
