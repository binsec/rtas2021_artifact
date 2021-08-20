
#include <simulator_if.h>

Simulator *simulator = 0;

EXPORTED_FUNCTION void   createObject(int argc, char **argv) {
	if (simulator != 0) {
		delete simulator;
	}

	simulator = new Simulator(argc, argv);

	switch(simulator->Setup())
	{
		case unisim::kernel::Simulator::ST_OK_DONT_START:
			simulator->GeneratePim();
			break;
		case unisim::kernel::Simulator::ST_WARNING:
			cerr << "Some warnings occurred during setup" << endl;
			break;
		case unisim::kernel::Simulator::ST_OK_TO_START:
			cerr << "Starting simulation at supervisor privilege level (kernel mode)" << endl;
//			simulator->Run();

			break;
		case unisim::kernel::Simulator::ST_ERROR:
			cerr << "Cannot start simulation because of previous errors" << endl;
			break;
	}

}

EXPORTED_FUNCTION int computeWrapper(int inVal) {
	if (simulator == 0) {
		exit(-1);
	}

	if (simulator->RunSample(inVal)) {
		return 1;
	} else {
		return 0;
	}

}

EXPORTED_FUNCTION void   deleteObject() {

	if (simulator != 0) { simulator->Stop(NULL, 0, false); delete simulator; simulator = 0; }

}

EXPORTED_FUNCTION  double setATD0(double anValue8[8])
{
	return (simulator->Inject_ATD0(anValue8));
}

EXPORTED_FUNCTION  double setATD1(double anValue16[16])
{
	return (simulator->Inject_ATD1(anValue16));
}

EXPORTED_FUNCTION  double getPWM(bool (*pwmValue)[PWM_SIZE])
{
	return (simulator->Get_PWM(pwmValue));
}

EXPORTED_FUNCTION  double setCAN(CAN_DATATYPE msg)
{
	return (simulator->Inject_CAN(msg));
}

EXPORTED_FUNCTION  double getCAN(CAN_DATATYPE *msg)
{
	return (simulator->Get_CAN(msg));
}

EXPORTED_FUNCTION  double setCANArray(CAN_DATATYPE_ARRAY msg)
{
	return (simulator->Inject_CANArray(msg));
}

EXPORTED_FUNCTION  double getCANArray(CAN_DATATYPE_ARRAY *msg)
{
	return (simulator->getCANArray(msg));
}
