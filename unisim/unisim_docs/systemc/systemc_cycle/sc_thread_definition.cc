class UserModule : public sc_module
{
public:
	sc_in<int> in_port1, in_port2;
	sc_in_clk clk_port;

	UserModule(const sc_module_name& name) : sc_module(name)
	{
		SC_HAS_PROCESS(UserModule);
	
		SC_THREAD(MyProcess);
		sensitive << in_port1 << in_port2 << clk_port.neg();
	}

	void MyProcess()
	{
		while(1)
		{
			...
			wait();
			...
		}
	}
};
