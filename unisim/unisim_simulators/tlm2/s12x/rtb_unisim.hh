/*
 */


#include "atd_pwm_stub.hh"


// Cob includes
extern "C" {
	#include "network.h"
	#include "data.h"
	#include "dataline.h"
	#include "error.h"
	#include "control.h"
	#include "dialog.h"
}


// A module that should implement communication with RT-Builder
class RTBStub :
	public ATD_PWM_STUB
{


private:
	bool connected;
	client_struct * client_ctl;
	client_struct * client_in;
	client_struct * client_out;

	dataline_struct * atd0_dataline_in;
	data_struct * atd0_data_in;

	dataline_struct * atd1_dataline_in;
	data_struct * atd1_data_in;

	data_struct * pwm_data_out;


public:
	typedef ATD_PWM_STUB inherited;


	int init_client();
	void control();
	void Input_Cob2ATD0(double atd0_anValue[ATD0_SIZE]);
	void Input_Cob2ATD1(double atd1_anValue[ATD1_SIZE]);
	void output_PWM2Cob(bool pwmValue[PWM_SIZE]);

	RTBStub(const sc_module_name& name, Object *parent = 0);
	~RTBStub();

	void ProcessATD0();
	void ProcessATD1();
	void ProcessPWM();

private:

};

