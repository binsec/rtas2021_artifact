template <class ADDRESS>
class MemoryInjection
{
public:
	MemoryInjection(){}
	virtual ~MemoryInjection(){}

	virtual bool InjectReadMemory(ADDRESS addr, void *buffer, uint32_t size) = 0;
	virtual bool InjectWriteMemory(ADDRESS addr, const void *buffer, uint32_t size) = 0;
};
