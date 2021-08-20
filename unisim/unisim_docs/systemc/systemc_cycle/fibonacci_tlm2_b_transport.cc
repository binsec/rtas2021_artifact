#include <systemc.h>
#include <iostream>
#include <tlm.h>

using namespace tlm;

typedef unsigned int address_t;
typedef unsigned int word_t;

class MyTransaction
{
public:
	typedef enum {WRITE, READ} TransactionType;
	TransactionType type;
	address_t addr;
	word_t write_value;
	word_t read_value;
};

class Fibonacci : public sc_module
{
public:
	address_t sp;

	sc_port<tlm_blocking_transport_if<MyTransaction> > prt;

	Fibonacci(const sc_module_name& name) : sc_module(name)
	{
		SC_HAS_PROCESS(Fibonacci);

		SC_THREAD(Process);
	}

	word_t Read(address_t _addr)
	{
		MyTransaction my_trans;
		my_trans.type = MyTransaction::READ;
		my_trans.addr = _addr;
		prt->b_transport(my_trans);
		return my_trans.read_value;
	}

	void Write(address_t _addr, word_t _data)
	{
		MyTransaction my_trans;
		my_trans.type = MyTransaction::WRITE;
		my_trans.addr = _addr;
		my_trans.write_value = _data;
		prt->b_transport(my_trans);
	}

	void Fibo()
	{
		word_t n = Read(sp);
		if(n == 1 || n == 2)
		{
			word_t fibo_n = 1;
			std::cerr << sc_time_stamp() << ":" << name() << ": Computing Fibo(" << n << ") = " << fibo_n << std::endl;
			wait(10.0, SC_NS);
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
			wait(10.0, SC_NS);
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
		sc_stop(); wait();
	}
};

class Memory : public sc_module, public tlm_blocking_transport_if<MyTransaction>
{
public:
	static const unsigned int MEMORY_WORD_SIZE = 65536;
	word_t storage[MEMORY_WORD_SIZE];

	sc_export<tlm_blocking_transport_if<MyTransaction> > exp;

	Memory(const sc_module_name& name) : sc_module(name)
	{
		exp(*this);
	}

	virtual void b_transport(MyTransaction& trans)
	{
		switch(trans.type)
		{
			case MyTransaction::READ:
				wait(20.0, SC_NS);
				trans.read_value = storage[trans.addr % MEMORY_WORD_SIZE];
				break;
			case MyTransaction::WRITE:
				wait(20.0, SC_NS);
				storage[trans.addr % MEMORY_WORD_SIZE] = trans.write_value;
				break;
		}
	}
};

int sc_main(int argc, char *argv[])
{
	Fibonacci *fibo = new Fibonacci("fibo");
	Memory *mem = new Memory("mem");

	fibo->prt(mem->exp);

	sc_start();
	return 0;
}
