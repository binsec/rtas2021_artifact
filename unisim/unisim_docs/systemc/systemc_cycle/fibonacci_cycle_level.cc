#include <systemc.h>
#include <iostream>

typedef unsigned int address_t;
typedef unsigned int word_t;

class Fibonacci : public sc_module
{
public:
	sc_in_clk clock;

	sc_out<address_t> addr;
	sc_out<bool> write;
	sc_in<word_t> data_in;
	sc_in<bool> busy;
	sc_out<word_t> data_out;

	sc_time cycle_time;

	address_t sp; // stack pointer

	Fibonacci(const sc_module_name& name) : sc_module(name), cycle_time(10.0, SC_NS)
	{
		SC_HAS_PROCESS(Fibonacci);
		
		SC_THREAD(Process);
	}

	~Fibonacci()
	{
	}

	word_t Read(address_t _addr)
	{
		addr = _addr;
		write = false;
		do
		{
			std::cerr << sc_time_stamp() << ":" << name() << ": Read at 0x" << std::hex << addr << std::dec << std::endl;
			wait(cycle_time);
		} while(busy);
		std::cerr << sc_time_stamp() << ":" << name() << ": Read of " << data_in << " at 0x" << std::hex << addr << std::dec << std::endl;
		return data_in;
	}

	void Write(address_t _addr, word_t _data)
	{
		addr = _addr;
		write = true;
		data_out = _data;

		do
		{
			std::cerr << sc_time_stamp() << ":" << name() << ": Write of " << _data << " at 0x" << std::hex << _addr << std::dec << std::endl;
			wait(cycle_time);
		} while(busy);
		write = false;
	}

	void Fibo()
	{
		word_t n = Read(sp);
		if(n == 1 || n == 2)
		{
			word_t fibo_n = 1;
			std::cerr << sc_time_stamp() << ":" << name() << ": Computing Fibo(" << n << ") = " << fibo_n << std::endl;
			wait(cycle_time); // account for 1 cycle latency
			Write(sp + 1, fibo_n);
		}
		else
		{
			--sp; // alloc space for result of fibo(n-1)
			Write(--sp, n - 1); // push n - 1
			Fibo();
			// result of fibo(n-1) is currently at sp + 1
			// reuse space previously used for n - 1 to store result of fibo(n-2)
			Write(--sp, n - 2); // push n - 2
			Fibo();
			// result of fibo(n-2) is currently at sp + 1
			word_t fibo_n_minus_2 = Read(sp + 1);
			word_t fibo_n_minus_1 = Read(sp + 2);
			word_t fibo_n = fibo_n_minus_1 + fibo_n_minus_2;
			std::cerr << sc_time_stamp() << ":" << name() << ": Computing Fibo(" << n << ") = Fibo(" << n-2 << ") + Fibo(" << n-1 << ") = " << fibo_n_minus_2 << " + " << fibo_n_minus_1 << " = " << fibo_n << std::endl;
			wait(cycle_time); // account for 1 cycle latency for the addition
			Write(sp + 4, fibo_n); // store fibo(n)
			sp += 3; // release the allocated space
		}
	}

	void Process()
	{
		static const word_t N = 10;
		sp = 65536; // initialize stack pointer
		--sp; // alloc space for result of fibo(n)
		Write(--sp, N); // push n
		Fibo();
		std::cerr << "Fibo(" << N << ")=" << Read(sp + 1) << std::endl;
		sc_stop();
	}
};


class Memory : public sc_module
{
public:
	static const unsigned int MEMORY_WORD_SIZE = 65536;
	static const unsigned int WAIT_STATES = 1;
	

	sc_in_clk clock;
	sc_in<address_t> addr;
	sc_in<bool> write;
	sc_in<word_t> data_in;
	sc_out<word_t> data_out;
	sc_out<bool> busy;

	sc_signal<unsigned int> wait_states;
	sc_time cycle_time;
	
	address_t latched_addr;

	word_t storage[MEMORY_WORD_SIZE];

	Memory(const sc_module_name& name) : sc_module(name), cycle_time(10.0, SC_NS), latched_addr(0)
	{
		SC_HAS_PROCESS(Memory);
		
		SC_THREAD(Process);

		SC_METHOD(Read)
		sensitive << addr << write << data_in << wait_states;

		SC_METHOD(Control);
		sensitive << addr << write << data_in << wait_states;
	}

	~Memory()
	{
	}

	void Process()
	{
		while(1)
		{
			wait(cycle_time);
			if(addr != latched_addr)
			{
				latched_addr = addr;
				wait_states = 0;
			}
			if(wait_states == WAIT_STATES)
			{
				if(write)
				{
					std::cerr << sc_time_stamp() << ":" << name() << ": Write " << data_in  << " at 0x" << std::hex <<  addr << std::dec << std::endl;
					storage[addr % MEMORY_WORD_SIZE] = data_in;
				}
				wait_states = 0;
			}
			else
			{
				std::cerr << sc_time_stamp() << ":" << name() << ": wait state (" << wait_states << ")" << std::endl;
				wait_states = wait_states + 1;
			}
		}
	}

	void Read()
	{
		if(!write && wait_states == WAIT_STATES)
		{
			std::cerr << sc_time_stamp() << ": Read of " << storage[addr % (sizeof(storage) / sizeof(storage[0]))] << " at 0x" << std::hex << addr << std::dec << std::endl;
			std::cerr.flush();
			data_out = storage[addr % MEMORY_WORD_SIZE];
		}
	}

	void Control()
	{
		std::cerr << sc_time_stamp() << ":" << name() << ":busy=" << (wait_states != WAIT_STATES) << std::endl;
		busy = wait_states != WAIT_STATES;
	}
};

int sc_main(int argc, char *argv[])
{
	sc_clock clock("clock", sc_time(10.0, SC_NS));
	sc_signal<address_t> addr;
	sc_signal<bool> write;
	sc_signal<bool> busy;
	sc_signal<word_t> data_fibo_to_mem, data_mem_to_fibo;

	Fibonacci *fibo = new Fibonacci("fibonacci");
	Memory *mem = new Memory("memory");

	fibo->clock(clock);
	fibo->addr(addr);
	fibo->write(write);
	fibo->busy(busy);
	fibo->data_out(data_fibo_to_mem);
	fibo->data_in(data_mem_to_fibo);

	mem->clock(clock);
	mem->addr(addr);
	mem->write(write);
	mem->busy(busy);
	mem->data_in(data_fibo_to_mem);
	mem->data_out(data_mem_to_fibo);

	sc_start();
	std::cerr << sc_time_stamp() << std::endl;

	return 0;
}
