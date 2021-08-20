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

#include <unisim/component/cxx/isa/i8042/i8042.hh>
#include <cstring>

namespace unisim {
namespace component {
namespace cxx {
namespace isa {
namespace i8042 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using std::endl;
using std::hex;
using std::dec;

I8042::I8042(const char *name, Object *parent)
	: Object(name, parent, "i8042 PS/2 keyboard/mouse controller")
	, Client<Keyboard>(name, parent)
	, Client<Mouse>(name, parent)
	, keyboard_import("keyboard-import", this)
	, mouse_import("mouse-import", this)
	, typematic_rate(30.0)
	, typematic_delay(0.250)
	, speed_boost(30.0)
	, aux_status(0)
	, aux_log2_resolution(2)
	, aux_sample_rate(100)
	, aux_wrap(false)
	, logger(*this)
	, verbose(false)
	, isa_bus_frequency(8)
	, fsb_frequency(0)
	, status(0)
	, control(0)
	, kbd_irq_level(false)
	, aux_irq_level(false)
	, kbd_scanning(false)
	, param_isa_bus_frequency("isa-bus-frequency", this, isa_bus_frequency, "ISA bus frequency in Mhz")
	, param_fsb_frequency("fsb-frequency", this, fsb_frequency, "front side bus frequency in Mhz")
	, param_typematic_rate("typematic-rate", this, typematic_rate, "typematic rate (key strokes per second)")
	, param_typematic_delay("typematic-delay", this, typematic_delay, "typematic delay (key repeat delay in seconds)")
	, param_speed_boost("speed-boost", this, speed_boost, "speed-boost factor")
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
{
	param_fsb_frequency.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_isa_bus_frequency.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_typematic_rate.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_typematic_delay.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_speed_boost.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	// PS/2 set 2 keyboard ID
	kbd_id[0] = 0xab;
	kbd_id[1] = 0x83;

	// PS/2 mouse ID
	aux_id[0] = 0x00;
	
	memset(aux_packet, 0, sizeof(aux_packet));

	// Do not repeat key # by default
	memset(key_num_repeat, 0, sizeof(key_num_repeat));

	// KEY #1
	key_num_down_to_ps2_raw_set2[1].push_back(0x0e);
	key_num_up_to_ps2_raw_set2[1].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[1].push_back(0x0e);
	key_num_repeat[1] = true;

	// KEY #2
	key_num_down_to_ps2_raw_set2[2].push_back(0x16);
	key_num_up_to_ps2_raw_set2[2].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[2].push_back(0x16);
	key_num_repeat[2] = true;

	// KEY #3
	key_num_down_to_ps2_raw_set2[3].push_back(0x1e);
	key_num_up_to_ps2_raw_set2[3].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[3].push_back(0x1e);
	key_num_repeat[3] = true;

	// KEY #4
	key_num_down_to_ps2_raw_set2[4].push_back(0x26);
	key_num_up_to_ps2_raw_set2[4].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[4].push_back(0x26);
	key_num_repeat[4] = true;

	// KEY #5
	key_num_down_to_ps2_raw_set2[5].push_back(0x25);
	key_num_up_to_ps2_raw_set2[5].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[5].push_back(0x25);
	key_num_repeat[5] = true;

	// KEY #6
	key_num_down_to_ps2_raw_set2[6].push_back(0x2e);
	key_num_up_to_ps2_raw_set2[6].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[6].push_back(0x2e);
	key_num_repeat[6] = true;

	// KEY #7
	key_num_down_to_ps2_raw_set2[7].push_back(0x36);
	key_num_up_to_ps2_raw_set2[7].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[7].push_back(0x36);
	key_num_repeat[7] = true;

	// KEY #8
	key_num_down_to_ps2_raw_set2[8].push_back(0x3d);
	key_num_up_to_ps2_raw_set2[8].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[8].push_back(0x3d);
	key_num_repeat[8] = true;

	// KEY #9
	key_num_down_to_ps2_raw_set2[9].push_back(0x3e);
	key_num_up_to_ps2_raw_set2[9].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[9].push_back(0x3e);
	key_num_repeat[9] = true;

	// KEY #10
	key_num_down_to_ps2_raw_set2[10].push_back(0x46);
	key_num_up_to_ps2_raw_set2[10].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[10].push_back(0x46);
	key_num_repeat[10] = true;

	// KEY #11
	key_num_down_to_ps2_raw_set2[11].push_back(0x45);
	key_num_up_to_ps2_raw_set2[11].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[11].push_back(0x45);
	key_num_repeat[11] = true;

	// KEY #12
	key_num_down_to_ps2_raw_set2[12].push_back(0x4e);
	key_num_up_to_ps2_raw_set2[12].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[12].push_back(0x4e);
	key_num_repeat[12] = true;

	// KEY #13
	key_num_down_to_ps2_raw_set2[13].push_back(0x55);
	key_num_up_to_ps2_raw_set2[13].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[13].push_back(0x55);
	key_num_repeat[13] = true;

	// Note: no key #14

	// KEY #15
	key_num_down_to_ps2_raw_set2[15].push_back(0x66);
	key_num_up_to_ps2_raw_set2[15].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[15].push_back(0x66);
	key_num_repeat[15] = true;

	// KEY #16
	key_num_down_to_ps2_raw_set2[16].push_back(0x0d);
	key_num_up_to_ps2_raw_set2[16].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[16].push_back(0x0d);
	key_num_repeat[16] = true;

	// KEY #17
	key_num_down_to_ps2_raw_set2[17].push_back(0x15);
	key_num_up_to_ps2_raw_set2[17].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[17].push_back(0x15);
	key_num_repeat[17] = true;

	// KEY #18
	key_num_down_to_ps2_raw_set2[18].push_back(0x1d);
	key_num_up_to_ps2_raw_set2[18].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[18].push_back(0x1d);
	key_num_repeat[18] = true;

	// KEY #19
	key_num_down_to_ps2_raw_set2[19].push_back(0x24);
	key_num_up_to_ps2_raw_set2[19].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[19].push_back(0x24);
	key_num_repeat[19] = true;

	// KEY #20
	key_num_down_to_ps2_raw_set2[20].push_back(0x2d);
	key_num_up_to_ps2_raw_set2[20].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[20].push_back(0x2d);
	key_num_repeat[20] = true;

	// KEY #21
	key_num_down_to_ps2_raw_set2[21].push_back(0x2c);
	key_num_up_to_ps2_raw_set2[21].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[21].push_back(0x2c);
	key_num_repeat[21] = true;

	// KEY #22
	key_num_down_to_ps2_raw_set2[22].push_back(0x35);
	key_num_up_to_ps2_raw_set2[22].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[22].push_back(0x35);
	key_num_repeat[22] = true;

	// KEY #23
	key_num_down_to_ps2_raw_set2[23].push_back(0x3c);
	key_num_up_to_ps2_raw_set2[23].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[23].push_back(0x3c);
	key_num_repeat[23] = true;

	// KEY #24
	key_num_down_to_ps2_raw_set2[24].push_back(0x43);
	key_num_up_to_ps2_raw_set2[24].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[24].push_back(0x43);
	key_num_repeat[24] = true;

	// KEY #25
	key_num_down_to_ps2_raw_set2[25].push_back(0x44);
	key_num_up_to_ps2_raw_set2[25].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[25].push_back(0x44);
	key_num_repeat[25] = true;

	// KEY #26
	key_num_down_to_ps2_raw_set2[26].push_back(0x4d);
	key_num_up_to_ps2_raw_set2[26].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[26].push_back(0x4d);
	key_num_repeat[26] = true;

	// KEY #27
	key_num_down_to_ps2_raw_set2[27].push_back(0x54);
	key_num_up_to_ps2_raw_set2[27].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[27].push_back(0x54);
	key_num_repeat[27] = true;

	// KEY #28
	key_num_down_to_ps2_raw_set2[28].push_back(0x5b);
	key_num_up_to_ps2_raw_set2[28].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[28].push_back(0x5b);
	key_num_repeat[28] = true;

	// KEY #29
	key_num_down_to_ps2_raw_set2[29].push_back(0x5d);
	key_num_up_to_ps2_raw_set2[29].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[29].push_back(0x5d);
	key_num_repeat[29] = true;

	// KEY #30
	key_num_down_to_ps2_raw_set2[30].push_back(0x58);
	key_num_up_to_ps2_raw_set2[30].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[30].push_back(0x58);

	// KEY #31
	key_num_down_to_ps2_raw_set2[31].push_back(0x1c);
	key_num_up_to_ps2_raw_set2[31].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[31].push_back(0x1c);
	key_num_repeat[31] = true;

	// KEY #32
	key_num_down_to_ps2_raw_set2[32].push_back(0x1b);
	key_num_up_to_ps2_raw_set2[32].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[32].push_back(0x1b);
	key_num_repeat[32] = true;

	// KEY #33
	key_num_down_to_ps2_raw_set2[33].push_back(0x23);
	key_num_up_to_ps2_raw_set2[33].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[33].push_back(0x23);
	key_num_repeat[33] = true;

	// KEY #34
	key_num_down_to_ps2_raw_set2[34].push_back(0x2b);
	key_num_up_to_ps2_raw_set2[34].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[34].push_back(0x2b);
	key_num_repeat[34] = true;

	// KEY #35
	key_num_down_to_ps2_raw_set2[35].push_back(0x34);
	key_num_up_to_ps2_raw_set2[35].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[35].push_back(0x34);
	key_num_repeat[35] = true;

	// KEY #36
	key_num_down_to_ps2_raw_set2[36].push_back(0x33);
	key_num_up_to_ps2_raw_set2[36].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[36].push_back(0x33);
	key_num_repeat[36] = true;

	// KEY #37
	key_num_down_to_ps2_raw_set2[37].push_back(0x3b);
	key_num_up_to_ps2_raw_set2[37].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[37].push_back(0x3b);
	key_num_repeat[37] = true;

	// KEY #38
	key_num_down_to_ps2_raw_set2[38].push_back(0x42);
	key_num_up_to_ps2_raw_set2[38].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[38].push_back(0x42);
	key_num_repeat[38] = true;

	// KEY #39
	key_num_down_to_ps2_raw_set2[39].push_back(0x4b);
	key_num_up_to_ps2_raw_set2[39].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[39].push_back(0x4b);
	key_num_repeat[39] = true;

	// KEY #40
	key_num_down_to_ps2_raw_set2[40].push_back(0x4c);
	key_num_up_to_ps2_raw_set2[40].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[40].push_back(0x4c);
	key_num_repeat[40] = true;

	// KEY #41
	key_num_down_to_ps2_raw_set2[41].push_back(0x52);
	key_num_up_to_ps2_raw_set2[41].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[41].push_back(0x52);
	key_num_repeat[41] = true;

	// KEY #42
	key_num_down_to_ps2_raw_set2[42].push_back(0x5d);
	key_num_up_to_ps2_raw_set2[42].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[42].push_back(0x5d);
	key_num_repeat[42] = true;
	// Note: no key #42

	// KEY #43
	key_num_down_to_ps2_raw_set2[43].push_back(0x5a);
	key_num_up_to_ps2_raw_set2[43].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[43].push_back(0x5a);
	key_num_repeat[43] = true;

	// KEY #44
	key_num_down_to_ps2_raw_set2[44].push_back(0x12);
	key_num_up_to_ps2_raw_set2[44].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[44].push_back(0x12);

	// Note: no key #45

	// KEY #46
	key_num_down_to_ps2_raw_set2[46].push_back(0x1a);
	key_num_up_to_ps2_raw_set2[46].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[46].push_back(0x1a);
	key_num_repeat[46] = true;

	// KEY #47
	key_num_down_to_ps2_raw_set2[47].push_back(0x22);
	key_num_up_to_ps2_raw_set2[47].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[47].push_back(0x22);
	key_num_repeat[47] = true;

	// KEY #48
	key_num_down_to_ps2_raw_set2[48].push_back(0x21);
	key_num_up_to_ps2_raw_set2[48].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[48].push_back(0x21);
	key_num_repeat[48] = true;

	// KEY #49
	key_num_down_to_ps2_raw_set2[49].push_back(0x2a);
	key_num_up_to_ps2_raw_set2[49].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[49].push_back(0x2a);
	key_num_repeat[49] = true;

	// KEY #50
	key_num_down_to_ps2_raw_set2[50].push_back(0x32);
	key_num_up_to_ps2_raw_set2[50].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[50].push_back(0x32);
	key_num_repeat[50] = true;

	// KEY #51
	key_num_down_to_ps2_raw_set2[51].push_back(0x31);
	key_num_up_to_ps2_raw_set2[51].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[51].push_back(0x31);
	key_num_repeat[51] = true;

	// KEY #52
	key_num_down_to_ps2_raw_set2[52].push_back(0x3a);
	key_num_up_to_ps2_raw_set2[52].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[52].push_back(0x3a);
	key_num_repeat[52] = true;

	// KEY #53
	key_num_down_to_ps2_raw_set2[53].push_back(0x41);
	key_num_up_to_ps2_raw_set2[53].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[53].push_back(0x41);
	key_num_repeat[53] = true;

	// KEY #54
	key_num_down_to_ps2_raw_set2[54].push_back(0x49);
	key_num_up_to_ps2_raw_set2[54].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[54].push_back(0x49);
	key_num_repeat[54] = true;

	// KEY #55
	key_num_down_to_ps2_raw_set2[55].push_back(0x4a);
	key_num_up_to_ps2_raw_set2[55].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[55].push_back(0x4a);
	key_num_repeat[55] = true;

	// Note: no key #56

	// KEY #57
	key_num_down_to_ps2_raw_set2[57].push_back(0x59);
	key_num_up_to_ps2_raw_set2[57].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[57].push_back(0x59);

	// KEY #58
	key_num_down_to_ps2_raw_set2[58].push_back(0x14);
	key_num_up_to_ps2_raw_set2[58].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[58].push_back(0x14);

	// Note: no key #59

	// KEY #60
	key_num_down_to_ps2_raw_set2[60].push_back(0x11);
	key_num_up_to_ps2_raw_set2[60].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[60].push_back(0x11);

	// KEY #61
	key_num_down_to_ps2_raw_set2[61].push_back(0x29);
	key_num_up_to_ps2_raw_set2[61].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[61].push_back(0x29);
	key_num_repeat[61] = true;

	// KEY #62
	key_num_down_to_ps2_raw_set2[62].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[62].push_back(0x11);
	key_num_up_to_ps2_raw_set2[62].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[62].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[62].push_back(0x11);

	// Note: no key #63

	// KEY #64
	key_num_down_to_ps2_raw_set2[64].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[64].push_back(0x14);
	key_num_up_to_ps2_raw_set2[64].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[64].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[64].push_back(0x14);

	// Note: no key #65-74

	// KEY #75
	key_num_down_to_ps2_raw_set2[75].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[75].push_back(0x70);
	key_num_up_to_ps2_raw_set2[75].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[75].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[75].push_back(0x70);

	// KEY #76
	key_num_down_to_ps2_raw_set2[76].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[76].push_back(0x71);
	key_num_up_to_ps2_raw_set2[76].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[76].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[76].push_back(0x71);

	// Note: no key #77-78

	// KEY #79
	key_num_down_to_ps2_raw_set2[79].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[79].push_back(0x6b);
	key_num_up_to_ps2_raw_set2[79].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[79].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[79].push_back(0x6b);
	key_num_repeat[79] = true;

	// KEY #80
	key_num_down_to_ps2_raw_set2[80].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[80].push_back(0x6c);
	key_num_up_to_ps2_raw_set2[80].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[80].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[80].push_back(0x6c);

	// KEY #81
	key_num_down_to_ps2_raw_set2[81].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[81].push_back(0x69);
	key_num_up_to_ps2_raw_set2[81].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[81].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[81].push_back(0x69);

	// Note: no key #82

	// KEY #83
	key_num_down_to_ps2_raw_set2[83].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[83].push_back(0x75);
	key_num_up_to_ps2_raw_set2[83].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[83].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[83].push_back(0x75);
	key_num_repeat[83] = true;

	// KEY #84
	key_num_down_to_ps2_raw_set2[84].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[84].push_back(0x72);
	key_num_up_to_ps2_raw_set2[84].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[84].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[84].push_back(0x72);
	key_num_repeat[84] = true;

	// KEY #85
	key_num_down_to_ps2_raw_set2[85].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[85].push_back(0x7d);
	key_num_up_to_ps2_raw_set2[85].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[85].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[85].push_back(0x7d);

	// KEY #86
	key_num_down_to_ps2_raw_set2[86].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[86].push_back(0x7a);
	key_num_up_to_ps2_raw_set2[86].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[86].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[86].push_back(0x7a);

	// Note: no key #87-88

	// KEY #89
	key_num_down_to_ps2_raw_set2[89].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[89].push_back(0x74);
	key_num_up_to_ps2_raw_set2[89].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[89].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[89].push_back(0x74);
	key_num_repeat[89] = true;

	// KEY #90
	key_num_down_to_ps2_raw_set2[90].push_back(0x77);
	key_num_up_to_ps2_raw_set2[90].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[90].push_back(0x77);

	// KEY #91
	key_num_down_to_ps2_raw_set2[91].push_back(0x6c);
	key_num_up_to_ps2_raw_set2[91].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[91].push_back(0x6c);
	key_num_repeat[91] = true;

	// KEY #92
	key_num_down_to_ps2_raw_set2[92].push_back(0x6b);
	key_num_up_to_ps2_raw_set2[92].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[92].push_back(0x6b);
	key_num_repeat[92] = true;

	// KEY #93
	key_num_down_to_ps2_raw_set2[93].push_back(0x69);
	key_num_up_to_ps2_raw_set2[93].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[93].push_back(0x69);
	key_num_repeat[93] = true;

	// Note: no key #94

	// KEY #95
	key_num_down_to_ps2_raw_set2[95].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[95].push_back(0x4a);
	key_num_up_to_ps2_raw_set2[95].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[95].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[95].push_back(0x4a);
	key_num_repeat[95] = true;

	// KEY #96
	key_num_down_to_ps2_raw_set2[96].push_back(0x75);
	key_num_up_to_ps2_raw_set2[96].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[96].push_back(0x75);
	key_num_repeat[96] = true;

	// KEY #97
	key_num_down_to_ps2_raw_set2[97].push_back(0x73);
	key_num_up_to_ps2_raw_set2[97].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[97].push_back(0x73);
	key_num_repeat[97] = true;

	// KEY #98
	key_num_down_to_ps2_raw_set2[98].push_back(0x72);
	key_num_up_to_ps2_raw_set2[98].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[98].push_back(0x72);
	key_num_repeat[98] = true;

	// KEY #99
	key_num_down_to_ps2_raw_set2[99].push_back(0x70);
	key_num_up_to_ps2_raw_set2[99].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[99].push_back(0x70);
	key_num_repeat[99] = true;

	// KEY #100
	key_num_down_to_ps2_raw_set2[100].push_back(0x7c);
	key_num_up_to_ps2_raw_set2[100].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[100].push_back(0x7c);
	key_num_repeat[100] = true;

	// KEY #101
	key_num_down_to_ps2_raw_set2[101].push_back(0x7d);
	key_num_up_to_ps2_raw_set2[101].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[101].push_back(0x7d);
	key_num_repeat[101] = true;

	// KEY #102
	key_num_down_to_ps2_raw_set2[102].push_back(0x74);
	key_num_up_to_ps2_raw_set2[102].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[102].push_back(0x74);
	key_num_repeat[102] = true;

	// KEY #103
	key_num_down_to_ps2_raw_set2[103].push_back(0x7a);
	key_num_up_to_ps2_raw_set2[103].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[103].push_back(0x7a);
	key_num_repeat[103] = true;

	// KEY #104
	key_num_down_to_ps2_raw_set2[104].push_back(0x71);
	key_num_up_to_ps2_raw_set2[104].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[104].push_back(0x71);
	key_num_repeat[104] = true;

	// KEY #105
	key_num_down_to_ps2_raw_set2[105].push_back(0x7b);
	key_num_up_to_ps2_raw_set2[105].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[105].push_back(0x7b);
	key_num_repeat[105] = true;

	// KEY #106
	key_num_down_to_ps2_raw_set2[106].push_back(0x79);
	key_num_up_to_ps2_raw_set2[106].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[106].push_back(0x79);
	key_num_repeat[106] = true;

	// Note: no key #107

	// KEY #108
	key_num_down_to_ps2_raw_set2[108].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[108].push_back(0x5a);
	key_num_up_to_ps2_raw_set2[108].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[108].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[108].push_back(0x5a);
	key_num_repeat[108] = true;

	// Note: no key #109

	// KEY #110
	key_num_down_to_ps2_raw_set2[110].push_back(0x76);
	key_num_up_to_ps2_raw_set2[110].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[110].push_back(0x76);
	key_num_repeat[110] = true;

	// Note: no key #111

	// KEY #112
	key_num_down_to_ps2_raw_set2[112].push_back(0x05);
	key_num_up_to_ps2_raw_set2[112].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[112].push_back(0x05);

	// KEY #113
	key_num_down_to_ps2_raw_set2[113].push_back(0x06);
	key_num_up_to_ps2_raw_set2[113].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[113].push_back(0x06);

	// KEY #114
	key_num_down_to_ps2_raw_set2[114].push_back(0x04);
	key_num_up_to_ps2_raw_set2[114].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[114].push_back(0x04);

	// KEY #115
	key_num_down_to_ps2_raw_set2[115].push_back(0x0c);
	key_num_up_to_ps2_raw_set2[115].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[115].push_back(0x0c);

	// KEY #116
	key_num_down_to_ps2_raw_set2[116].push_back(0x03);
	key_num_up_to_ps2_raw_set2[116].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[116].push_back(0x03);

	// KEY #117
	key_num_down_to_ps2_raw_set2[117].push_back(0x0b);
	key_num_up_to_ps2_raw_set2[117].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[117].push_back(0x0b);

	// KEY #118
	key_num_down_to_ps2_raw_set2[118].push_back(0x83);
	key_num_up_to_ps2_raw_set2[118].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[118].push_back(0x83);

	// KEY #119
	key_num_down_to_ps2_raw_set2[119].push_back(0x0a);
	key_num_up_to_ps2_raw_set2[119].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[119].push_back(0x0a);

	// KEY #120
	key_num_down_to_ps2_raw_set2[120].push_back(0x01);
	key_num_up_to_ps2_raw_set2[120].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[120].push_back(0x01);

	// KEY #121
	key_num_down_to_ps2_raw_set2[121].push_back(0x09);
	key_num_up_to_ps2_raw_set2[121].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[121].push_back(0x09);

	// KEY #122
	key_num_down_to_ps2_raw_set2[122].push_back(0x78);
	key_num_up_to_ps2_raw_set2[122].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[122].push_back(0x78);

	// KEY #123
	key_num_down_to_ps2_raw_set2[123].push_back(0x07);
	key_num_up_to_ps2_raw_set2[123].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[123].push_back(0x07);

	// KEY #124
	key_num_down_to_ps2_raw_set2[124].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[124].push_back(0x12);
	key_num_down_to_ps2_raw_set2[124].push_back(0xe0);
	key_num_down_to_ps2_raw_set2[124].push_back(0x7c);
	key_num_up_to_ps2_raw_set2[124].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[124].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[124].push_back(0x7c);
	key_num_up_to_ps2_raw_set2[124].push_back(0xe0);
	key_num_up_to_ps2_raw_set2[124].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[124].push_back(0x12);

	// KEY #125
	key_num_down_to_ps2_raw_set2[125].push_back(0x46);
	key_num_up_to_ps2_raw_set2[125].push_back(0xf0);
	key_num_up_to_ps2_raw_set2[125].push_back(0x46);

	// KEY #126
	key_num_down_to_ps2_raw_set2[126].push_back(0xe1);
	key_num_down_to_ps2_raw_set2[126].push_back(0x14);
	key_num_down_to_ps2_raw_set2[126].push_back(0x77);
	key_num_down_to_ps2_raw_set2[126].push_back(0xe1);
	key_num_down_to_ps2_raw_set2[126].push_back(0xf0);
	key_num_down_to_ps2_raw_set2[126].push_back(0x14);
	key_num_down_to_ps2_raw_set2[126].push_back(0xf0);
	key_num_down_to_ps2_raw_set2[126].push_back(0x77);
	// no key up scancode

	// Note: no key #127

	I8042::Reset();
}

I8042::~I8042()
{
}

bool I8042::EndSetup()
{
	if(!keyboard_import)
	{
		logger << DebugWarning << "No keyboard service connected" << EndDebugWarning;
	}
	if(!mouse_import)
	{
		logger << DebugWarning << "No mouse service connected" << EndDebugWarning;
	}
	Reset();
	return true;
}

void I8042::KbdReset()
{
	kbd_out.clear();
	kbd_command.pending = false;
	kbd_leds.num_lock = false;
	kbd_leds.scroll_lock = false;
	kbd_leds.caps_lock = false;
	kbd_irq_level = false;
	kbd_scanning = false;
	last_key_action.key_num = 0;
	last_key_action.action = unisim::service::interfaces::Keyboard::KeyAction::KEY_UP;
	kbd_last_byte = 0;
	if(keyboard_import) keyboard_import->ResetKeyboard();
}

void I8042::AuxReset()
{
	aux_out.clear();
	aux_irq_level = false;
	aux_command.pending = false;
	aux_status = 0; // stream mode, disabled, all mouse buttons released
	aux_log2_resolution = 2; // 4 count/mm
	aux_sample_rate = 100; // 100 sample/seconds
	mouse_state.dx = 0;
	mouse_state.dy = 0;
	mouse_state.left_button = false;
	mouse_state.middle_button = false;
	mouse_state.right_button = false;
	aux_wrap = false;
	if(mouse_import) mouse_import->ResetMouse();
}

void I8042::Reset()
{
	KbdReset();
	AuxReset();
	status = I8042_STR_KEYLOCK | I8042_STR_CMDDAT;
	control = I8042_CTR_KBDINT | (enable_aux ? I8042_CTR_AUXINT : 0) | (!enable_aux ? I8042_CTR_AUXDIS : 0);
	i8042_command.pending = false;
}

bool I8042::WriteIO(isa_address_t addr, const void *buffer, uint32_t size)
{
	if(size == 1)
	{
		Lock();
		switch(addr)
		{
			case I8042_DATA_REG:
				// fill-in input buffer
				if(verbose)
				{
					logger << DebugInfo << "Write I/O. port 0x" << hex << addr << " (I8042_DATA_REG), value=0x" << (unsigned int) *(uint8_t *) buffer << dec << EndDebugInfo;
				}
				WriteData(*(uint8_t *) buffer);
				Unlock();
				return true;
			case I8042_COMMAND_REG:
				// fill-in input buffer
				if(verbose)
				{
					logger << DebugInfo << "Write I/O. port 0x" << hex << addr << " (I8042_COMMAND_REG), value=0x" << (unsigned int) *(uint8_t *) buffer << dec << EndDebugInfo;
				}
				WriteCommand(*(uint8_t *) buffer);
				Unlock();
				return true;
		}
		Unlock();
	}
	return false;
}

void I8042::WriteData(uint8_t data)
{
	if(i8042_command.pending)
	{
		switch(i8042_command.cmd)
		{
			case I8042_CMD_CTL_WCTR:
				if(verbose)
				{
					logger << DebugInfo << "Handling write command I8042_CMD_CTL_WCTR" << EndDebugInfo;
				}
				WriteControl(data);
				i8042_command.pending = false;
				break;
			case I8042_CMD_AUX_LOOP:
				if(verbose)
				{
					logger << DebugInfo << "Handling write command I8042_CMD_AUX_LOOP" << EndDebugInfo;
				}
				AuxEnqueue(data);
				i8042_command.pending = false;
				break;
			case I8042_CMD_AUX_WRITE: // passthrough: data is for Aux
				WriteAux(data);
				i8042_command.pending = false;
				break;
		}
	}
	else
	{
		// passthrough: data is for keyboard
		WriteKbd(data);
	}
}

void I8042::WriteKbd(uint8_t data)
{
	if(kbd_command.pending)
	{
		switch(kbd_command.cmd)
		{
			case KBD_CMD_SET_LEDS:
				kbd_leds.scroll_lock = (data & KBD_LED_SCROLL_LOCK) ? true : false;
				kbd_leds.caps_lock = (data & KBD_LED_CAPS_LOCK) ? true : false;
				kbd_leds.num_lock = (data & KBD_LED_NUM_LOCK) ? true : false;

				if(verbose)
				{
					logger << DebugInfo << "Scroll lock LED: " << (kbd_leds.scroll_lock ? "on": "off") << EndDebugInfo;
					logger << DebugInfo << "Num lock LED: " << (kbd_leds.num_lock ? "on": "off") << EndDebugInfo;
					logger << DebugInfo << "Caps lock LED: " << (kbd_leds.caps_lock ? "on": "off") << EndDebugInfo;
				}
				KbdEnqueue(KBD_RET_ACK);
				kbd_command.pending = false;
				break;
				
			case KBD_CMD_SET_RATE:
				SetTypematicDelay((1 + ((data >> 5) & 3)) * 0.250);
				SetTypematicRate(1.0 / ((8 + (data & 7)) * (1 << ((data >> 3) & 3)) * 0.00417));
				KbdEnqueue(KBD_RET_ACK);
				kbd_command.pending = false;
				break;
		}
	}
	else
	{
		switch(data)
		{
			case KBD_CMD_SET_LEDS:
				if(verbose)
				{
					logger << DebugInfo << "Handling keyboard command KBD_CMD_SET_LEDS" << EndDebugInfo;
				}
				kbd_command.pending = true;
				kbd_command.cmd = data;
				KbdEnqueue(KBD_RET_ACK);
				break;
			case KBD_CMD_GET_ID:
				{
					if(verbose)
					{
						logger << DebugInfo << "Handling keyboard command KBD_CMD_GET_ID" << EndDebugInfo;
					}
					KbdEnqueue(KBD_RET_ACK);
					if(verbose)
					{
						logger << DebugInfo << "Indentifying keyboard" << EndDebugInfo;
					}
					unsigned int i;
					for(i = 0; i < sizeof(kbd_id) / sizeof(kbd_id[0]); i++)
					{
						KbdEnqueue(kbd_id[i]);
					}
					break;
				}
			case KBD_CMD_RESET:
				if(verbose)
				{
					logger << DebugInfo << "Keyboard reset" << EndDebugInfo;
				}
				KbdReset();
				// self test is okay
				if(verbose)
				{
					logger << DebugInfo << "Self Test" << EndDebugInfo;
				}
				WriteControl(control | I8042_CTR_SYSFLAG);
				KbdEnqueue(KBD_RET_ACK);
				KbdEnqueue(KBD_RET_PWR_ON_RESET);
				break;
			case KBD_CMD_SET_RATE:
				if(verbose)
				{
					logger << DebugInfo << "Handling keyboard command KBD_CMD_SET_RATE" << EndDebugInfo;
				}
				kbd_command.pending = true;
				kbd_command.cmd = data;
				KbdEnqueue(KBD_RET_ACK);
				break;
			case KBD_CMD_ENABLE:
				if(verbose)
				{
					logger << DebugInfo << "Handling keyboard command KBD_CMD_ENABLE" << EndDebugInfo;
				}
				KbdEnqueue(KBD_RET_ACK);
				kbd_scanning = true;
				break;
			case KBD_CMD_RESET_DISABLE:
				if(verbose)
				{
					logger << DebugInfo << "Handling keyboard command KBD_CMD_RESET_DISABLE" << EndDebugInfo;
				}
				KbdResetQueue();
				KbdEnqueue(KBD_RET_ACK);
				kbd_scanning = false;
				break;
			case KBD_CMD_RESET_ENABLE:
				if(verbose)
				{
					logger << DebugInfo << "Handling keyboard command KBD_CMD_RESET_ENABLE" << EndDebugInfo;
				}
				KbdResetQueue();
				KbdEnqueue(KBD_RET_ACK);
				kbd_scanning = true;
				break;
			case KBD_CMD_RESEND:
				KbdResend();
				break;
			default:
				if(verbose)
				{
					logger << DebugInfo << "Unknown keyboard command 0x" << hex << (unsigned int) data << dec << EndDebugInfo;	
				}
		}
	}
}

void I8042::WriteAux(uint8_t data)
{
	if(aux_command.pending)
	{
		switch(aux_command.cmd)
		{
			case AUX_CMD_SET_RESOLUTION:
				if(SetAuxResolution(data))
					AuxEnqueue(AUX_RET_ACK);
				else
					AuxEnqueue(AUX_RET_NACK);
				aux_command.pending = false;
				break;
			case AUX_CMD_SET_SAMPLE_RATE:
				if(SetAuxSampleRate(data))
					AuxEnqueue(AUX_RET_ACK);
				else
					AuxEnqueue(AUX_RET_NACK);
				aux_command.pending = false;
				break;
		}
	}
	else
	{
		if(aux_wrap && data != AUX_CMD_RESET && data != AUX_CMD_RESET_WRAP_MODE)
		{
			// In wrap mode every byte received by the mouse is sent back to the host
			AuxEnqueue(data);
		}
		else
		{
			switch(data)
			{
				case AUX_CMD_SET_SCALING11:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_SET_SCALING11" << EndDebugInfo;
					}
					aux_status = aux_status & ~AUX_STR_SCALING;
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_SET_SCALING21:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_SET_SCALING21" << EndDebugInfo;
					}
					aux_status = aux_status | AUX_STR_SCALING;
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_SET_RESOLUTION:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_SET_RESOLUTION" << EndDebugInfo;
					}
					aux_command.cmd = data;
					aux_command.pending = true;
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_SET_SAMPLE_RATE:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_SET_SAMPLE_RATE" << EndDebugInfo;
					}
					aux_command.cmd = data;
					aux_command.pending = true;
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_SET_DEFAULTS:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_SET_DEFAULTS" << EndDebugInfo;
					}
					AuxReset();
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_GET_STATUS:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_GET_STATUS" << EndDebugInfo;
					}
					AuxEnqueue(AUX_RET_ACK);
					AuxEnqueue(aux_status);
					AuxEnqueue(aux_log2_resolution);
					AuxEnqueue(aux_sample_rate);
					break;
				case AUX_CMD_ENABLE:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_ENABLE" << EndDebugInfo;
					}
					aux_status = (aux_status | AUX_STR_ENABLE) & ~AUX_STR_MODE;
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_DISABLE:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_DISABLE" << EndDebugInfo;
					}
					aux_status = aux_status & ~AUX_STR_ENABLE & ~AUX_STR_MODE;
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_RESET:
					if(verbose)
					{
						logger << DebugInfo << "Mouse reset" << EndDebugInfo;
					}
					AuxReset();
					// self test is okay
					if(verbose)
					{
						logger << DebugInfo << "Self Test" << EndDebugInfo;
					}
					AuxEnqueue(AUX_RET_ACK);
					AuxEnqueue(AUX_RET_PWR_ON_RESET);
					break;
				case AUX_CMD_GET_ID:
					{
						if(verbose)
						{
							logger << DebugInfo << "Handling Aux command AUX_CMD_GET_ID" << EndDebugInfo;
						}
						AuxEnqueue(AUX_RET_ACK);
						if(verbose)
						{
							logger << DebugInfo << "Indentifying Mouse" << EndDebugInfo;
						}
						unsigned int i;
						for(i = 0; i < sizeof(aux_id) / sizeof(aux_id[0]); i++)
						{
							AuxEnqueue(aux_id[i]);
						}
						break;
					}
					
				case AUX_CMD_READ_DATA:
					{
						bool overflow;
						AuxEnqueue(AUX_RET_ACK);
						AuxSendPacket(overflow);
						break;
					}
				case AUX_CMD_RESEND:
					// Note: no ACK is needed for this command
					AuxResendPacket();
					break;
				case AUX_CMD_SET_REMOTE_MODE:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_SET_REMOTE_MODE" << EndDebugInfo;
					}
					aux_status = aux_status | AUX_STR_MODE;
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_SET_WRAP_MODE:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_SET_WRAP_MODE" << EndDebugInfo;
					}
					aux_wrap = true;
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_RESET_WRAP_MODE:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_SET_WRAP_MODE" << EndDebugInfo;
					}
					aux_wrap = false;
					AuxEnqueue(AUX_RET_ACK);
					break;
				case AUX_CMD_SET_STREAM_MODE:
					if(verbose)
					{
						logger << DebugInfo << "Handling Aux command AUX_CMD_SET_STREAM_MODE" << EndDebugInfo;
					}
					aux_status = aux_status & ~AUX_STR_MODE;
					AuxEnqueue(AUX_RET_ACK);
					break;
				default:
					if(verbose)
					{
						logger << DebugInfo << "Unknown Aux command 0x" << hex << (unsigned int) data << dec << EndDebugInfo;	
					}
			}
		}
	}
}

