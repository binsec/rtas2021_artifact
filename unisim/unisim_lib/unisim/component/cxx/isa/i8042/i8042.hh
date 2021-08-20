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

#ifndef __UNISIM_COMPONENT_CXX_ISA_I8042_I8042_HH__
#define __UNISIM_COMPONENT_CXX_ISA_I8042_I8042_HH__

#include <unisim/component/cxx/isa/types.hh>
#include <unisim/service/interfaces/keyboard.hh>
#include <unisim/service/interfaces/mouse.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <list>
#include <vector>

namespace unisim {
namespace component {
namespace cxx {
namespace isa {
namespace i8042 {

using std::list;
using std::vector;
using unisim::component::cxx::isa::isa_address_t;

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;

using unisim::service::interfaces::Keyboard;
using unisim::service::interfaces::Mouse;

class I8042 :
	public Client<Keyboard>,
	public Client<Mouse>
{
public:
	static const unsigned int KBD_INDEX = 0;
	static const unsigned int AUX_INDEX = 1;

	static const isa_address_t I8042_DATA_REG    = 0x60; // read/write
	static const isa_address_t I8042_COMMAND_REG = 0x64; // write only
	static const isa_address_t I8042_STATUS_REG  = 0x64; // read only

	// Status register bits
	static const uint8_t I8042_STR_PARITY  = 0x80; // Parity error (1=parity error on transmission, 0=no error)
	static const uint8_t I8042_STR_TIMEOUT = 0x40; // Keyboard Time-out error (1=keyboard time-out, 0=no time-out error)
	static const uint8_t I8042_STR_AUX_OBF = 0x20; // aux output buffer full (1=full, 0=empty)
	static const uint8_t I8042_STR_KEYLOCK = 0x10; // keyboard active (1=enabled, 0=disabled)
	static const uint8_t I8042_STR_CMDDAT  = 0x08; // Command/Data available (0=data available ar port 60h, 1=command available at port 64h)
	static const uint8_t I8042_STR_SYSFLAG = 0x04; // system flag (1= self test passed, 0=failed)
	static const uint8_t I8042_STR_IBF     = 0x02; // input buffer full (1=full, 0=empty)
	static const uint8_t I8042_STR_OBF     = 0x01; // output buffer full (1=full, 0=empty)

	// i8042 on-board microcontroller control register
	// to access command byte, processor must first write a command to port 64h: read=20h (I8042_CMD_RCTR), write=60h (I8042_CMD_WCTR)
	// then the command can be either read or write on port 60h
	static const uint8_t I8042_CTR_KBDINT     = 0x01; // keyboard interrupt (1=enabled, 0=disabled)
	static const uint8_t I8042_CTR_AUXINT     = 0x02; // aux interrupt (1=enabled, 0=disabled)
	static const uint8_t I8042_CTR_SYSFLAG    = 0x04; // system flag (1= self test passed, 0=failed)
	static const uint8_t I8042_CTR_IGNKEYLOCK = 0x08; // PC/AT inhibit override/ignore keylock (1=enabled always)
	static const uint8_t I8042_CTR_KBDDIS     = 0x10; // keyboard disable (1=disable keyboard, 0=no action)
	static const uint8_t I8042_CTR_AUXDIS     = 0x20; //aux disable (1=disable aux, 0=no action) 
	static const uint8_t I8042_CTR_XLATE      = 0x40; // IBM PC Compatibility Mode (1=translate kbd codes to PC scancodes)

	// i8042 on-board microcontroller commands (port 64h)
	static const uint8_t I8042_CMD_CTL_RCTR         = 0x20; // transmit keyboard controller's command byte to system as a scancode at port 60h
	static const uint8_t I8042_CMD_CTL_WCTR         = 0x60; // the next byte written to port 60h will be stored in the keyboard controller's command byte
	static const uint8_t I8042_CMD_CTL_PASSWD_TEST  = 0xa4; // Test if a password is installed (PS/2 only).
	                                                        // result comes back in port 60h. 0FAh means a password is installed, 0F1h means no password
	static const uint8_t I8042_CMD_CTL_PASSWD_SET   = 0xa5; // Transmit password (PS/2 only). Starts receipt of password.
	                                                        // The next sequence of scan codes written to port 60h, ending with a zero byte, are the new password
	static const uint8_t I8042_CMD_CTL_PASSWD_MATCH = 0xa6; // Password match. Characters from the keyboard are compared to password until a match occurs
	static const uint8_t I8042_CMD_CTL_AUX_DISABLE  = 0xa7; // Disable the aux device (i.e. the mouse, PS/2 only). Identical to setting bit five of the command byte
	static const uint8_t I8042_CMD_CTL_AUX_ENABLE   = 0xa8; // Enable the aux device (i.e. the mouse, PS/2 only). Identical to clearing bit five of the command byte
	static const uint8_t I8042_CMD_CTL_AUX_TEST     = 0xa9; // Test the aux device (i.e. the mouse, PS/2 only). Returns 0 if okay, 1 or 2 if there is a stuck clock,
	                                                        // 3 or 4 if there is a stuck data line. results come back in port 60h
	static const uint8_t I8042_CMD_CTL_SELF_TEST    = 0xaa; // Keyboard interface test. Tests the keyboard interface. Returns 0 if okay, 1 or 2 if there is a stuck clock,
	                                                        // 3 or 4 if there is a stuck data line. Results come back in port 60h.

	static const uint8_t I8042_CMD_AUX_LOOP = 0xd3;
	static const uint8_t I8042_CMD_AUX_WRITE = 0xd4;
	// more commands to be added...

	// i8042 command results
	static const uint8_t I8042_RET_CTL_TEST = 0x55; // self test okay
	static const uint8_t I8042_RET_AUX_TEST_OK = 0x00; // aux test okay

	// keyboard microcontroller commands (port 60h)
	static const uint8_t KBD_CMD_SET_LEDS      = 0xed;    // set keyboard leds
	static const uint8_t KBD_CMD_ECHO          = 0xee;    // echo
	static const uint8_t KBD_CMD_GET_ID        = 0xf2;    // get keyboard ID
	static const uint8_t KBD_CMD_SET_RATE      = 0xf3;    // Set typematic rate
	static const uint8_t KBD_CMD_ENABLE        = 0xf4;    // Enable scanning
	static const uint8_t KBD_CMD_RESET_DISABLE = 0xf5;    // reset and disable scanning
	static const uint8_t KBD_CMD_RESET_ENABLE  = 0xf6;    // reset and enable scanning
	static const uint8_t KBD_CMD_RESET         = 0xff;    // reset
	static const uint8_t KBD_CMD_RESEND        = 0xfe;    // resend last byte

	// Status of keyboard LEDs
	static const uint8_t KBD_LED_SCROLL_LOCK   = 0x01;    // scroll lock (1=on, 0=off)
	static const uint8_t KBD_LED_NUM_LOCK      = 0x02;    // Num lock (1=on, 0=off)
	static const uint8_t KBD_LED_CAPS_LOCK     = 0x04;    // Caps lock (1=on, 0=off)

	// Keyboard command results
	static const uint8_t KBD_RET_ACK           = 0xfa;    // keyboard command acknowledge
	static const uint8_t KBD_RET_NACK          = 0xfe;    // keyboard command acknowledge with error
	static const uint8_t KBD_RET_PWR_ON_RESET  = 0xaa;    // Power on reset

	// Aux/Mouse microcontroller commands (port 60h)
	static const uint8_t AUX_CMD_SET_SCALING11   = 0xe6;   // set scaling 1:1
	static const uint8_t AUX_CMD_SET_SCALING21   = 0xe7;   // set scaling 2:1
	static const uint8_t AUX_CMD_SET_RESOLUTION  = 0xe8;   // set resolution
	static const uint8_t AUX_CMD_GET_STATUS      = 0xe9;    // get aux/mouse status
	static const uint8_t AUX_CMD_READ_DATA       = 0xeb;    // read data
	static const uint8_t AUX_CMD_GET_ID          = 0xf2;    // get aux/mouse ID
	static const uint8_t AUX_CMD_SET_SAMPLE_RATE = 0xf3;  // set sample rate
	static const uint8_t AUX_CMD_ENABLE          = 0xf4;    // enable aux
	static const uint8_t AUX_CMD_DISABLE         = 0xf5;    // disable aux
	static const uint8_t AUX_CMD_SET_DEFAULTS    = 0xf6;    // set defaults
	static const uint8_t AUX_CMD_RESET           = 0xff;    // reset
	static const uint8_t AUX_CMD_RESEND          = 0xfe;    // resend last packet
	static const uint8_t AUX_CMD_SET_REMOTE_MODE = 0xf0;  // enter remote mode
	static const uint8_t AUX_CMD_SET_WRAP_MODE   = 0xee;    // enter wrap mode
	static const uint8_t AUX_CMD_RESET_WRAP_MODE = 0xec;  // leave wrap mode
	static const uint8_t AUX_CMD_SET_STREAM_MODE = 0xea;  // enter stream mode
	
	// Aux/Mouse command results
	static const uint8_t AUX_RET_ACK           = 0xfa;    // Aux/Mouse command acknowledge
	static const uint8_t AUX_RET_NACK          = 0xfe;    // Aux/Mouse command acknowledge with error
	static const uint8_t AUX_RET_ERROR         = 0xfc;    // Aux/Mouse error
	static const uint8_t AUX_RET_PWR_ON_RESET  = 0xaa;    // Power on reset (self test ok)

	// Aux/Mouse status register bits
	static const uint8_t AUX_STR_MODE          = 0x40;    // Aux mode (0=stream, 1=remote)
	static const uint8_t AUX_STR_ENABLE        = 0x20;    // Aux active (0=disabled, 1=enabled)
	static const uint8_t AUX_STR_SCALING       = 0x10;    // Aux scaling (0=1:1 1=2:1)
	static const uint8_t AUX_STR_LBUTTON       = 0x04;    // left mouse button status (0=released, 1=pressed)
	static const uint8_t AUX_STR_MBUTTON       = 0x02;    // middle mouse button status (0=released, 1=pressed)
	static const uint8_t AUX_STR_RBUTTON       = 0x01;    // right mouse button status (0=released, 1=pressed)
	
	// Aux/Mouse byte 1 bits
	static const uint8_t AUX_BYTE1_Y_OVERFLOW   = 0x80;
	static const uint8_t AUX_BYTE1_X_OVERFLOW   = 0x40;
	static const uint8_t AUX_BYTE1_Y_SIGN       = 0x20;
	static const uint8_t AUX_BYTE1_X_SIGN       = 0x10;
	static const uint8_t AUX_BYTE1_ALWAYS_1     = 0x08;
	static const uint8_t AUX_BYTE1_MBUTTON      = 0x04;
	static const uint8_t AUX_BYTE1_RBUTTON      = 0x02;
	static const uint8_t AUX_BYTE1_LBUTTON      = 0x01;
	
	static const bool enable_aux = true;

	ServiceImport<Keyboard> keyboard_import;
	ServiceImport<Mouse> mouse_import;
	
	I8042(const char *name, Object *parent = 0);
	virtual ~I8042();
	
	virtual bool EndSetup();
	
	bool WriteIO(isa_address_t addr, const void *buffer, uint32_t size);
	bool ReadIO(isa_address_t addr, void *buffer, uint32_t size);
	virtual void TriggerKbdInterrupt(bool level);
	virtual void TriggerAuxInterrupt(bool level);
	bool CaptureKey();
	void CaptureMouse();
	virtual void Reset();
	virtual void Lock();
	virtual void Unlock();
protected:
	// PS/2 keyboard
	double typematic_rate;  // Aka key per second
	double typematic_delay; // Aka key repeat delay
	double speed_boost;     // speed boost factor
	
	// AUX / PS/2 MOUSE
	uint8_t aux_status;            // Aux status byte
	uint8_t aux_log2_resolution;   // log2(resolution) where resolution is either 1, 2, 4, or 8 count/mm
	unsigned int aux_sample_rate;        // Aka mouse sample per second
	bool aux_wrap;                 // wrap mode
	
	// the kernel logger
	unisim::kernel::logger::Logger logger;
	bool verbose;

	// Bus frequencies
	unsigned int isa_bus_frequency;
	unsigned int fsb_frequency;
private:
	// Keyboard IDs
	uint8_t kbd_id[2];    // Keyboard ID

	// Aux IDs
	uint8_t aux_id[1];    // Aux ID

	uint8_t status; // read-only, port 64h (on-board i8042 microcontroller)
	uint8_t control; // on-board i8042 keyboard microcontroller control register

	// internals
	struct
	{
		bool pending;
		uint8_t cmd;
	} i8042_command; // pending i8042 command waiting additional bytes

	struct
	{
		bool pending;
		uint8_t cmd;
	} kbd_command; // pending keyboard command waiting additional bytes

	struct
	{
		bool pending;
		uint8_t cmd;
	} aux_command; // pending Aux command waiting additional bytes

	struct
	{
		bool caps_lock;
		bool scroll_lock;
		bool num_lock;
	} kbd_leds; // keyboard LEDs status

	list<uint8_t> kbd_out;
	list<uint8_t> aux_out;
	bool kbd_irq_level;
	bool aux_irq_level;
	bool kbd_scanning;

	// Translation of key # to PS/2 raw set 2 scancodes
	vector<uint8_t> key_num_down_to_ps2_raw_set2[256];
	vector<uint8_t> key_num_up_to_ps2_raw_set2[256];

	// Whether key # should be repeated after typematic delay
	bool key_num_repeat[256];

	unisim::service::interfaces::Keyboard::KeyAction last_key_action;
	uint8_t kbd_last_byte;
	unisim::service::interfaces::Mouse::MouseState mouse_state;
	uint8_t aux_packet[3];

	Parameter<unsigned int> param_isa_bus_frequency;
	Parameter<unsigned int> param_fsb_frequency;
	Parameter<double> param_typematic_rate;
	Parameter<double> param_typematic_delay;
	Parameter<double> param_speed_boost;
	Parameter<bool> param_verbose;

	void KbdReset();
	void AuxReset();
	void WriteData(uint8_t data);     // Write on port 60h
	void ReadData(uint8_t& data);     // Read on port 60h
	void ReadStatus(uint8_t& value);   // Read on port 64h
	void WriteCommand(uint8_t& cmd); // Write on port 64h
	void WriteControl(uint8_t value);
	void WriteKbd(uint8_t data);
	void WriteAux(uint8_t data);
	bool HasKbdData();
	bool HasAuxData();
	void KbdDequeue(uint8_t& data);
	void AuxDequeue(uint8_t& data);
	void KbdEnqueue(uint8_t data);
	void KbdResend();
	void AuxEnqueue(uint8_t data);
	void EnqueueScancodes(const vector<uint8_t>& scancodes);
	void AuxResendPacket();
	void AuxSendPacket(bool& overflow);
	void UpdateStatus();
	void UpdateIRQ();
	void KbdResetQueue();
	void SetTypematicRate(double rate);
	void SetTypematicDelay(double delay);
	bool SetAuxSampleRate(unsigned int rate);
	bool SetAuxResolution(unsigned int log2_resolution);
};

} // end of namespace i8042
} // end of namespace isa
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
