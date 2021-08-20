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
 * Authors: Khaled Rahmouni (khaled.rahmouni@fr.schneider-electric.com)
 */


#ifndef __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_GENERIC_ADC_ADC_TCC__
#define __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_GENERIC_ADC_ADC_TCC__


#include "adc.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace signal_converter {
namespace generic_adc {


ADC::ADC(const sc_module_name& name) :
		sc_module(name),
		mosi("adc_mosi"),
		miso("adc_miso"),
		data_ready("adc_it_in"),
		chip_select("adc_it_out")
{
	SC_HAS_PROCESS(ADC);
	SC_THREAD(ADC_process);

	// Register callback for incoming b_transport interface method call
	mosi.register_b_transport(this, &ADC::adc_receive_frame);
	chip_select.register_b_transport(this, &ADC::chip_select_in);


	C1_AMPL = 0.1;
	C2_AMPL = 0.3;
	C3_AMPL = 0.5;
	NbByte = 0;
	ADC_CHANNEL=CHANNEL0;
	end_cycle = false;

	for (int i = 0; i < 7; i++)
	stream2send [i] = 0x00;
}


ADC::~ADC()
{
}

uint8_t ADC::Code(uint16_t header)
{
  return (uint8_t) ((header & 0x007EU) >> 1);
}


uint8_t ADC::Address(uint16_t header)
{
  return (uint8_t) ((header & 0xFF80U) >> 7);
}


uint8_t ADC::CheckSum()
{
  return ~(uint8_t) (stream2send[0] + stream2send[1] + stream2send[2] + stream2send[3] + stream2send[4] + stream2send[5]);
}

double ADC::Funct(double time, int type, int freq, int phase, double amp)
{
    switch (type) {
        case 0: // NULL FUNCTION
            return 0;
        case 1: // SINUSOIDE
            return amp * sin((2 * PI * freq * time) + ((2 * PI * phase) / 360));
        case 2: // COSINUSOIDE
            return amp * cos((2 * PI * freq * time) + ((2 * PI * phase) / 360));
            // TO DO :
            // IMPLEMENT OTHERS FUNCTIONS IF NECESSARY
        default:
            return amp;
    }
    return 0;
}


void ADC::adc_receive_frame(tlm::tlm_generic_payload& trans, sc_time& delay)
{
 
                uint8_t *data;
                data = (uint8_t *) trans.get_data_ptr();
                stream2receive[NbByte] = *data;
                //printf("DATA_READ=%x\n", *data);


                tlm::tlm_generic_payload byte2send;
                sc_time time(0, SC_NS);
                byte2send.set_command(tlm::TLM_WRITE_COMMAND);
                byte2send.set_address(0x00);
                byte2send.set_data_length(1);
                byte2send.set_data_ptr((unsigned char *) &(mirror_binary_pattern [stream2send[NbByte]]));
                miso->b_transport(byte2send, time);

                if (NbByte == 6)// on teste si on a recu la trame de 7 octects
                {
                    NbByte = 0;
                    Header = stream2receive[0] + (stream2receive[1] << 8);
                    switch (Code(Header))
                    {
                    case ADC_CMD_RAM:
                        stream2send[0] = stream2receive[0];
                        stream2send[1] = stream2receive[1];
                    

                        switch (Address(Header))
                        {
                        case ADC_RAM_I_V_VALUE0:
                            stream2send[2] = (Ram_U[1] & 0x00FF);
                            stream2send[3] = (Ram_U[1] >> 8);
                            stream2send[4] = (Ram_I[1] & 0x00FF);
                            stream2send[5] = (Ram_I[1] >> 8);

                            break;

                        case ADC_RAM_I_V_VALUE1:
                            stream2send[2] = (Ram_U[2] & 0x00FF);
                            stream2send[3] = (Ram_U[2] >> 8);
                            stream2send[4] = (Ram_I[2] & 0x00FF);
                            stream2send[5] = (Ram_I[2] >> 8);
                            break;

                        case ADC_RAM_I_V_VALUE2:
                            stream2send[2] = (Ram_U[3] & 0x00FF);
                            stream2send[3] = (Ram_U[3] >> 8);
                            stream2send[4] = (Ram_I[3] & 0x00FF);
                            stream2send[5] = (Ram_I[3] >> 8);
                            break;

                        case ADC_RAM_I_V_VALUE3:
                            stream2send[2] = (Ram_U[0] & 0x00FF);
                            stream2send[3] = (Ram_U[0] >> 8);
                            stream2send[4] = (Ram_I[0] & 0x00FF);
                            stream2send[5] = (Ram_I[0] >> 8);
                            break;
                        }

                        stream2send[6] = CheckSum();

                        break;

                    default:
                        break;
                    } //end switch(Code(stream2receive))

                }//end if(NbByte==6)
                        else {
                            NbByte++;
                        }

                    }