void I8042::WriteCommand(uint8_t& cmd)
{
	switch(cmd)
	{
		case I8042_CMD_CTL_RCTR:
			if(verbose)
			{
				logger << DebugInfo << "Handling command I8042_CMD_CTL_RCTR" << EndDebugInfo;
			}
			KbdEnqueue(control);
			break;
		case I8042_CMD_CTL_SELF_TEST:
			if(verbose)
			{
				logger << DebugInfo << "Handling command I8042_CMD_CTL_SELF_TEST" << EndDebugInfo;
			}
			// self test is okay
			WriteControl(control | I8042_CTR_SYSFLAG);
			status |= I8042_STR_SYSFLAG;
			KbdEnqueue(I8042_RET_CTL_TEST);
			break;
		case I8042_CMD_CTL_WCTR:
			if(verbose)
			{
				logger << DebugInfo << "Handling command I8042_CMD_CTL_WCTR" << EndDebugInfo;
			}
			i8042_command.pending = true;
			i8042_command.cmd = cmd;
			break;
		case I8042_CMD_AUX_LOOP:
			if(!enable_aux) break;
			if(verbose)
			{
				logger << DebugInfo << "Handling command I8042_CMD_AUX_LOOP" << EndDebugInfo;
			}
			i8042_command.pending = true;
			i8042_command.cmd = cmd;
			break;
		case I8042_CMD_CTL_AUX_TEST:
			if(!enable_aux) break;
			if(verbose)
			{
				logger << DebugInfo << "Handling command I8042_CMD_CTL_AUX_TEST" << EndDebugInfo;
			}
			KbdEnqueue(I8042_RET_AUX_TEST_OK);
			break;
		case I8042_CMD_CTL_AUX_DISABLE:
			if(!enable_aux) break;
			if(verbose)
			{
				logger << DebugInfo << "Handling command I8042_CMD_CTL_AUX_DISABLE" << EndDebugInfo;
			}
			WriteControl(control | I8042_CTR_AUXDIS);
			break;
		case I8042_CMD_CTL_AUX_ENABLE:
			if(!enable_aux) break;
			if(verbose)
			{
				logger << DebugInfo << "Handling command I8042_CMD_CTL_AUX_ENABLE" << EndDebugInfo;
			}
			WriteControl(control & ~I8042_CTR_AUXDIS);
			break;
		case I8042_CMD_AUX_WRITE:
			if(!enable_aux) break;
			if(verbose)
			{
				logger << DebugInfo << "handling command I8042_CMD_AUX_WRITE" << EndDebugInfo;
			}
			i8042_command.pending = true;
			i8042_command.cmd = cmd;
			break;
		default:
			if(verbose)
			{
				logger << DebugInfo << "Unknown i8042 command 0x" << hex << (unsigned int) cmd << dec << EndDebugInfo;	
			}
	}
}

