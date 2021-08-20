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


#ifndef __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_GENERIC_ADC_ADC_HH__
#define __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_GENERIC_ADC_ADC_HH__

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <inttypes.h>

#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/component/tlm2/interrupt/types.hh"
#include "unisim/service/interfaces/trap_reporting.hh"


namespace unisim {
    namespace component {
        namespace tlm2 {
            namespace signal_converter {
                namespace generic_adc {

                    using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
                    using unisim::component::tlm2::interrupt::TLMInterruptPayload;
                    using unisim::kernel::tlm2::PayloadFabric;


                    /* RESOLUTIONS ***************************************************************/

                    static const double ADC_FULLSCALE = 59134.0; // ADC 16 bits, 59134 points


                    /* FREQUENCES ****************************************************************/

                    static const double ADC_FREQ_SAMPLING = 1800.0; /* 1800 Hz                                           */


                    /* REFERENCE VOLTAGE *****************************************************/
                    static const double ADC_VREF_I = 1.2;
                    static const double ADC_VREF_U = 1.0;


                    /* CURRENT PARAMETERS  *****************************************************/

                    static const uint8_t C1_TYPE = 1; // 1 : sinus ; 2 : cosinus ; 3 : constant
                    static const uint8_t C1_FREQUENCY = 50; // Hz
                    static const uint8_t C1_DEPHASAGE = 0; //degree

                    static const uint8_t C2_TYPE = 1; // 1 : sinus ; 2 : cosinus ; 3 : constant
                    static const uint8_t C2_FREQUENCY = 50; // Hz
                    static const uint8_t C2_DEPHASAGE = 120; //degree

                    static const uint8_t C3_TYPE = 1; // 1 : sinus ; 2 : cosinus ; 3 : constant
                    static const uint8_t C3_FREQUENCY = 50; // Hz
                    static const uint8_t C3_DEPHASAGE = 240; //degree


                    /* CONSTANTS  *****************************************************/

                    static const double PI = 3.141592653589793; // pi Constant
                    static const double sensor_coeff = 17.9;
                    static const uint8_t mirror_binary_pattern [256] = {
                        0x00U, 0x80U, 0x40U, 0xC0U, 0x20U, 0xA0U, 0x60U, 0xE0U, 0x10U, 0x90U, 0x50U, 0xD0U, 0x30U, 0xB0U, 0x70U, 0xF0U,
                        0x08U, 0x88U, 0x48U, 0xC8U, 0x28U, 0xA8U, 0x68U, 0xE8U, 0x18U, 0x98U, 0x58U, 0xD8U, 0x38U, 0xB8U, 0x78U, 0xF8U,
                        0x04U, 0x84U, 0x44U, 0xC4U, 0x24U, 0xA4U, 0x64U, 0xE4U, 0x14U, 0x94U, 0x54U, 0xD4U, 0x34U, 0xB4U, 0x74U, 0xF4U,
                        0x0CU, 0x8CU, 0x4CU, 0xCCU, 0x2CU, 0xACU, 0x6CU, 0xECU, 0x1CU, 0x9CU, 0x5CU, 0xDCU, 0x3CU, 0xBCU, 0x7CU, 0xFCU,
                        0x02U, 0x82U, 0x42U, 0xC2U, 0x22U, 0xA2U, 0x62U, 0xE2U, 0x12U, 0x92U, 0x52U, 0xD2U, 0x32U, 0xB2U, 0x72U, 0xF2U,
                        0x0AU, 0x8AU, 0x4AU, 0xCAU, 0x2AU, 0xAAU, 0x6AU, 0xEAU, 0x1AU, 0x9AU, 0x5AU, 0xDAU, 0x3AU, 0xBAU, 0x7AU, 0xFAU,
                        0x06U, 0x86U, 0x46U, 0xC6U, 0x26U, 0xA6U, 0x66U, 0xE6U, 0x16U, 0x96U, 0x56U, 0xD6U, 0x36U, 0xB6U, 0x76U, 0xF6U,
                        0x0EU, 0x8EU, 0x4EU, 0xCEU, 0x2EU, 0xAEU, 0x6EU, 0xEEU, 0x1EU, 0x9EU, 0x5EU, 0xDEU, 0x3EU, 0xBEU, 0x7EU, 0xFEU,
                        0x01U, 0x81U, 0x41U, 0xC1U, 0x21U, 0xA1U, 0x61U, 0xE1U, 0x11U, 0x91U, 0x51U, 0xD1U, 0x31U, 0xB1U, 0x71U, 0xF1U,
                        0x09U, 0x89U, 0x49U, 0xC9U, 0x29U, 0xA9U, 0x69U, 0xE9U, 0x19U, 0x99U, 0x59U, 0xD9U, 0x39U, 0xB9U, 0x79U, 0xF9U,
                        0x05U, 0x85U, 0x45U, 0xC5U, 0x25U, 0xA5U, 0x65U, 0xE5U, 0x15U, 0x95U, 0x55U, 0xD5U, 0x35U, 0xB5U, 0x75U, 0xF5U,
                        0x0DU, 0x8DU, 0x4DU, 0xCDU, 0x2DU, 0xADU, 0x6DU, 0xEDU, 0x1DU, 0x9DU, 0x5DU, 0xDDU, 0x3DU, 0xBDU, 0x7DU, 0xFDU,
                        0x03U, 0x83U, 0x43U, 0xC3U, 0x23U, 0xA3U, 0x63U, 0xE3U, 0x13U, 0x93U, 0x53U, 0xD3U, 0x33U, 0xB3U, 0x73U, 0xF3U,
                        0x0BU, 0x8BU, 0x4BU, 0xCBU, 0x2BU, 0xABU, 0x6BU, 0xEBU, 0x1BU, 0x9BU, 0x5BU, 0xDBU, 0x3BU, 0xBBU, 0x7BU, 0xFBU,
                        0x07U, 0x87U, 0x47U, 0xC7U, 0x27U, 0xA7U, 0x67U, 0xE7U, 0x17U, 0x97U, 0x57U, 0xD7U, 0x37U, 0xB7U, 0x77U, 0xF7U,
                        0x0FU, 0x8FU, 0x4FU, 0xCFU, 0x2FU, 0xAFU, 0x6FU, 0xEFU, 0x1FU, 0x9FU, 0x5FU, 0xDFU, 0x3FU, 0xBFU, 0x7FU, 0xFFU
                    };
                    static const double INTERRUPT_FREQ = 1000000000.0 / 7200.0; //  ADC interrupt frequency 7200HZ
                    static const uint8_t ADC_OP_WRITE = 0; //  Write operation
                    static const uint8_t ADC_OP_READ = 1; //  Read operation


