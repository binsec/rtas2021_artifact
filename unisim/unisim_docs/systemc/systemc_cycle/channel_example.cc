class MyInterface : public sc_interface {
public:
	virtual int Read(int addr) = 0;
};

class MyChannel : public sc_module, public MyInterface {
public:
	sc_export<MyInterface> exp;
	virtual int Read(int addr) { ... }
};

class MyModule : public sc_module {
public:
	sc_port<MyInterface> prt;
	void Process() {
		while(1) {
			...
			read_value = prt->Read(addr);
			...
			wait();
		}
	}
};

int sc_main(int argc, char *argv[]) {
	MyModule *m = new MyModule("m");
	MyChannel *c = new MyChannel("c");
	m->prt(c->exp);
	sc_start();
	return 0;
}