void I8042::WriteControl(uint8_t value)
{
	if(verbose)
	{
		logger << DebugInfo << "Writing control (0x" << hex << (unsigned int) value << dec << "):" << EndDebugInfo;
	}
	control = value;
	if(!enable_aux)
	{
		control = control | I8042_CTR_AUXDIS;
		control = control & ~I8042_CTR_AUXINT;
	}
	if(verbose)
	{
		logger << DebugInfo << "  - Keyboard interrupt " << ((control & I8042_CTR_KBDINT) ? "enabled" : "disabled") << EndDebugInfo;
		logger << DebugInfo << "  - Aux interrupt " << ((control & I8042_CTR_AUXINT) ? "enabled" : "disabled") << EndDebugInfo;
		logger << DebugInfo << "  - Self test " << ((control & I8042_CTR_SYSFLAG) ? "passed" : "failed") << EndDebugInfo;
		logger << DebugInfo << "  - PC/AT inhibit override/ignore keylock " << ((control & I8042_CTR_IGNKEYLOCK) ? "enabled" : "disabled") << EndDebugInfo;
		logger << DebugInfo << "  - Keyboard " << ((control & I8042_CTR_KBDDIS) ? "disabled" : "enabled") << EndDebugInfo;
		logger << DebugInfo << "  - Aux " << ((control & I8042_CTR_AUXDIS) ? "disabled" : "enabled") << EndDebugInfo;
		logger << DebugInfo << "  - IBM PC Compatibility Mode " << ((control & I8042_CTR_XLATE) ? "enabled" : "disabled") << EndDebugInfo;
	}
	UpdateStatus();
	UpdateIRQ();
}

