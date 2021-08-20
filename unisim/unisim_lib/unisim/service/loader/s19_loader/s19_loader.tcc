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

/*
 *  s19_loader.tcc      s19 loader, using UNISIM RAM.
 */


#include <unisim/service/loader/s19_loader/s19_loader.hh>

namespace unisim {
namespace service {
namespace loader {
namespace s19_loader {

template <class MEMORY_ADDR>	
S19_Loader<MEMORY_ADDR>::S19_Loader(char const *name, Object *parent) :
	Object(name,parent, "S19 loader"),
	Client<Memory<MEMORY_ADDR> >(name, parent),
	Service<Loader>(name, parent),
	memory_import("memory-import", this),
	loader_export("loader-export", this),
	filename(),
	entry_point(0),
	param_filename("filename", this, filename),
	isFirstDataRec(true)
{

	loader_export.SetupDependsOn(memory_import);

}

template <class MEMORY_ADDR>
S19_Loader<MEMORY_ADDR>::~S19_Loader() 
{ 
}

template <class MEMORY_ADDR>
void S19_Loader<MEMORY_ADDR>::OnDisconnect()
{
}

template <class MEMORY_ADDR>
void S19_Loader<MEMORY_ADDR>::Reset() 
{
	if(memory_import) memory_import->ResetMemory();
}

template <class MEMORY_ADDR>
MEMORY_ADDR S19_Loader<MEMORY_ADDR>::GetEntryPoint() const
{ 
	return ((MEMORY_ADDR) entry_point);
}

template <class MEMORY_ADDR>
MEMORY_ADDR S19_Loader<MEMORY_ADDR>::GetTopAddr() const
{  
	return (-1);
}

template <class MEMORY_ADDR>
MEMORY_ADDR S19_Loader<MEMORY_ADDR>::GetStackBase() const
{  // TODO
	return (0);
}

template <class MEMORY_ADDR>
bool S19_Loader<MEMORY_ADDR>::BeginSetup() {
	return (true);
}

template <class MEMORY_ADDR>
bool S19_Loader<MEMORY_ADDR>::Setup(ServiceExportBase *srv_export) {
	return (true);
}

template <class MEMORY_ADDR>
bool S19_Loader<MEMORY_ADDR>::EndSetup() {
	return (Load());
}

template <class MEMORY_ADDR>
bool S19_Loader<MEMORY_ADDR>::Load(const char *_filename) {
	if(memory_import) memory_import->ResetMemory();
	filename = _filename;
	return (Load());
}

template <class MEMORY_ADDR>
bool S19_Loader<MEMORY_ADDR>::Load() {
	 
	int             linenum;            /* tracks line number in bootstrap file */
	char            srec[S_RECORD_SIZE];          /* holds S-record from bootstrap file */
	FILE            *bootptr;           /* pointer to bootstrap file */
	bool			success = true;

	if(filename.empty()) return (true);
	
	linenum = 0;

	bootptr = fopen(Object::GetSimulator()->SearchSharedDataFile(filename.c_str()).c_str(), "r");
	if (!bootptr)  {
		ShowError(ERR_NOFILE,0,NULL);
		return (false);
	}

	std::cerr << Object::GetName() << ": Load file \"" << filename << "\" to simulated RAM." << std::endl;
	std::cerr << Object::GetName() << ": Load started..." << std::endl;

	do
	{
		if(!fgets(srec, S_RECORD_SIZE, bootptr))
		{
			if(!feof(bootptr))
			{
				ShowError(ERR_IO,0,NULL);	
			}
			break;
		}
		linenum++;
		success = success && ProcessRecord(linenum,srec);
	}
	while (1);

	fclose(bootptr);

	std::cerr << Object::GetName() << ": File \"" << filename;
	if (success) {
		std::cerr << "\" Load success." << std::endl;
	} else {
		std::cerr << "\" Load fail!" << std::endl;
	}

	return (success);
}

template <class MEMORY_ADDR>
bool  S19_Loader<MEMORY_ADDR>::ProcessRecord(int linenum, char srec[S_RECORD_SIZE])
{
	int     cnt, s5cnt;
	int     chksum;
	int     tchksum;
	unsigned char     sdata[254];

	s19_address_t s19_addr;

	int     sdataIndex, nDataByte;
	int		addrSize;

	if (srec[0] == '\0')  return (true);           /* just in case */
	if (srec[0] == '\n')  return (true);           /* just in case */
	if (srec[0] == '*')  return (true);            /* * in column 1 = comment */
	if (srec[0] != 'S')  {                  /* no S in column 1... */
		ShowError(ERR_BADREC,linenum,srec); /* show bad record format */
		return (false);                            /* exit fatally */
	}


	sscanf(srec+2, "%2x", &cnt);            /* get the number of bytes in rec */
	chksum = cnt;

	switch (srec[1]) {
		case S0: {	/* S0 = header, ignore it */
			return (true);
		} break;
		
		case S5: {
		    /* S5 = A record containing the number of S1, S2, and S3 records
		     *      transmitted in a particular block.
		     */
			sscanf(srec+4, "%4x", &s5cnt);           /* get addr of this rec */
			chksum += (s5cnt >> 8);
			chksum += (s5cnt & 0xff);
	
			sscanf(srec+2+(cnt*2), "%2x", &tchksum);
			if ((tchksum + (chksum & 0xff)) != 0xff)  {
				ShowError(ERR_BADCHKSUM,linenum,srec);
				return (false);
			}
			
			return (true);

		} break;
		
		case S7: {	/* S7 = A termination record for a block S3,
					 * may contain entry point. default 0x00000000 
					 */
			sscanf(srec+4, "%8x", &s19_addr);           /* get addr of this rec */

			if (s19_addr != 0x00) {
				entry_point = s19_addr;
			}

			return (true);
		} break;
		
		case S8: {	/* S8 = A termination record for a block S2,
					 * may contain entry point. default 0x000000 
					 */
			sscanf(srec+4, "%6x", &s19_addr);           /* get addr of this rec */

			if (s19_addr != 0x00) {
				entry_point = s19_addr;
			}

			return (true);
		} break;
		
		case S9: {	/* S9 = A termination record for a block S1,
					 * may contain entry point. default 0x0000 
					 */
			sscanf(srec+4, "%4x", &s19_addr);           /* get addr of this rec */

			if (s19_addr != 0x00) {
				entry_point = s19_addr;
			}

			return (true);
		} break;
		
		default: {
		
			switch (srec[1]) {
				case S1: { /* A record containing code/data and the 2-byte address (offset) at which the code/data is reside */
					addrSize = 2;	// S1: address is coded on 4 hex digits (2 bytes)
					sscanf(srec+4, "%4x", &s19_addr);           /* get addr of this rec */
					chksum += (s19_addr >> 8);
					chksum += (s19_addr & 0xff);
				} break;
				
				case S2: { /* A record containing code/data and the 3-byte address (offset) at which the code/data is reside */
					addrSize = 3;	// S2: address is coded on 6 hex digits (3 bytes)
					sscanf(srec+4, "%6x", &s19_addr);           /* get addr of this rec */
					chksum += (s19_addr >> 16);
					chksum += ((s19_addr >> 8) & 0xff);			
					chksum += (s19_addr & 0xff);
				} break;
				
				case S3: { /* A record containing code/data and the 4-byte address (offset) at which the code/data is reside */
					addrSize = 4;	// S3: address is coded on 8 hex digits (4 bytes)		
					sscanf(srec+4, "%8x", &s19_addr);           /* get addr of this rec */
					chksum += (s19_addr >> 24);
					chksum += ((s19_addr >> 16) & 0xff);			
					chksum += ((s19_addr >> 8) & 0xff);			
					chksum += (s19_addr & 0xff);
				} break;
				
				default: {
				    /* not (S1 or S2 or S3), unsupported record */
		    		/* show the error */
					ShowError(ERR_NOSUPPORT,linenum,srec);  /* show the error */
					return (false);
				}
			}

			nDataByte = (cnt-addrSize-1); 
			for (sdataIndex=0; sdataIndex<nDataByte; sdataIndex++)  {
				unsigned int v;
				sscanf(srec+(addrSize+2+sdataIndex)*2, "%2x", &v);
				sdata[sdataIndex] = v;
				chksum += sdata[sdataIndex];

			}

			
			sscanf(srec+2+(cnt*2), "%2x", &tchksum);

			if ((tchksum + (chksum & 0xff)) != 0xff)  {
				std::cerr << "check sum " << chksum << "\n";
				ShowError(ERR_BADCHKSUM,linenum,srec);
				return (false);
			}

			if (srec[1] == S1)
			{
				if ((s19_addr + nDataByte-2) == CPU12_RESET_ADDR) {
					entry_point = (uint16_t)(sdata[nDataByte-2] << 8) + sdata[nDataByte-1];
				}
			}
			else if (srec[1] == S2) // S2 record
			{
				if ((s19_addr + nDataByte-2) == GLOBAL_CPU12_RESET_ADDR) {
					entry_point = (uint16_t)(sdata[nDataByte-2] << 8) + sdata[nDataByte-1];
				}
				
			}
			
			return (memWrite(s19_addr, sdata,nDataByte));

		}
	}
	
}

template <class MEMORY_ADDR>
bool S19_Loader<MEMORY_ADDR>::memWrite(uint32_t addr, const void *buffer, uint32_t size) {

	bool success = false;
	
	if(memory_import)
	{
		if(size > 0) 
		{
			if(!memory_import->WriteMemory(addr, buffer, size))
			{
				std::cerr << Object::GetName() << ": Can't write into memory (@0x" << std::hex << addr << " - @0x" << (addr +  size - 1) << std::dec << ")" << std::endl;
			}
			else 
			{
				success = true;
			}
		}
	}

	return (success);
}

template <class MEMORY_ADDR>
void  S19_Loader<MEMORY_ADDR>::ShowError(int  errnum, int linenum, char srec[S_RECORD_SIZE])
{
	std::cerr << Object::GetName() << ": ";
	switch (errnum) {
		case ERR_NOFILE: std::cerr << "Error: Unable to open \"" << filename << "\""; break;
		case ERR_BADREC: std::cerr << "Error: Bad S19 record."; break;
		case ERR_NOSUPPORT: std::cerr << "Error: Unsupported S-record format; must be S0, S1 or S9."; break;
		case ERR_BADADDR: std::cerr << "Error: Address is out of range for this MCU."; break;
		case ERR_BADCHKSUM: std::cerr << "Error: Record checksum is bad."; break;
		case ERR_BADFILENAME: std::cerr << "Error: Illegal character in file name."; break;
		case ERR_IO : std::cerr << "Error: Input/Output !"; break;
		default: std::cerr << "Error: Unknown!"; break;
	}
	
	if (linenum)  {
		std::cerr << "\nline " << linenum << " " << srec ;
	}
	
	std::cerr << std::endl;
}

} // end UNISIM namespace
} // end SERVICE namespace
} // end LOADER namespace
} // end S19_LOADER

