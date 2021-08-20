template <class ADDRESS>
class Disassembly
{
public:
	virtual std::string Disasm(ADDRESS addr, ADDRESS& next_addr) = 0;
};
