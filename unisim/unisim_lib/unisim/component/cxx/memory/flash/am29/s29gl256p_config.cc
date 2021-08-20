/*
 *  Copyright (c) 2011,
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
 
#include "unisim/component/cxx/memory/flash/am29/s29gl256p_config.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29 {

//-----------------------------------------------------------------------------------------------------------------------------------
//                                            S29GL256P Configuration
//-----------------------------------------------------------------------------------------------------------------------------------

const unsigned int S29GL256PConfig::PAGE_SIZE;

const char *S29GL256PConfig::DEVICE_NAME = "S29GL256P";

const uint64_t S29GL256PConfig::PROGRAMMING_TIME[S29GL256PConfig::MAX_IO_WIDTH] = { 60000, 60000 }; // 60 us/60 us

// Sector address table
const SECTOR_ADDRESS_RANGE<S29GL256PConfig::ADDRESS> S29GL256PConfig::SECTOR_MAP[S29GL256PConfig::NUM_SECTORS] = {
	{ 0x00000000, 131072 }, // SA0/128 KB
	{ 0x00020000, 131072 }, // SA1/128 KB
	{ 0x00040000, 131072 }, // SA2/128 KB
	{ 0x00060000, 131072 }, // SA3/128 KB
	{ 0x00080000, 131072 }, // SA4/128 KB
	{ 0x000a0000, 131072 }, // SA5/128 KB
	{ 0x000c0000, 131072 }, // SA6/128 KB
	{ 0x000e0000, 131072 }, // SA7/128 KB
	{ 0x00100000, 131072 }, // SA8/128 KB
	{ 0x00120000, 131072 }, // SA9/128 KB
	{ 0x00140000, 131072 }, // SA10/128 KB
	{ 0x00160000, 131072 }, // SA11/128 KB
	{ 0x00180000, 131072 }, // SA12/128 KB
	{ 0x001a0000, 131072 }, // SA13/128 KB
	{ 0x001c0000, 131072 }, // SA14/128 KB
	{ 0x001e0000, 131072 }, // SA15/128 KB
	{ 0x00200000, 131072 }, // SA16/128 KB
	{ 0x00220000, 131072 }, // SA17/128 KB
	{ 0x00240000, 131072 }, // SA18/128 KB
	{ 0x00260000, 131072 }, // SA19/128 KB
	{ 0x00280000, 131072 }, // SA20/128 KB
	{ 0x002a0000, 131072 }, // SA21/128 KB
	{ 0x002c0000, 131072 }, // SA22/128 KB
	{ 0x002e0000, 131072 }, // SA23/128 KB
	{ 0x00300000, 131072 }, // SA24/128 KB
	{ 0x00320000, 131072 }, // SA25/128 KB
	{ 0x00340000, 131072 }, // SA26/128 KB
	{ 0x00360000, 131072 }, // SA27/128 KB
	{ 0x00380000, 131072 }, // SA28/128 KB
	{ 0x003a0000, 131072 }, // SA29/128 KB
	{ 0x003c0000, 131072 }, // SA30/128 KB
	{ 0x003e0000, 131072 }, // SA31/128 KB
	{ 0x00400000, 131072 }, // SA32/128 KB
	{ 0x00420000, 131072 }, // SA33/128 KB
	{ 0x00440000, 131072 }, // SA34/128 KB
	{ 0x00460000, 131072 }, // SA35/128 KB
	{ 0x00480000, 131072 }, // SA36/128 KB
	{ 0x004a0000, 131072 }, // SA37/128 KB
	{ 0x004c0000, 131072 }, // SA38/128 KB
	{ 0x004e0000, 131072 }, // SA39/128 KB
	{ 0x00500000, 131072 }, // SA40/128 KB
	{ 0x00520000, 131072 }, // SA41/128 KB
	{ 0x00540000, 131072 }, // SA42/128 KB
	{ 0x00560000, 131072 }, // SA43/128 KB
	{ 0x00580000, 131072 }, // SA44/128 KB
	{ 0x005a0000, 131072 }, // SA45/128 KB
	{ 0x005c0000, 131072 }, // SA46/128 KB
	{ 0x005e0000, 131072 }, // SA47/128 KB
	{ 0x00600000, 131072 }, // SA48/128 KB
	{ 0x00620000, 131072 }, // SA49/128 KB
	{ 0x00640000, 131072 }, // SA50/128 KB
	{ 0x00660000, 131072 }, // SA51/128 KB
	{ 0x00680000, 131072 }, // SA52/128 KB
	{ 0x006a0000, 131072 }, // SA53/128 KB
	{ 0x006c0000, 131072 }, // SA54/128 KB
	{ 0x006e0000, 131072 }, // SA55/128 KB
	{ 0x00700000, 131072 }, // SA56/128 KB
	{ 0x00720000, 131072 }, // SA57/128 KB
	{ 0x00740000, 131072 }, // SA58/128 KB
	{ 0x00760000, 131072 }, // SA59/128 KB
	{ 0x00780000, 131072 }, // SA60/128 KB
	{ 0x007a0000, 131072 }, // SA61/128 KB
	{ 0x007c0000, 131072 }, // SA62/128 KB
	{ 0x007e0000, 131072 }, // SA63/128 KB
	{ 0x00800000, 131072 }, // SA64/128 KB
	{ 0x00820000, 131072 }, // SA65/128 KB
	{ 0x00840000, 131072 }, // SA66/128 KB
	{ 0x00860000, 131072 }, // SA67/128 KB
	{ 0x00880000, 131072 }, // SA68/128 KB
	{ 0x008a0000, 131072 }, // SA69/128 KB
	{ 0x008c0000, 131072 }, // SA70/128 KB
	{ 0x008e0000, 131072 }, // SA71/128 KB
	{ 0x00900000, 131072 }, // SA72/128 KB
	{ 0x00920000, 131072 }, // SA73/128 KB
	{ 0x00940000, 131072 }, // SA74/128 KB
	{ 0x00960000, 131072 }, // SA75/128 KB
	{ 0x00980000, 131072 }, // SA76/128 KB
	{ 0x009a0000, 131072 }, // SA77/128 KB
	{ 0x009c0000, 131072 }, // SA78/128 KB
	{ 0x009e0000, 131072 }, // SA79/128 KB
	{ 0x00a00000, 131072 }, // SA80/128 KB
	{ 0x00a20000, 131072 }, // SA81/128 KB
	{ 0x00a40000, 131072 }, // SA82/128 KB
	{ 0x00a60000, 131072 }, // SA83/128 KB
	{ 0x00a80000, 131072 }, // SA84/128 KB
	{ 0x00aa0000, 131072 }, // SA85/128 KB
	{ 0x00ac0000, 131072 }, // SA86/128 KB
	{ 0x00ae0000, 131072 }, // SA87/128 KB
	{ 0x00b00000, 131072 }, // SA88/128 KB
	{ 0x00b20000, 131072 }, // SA89/128 KB
	{ 0x00b40000, 131072 }, // SA90/128 KB
	{ 0x00b60000, 131072 }, // SA91/128 KB
	{ 0x00b80000, 131072 }, // SA92/128 KB
	{ 0x00ba0000, 131072 }, // SA93/128 KB
	{ 0x00bc0000, 131072 }, // SA94/128 KB
	{ 0x00be0000, 131072 }, // SA95/128 KB
	{ 0x00c00000, 131072 }, // SA96/128 KB
	{ 0x00c20000, 131072 }, // SA97/128 KB
	{ 0x00c40000, 131072 }, // SA98/128 KB
	{ 0x00c60000, 131072 }, // SA99/128 KB
	{ 0x00c80000, 131072 }, // SA100/128 KB
	{ 0x00ca0000, 131072 }, // SA101/128 KB
	{ 0x00cc0000, 131072 }, // SA102/128 KB
	{ 0x00ce0000, 131072 }, // SA103/128 KB
	{ 0x00d00000, 131072 }, // SA104/128 KB
	{ 0x00d20000, 131072 }, // SA105/128 KB
	{ 0x00d40000, 131072 }, // SA106/128 KB
	{ 0x00d60000, 131072 }, // SA107/128 KB
	{ 0x00d80000, 131072 }, // SA108/128 KB
	{ 0x00da0000, 131072 }, // SA109/128 KB
	{ 0x00dc0000, 131072 }, // SA110/128 KB
	{ 0x00de0000, 131072 }, // SA111/128 KB
	{ 0x00e00000, 131072 }, // SA112/128 KB
	{ 0x00e20000, 131072 }, // SA113/128 KB
	{ 0x00e40000, 131072 }, // SA114/128 KB
	{ 0x00e60000, 131072 }, // SA115/128 KB
	{ 0x00e80000, 131072 }, // SA116/128 KB
	{ 0x00ea0000, 131072 }, // SA117/128 KB
	{ 0x00ec0000, 131072 }, // SA118/128 KB
	{ 0x00ee0000, 131072 }, // SA119/128 KB
	{ 0x00f00000, 131072 }, // SA120/128 KB
	{ 0x00f20000, 131072 }, // SA121/128 KB
	{ 0x00f40000, 131072 }, // SA122/128 KB
	{ 0x00f60000, 131072 }, // SA123/128 KB
	{ 0x00f80000, 131072 }, // SA124/128 KB
	{ 0x00fa0000, 131072 }, // SA125/128 KB
	{ 0x00fc0000, 131072 }, // SA126/128 KB
	{ 0x00fe0000, 131072 }, // SA127/128 KB
	{ 0x01000000, 131072 }, // SA128/128 KB
	{ 0x01020000, 131072 }, // SA129/128 KB
	{ 0x01040000, 131072 }, // SA130/128 KB
	{ 0x01060000, 131072 }, // SA131/128 KB
	{ 0x01080000, 131072 }, // SA132/128 KB
	{ 0x010a0000, 131072 }, // SA133/128 KB
	{ 0x010c0000, 131072 }, // SA134/128 KB
	{ 0x010e0000, 131072 }, // SA135/128 KB
	{ 0x01100000, 131072 }, // SA136/128 KB
	{ 0x01120000, 131072 }, // SA137/128 KB
	{ 0x01140000, 131072 }, // SA138/128 KB
	{ 0x01160000, 131072 }, // SA139/128 KB
	{ 0x01180000, 131072 }, // SA140/128 KB
	{ 0x011a0000, 131072 }, // SA141/128 KB
	{ 0x011c0000, 131072 }, // SA142/128 KB
	{ 0x011e0000, 131072 }, // SA143/128 KB
	{ 0x01200000, 131072 }, // SA144/128 KB
	{ 0x01220000, 131072 }, // SA145/128 KB
	{ 0x01240000, 131072 }, // SA146/128 KB
	{ 0x01260000, 131072 }, // SA147/128 KB
	{ 0x01280000, 131072 }, // SA148/128 KB
	{ 0x012a0000, 131072 }, // SA149/128 KB
	{ 0x012c0000, 131072 }, // SA150/128 KB
	{ 0x012e0000, 131072 }, // SA151/128 KB
	{ 0x01300000, 131072 }, // SA152/128 KB
	{ 0x01320000, 131072 }, // SA153/128 KB
	{ 0x01340000, 131072 }, // SA154/128 KB
	{ 0x01360000, 131072 }, // SA155/128 KB
	{ 0x01380000, 131072 }, // SA156/128 KB
	{ 0x013a0000, 131072 }, // SA157/128 KB
	{ 0x013c0000, 131072 }, // SA158/128 KB
	{ 0x013e0000, 131072 }, // SA159/128 KB
	{ 0x01400000, 131072 }, // SA160/128 KB
	{ 0x01420000, 131072 }, // SA161/128 KB
	{ 0x01440000, 131072 }, // SA162/128 KB
	{ 0x01460000, 131072 }, // SA163/128 KB
	{ 0x01480000, 131072 }, // SA164/128 KB
	{ 0x014a0000, 131072 }, // SA165/128 KB
	{ 0x014c0000, 131072 }, // SA166/128 KB
	{ 0x014e0000, 131072 }, // SA167/128 KB
	{ 0x01500000, 131072 }, // SA168/128 KB
	{ 0x01520000, 131072 }, // SA169/128 KB
	{ 0x01540000, 131072 }, // SA170/128 KB
	{ 0x01560000, 131072 }, // SA171/128 KB
	{ 0x01580000, 131072 }, // SA172/128 KB
	{ 0x015a0000, 131072 }, // SA173/128 KB
	{ 0x015c0000, 131072 }, // SA174/128 KB
	{ 0x015e0000, 131072 }, // SA175/128 KB
	{ 0x01600000, 131072 }, // SA176/128 KB
	{ 0x01620000, 131072 }, // SA177/128 KB
	{ 0x01640000, 131072 }, // SA178/128 KB
	{ 0x01660000, 131072 }, // SA179/128 KB
	{ 0x01680000, 131072 }, // SA180/128 KB
	{ 0x016a0000, 131072 }, // SA181/128 KB
	{ 0x016c0000, 131072 }, // SA182/128 KB
	{ 0x016e0000, 131072 }, // SA183/128 KB
	{ 0x01700000, 131072 }, // SA184/128 KB
	{ 0x01720000, 131072 }, // SA185/128 KB
	{ 0x01740000, 131072 }, // SA186/128 KB
	{ 0x01760000, 131072 }, // SA187/128 KB
	{ 0x01780000, 131072 }, // SA188/128 KB
	{ 0x017a0000, 131072 }, // SA189/128 KB
	{ 0x017c0000, 131072 }, // SA190/128 KB
	{ 0x017e0000, 131072 }, // SA191/128 KB
	{ 0x01800000, 131072 }, // SA192/128 KB
	{ 0x01820000, 131072 }, // SA193/128 KB
	{ 0x01840000, 131072 }, // SA194/128 KB
	{ 0x01860000, 131072 }, // SA195/128 KB
	{ 0x01880000, 131072 }, // SA196/128 KB
	{ 0x018a0000, 131072 }, // SA197/128 KB
	{ 0x018c0000, 131072 }, // SA198/128 KB
	{ 0x018e0000, 131072 }, // SA199/128 KB
	{ 0x01900000, 131072 }, // SA200/128 KB
	{ 0x01920000, 131072 }, // SA201/128 KB
	{ 0x01940000, 131072 }, // SA202/128 KB
	{ 0x01960000, 131072 }, // SA203/128 KB
	{ 0x01980000, 131072 }, // SA204/128 KB
	{ 0x019a0000, 131072 }, // SA205/128 KB
	{ 0x019c0000, 131072 }, // SA206/128 KB
	{ 0x019e0000, 131072 }, // SA207/128 KB
	{ 0x01a00000, 131072 }, // SA208/128 KB
	{ 0x01a20000, 131072 }, // SA209/128 KB
	{ 0x01a40000, 131072 }, // SA210/128 KB
	{ 0x01a60000, 131072 }, // SA211/128 KB
	{ 0x01a80000, 131072 }, // SA212/128 KB
	{ 0x01aa0000, 131072 }, // SA213/128 KB
	{ 0x01ac0000, 131072 }, // SA214/128 KB
	{ 0x01ae0000, 131072 }, // SA215/128 KB
	{ 0x01b00000, 131072 }, // SA216/128 KB
	{ 0x01b20000, 131072 }, // SA217/128 KB
	{ 0x01b40000, 131072 }, // SA218/128 KB
	{ 0x01b60000, 131072 }, // SA219/128 KB
	{ 0x01b80000, 131072 }, // SA220/128 KB
	{ 0x01ba0000, 131072 }, // SA221/128 KB
	{ 0x01bc0000, 131072 }, // SA222/128 KB
	{ 0x01be0000, 131072 }, // SA223/128 KB
	{ 0x01c00000, 131072 }, // SA224/128 KB
	{ 0x01c20000, 131072 }, // SA225/128 KB
	{ 0x01c40000, 131072 }, // SA226/128 KB
	{ 0x01c60000, 131072 }, // SA227/128 KB
	{ 0x01c80000, 131072 }, // SA228/128 KB
	{ 0x01ca0000, 131072 }, // SA229/128 KB
	{ 0x01cc0000, 131072 }, // SA230/128 KB
	{ 0x01ce0000, 131072 }, // SA231/128 KB
	{ 0x01d00000, 131072 }, // SA232/128 KB
	{ 0x01d20000, 131072 }, // SA233/128 KB
	{ 0x01d40000, 131072 }, // SA234/128 KB
	{ 0x01d60000, 131072 }, // SA235/128 KB
	{ 0x01d80000, 131072 }, // SA236/128 KB
	{ 0x01da0000, 131072 }, // SA237/128 KB
	{ 0x01dc0000, 131072 }, // SA238/128 KB
	{ 0x01de0000, 131072 }, // SA239/128 KB
	{ 0x01e00000, 131072 }, // SA240/128 KB
	{ 0x01e20000, 131072 }, // SA241/128 KB
	{ 0x01e40000, 131072 }, // SA242/128 KB
	{ 0x01e60000, 131072 }, // SA243/128 KB
	{ 0x01e80000, 131072 }, // SA244/128 KB
	{ 0x01ea0000, 131072 }, // SA245/128 KB
	{ 0x01ec0000, 131072 }, // SA246/128 KB
	{ 0x01ee0000, 131072 }, // SA247/128 KB
	{ 0x01f00000, 131072 }, // SA248/128 KB
	{ 0x01f20000, 131072 }, // SA249/128 KB
	{ 0x01f40000, 131072 }, // SA250/128 KB
	{ 0x01f60000, 131072 }, // SA251/128 KB
	{ 0x01f80000, 131072 }, // SA252/128 KB
	{ 0x01fa0000, 131072 }, // SA253/128 KB
	{ 0x01fc0000, 131072 }, // SA254/128 KB
	{ 0x01fe0000, 131072 }  // SA255/128 KB
};

const uint8_t S29GL256PConfig::MANUFACTURER_ID[MAX_IO_WIDTH] = { 0x01, 0x00 };
const S29GL256PConfig::ADDRESS S29GL256PConfig::DEVICE_ID_ADDR[DEVICE_ID_LENGTH] = { 0x02, 0x1c, 0x1e };
const uint8_t S29GL256PConfig::DEVICE_ID[DEVICE_ID_LENGTH][MAX_IO_WIDTH] = { { 0x7e, 0x22 }, { 0x22, 0x22 }, { 0x01, 0x22 } };
const uint8_t S29GL256PConfig::PROTECTED[MAX_IO_WIDTH] = { 0x01, 0x00 };
const uint8_t S29GL256PConfig::UNPROTECTED[MAX_IO_WIDTH] = { 0x00, 0x00 };

// CFI queries
const S29GL256PConfig::CFI_QUERY S29GL256PConfig::CFI_QUERIES[NUM_CFI_QUERIES] = {
	// CFI Query Indentification String
	{ 0x20, { 'Q', 0 } },    // Query Unique ASCII string "QRY"
	{ 0x22, { 'R', 0 } },    // Query Unique ASCII string "QRY"
	{ 0x24, { 'Y', 0 } },    // Query Unique ASCII string "QRY"
	{ 0x26, { 0x02, 0x00 } },  // Primary OEM Command Set
	{ 0x28, { 0x00, 0x00 } },  // Primary OEM Command Set
	{ 0x2a, { 0x40, 0x00 } },  // Address for Primary Extended Table
	{ 0x2c, { 0x00, 0x00 } },  // Address for Primary Extended Table
	{ 0x2e, { 0x00, 0x00 } },  // Alternate OEM Command Set (00h = none exists)
	{ 0x30, { 0x00, 0x00 } },  // Alternate OEM Command Set (00h = none exists)
	{ 0x32, { 0x00, 0x00 } },  // Address for Alternate OEM Extended Table (00h = none exists)
	{ 0x34, { 0x00, 0x00 } },  // Address for Alternate OEM Extended Table (00h = none exists)
	{ 0x2c, { 0x00, 0x00 } },  // Address for Primary Extended Table
	// System Interface String
	{ 0x36, { 0x27, 0x00 } },  // Vcc Min (write/erase): D7-D4: volt, D3-D0: 100 mV
	{ 0x38, { 0x36, 0x00 } },  // Vcc Max (write/erase): D7-D4: volt, D3-D0: 100 mV
	{ 0x3a, { 0x00, 0x00 } },  // Vpp Min voltage (00h = no Vpp pin present)
	{ 0x3c, { 0x00, 0x00 } },  // Vpp Max voltage (00h = no Vpp pin present)
	{ 0x3e, { 0x06, 0x00 } },  // Typical timeout per single byte/word 2^N us
	{ 0x40, { 0x06, 0x00 } },  // Typical timeout for Min size buffer write 2^N us (00h = not supported)
	{ 0x42, { 0x09, 0x00 } },  // Typical timeout per individual block erase 2^N ms
	{ 0x44, { 0x13, 0x00 } },  // Typical timeout for full chip erase 2^N ms (00h = not suppported)
	{ 0x46, { 0x03, 0x00 } },  // Max timeout for byte/word write 2^N times typical
	{ 0x48, { 0x03, 0x00 } },  // Max timeout for buffer write 2^N times typical
	{ 0x4a, { 0x03, 0x00 } },  // Max timeout per individual block erase 2^N times typical
	{ 0x4c, { 0x02, 0x00 } },  // Max timeout for full chip erase 2^N times typical (00h = not supported)
	// Device Geometry Definition
	{ 0x4e, { 0x19, 0x00 } },  // Device Size = 2^N (i.e. 256 Mb)
	{ 0x50, { 0x02, 0x00 } },  // Flash Device Interface description
	{ 0x52, { 0x00, 0x00 } },  // Flash Device Interface description
	{ 0x54, { 0x06, 0x00 } },  // Max number of byte in multi-byte write = 2^N (00h = not supported)
	{ 0x56, { 0x00, 0x00 } },  // Max number of byte in multi-byte write = 2^N (00h = not supported)
	{ 0x58, { 0x01, 0x00 } },  // Number of Erase Block Regions within device (01h = uniform device, 02h = boot device) 
	{ 0x5a, { 0xff, 0x00 } },  // Erase Block Region 1 Information
	{ 0x5c, { 0x00, 0x00 } },  // Erase Block Region 1 Information
	{ 0x5e, { 0x00, 0x00 } },  // Erase Block Region 1 Information
	{ 0x60, { 0x02, 0x00 } },  // Erase Block Region 1 Information
	{ 0x62, { 0x00, 0x00 } },  // Erase Block Region 2 Information
	{ 0x64, { 0x00, 0x00 } },  // Erase Block Region 2 Information
	{ 0x66, { 0x00, 0x00 } },  // Erase Block Region 2 Information
	{ 0x68, { 0x00, 0x00 } },  // Erase Block Region 2 Information
	{ 0x6a, { 0x00, 0x00 } },  // Erase Block Region 3 Information
	{ 0x6c, { 0x00, 0x00 } },  // Erase Block Region 3 Information
	{ 0x6e, { 0x00, 0x00 } },  // Erase Block Region 3 Information
	{ 0x70, { 0x00, 0x00 } },  // Erase Block Region 3 Information
	{ 0x72, { 0x00, 0x00 } },  // Erase Block Region 4 Information
	{ 0x74, { 0x00, 0x00 } },  // Erase Block Region 4 Information
	{ 0x76, { 0x00, 0x00 } },  // Erase Block Region 4 Information
	{ 0x78, { 0x00, 0x00 } },  // Erase Block Region 4 Information
	// Primary Vendor-Specific Extended Query
	{ 0x80, { 'P', 'P' } },  // Query-unique ASCII string "PRI"
	{ 0x82, { 'R', 'R' } },  // Query-unique ASCII string "PRI"
	{ 0x84, { 'I', 'I' } },  // Query-unique ASCII string "PRI"
	{ 0x86, { 0x31, 0x00 } },  // Major version number, ASCII
	{ 0x88, { 0x33, 0x00 } },  // Minor version number, ASCII
	{ 0x8a, { 0x14, 0x00 } },  // Address Sensitive Unlock (Bits 1-0) 0 = Required, 1 = Not Required, Process Technology (Bits 7-2) 0101b = 90nm MirrorBit 
	{ 0x8c, { 0x02, 0x00 } },  // Erase Suspend 0 = Not supported, 1 = To Read Only, 2 = To Read & Write
	{ 0x8e, { 0x01, 0x00 } },  // Sector Protect 0 = Not supported, X = Number of section in per group
	{ 0x90, { 0x00, 0x00 } },  // Sector Temporary Unprotect 00h = Not Supported, 01 = Supported
	{ 0x92, { 0x08, 0x00 } },  // Sector Protect/Unprotect Scheme 0008h = Advanced Sector Protection
	{ 0x94, { 0x00, 0x00 } },  // Simultaneous Operation 00 = Not Supported, X = Number of Sectors
	{ 0x96, { 0x00, 0x00 } },  // Burst Mode Type 00 = Not Supported, 01 = Supported
	{ 0x98, { 0x02, 0x00 } },  // Page Mode Type 00 = Not Supported, 01 = 4 Word Page, 02 = 8 Word Page
	{ 0x9a, { 0xb5, 0x00 } },  // ACC (Acceleration) Supply Minimum 00h = Not Supported, D7-D4: volt, D4-D0: 100 mV
	{ 0x9c, { 0xc5, 0x00 } },  // ACC (Acceleration) Supply Maximum 00h = Not Supported, D7-D4: volt, D4-D0: 100 mV
	{ 0x9e, { 0x05, 0x00 } },  // WP# Protection 04h = Uniform sector bottom WP# protect, 05h = Uniform sectors top WP # protect
	{ 0xa0, { 0x01, 0x00 } }   // Program Suspend 00h = Not Supported, 01h = Supported
};

// S1-[command,addr,data/action]->S2
const TRANSITION<S29GL256PConfig::ADDRESS, S29GL256PConfig::MAX_IO_WIDTH, S29GL256PConfig::STATE> S29GL256PConfig::FSM[S29GL256PConfig::NUM_TRANSITIONS] = {
	// unlock cycles transitions
	{ ST_I0, CMD_WRITE, false, 0xaaa, false, { 0xaa, 0x00 }, ST_I1, ACT_NOP }, // (I0) -[W,AAA,AA/-]-> (I1)
	{ ST_I0, CMD_WRITE, false, 0xaa, false, { 0x98, 0 }, ST_CFI_QUERY, ACT_NOP }, // (I0)-[W,0x55,0x98/-]->(CFI_QUERY)
	{ ST_I0, CMD_WRITE, true, 0, true, { 0,0 }, ST_I0, ACT_NOP }, // (I0) -[W,*,*/-]->(I0)
	{ ST_I1, CMD_WRITE, false, 0x555, false, { 0x55, 0x00 }, ST_I2, ACT_NOP }, // (I1) -[W,555,55/-]->(I2)
	{ ST_I1, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (I1) -[W,*,*/-]->(I0)
	// commands after the two unclock cycles recognition transitions
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0x90, 0x00 }, ST_AUTOSELECT, ACT_NOP }, // (I2) -[W,AAA,90/-]->(AUTOSELECT)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0xa0, 0x00 }, ST_PROGRAM, ACT_NOP }, // (I2) -[W,AAA,A0/-]->(PROGRAM)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0x20, 0x00 }, ST_UNLOCKED, ACT_NOP }, // (I2) -[W,AAA,20/-]->(UNLOCKED)
	{ ST_I2, CMD_WRITE, false, 0xaaa, false, { 0x80, 0x00 }, ST_ERASE0, ACT_NOP }, // (I2) -[W,AAA,80/-]->(ERASE0)
	{ ST_I2, CMD_WRITE, true, 0, false, { 0x25, 0x00 }, ST_OPENED_PAGE, ACT_OPEN_PAGE }, // (I2) -[W,*,25/OPEN_PAGE]->(WRITE_TO_BUFFER)
	{ ST_I2, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (I2) -[W,*,*/-]->(I0)
	// autoselect transitions
	{ ST_AUTOSELECT, CMD_READ, true, 0, true, { 0, 0 }, ST_AUTOSELECT, ACT_READ_AUTOSELECT }, // (AUTOSELECT) -[R,*,*/READ_AUTOSELECT]->(AUTOSELECT)
	{ ST_AUTOSELECT, CMD_WRITE, true, 0, false, { 0xf0, 0 }, ST_I0, ACT_NOP }, // (AUTOSELECT) -[W,*,F0/-]->(I0)
	// single program transitions
	{ ST_PROGRAM, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_PROGRAM }, // (PROGRAM) -[W,*,*/PROGRAM]->(I0)
	// unlock bypass transitions
	{ ST_UNLOCKED, CMD_WRITE, true, 0, false, { 0xa0, 0x00 }, ST_UNLOCKED_PROGRAM, ACT_NOP }, // (UNLOCKED) -[W,*,A0/-]->(UNLOCKED_PROGRAM)
	{ ST_UNLOCKED, CMD_WRITE, true, 0, false, { 0x80, 0x00 }, ST_UNLOCKED_ERASE, ACT_NOP }, // (UNLOCKED) -[W,*,80/-]->(UNLOCKED_ERASE)
	{ ST_UNLOCKED, CMD_WRITE, true, 0, false, { 0x90, 0x00 }, ST_UNLOCKED_RESET, ACT_NOP }, // (UNLOCKED) -[W,*,90/-]->(UNLOCKED_RESET)
	{ ST_UNLOCKED, CMD_WRITE, true, 0, true, { 0, 0 }, ST_UNLOCKED, ACT_NOP }, // (UNLOCKED) -[W,*,*/-]->(UNLOCKED)
	{ ST_UNLOCKED_PROGRAM, CMD_WRITE, true, 0, true, { 0, 0 }, ST_UNLOCKED, ACT_PROGRAM }, // (UNLOCKED_PROGRAM) -[W,*,*/PROGRAM]->(UNLOCKED)
	{ ST_UNLOCKED_RESET, CMD_WRITE, true, 0, false, { 0, 0 }, ST_I0, ACT_NOP }, // (UNLOCKED_RESET) -[W,*,00/-]->(I0)
	{ ST_UNLOCKED_RESET, CMD_WRITE, true, 0, true, { 0, 0 }, ST_UNLOCKED_RESET, ACT_NOP }, // (UNLOCKED_RESET) -[W,*,*/-]->(UNLOCKED_RESET)
	{ ST_UNLOCKED_RESET, CMD_WRITE, true, 0, false, { 0x90, 0 }, ST_I0, ACT_NOP }, // (UNLOCKED_RESET) -[W,*,90/-]->(I0)
	{ ST_UNLOCKED_ERASE, CMD_WRITE, true, 0, false, { 0x30, 0 }, ST_UNLOCKED, ACT_SECTOR_ERASE }, // (UNLOCKED_ERASE) -[W,*,30/SECTOR_ERASE]->(UNLOCKED)
	{ ST_UNLOCKED_ERASE, CMD_WRITE, true, 0, false, { 0x10, 0 }, ST_UNLOCKED, ACT_CHIP_ERASE }, // (UNLOCKED_ERASE) -[W,*,10/CHIP_ERASE]->(UNLOCKED)
	// single erase transitions
	{ ST_ERASE0, CMD_WRITE, false, 0xaaa, false, { 0xaa, 0x00 }, ST_ERASE1, ACT_NOP }, // (I2) -[W,AAA,AA/-]->(ERASE1)
	{ ST_ERASE1, CMD_WRITE, false, 0x555, false, { 0x55, 0x00 }, ST_ERASE2, ACT_NOP }, // (ERASE1) -[W,555,55/-]->(ERASE2)
	{ ST_ERASE1, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (ERASE1) -[W,*,*/-]->(I0)
	{ ST_ERASE2, CMD_WRITE, false, 0xaaa, false, { 0x10, 0x00 }, ST_I0, ACT_CHIP_ERASE }, // (ERASE2) -[W,AAA,10/CHIP_ERASE]->(I0)
	{ ST_ERASE2, CMD_WRITE, true, 0, false, { 0x30, 0x00 }, ST_I0, ACT_SECTOR_ERASE }, // (ERASE2) -[W,*,30/SECTOR_ERASE]->(I0)
	{ ST_ERASE2, CMD_WRITE, true, 0, true, { 0, 0 }, ST_I0, ACT_NOP }, // (ERASE2) -[W,*,*/-]->(I,0)
	// CFI query transitions
	{ ST_CFI_QUERY, CMD_READ, true, 0, true, { 0, 0 }, ST_CFI_QUERY, ACT_CFI_QUERY }, // (CFI_QUERY)-[W,*,*/CFI_QUERY]->(CFI_QUERY)
	{ ST_CFI_QUERY, CMD_WRITE, true, 0, false, { 0xf0, 0 }, ST_I0, ACT_NOP }, // (CFI_QUERY) -[W,*,F0/-]->(I0)
	// Write to buffer transitions
	{ ST_OPENED_PAGE, CMD_WRITE, true, 0, true, { 0, 0 }, ST_WRITE_TO_BUFFER, ACT_LOAD_WC }, // (OPENED_PAGE)-[W,*,*/LOAD_WC]->(WRITE_TO_BUFFER)
	{ ST_WRITE_TO_BUFFER, CMD_WRITE, true, 0, false, { 0x29, 0x00 }, ST_I0, ACT_PROGRAM_BUFFER_TO_FLASH }, // (WRITE_TO_BUFFER)-[W,*,29/PROGRAM_BUFFER_TO_FLASH]->(I0)
	{ ST_WRITE_TO_BUFFER, CMD_WRITE, true, 0, true, { 0, 0 }, ST_WRITE_TO_BUFFER, ACT_WRITE_TO_BUFFER }, // (WRITE_TO_BUFFER)-[W,*,*/WRITE_TO_BUFFER]->(WRITE_TO_BUFFER)
	{ ST_WRITE_TO_BUFFER_ABORTED, CMD_WRITE, false, 0xaaa, false, { 0xaa, 0x00 }, ST_WRITE_TO_BUFFER_ABORTED, ACT_NOP }, // (WRITE_TO_BUFFER_ABORTED) -[W,AAA,AA/-]-> (WRITE_TO_BUFFER_ABORTED1)
	{ ST_WRITE_TO_BUFFER_ABORTED, CMD_READ, true, 0, true, { 0, 0 }, ST_WRITE_TO_BUFFER_ABORTED, ACT_READ_WRITE_STATUS }, // (WRITE_TO_BUFFER_ABORTED) -[R,*,*/READ_WRITE_STATUS]-> (WRITE_TO_BUFFER_ABORTED)
	{ ST_WRITE_TO_BUFFER_ABORTED1, CMD_WRITE, false, 0x555, false, { 0x55, 0x00 }, ST_WRITE_TO_BUFFER_ABORTED2, ACT_NOP }, // (WRITE_TO_BUFFER_ABORTED1) -[W,555,55/-]-> (WRITE_TO_BUFFER_ABORTED2)
	{ ST_WRITE_TO_BUFFER_ABORTED1, CMD_READ, true, 0, true, { 0, 0 }, ST_WRITE_TO_BUFFER_ABORTED1, ACT_READ_WRITE_STATUS }, // (WRITE_TO_BUFFER_ABORTED1) -[R,*,*/READ_WRITE_STATUS]-> (WRITE_TO_BUFFER_ABORTED1)
	{ ST_WRITE_TO_BUFFER_ABORTED2, CMD_WRITE, false, 0x555, false, { 0xf0, 0x00 }, ST_I0, ACT_NOP }, // (WRITE_TO_BUFFER_ABORT2) -[W,555,F0/-]-> (ST_I0)
	{ ST_WRITE_TO_BUFFER_ABORTED2, CMD_READ, true, 0, true, { 0, 0 }, ST_WRITE_TO_BUFFER_ABORTED2, ACT_READ_WRITE_STATUS }, // (WRITE_TO_BUFFER_ABORTED2) -[R,*,*/READ_WRITE_STATUS]-> (WRITE_TO_BUFFER_ABORTED2)
	// Suspend/Resume transitions
	{ ST_ANY, CMD_WRITE, true, 0, false, { 0xb0, 0x00 }, ST_ANY, ACT_NOP }, // (*) -[W,*,B0/-]-> (*)
	{ ST_ANY, CMD_WRITE, true, 0, false, { 0x30, 0x00 }, ST_ANY, ACT_NOP }, // (*) -[W,*,30/-]-> (*)
	// Fallback transitions
	{ ST_ANY, CMD_WRITE, true, 0, true, { 0,0 }, ST_ANY, ACT_NOP }, // (*) -[W,*,*/-]->(*)
	{ ST_ANY, CMD_READ, true, 0, true, { 0,0 }, ST_ANY, ACT_READ }, // (*) -[R,*,*/READ]->(*)
};

} // end of namespace am29
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