                    void ADC::chip_select_in(TLMInterruptPayload& trans, sc_time& delay) {

                    }

                    void ADC::ADC_process() {
                        sc_core::sc_time t = SC_ZERO_TIME;
                        while (true) {

                              /* send the interrupt level through the data_ready port */
                            TLMInterruptPayload *trans = data_ready_signal.allocate();
                            trans->level = true;
                            data_ready->b_transport(*trans, t); // generate interrupt to signal that a data is ready

                            switch (ADC_CHANNEL) {
                                case CHANNEL0:
                                    val[0] = C0_AMPL / sensor_coeff;
                                    Ram_U[0] = (int16_t) (val[0] * (ADC_FULLSCALE / ADC_VREF_U));
                                    if (Ram_U[0] > (ADC_FULLSCALE / (2.0 * ADC_VREF_U))) Ram_U[0] = (int16_t) (ADC_FULLSCALE / 2.0 * ADC_VREF_U);
                                    if (Ram_U[0] <-(ADC_FULLSCALE / (2.0 * ADC_VREF_U))) Ram_U[0] = -(int16_t) (ADC_FULLSCALE / 2.0 * ADC_VREF_U);
                                    Ram_I[0] = Ram_U[0];
                                    ADC_CHANNEL = CHANNEL1;
                                    break;

                                case CHANNEL1:
                                    val[1] = (Funct(RelativeTime, C1_TYPE, C1_FREQUENCY, C1_DEPHASAGE, C1_AMPL / sensor_coeff));
                                    Ram_U[1] = (int16_t) (val[1] * (ADC_FULLSCALE / ADC_VREF_U));
                                    if (Ram_U[1] > (ADC_FULLSCALE / (2.0 * ADC_VREF_U))) Ram_U[1] = (int16_t) (ADC_FULLSCALE / 2.0 * ADC_VREF_U);
                                    if (Ram_U[1] <-(ADC_FULLSCALE / (2.0 * ADC_VREF_U))) Ram_U[1] = -(int16_t) (ADC_FULLSCALE / 2.0 * ADC_VREF_U);
                                    Ram_I[1] = Ram_U[1];
                                    ADC_CHANNEL = CHANNEL2;
                                    break;

                                case CHANNEL2:
                                    val[2] = (Funct(RelativeTime, C2_TYPE, C2_FREQUENCY, C2_DEPHASAGE, C2_AMPL / sensor_coeff));
                                    Ram_U[2] = (int16_t) (val[2] * (ADC_FULLSCALE / ADC_VREF_U));
                                    if (Ram_U[2] > (ADC_FULLSCALE / (2.0 * ADC_VREF_U))) Ram_U[2] = (int16_t) (ADC_FULLSCALE / 2.0 * ADC_VREF_U);
                                    if (Ram_U[2] <-(ADC_FULLSCALE / (2.0 * ADC_VREF_U))) Ram_U[2] = -(int16_t) (ADC_FULLSCALE / 2.0 * ADC_VREF_U);
                                    Ram_I[2] = Ram_U[2];
                                    ADC_CHANNEL = CHANNEL3;
                                    break;

                                case CHANNEL3:
                                    val[3] = (Funct(RelativeTime, C3_TYPE, C3_FREQUENCY, C3_DEPHASAGE, C3_AMPL / sensor_coeff));
                                    Ram_U[3] = (int16_t) (val[3] * (ADC_FULLSCALE / ADC_VREF_U));
                                    if (Ram_U[3] > (ADC_FULLSCALE / (2.0 * ADC_VREF_U))) Ram_U[3] = (int16_t) (ADC_FULLSCALE / 2.0 * ADC_VREF_U);
                                    if (Ram_U[3] <-(ADC_FULLSCALE / (2.0 * ADC_VREF_U))) Ram_U[3] = -(int16_t) (ADC_FULLSCALE / 2.0 * ADC_VREF_U);
                                    Ram_I[3] = Ram_U[3];
                                    ADC_CHANNEL = CHANNEL0;
                                    end_cycle = true;
                                    break;
                            }

                            if (end_cycle == true) {
                                end_cycle = false;
                                RelativeTime += (1.000 / ADC_FREQ_SAMPLING);
                                if (RelativeTime == (35.000 / ADC_FREQ_SAMPLING)) {
                                    RelativeTime = 0.0;
                                }
                            }
                            wait(INTERRUPT_FREQ, SC_NS); //to simulate the interrupts (data_ready)
                            //     cout << " the time is " << sc_time_stamp() << endl;

                            /* send the interrupt level through the data_ready port */
                            trans->level = false;
                            data_ready->b_transport(*trans, t); // generate interrupt to signal that a data is ready
                            /* release the transaction */
                            trans->release();
                        }
                    }

                } // end of namespace generic_adc
            } // end of namespace signal_converter
        } // end of namespace tlm2
    } // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_GENERIC_ADC_ADC_TCC__



