#include <systemc.h>

int sc_main(int argc, char *argv[])
{
    sc_clock clock("clock", sc_time(10.0, SC_NS));
    sc_signal<bool> s1, s2, s3, s4, s5, s6;

    M1 *m1 = new M1("M1");
    M2 *m2 = new M2("M2");
    M3 *m3 = new M3("M3");
    M4 *m4 = new M4("M4");
    M5 *m5 = new M5("M5");

    m1->clock(clock);
	m1->input(s6);
    m1->output[0](s1);
    m1->output[1](s2);
	m2->input(s1);
	m2->output(s3);
	m3->input(s2);
	m3->output(s4);
	m4->input(s3);
	m4->output(s5);
	m5->input[0](s4);
	m5->input[1](s5);
	m5->output(s6);

    sc_start(sc_time(1000.0, SC_NS));
    return 0;
}
