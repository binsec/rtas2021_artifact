#include <systemc.h>
#include <iostream>
#include <tlm.h>
#include <inttypes.h>

using namespace tlm;

typedef uint32_t address_t;
typedef uint32_t word_t;

class Fibonacci : public sc_module, public tlm_bw_b_transport_if
{
public:
	address_t sp;

	tlm_b_initiator_socket<> master_sock;

	Fibonacci(const sc_module_name& name) : sc_module(name), master_sock("master_sock")
	{
		master_sock(*this);

		SC_HAS_PROCESS(Fibonacci);

		SC_THREAD(Process);
	}

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
	{
	}

	word_t Read(address_t _addr)
	{
		word_t read_data;
		tlm_generic_payload payload;
		payload.set_read();
		payload.set_address(_addr * sizeof(word_t));
		payload.set_data_ptr((unsigned char *) &read_data);
		payload.set_data_length(sizeof(word_t));
		master_sock->b_transport(payload);
		std::cerr << sc_time_stamp() << ":" << name() << ": Read of " << read_data << " at 0x" << std::hex << _addr << std::dec << std::endl;
		return read_data;
	}

	void Write(address_t _addr, word_t _data)
	{
		std::cerr << sc_time_stamp() << ":" << name() << ": Write of " << _data << " at 0x" << std::hex << _addr << std::dec << std::endl;
		tlm_generic_payload payload;
		payload.set_write();
		payload.set_address(_addr * sizeof(word_t));
		payload.set_data_ptr((unsigned char *) &_data);
		payload.set_data_length(sizeof(word_t));
		master_sock->b_transport(payload);
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

class Memory : public sc_module, public tlm_fw_b_transport_if<>
{
public:
	static const unsigned int MEMORY_WORD_SIZE = 65536;
	word_t storage[MEMORY_WORD_SIZE];

	tlm_b_target_socket<> slave_sock;

	Memory(const sc_module_name& name) : sc_module(name), slave_sock("slave_sock")
	{
		slave_sock(*this);
	}

	virtual bool get_direct_mem_ptr(const sc_dt::uint64& address, tlm_dmi_mode& dmi_mode, tlm_dmi&  dmi_data)
	{
		return false;
	}

	virtual unsigned int transport_dbg(tlm_debug_payload& r)
	{
		return 0;
	}

	virtual void b_transport(tlm_generic_payload& payload)
	{
		tlm_command cmd = payload.get_command();
		sc_dt::uint64 addr = payload.get_address();
		unsigned char *data_ptr = payload.get_data_ptr();
		
		switch(cmd)
		{
			case TLM_READ_COMMAND:
				*(word_t *) data_ptr = storage[addr / sizeof(word_t)];
				break;
			case TLM_WRITE_COMMAND:
				storage[addr / sizeof(word_t)] = *(word_t *) data_ptr;
				break;
		}
		payload.set_response_status(TLM_OK_RESPONSE);
	}
};

int sc_main(int argc, char *argv[])
{
	Fibonacci *fibo = new Fibonacci("fibo");
	Memory *mem = new Memory("mem");

	fibo->master_sock(mem->slave_sock);

	sc_start();
	return 0;
}