bool I8042::ReadIO(isa_address_t addr, void *buffer, uint32_t size)
{
	if(size == 1)
	{
		Lock();
		switch(addr)
		{
			case I8042_DATA_REG:
				ReadData(*(uint8_t *) buffer);
				if(verbose)
				{
					logger << DebugInfo << "Read I/O. port 0x" << hex << addr << " (I8042_DATA_REG), value=0x" << (unsigned int) *(uint8_t *) buffer << dec << EndDebugInfo;
				}
				UpdateIRQ();
				Unlock();
				return true;
			case I8042_STATUS_REG:
				ReadStatus(*(uint8_t *) buffer);
				if(verbose)
				{
					logger << DebugInfo << "Read I/O. port 0x" << hex << addr << " (I8042_STATUS_REG), value=0x" << (unsigned int) status << dec << "(OBF=" << ((status & I8042_STR_OBF) ? 1:0) << ",AUXOBF=" << ((status & I8042_STR_AUX_OBF) ? 1:0) << ",IBF=" << ((status & I8042_STR_IBF) ? 1:0) << ")" << EndDebugInfo;
				}
				Unlock();
				return true;
		}
		Unlock();
	}
	return false;
}

void I8042::ReadData(uint8_t& data)
{
	if(HasKbdData())
		KbdDequeue(data);
	else if(HasAuxData())
		AuxDequeue(data);
	else data = 0;
}

