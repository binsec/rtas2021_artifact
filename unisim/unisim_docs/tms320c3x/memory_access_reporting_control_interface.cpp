class MemoryAccessReportingControl
{
public:
	virtual void RequiresMemoryAccessReporting(bool report) = 0;
	virtual void RequiresFinishedInstructionReporting(bool report) = 0;
};
