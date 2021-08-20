class TI_C_IO
{
public:
	typedef enum
	{
		ERROR = -1,
		OK    = 0,
		EXIT  = 1,
	} Status;

	virtual Status HandleEmulatorInterrupt() = 0;
};