bool I8042::HasKbdData()
{
	return !kbd_out.empty();
}

void I8042::KbdResetQueue()
{
	kbd_last_byte = 0;
	kbd_out.clear();
	UpdateStatus();
	UpdateIRQ();
}

void I8042::KbdResend()
{
	if(verbose)
	{
		logger << DebugInfo << "Enqueuing 0x" << hex << (unsigned int) kbd_last_byte << dec << " from KBD" << EndDebugInfo;
	}
	kbd_out.push_back(kbd_last_byte);
	UpdateStatus();
	UpdateIRQ();
}

void I8042::KbdEnqueue(uint8_t data)
{
	if(verbose)
	{
		logger << DebugInfo << "Enqueuing 0x" << hex << (unsigned int) data << dec << " from KBD" << EndDebugInfo;
	}
	if(data != KBD_CMD_RESEND) kbd_last_byte = data;
	kbd_out.push_back(data);
	UpdateStatus();
	UpdateIRQ();
}

void I8042::KbdDequeue(uint8_t& data)
{
	if(!kbd_out.empty())
	{
		data = kbd_out.front();
		kbd_out.pop_front();

		if(verbose)
		{
			logger << DebugInfo << "Dequeuing KBD data 0x" << hex << (unsigned int) data << dec << EndDebugInfo;
		}
		TriggerKbdInterrupt(false);

		UpdateStatus();
		UpdateIRQ();
	}
}

