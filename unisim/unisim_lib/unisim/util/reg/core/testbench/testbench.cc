#include <unisim/util/reg/core/register.hh>
#include <unisim/util/reg/core/register.tcc>

using unisim::util::reg::core::Register;
using unisim::util::reg::core::Field;
using unisim::util::reg::core::FieldSet;
using unisim::util::reg::core::HW_RO;
using unisim::util::reg::core::HW_RW;
using unisim::util::reg::core::SW_R;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_W1;
using unisim::util::reg::core::SW_RW1;
using unisim::util::reg::core::WarningStatus;
using unisim::util::reg::core::RegisterAddressMap;
using unisim::util::reg::core::AddressableRegister;
using unisim::util::reg::core::AddressableRegisterHandle;
using unisim::util::reg::core::AddressableRegisterBase;

/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// testbench //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

#define REV(N) ((SIZE - 1) - N)

struct MSR : public AddressableRegister<MSR, 32, HW_RW>
{
	typedef AddressableRegister<MSR, 32, HW_RW> Super;
	
	struct UCLE : Field<UCLE, REV(5), 2, SW_R> {};
	struct SPE : Field<SPE, REV(6), 1, SW_W> {};
	struct WE : Field<WE, REV(13)> {};
	struct PR : Field<PR, REV(0)> {};
	
	typedef FieldSet<UCLE, SPE, WE, PR> ALL;
	
	MSR();
	MSR(uint32_t value);
	void Initialize(uint32_t value);
	
	using Super::operator =;
};

struct MSR2 : public AddressableRegister<MSR2, 32, HW_RO>
{
	typedef AddressableRegister<MSR2, 32, HW_RO> Super;

	struct UCLE : Field<UCLE, REV(5), 2> {};
	struct SPE : Field<SPE, REV(6)> {};
	struct WE : Field<WE, REV(13)> {};
	struct PR : Field<PR, REV(0)> {};
	

	typedef FieldSet<UCLE, SPE, WE, PR> ALL;
	
	MSR2();
	MSR2(uint32_t value);
	void Initialize(uint32_t value);
	
	using Super::operator =;
};

MSR::MSR()
	: Super()
{
	Initialize(0);
}

MSR::MSR(uint32_t value)
	: Super()
{
	Initialize(value);
}

void MSR::Initialize(uint32_t value)
{
	Super::Initialize(value);
	SetName("MSR");
	SetDisplayName("MSRhasaverylongregisterdisplayname");
	SetDescription("Machine State Register");
	UCLE::SetName("UCLE");
	SPE::SetName("SPE");
	WE::SetName("WE");
	PR::SetName("PR");
	UCLE::SetDescription("Unusable Common Language Enabled");
	SPE::SetDescription("Signal Processing Enabled");
	WE::SetDescription("Write Enable");
	PR::SetDescription("Problem State");
}

MSR2::MSR2()
	: Super()
{
	Initialize(0);
}

MSR2::MSR2(uint32_t value)
	: Super()
{
	Initialize(value);
}

void MSR2::Initialize(uint32_t value)
{
	Super::Initialize(value);
	SetName("MSR2");
	SetDisplayName("MSR2");
	SetDescription("Machine State Register 2");
	UCLE::SetName("UCLE");
	SPE::SetName("SPE");
	WE::SetName("WE");
	PR::SetName("PR");
	UCLE::SetDescription("Unusable Common Language Enabled");
	SPE::SetDescription("Signal Processing Enabled");
	WE::SetDescription("Write Enable");
	PR::SetDescription("Problem State");
}

int main(int argc, const char *argv[])
{
	MSR msr;
	MSR2 msr2;
	RegisterAddressMap<uint32_t> reg_addr_map;
	
	reg_addr_map.MapRegister(0x1004, &msr);
	reg_addr_map.MapRegister(0x1000, &msr2);

	uint32_t v;
	
	msr = 0x0;
	msr2 = 0x0;
	
	WarningStatus ws = msr.Write(0xffffffff, 0xffffffff);
	if(ws) std::cerr << ws << std::endl;
	
	ws = msr.Read(v);
	if(ws) std::cerr << ws << std::endl;
	
	std::cout << msr.GetAccess() << std::endl;
	std::cout << std::hex;
	msr.ShortPrettyPrint(std::cout);
	std::cout << std::endl;
	msr.LongPrettyPrint(std::cout);
	std::cout << std::dec << std::endl;
	std::cout << std::hex;
	msr2.LongPrettyPrint(std::cout);
	std::cout << std::dec << std::endl;
	std::cout << MSR::ALL::GetName() << std::endl;
	std::cout << "sizeof(MSR)=" << sizeof(msr) << std::endl;
// 	std::cout << "toto = " << std::hex << toto << std::dec << std::endl;
// 	std::cout << "0x" << std::hex << msr << std::dec << std::endl;
	
	AddressableRegisterHandle<uint32_t> *arh = reg_addr_map.FindAddressableRegister(0x1004);

	uint32_t b = 0x12345678;
	arh->DebugWrite((unsigned char *) &b);
	
	std::cout << std::hex;
	msr.ShortPrettyPrint(std::cout);
	std::cout << std::dec << std::endl;
	
	uint32_t r;
	arh->DebugRead((unsigned char *) &r);
	
	std::cout << std::hex << "read 0x" << r << std::dec << std::endl;
}
