template <class ADDRESS>
class DebugControl
{
public:
	typedef enum { DBG_STEP, DBG_SYNC, DBG_KILL, DBG_RESET } DebugCommand;

	virtual DebugCommand FetchDebugCommand(ADDRESS cia) = 0;
};
