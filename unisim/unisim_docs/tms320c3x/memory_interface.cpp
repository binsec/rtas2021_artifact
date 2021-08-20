template <class ADDRESS>
class Memory
{
public:
	Memory(){}
	virtual ~Memory(){}

	virtual void Reset() = 0;
	virtual bool ReadMemory(ADDRESS addr, void *buffer, uint32_t size) = 0;
	virtual bool WriteMemory(ADDRESS addr, const void *buffer, uint32_t size) = 0;
};
