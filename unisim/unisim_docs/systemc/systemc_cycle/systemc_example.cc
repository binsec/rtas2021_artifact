class M1 : public sc_module {
public:
	sc_in_clk clock;
	sc_in<int> input;
	sc_out<int> output[2];

	M1(const sc_module_name& name) : sc_module(name), state(0)
	{
		SC_HAS_PROCESS(M1);
		
		SC_METHOD(P1);
		sensitive_pos << inClock;
	}

	void P1() {
		state = in;
		state = state + 1;
		output[0] = state;
		output[1] = state;
	}
private:
	int state;
};

class M5 : public sc_module
{
public:
	sc_in<int> input[2];
	sc_out<int> output;
	
	M5(const sc_module_name& name) : sc_module(name)
	{
		SC_HAS_PROCESS(M5);
		
		SC_METHOD(P5);
		sensitive << input[0] << input[1];
	}
	
	void P5() {
		output = input[0] * 2 + input[1];
	}
};
