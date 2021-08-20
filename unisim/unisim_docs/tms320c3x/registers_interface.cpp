class Registers
{
public:
	virtual unisim::util::debug::Register *GetRegister(const char *name) = 0;
};
