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


#ifndef __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_LCD_DRIVER_EM6124_TCC__
#define __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_LCD_DRIVER_EM6124_TCC__


#include "em6124.hh"

namespace unisim {
    namespace component {
        namespace tlm2 {
            namespace signal_converter {
                namespace lcd_driver {

                    EM6124::EM6124(const sc_module_name& name) :
                    sc_module(name),
                    mosi("adc_mosi")
 {

                        // Register callback for incoming b_transport interface method call
                        mosi.register_b_transport(this, &EM6124::lcd_receive_frame);
                    }

                    int EM6124::ReadDeviceSegments(uint8_t * buf) {
                        uint16_t SegChar;
                        uint32_t SegDot;
                        uint32_t CharNum;
                        uint32_t SegNum;
                        const tSegment * pSeg;

                        SegDot = 0x00000000U;
                        int nbRead = 0;
                        for (CharNum = 0; CharNum < 12 + 20; CharNum++, nbRead += 2) {
                            SegChar = 0x0000U;
                            pSeg = &Disperser[ CharNum ][ 0 ];

                            for (SegNum = 0; SegNum < 16; SegNum++, pSeg++) {
                                if (EM6124_Memory[ pSeg->Offset ] & pSeg->Mask) {
                                    SegChar |= (1 << SegNum);
                                }
                            }

                            if (EM6124_Memory[ pSeg->Offset ] & pSeg->Mask) {
                                SegDot |= (0x80000000U >> CharNum);
                            }

                            buf[ nbRead ] = (uint8_t) (SegChar >> 8);
                            buf[ nbRead + 1 ] = (uint8_t) SegChar;
                        }

                        buf[ nbRead ] = (uint8_t) (SegDot >> 16);
                        buf[ nbRead + 1 ] = (uint8_t) (SegDot >> 8);
                        buf[ nbRead + 2 ] = (uint8_t) SegDot;

                        nbRead += 3;

                        return (nbRead);
                    }

                    void EM6124::lcd_receive_frame(tlm::tlm_generic_payload& trans, sc_time& delay) {

                        uint16_t *data;
                        data = (uint16_t *) trans.get_data_ptr();
                        stream2receive[NbByte] = *data;
                       // printf("DATA_READ=%x\n", *data);
                        if (((Byte2Receive & 0xc0) == 0xc0) && (NbByte == 0)) // une trame d'initialisation est en cours...
                        {
                            State = INIT_EM_6124;
                        }

                        switch (State) {
                            case INIT_EM_6124:
                            {
                                //	printf("INIT_EM_6124 = 0x%02x\n",Byte2Receive);
                                cpt++;
                                if (cpt == 4) {
                                    State = MAIN_JOB;
                                    cpt = 0;
                                }
                                //la trame de config fait 4 octets
                            }
                                break;

                            case MAIN_JOB:
                            {
                                EM6124_Memory[NbByte] = Byte2Receive;
                                if (NbByte == ((NB_FRAME * FRAME_SIZE) / BYTE_SIZE) - 1)// les 8 trames de balayage recues
                                {
                                    //printf("\n---Memoire Ecran---\n");
                                    NbByte = 0;
                                    ReadDeviceSegments(buffer); //on reconstruit la memoire d'ecran (segments+fleches+points) a partir des trames SPI

                                    for (int i = 0; i < 32; i++) {
                                        int j = 0;
                                        buffer16[i] = (((uint16_t) buffer[2 * i]) << 8) | ((uint16_t) buffer[(2 * i) + 1]);

                                        while ((tabSegments[j] != buffer16[i]) && (j < SIZE_TAB)) //recherche de l'unicode correspondant dans la table
                                        {
                                            j++;
                                        }
                                        if (j >= SIZE_TAB) {
                                            Full_Screen[i] = 'X';
                                        } else {
                                            Full_Screen[i] = (char) tabUnicode[j];
                                        }
                                        j = 0;
                                    }
                                    // demarrage_led=1;
                                } else {
                                    NbByte++;
                                }
                            }
                                break;
                        }


                    }

                    EM6124::~EM6124() {
                    }

                } // end of namespace lcd_driver
            } // end of namespace signal_converter
        } // end of namespace tlm2
    } // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_GENERIC_EM6124_EM6124_TCC__



