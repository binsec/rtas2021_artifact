#include <systemc.h>
#include <iostream>
#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <inttypes.h>

using namespace tlm;
using namespace tlm_utils;

typedef uint32_t address_t;
typedef uint32_t word_t;

class Fibonacci : public sc_module, public tlm_bw_transport_if<>
{
public:
	const sc_time cycle_time;

	address_t sp;

	tlm_initiator_socket<> master_sock;
	tlm_quantumkeeper quantumkeeper;
	sc_event e;

	Fibonacci(const sc_module_name& name) : sc_module(name), master_sock("master_sock"), cycle_time(10.0, SC_NS)
	{
		master_sock(*this);
		quantumkeeper.set_global_quantum(cycle_time); // leave control at every 1000 clock cycles

		SC_HAS_PROCESS(Fibonacci);

		SC_THREAD(Process);
	}

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
	{
	}

	virtual tlm_sync_enum nb_transport_bw(tlm_generic_payload& payload, tlm_phase& phase, sc_core::sc_time& t)
	{
		if(phase == BEGIN_RESP)
		{
			payload.set_response_status(TLM_OK_RESPONSE);
		}
		else
		{
			payload.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
		}
		e.notify();

		return TLM_COMPLETED;
	}

	word_t Read(address_t _addr)
	{
		std::cerr << quantumkeeper.get_current_time() << ":" << name() << ": Initiating Read at 0x" << std::hex << _addr << std::dec << std::endl;

		tlm_phase phase = BEGIN_REQ;
		word_t read_data;
		tlm_generic_payload payload;
		payload.set_read();
		payload.set_address(_addr * sizeof(word_t));
		payload.set_data_ptr((unsigned char *) &read_data);
		payload.set_data_length(sizeof(word_t));

		sc_time local_time = quantumkeeper.get_local_time();
		tlm_sync_enum ret = master_sock->nb_transport_fw(payload, phase, local_time);

		switch(ret)
		{
			case TLM_ACCEPTED:
				wait(e);
				break;
			case TLM_UPDATED:
				quantumkeeper.set(local_time);
				if(quantumkeeper.need_sync()) quantumkeeper.sync();
				wait(e);
				break;
			case TLM_COMPLETED:
				quantumkeeper.set(local_time);
				if(quantumkeeper.need_sync()) quantumkeeper.sync();
				break; // nothing to do
		}
		
		std::cerr << quantumkeeper.get_current_time() << ":" << name() << ": Read of " << read_data << " at 0x" << std::hex << _addr << std::dec << std::endl;
		return read_data;
	}

	void Write(address_t _addr, word_t _data)
	{
		tlm_phase phase = BEGIN_REQ;
		std::cerr << quantumkeeper.get_current_time() << ":" << name() << ": Write of " << _data << " at 0x" << std::hex << _addr << std::dec << std::endl;
		tlm_generic_payload payload;
		payload.set_write();
		payload.set_address(_addr * sizeof(word_t));
		payload.set_data_ptr((unsigned char *) &_data);
		payload.set_data_length(sizeof(word_t));

		sc_time local_time = quantumkeeper.get_local_time();
		tlm_sync_enum ret = master_sock->nb_transport_fw(payload, phase, local_time);

		switch(ret)
		{
			case TLM_ACCEPTED:
				wait(e);
				break;
			case TLM_UPDATED:
				quantumkeeper.set(local_time);
				if(quantumkeeper.need_sync()) quantumkeeper.sync();
				wait(e);
				break;
			case TLM_COMPLETED:
				quantumkeeper.set(local_time);
				if(quantumkeeper.need_sync()) quantumkeeper.sync();
				break; // nothing to do
		}
		std::cerr << quantumkeeper.get_current_time() << ":" << name() << ": Write finished" << std::endl;
	}

	void Fibo()
	{
		word_t n = Read(sp);
		if(n == 1 || n == 2)
		{
			word_t fibo_n = 1;
			std::cerr << quantumkeeper.get_current_time() << ":" << name() << ": Computing Fibo(" << n << ") = " << fibo_n << std::endl;
			quantumkeeper.inc(cycle_time);
			if(quantumkeeper.need_sync()) quantumkeeper.sync();
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
			std::cerr << quantumkeeper.get_current_time() << ":" << name() << ": Computing Fibo(" << n << ") = Fibo(" << n-2 << ") + Fibo(" << n-1 << ") = " << fibo_n_minus_2 << " + " << fibo_n_minus_1 << " = " << fibo_n << std::endl;
			quantumkeeper.inc(cycle_time);
			if(quantumkeeper.need_sync()) quantumkeeper.sync();
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

class Memory : public sc_module, public tlm_fw_transport_if<>
{
public:
	static const unsigned int MEMORY_WORD_SIZE = 65536;
	word_t storage[MEMORY_WORD_SIZE];
	const sc_time cycle_time;

	tlm_target_socket<> slave_sock;

	Memory(const sc_module_name& name) : sc_module(name), slave_sock("slave_sock"), cycle_time(10.0, SC_NS)
	{
		slave_sock(*this);
	}

	virtual bool get_direct_mem_ptr(tlm_generic_payload& trans, tlm_dmi&  dmi_data)
	{
		return false;
	}


	virtual unsigned int transport_dbg(tlm_generic_payload& trans)
	{
		return 0;
	}

	virtual tlm_sync_enum nb_transport_fw(tlm_generic_payload& payload, tlm_phase& phase, sc_core::sc_time& t)
	{
		if(phase == BEGIN_REQ)
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
		else
		{
			payload.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
		}

		phase = BEGIN_RESP;
		t = t + cycle_time;
		return TLM_COMPLETED;
	}

	virtual void b_transport(tlm_generic_payload& payload, sc_core::sc_time& t)
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
		t = t + cycle_time;
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