bool I8042::HasAuxData()
{
	return !aux_out.empty();
}

void I8042::AuxEnqueue(uint8_t data)
{
	if(verbose)
	{
		logger << DebugInfo << "Enqueuing 0x" << hex << (unsigned int) data << dec << " from AUX" << EndDebugInfo;
	}
	aux_out.push_back(data);
	UpdateStatus();
	UpdateIRQ();
}

void I8042::AuxDequeue(uint8_t& data)
{
	if(!aux_out.empty())
	{
		data = aux_out.front();
		aux_out.pop_front();

		if(verbose)
		{
			logger << DebugInfo << "Dequeuing AUX data 0x" << hex << (unsigned int) data << dec << EndDebugInfo;
		}
		TriggerAuxInterrupt(false);

		UpdateStatus();
		UpdateIRQ();
	}
}

void I8042::ReadStatus(uint8_t& value)
{
	value = status;
}

void I8042::UpdateStatus()
{
	status = (HasKbdData() || HasAuxData()) ? status | I8042_STR_OBF : status & ~I8042_STR_OBF;
	status = HasAuxData() ? status | I8042_STR_AUX_OBF : status & ~I8042_STR_AUX_OBF;
	// update sysflag bit in the status register
	status = (control & I8042_CTR_SYSFLAG) ? status | I8042_STR_SYSFLAG : status & ~I8042_STR_SYSFLAG;
}

