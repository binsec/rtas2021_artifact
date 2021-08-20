template <class ADDRESS>
class MemoryAccessReporting
{
public:
	typedef enum { MAT_NONE = 0, MAT_READ = 1, MAT_WRITE = 2 } MemoryAccessType;
	typedef enum { MT_DATA = 0, MT_INSN = 1 } MemoryType;

	virtual void ReportMemoryAccess(MemoryAccessType mat, MemoryType mt, ADDRESS addr, uint32_t size) = 0;
	virtual void ReportFinishedInstruction(ADDRESS next_addr) = 0;
};
