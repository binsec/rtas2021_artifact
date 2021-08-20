/*
 */

#if HAVE_RTBCOB

#include "rtb_unisim.hh"

RTBStub::RTBStub(const sc_module_name& name, Object *parent) :
	ATD_PWM_STUB(name, parent)

{

	SC_HAS_PROCESS(RTBStub);

	SC_THREAD(ProcessATD0);
	SC_THREAD(ProcessATD1);
	SC_THREAD(ProcessPWM);

	init_client();

}

RTBStub::~RTBStub() {

}

/*
Initialization of  Client
*/
int RTBStub::init_client(){
	int NB_INPUT, NB_OUTPUT;

	char IN_DATA_TYPE[128],OUT_DATA_TYPE[128];

	char * host = getenv("COB_HOST");
	char * port_ptr = getenv("COB_PORT");
	int port = atoi(port_ptr) ;

	if (host == NULL || port_ptr == NULL) {
		connected = FALSE;
		printf("Cob isn't connected.\n");

	}else {
		connected = TRUE;
		printf("Cob is connected.\n");
	}
	if(connected){
		init_network();

		client_ctl = new_client_struct(host, port);

		if ( !connect_client_struct(client_ctl) ) {
			free_client_struct(client_ctl);
			printf("Connexion to client contoller failed.");
			clean_network();
			return 1;
		}

		client_in = new_client_struct(host , port );
		if ( !connect_client_struct(client_in)  ) {
			free_client_struct(client_ctl);
			free_client_struct(client_in);
			printf("Connexion to client in failed.");
			clean_network();
			return 0;
		}

		client_out = new_client_struct(host , port);
		if ( !connect_client_struct(client_out) ) {
			free_client_struct(client_ctl);
			free_client_struct(client_in);
			free_client_struct(client_out);
			printf("Connexion to client out failed.");
			clean_network();
			return 0;
		}

		stringstream atd0_ststr;
		for (int i=0; i<ATD0_SIZE; i++) {
			atd0_ststr << "dreal";
			if (i != (ATD0_SIZE -1)) {
				atd0_ststr << ",";
			}
		}
		atd0_dataline_in = new_dataline(ATD0_SIZE, atd0_ststr.str().c_str());
		init_dataline(atd0_dataline_in, client_in->sockfd);
		atd0_data_in = new_data(ATD0_SIZE, atd0_ststr.str().c_str(), TRUE);

		stringstream atd1_ststr;
		for (int i=0; i<ATD1_SIZE; i++) {
			atd1_ststr << "dreal";
			if (i != (ATD1_SIZE -1)) {
				atd1_ststr << ",";
			}
		}
		atd1_dataline_in = new_dataline(ATD1_SIZE, atd1_ststr.str().c_str());
		init_dataline(atd1_dataline_in, client_in->sockfd);
		atd1_data_in = new_data(ATD1_SIZE, atd1_ststr.str().c_str(), TRUE);

		stringstream pwm_ststr;
		for (int i=0; i<PWM_SIZE; i++) {
			pwm_ststr << "boolean";
			if (i != (PWM_SIZE -1)) {
				pwm_ststr << ",";
			}
		}

		pwm_data_out = new_data(PWM_SIZE, pwm_ststr.str().c_str(), TRUE);
	}
}

/*
Control
*/
void RTBStub::control() {

	bool isContinue = true;

	do {
		if ( is_socket_ready_toread(client_ctl->sockfd, 0) ) {
			command_enum command = COB_CONTINUE;
			if ( receive_control(client_ctl->sockfd, &command) ) {
				switch (command) {
				case COB_PAUSE:
					printf("--Command ---> COB_PAUSE received.\n");
					send_ack(client_ctl->sockfd);
					isContinue = false;
					break;

				case COB_CONTINUE:
					// do continue only if simulation is paused
					printf("--Command ---> COB_CONTINUE received.\n");
					send_ack(client_ctl->sockfd);
					isContinue = true;
					break;

				case COB_STOP:
					printf("--Command ---> COB_STOP received.\n");
					// simulation is ended
					//sends ack for stop
					send_ack(client_ctl->sockfd);
					//cout << "Press button for exiting the program" << endl ;
					getchar();
	//				exit(0);
					/**
					 *  - Stop simulation and
					 *  - Leave control to systemc kernel to gracefully clean the simulation context
					 */
					sc_stop();
					wait();

					break;
				default:
					printf("Controller: Unkwown command: %d.\n", command);
					break;
				}
			}
		}

	} while (!isContinue);

}

/*
This function can make the exchange process between RT-Builder and RTBstub
*/
void RTBStub::Input_Cob2ATD0(double atd0_anValue[ATD0_SIZE])
{
	double current_time = sc_time_stamp().to_seconds() ;

	/*********get data************/
	if ( !get_data_at_time(atd0_dataline_in, current_time, INFINITE, atd0_data_in) ) {
		cerr << "error get data " << endl;
		exit(1);
	}

	for (int i=0; i<ATD0_SIZE; i++) {
		cob_dreal data = get_dreal(atd0_data_in, i);
		atd0_anValue[i] = (double) data;
	}

	control();

	cout << "current_time : " << current_time  <<" s "<<endl;
}

void RTBStub::Input_Cob2ATD1(double atd1_anValue[ATD1_SIZE])
{
	double current_time = sc_time_stamp().to_seconds() ;

	/*********get data************/
	if ( !get_data_at_time(atd1_dataline_in, current_time, INFINITE, atd1_data_in) ) {
		cerr << "error get data " << endl;
		exit(1);
	}

	for (int i=0; i<ATD1_SIZE; i++) {
		cob_dreal data = get_dreal(atd1_data_in, i);
		atd1_anValue[i] = (double) data;
	}

	control();
	cout << "current_time : " << current_time  <<" s "<<endl;
}

void RTBStub::output_PWM2Cob(bool pwmValue[PWM_SIZE]){
	double current_time = sc_time_stamp().to_seconds() ;
	/*********send data************/
	if ( current_time > pwm_data_out->time ) {

		for (int i=0; i<PWM_SIZE; i++) {
			cob_boolean data = (cob_boolean) pwmValue[i];
			set_boolean(pwm_data_out, i, data);

		}

		pwm_data_out->time = current_time;
		pwm_data_out->validity = current_time ;
		if ( !send_data(client_out->sockfd, pwm_data_out) ) {
			cerr << "Cannot send data" << endl;
			exit(1);
		}
	}

	control();
	cout << "current_time : " << current_time  <<" s "<<endl;
}

void RTBStub::ProcessATD0() {

	double atd0_anValue[ATD0_SIZE];

	while (!isTerminated()) {
	{

		Input_Cob2ATD0(atd0_anValue);

		Output_ATD0(atd0_anValue);

	}

}

void RTBStub::ProcessATD1() {

	double atd1_anValue[ATD1_SIZE];

	while (!isTerminated()) {
	{

		Input_Cob2ATD1(atd1_anValue);

		Output_ATD1(atd1_anValue);

	}

}


void RTBStub::ProcessPWM() {

	bool pwmValue[PWM_SIZE];

	while (!isTerminated()) {
	{

		Input(pwmValue);

		output_PWM2Cob(pwmValue);

	}

}

#endif