void I8042::UpdateIRQ()
{
	bool kbd_irq_level = false;
	bool aux_irq_level = false;

	if(HasKbdData() && (control & I8042_CTR_KBDINT))
	{
		kbd_irq_level = true;
	}
	else
	{
		if(HasAuxData() && (control & I8042_CTR_AUXINT))
			aux_irq_level = true;
	}

	TriggerKbdInterrupt(kbd_irq_level);
	TriggerAuxInterrupt(aux_irq_level);
}

void I8042::EnqueueScancodes(const vector<uint8_t>& scancodes)
{
	if(!scancodes.empty())
	{
		unsigned int n = scancodes.size();
		unsigned int i;
		for(i = 0; i < n; i++)
		{
			uint8_t scancode = scancodes[i];
			// fill-in output buffer with the keyboard scancode
			KbdEnqueue(scancode);
		}
	}
}

bool I8042::CaptureKey()
{
	bool ret = false;
	Lock();
	if(kbd_scanning && !i8042_command.pending && !kbd_command.pending && !(control & I8042_CTR_KBDDIS) && !HasKbdData())
	{
		unisim::service::interfaces::Keyboard::KeyAction key_action;
		// get a scancode from the keyboard
		if(keyboard_import && keyboard_import->GetKeyAction(key_action))
		{
			vector<uint8_t>& scancodes = (key_action.action == unisim::service::interfaces::Keyboard::KeyAction::KEY_UP) ? key_num_up_to_ps2_raw_set2[key_action.key_num] : key_num_down_to_ps2_raw_set2[key_action.key_num];

			EnqueueScancodes(scancodes);

			last_key_action = key_action;
			ret = (key_action.action == unisim::service::interfaces::Keyboard::KeyAction::KEY_DOWN);
		}
	}
	Unlock();
	return ret;
}