                    /* RAM COMMAND and MAPPING  *****************************************/

                    static const uint8_t ADC_CMD_RAM = 0x00; //  definition of the command code
                    static const uint8_t ADC_RAM_I_V_VALUE0 = 0x9F;
                    static const uint8_t ADC_RAM_I_V_VALUE1 = 0xBF;
                    static const uint8_t ADC_RAM_I_V_VALUE2 = 0xDF;
                    static const uint8_t ADC_RAM_I_V_VALUE3 = 0xFF;

                            class ADC :public sc_module
                       {
                    
                    public:

                        /* socket for spi management */
                        tlm_utils::simple_target_socket<ADC> mosi; //master out slave in port
                        tlm_utils::simple_initiator_socket<ADC> miso; //slave in master out port
                        /* socket for spi management */

                        /* input and outputs sockets for interrupt */
                        tlm_utils::simple_initiator_socket<ADC, 1, InterruptProtocolTypes> data_ready; //out interrupt port
                        tlm_utils::simple_target_socket<ADC, 1, InterruptProtocolTypes> chip_select; //in chip select port
                        /* input and outputs sockets for interrupt */

                       
                        ADC(const sc_module_name& name);
                        virtual ~ADC();


                        // Get command code in stream received
                        uint8_t Code(uint16_t header);
                        // Get address in stream received
                        uint8_t Address(uint16_t header);
                        // calculate checksum of stream received
                        uint8_t CheckSum();
                        double Funct(double time, int type, int freq, int phase, double amp);

                        void adc_receive_frame(tlm::tlm_generic_payload& trans, sc_time& delay);
                        void chip_select_in(TLMInterruptPayload& trans, sc_time& delay);
                        void ADC_process();

                        int16_t Ram_I[4], Ram_U[4];
                        double val[4], RelativeTime, C0_AMPL, C1_AMPL, C2_AMPL, C3_AMPL;
                        uint8_t stream2send[7], stream2receive[7], NbByte;
                        uint16_t Header;
                        bool end_cycle;

                        typedef enum {
                            CHANNEL0, CHANNEL1, CHANNEL2, CHANNEL3
                        } ADC_CHANNEL_t;
                        ADC_CHANNEL_t ADC_CHANNEL;

                    private:

                        /* the interrupt payload fabric */
                        PayloadFabric<TLMInterruptPayload> data_ready_signal;

                    };
                } // end of namespace generic_adc
            } // end of namespace signal_converter
        } // end of namespace tlm2
    } // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_GENERIC_ADC_ADC_HH__

