/*
 *  Copyright (c) 2009,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Reda	Nouacer  <reda.nouacer@cea.fr>
 */


#ifndef XML_ATD_PWM_STUB_HH_
#define XML_ATD_PWM_STUB_HH_


#include "atd_pwm_stub.hh"


// A module that should implement communication with RT-Builder
class XML_ATD_PWM_STUB :
	public ATD_PWM_STUB
{
public:
	typedef ATD_PWM_STUB inherited;

	XML_ATD_PWM_STUB(const sc_module_name& name, Object *parent = 0);
	~XML_ATD_PWM_STUB();

	void processATD0();
	void processATD1();
	void processPWM();

	template <int ATD_SIZE>
	struct data_t {
		double volte[ATD_SIZE];
		double time;
	};

	template <int SIZE> int RandomizeData(std::vector<data_t<SIZE>* > &vect);
	template <int SIZE> int LoadXmlData(const char *filename, std::vector<data_t<SIZE>* > &vect);
	template <int SIZE> void parseRow (xmlDocPtr doc, xmlNodePtr cur, data_t<SIZE> &data);

	virtual bool BeginSetup();

	virtual void Inject_ATD0(double anValue[ATD0_SIZE]);
	virtual void Inject_ATD1(double anValue[ATD1_SIZE]);
	virtual void Get_PWM(bool (*pwmValue)[PWM_SIZE]);

private:

	std::vector<data_t<ATD0_SIZE>* > atd0_vect;
	std::vector<data_t<ATD1_SIZE>* > atd1_vect;
	bool pwmValue[PWM_SIZE];

//	sc_time injection_delay;

	string atd0_anx_stimulus_file;
	Parameter<string>	param_atd0_anx_stimulus_file;

	uint8_t atd0_anx_start_channel;
	Parameter<uint8_t> param_atd0_anx_start_channel;

	uint8_t atd0_anx_wrap_around_channel;
	Parameter<uint8_t> param_atd0_anx_wrap_around_channel;

	string atd1_anx_stimulus_file;
	Parameter<string>	param_atd1_anx_stimulus_file;

	uint8_t atd1_anx_start_channel;
	Parameter<uint8_t> param_atd1_anx_start_channel;

	uint8_t atd1_anx_wrap_around_channel;
	Parameter<uint8_t> param_atd1_anx_wrap_around_channel;

	bool	cosim_enabled;
	Parameter<bool>		param_cosim_enabled;

	bool	atd0_xml_enabled;
	Parameter<bool>		param_atd0_xml_enabled;

	bool	atd1_xml_enabled;
	Parameter<bool>		param_atd1_xml_enabled;

	bool	atd0_rand_enabled;
	Parameter<bool>		param_atd0_rand_enabled;

	bool	atd1_rand_enabled;
	Parameter<bool>		param_atd1_rand_enabled;

};


#endif /* XML_ATD_PWM_STUB_HH_ */