void I8042::AuxResendPacket()
{
	AuxEnqueue(aux_packet[0]);
	AuxEnqueue(aux_packet[1]);
	AuxEnqueue(aux_packet[2]);
}

void I8042::AuxSendPacket(bool& overflow)
{
	aux_packet[0] = AUX_BYTE1_ALWAYS_1;
	aux_packet[1] = 0;
	aux_packet[2] = 0;
	
	int dx9;
	int dy9;
	
	overflow = false;
	
	if(mouse_state.dx <= -256)
	{
		// overflow on X
		aux_packet[0] = aux_packet[0] | AUX_BYTE1_X_OVERFLOW;
		overflow = true;
		dx9 = -255;
	}
	else if(mouse_state.dx >= 256)
	{
		// overflow on X
		aux_packet[0] = aux_packet[0] | AUX_BYTE1_X_OVERFLOW;
		overflow = true;
		dx9 = +255;
	}
	else
	{
		dx9 = mouse_state.dx;
	}

	if(mouse_state.dy <= -256)
	{
		// overflow on Y
		aux_packet[0] = aux_packet[0] | AUX_BYTE1_Y_OVERFLOW;
		overflow = true;
		dy9 = -255;
	}
	else if(mouse_state.dy >= 256)
	{
		// overflow on Y
		aux_packet[0] = aux_packet[0] | AUX_BYTE1_Y_OVERFLOW;
		overflow = true;
		dy9 = +255;
	}
	else
	{
		dy9 = mouse_state.dy;
	}

	mouse_state.dx -= dx9;
	mouse_state.dy -= dy9;
	aux_packet[0] = aux_packet[0] | ((dx9 < 0) ? AUX_BYTE1_X_SIGN : 0);
	aux_packet[0] = aux_packet[0] | ((dy9 < 0) ? AUX_BYTE1_Y_SIGN : 0);

	aux_packet[1] = dx9 & 0xff;
	aux_packet[2] = dy9 & 0xff;
	
	if(mouse_state.left_button) aux_packet[0] = aux_packet[0] | AUX_BYTE1_LBUTTON;
	if(mouse_state.middle_button) aux_packet[0] = aux_packet[0] | AUX_BYTE1_MBUTTON;
	if(mouse_state.right_button) aux_packet[0] = aux_packet[0] | AUX_BYTE1_RBUTTON;
	AuxEnqueue(aux_packet[0]);
	AuxEnqueue(aux_packet[1]);
	AuxEnqueue(aux_packet[2]);
}

void I8042::CaptureMouse()
{
	Lock();
	if((aux_status & AUX_STR_ENABLE) && !i8042_command.pending && !aux_command.pending && !(control & I8042_CTR_AUXDIS) && !HasAuxData())
	{
		// update the state of the mouse
		if(mouse_import && mouse_import->GetMouseState(mouse_state))
		{
			bool overflow;
			
			do
			{
				AuxSendPacket(overflow);
			} while(overflow);
		}
	}
	Unlock();
}

void I8042::TriggerKbdInterrupt(bool level)
{
	// this should be implemented elsewhere as it is a virtual method
}

void I8042::TriggerAuxInterrupt(bool level)
{
	// this should be implemented elsewhere as it is a virtual method
}

void I8042::Lock()
{
	// this should be implemented elsewhere as it is a virtual method
}

void I8042::Unlock()
{
	// this should be implemented elsewhere as it is a virtual method
}

void I8042::SetTypematicRate(double rate)
{
	if(verbose)
	{
		logger << DebugInfo << "Setting typematic rate to " << rate << " scancodes/second" << EndDebugInfo;
	}
	typematic_rate = rate;
	keyboard_import->SetTypematicDelay(0, (unsigned int)((1.0 / typematic_rate) * 1000.0)); // in ms
}

void I8042::SetTypematicDelay(double delay)
{
	if(verbose)
	{
		logger << DebugInfo << "Setting typematic delay to " << delay << " seconds" << EndDebugInfo;
	}
	typematic_delay = delay;
	keyboard_import->SetTypematicDelay((unsigned int)(typematic_delay * 1000.0), 0);
}

bool I8042::SetAuxSampleRate(unsigned int rate)
{
	if(verbose)
	{
		logger << DebugInfo << "Setting AUX sample rate to " << rate << " samples/second" << EndDebugInfo;
	}
	
	if(rate != 10 && rate != 20 && rate != 40 && rate != 60 && rate != 80 && rate != 100 && rate != 200)
	{
		if(verbose)
		{
			logger << DebugInfo << "Invalid AUX sample" << EndDebugInfo;
		}
		return false;
	}
	
	aux_sample_rate = rate;
	return true;
}

bool I8042::SetAuxResolution(unsigned int log2_resolution)
{
	if(verbose)
	{
		logger << DebugInfo << "Setting AUX resolution to " << (1 << log2_resolution) << " counts/millimeter" << EndDebugInfo;
	}
	
	if(log2_resolution >= 4)
	{
		if(verbose)
		{
			logger << DebugInfo << "Invalid AUX resolution" << EndDebugInfo;
		}
		return false;
	}
	
	aux_log2_resolution = log2_resolution;
	return true;
}

} // end of namespace i8042
} // end of namespace isa
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
