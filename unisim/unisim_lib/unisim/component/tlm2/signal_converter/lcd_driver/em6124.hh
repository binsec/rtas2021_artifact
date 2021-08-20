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


#ifndef __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_LCD_DRIVER_EM6124_HH__
#define __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_LCD_DRIVER_EM6124_HH__

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
                namespace lcd_driver {

                    using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
                    using unisim::component::tlm2::interrupt::TLMInterruptPayload;
                    using unisim::kernel::tlm2::PayloadFabric;


                    /* CONSTANTS  *****************************************************/
                    //-------------------------------- Definitions ------------------------------//

                    static const uint8_t SCAN_LINE_NUMBER = 8U;
                    static const uint8_t SCAN_LINE_SIZE = 16U;

                    /// \def   A0 to A16
                    /// \brief Segment code
                    ///        A0   A1
                    ///      ------ ------
                    ///  A7 |  \   |A8 /  | A2
                    ///     |A15\  |  /A9 |
                    ///      -A14-- --A10-
                    ///  A6 |A13/  |  \A11| A3
                    ///     |  /   |A12\  |
                    ///      ------ ------
                    ///       A5    A4

                    static const uint32_t A0 = 0x000001;
                    static const uint32_t A1 = 0x000002;
                    static const uint32_t A2 = 0x000004;
                    static const uint32_t A3 = 0x000008;
                    static const uint32_t A4 = 0x000010;
                    static const uint32_t A5 = 0x000020;
                    static const uint32_t A6 = 0x000040;
                    static const uint32_t A7 = 0x000080;
                    static const uint32_t A8 = 0x000100;
                    static const uint32_t A9 = 0x000200;
                    static const uint32_t A10 = 0x000400;
                    static const uint32_t A11 = 0x000800;
                    static const uint32_t A12 = 0x001000;
                    static const uint32_t A13 = 0x002000;
                    static const uint32_t A14 = 0x004000;
                    static const uint32_t A15 = 0x008000;
                    static const uint32_t A16 = 0x010000;

                    /// \def   CODE_OTHERS
                    /// \brief Used code when the character is not in the table
                    static const uint32_t CODE_OTHERS = 0xFFFF;

                    /// \def   SIZE_TAB
                    /// \brief Table size
                    static const uint16_t SIZE_TAB = 129;


                    static const uint16_t LN = (0); /* No line                          */
                    static const uint16_t L0 = (0 * SCAN_LINE_SIZE); /* Line #0                          */
                    static const uint16_t L1 = (1U * SCAN_LINE_SIZE); /* Line #1                          */
                    static const uint16_t L2 = (2U * SCAN_LINE_SIZE); /* Line #2                          */
                    static const uint16_t L3 = (3U * SCAN_LINE_SIZE); /* Line #3                          */
                    static const uint16_t L4 = (4U * SCAN_LINE_SIZE); /* Line #4                          */
                    static const uint16_t L5 = (5U * SCAN_LINE_SIZE); /* Line #5                          */
                    static const uint16_t L6 = (6U * SCAN_LINE_SIZE); /* Line #6                          */
                    static const uint16_t L7 = (7U * SCAN_LINE_SIZE); /* Line #7                          */

                    /**
                     * \brief Offset of the column byte in the line (in bytes)
                     */
                    static const uint8_t CN = 0; /* No column                        */
                    static const uint8_t C00 = 1U; /* Column #0                        */
                    static const uint8_t C01 = 2U; /* Column #1                        */
                    static const uint8_t C02 = 3U; /* Column #2                        */
                    static const uint8_t C03 = 4U; /* Column #3                        */
                    static const uint8_t C04 = 5U; /* Column #4                        */
                    static const uint8_t C05 = 6U; /* Column #5                        */
                    static const uint8_t C06 = 7U; /* Column #6                        */
                    static const uint8_t C07 = 8U; /* Column #7                        */
                    static const uint8_t C08 = 9U; /* Column #8                        */
                    static const uint8_t C09 = 10U; /* Column #9                        */
                    static const uint8_t C10 = 11U; /* Column #10                       */
                    static const uint8_t C11 = 12U; /* Column #11                       */
                    static const uint8_t C12 = 13U; /* Column #12                       */
                    static const uint8_t C13 = 14U; /* Column #13                       */
                    static const uint8_t C14 = 15U; /* Column #14                       */

                    /**
                     * \brief Bit mask in the column byte
                     */
                    static const uint8_t BN = 0x00U; /* --------                         */
                    static const uint8_t B0 = 0x01U; /* -------1                         */
                    static const uint8_t B1 = 0x02U; /* ------1-                         */
                    static const uint8_t B2 = 0x04U; /* -----1--                         */
                    static const uint8_t B3 = 0x08U; /* ----1---                         */
                    static const uint8_t B4 = 0x10U; /* ---1----                         */
                    static const uint8_t B5 = 0x20U; /* --1-----                         */
                    static const uint8_t B6 = 0x40U; /* -1------                         */
                    static const uint8_t B7 = 0x80U; /* 1-------                         */

                    static const uint16_t FRAME_SIZE = 128;
                    static const uint16_t NB_FRAME = 8;
                    static const uint16_t BYTE_SIZE = 8;

                    typedef struct {
                        uint8_t Offset; /**< Index of the container byte               */
                        uint8_t Mask; /**< Selector of the bit in the container byte */
                    } tSegment;


                    static const tSegment Disperser[ 32 ][ 17 ] = {
                        /*  || Segment (Vipam/Varitronix)                                                                                                                                                                                ||  Line | Char. | Dot || */
                        /*  || A00/A1    | A01/A2    | A02/B     | A03/C     | A04/D2    | A05/D1    | A06/E     | A07/F     | A08/I     | A09/J     | A10/K     | A11/L     | A12/M     | A13/N     | A14/G     | A15/H     | A16/P     ||       |       |     || */
                        {
                            {L0 + C02, B2},
                            {L0 + C02, B0},
                            {L0 + C03, B7},
                            {L1 + C03, B7},
                            {L3 + C03, B7},
                            {L3 + C02, B1},
                            {L3 + C02, B2},
                            {L2 + C02, B2},
                            {L0 + C02, B1},
                            {L1 + C02, B0},
                            {L2 + C02, B0},
                            {L2 + C03, B7},
                            {L3 + C02, B0},
                            {L2 + C02, B1},
                            {L1 + C02, B1},
                            {L1 + C02, B2},
                            {LN + C00, BN}
                        }, /* 1    C1             */
                        {
                            {L0 + C03, B6},
                            {L0 + C03, B4},
                            {L0 + C03, B3},
                            {L1 + C03, B3},
                            {L3 + C03, B3},
                            {L3 + C03, B5},
                            {L3 + C03, B6},
                            {L2 + C03, B6},
                            {L0 + C03, B5},
                            {L1 + C03, B4},
                            {L2 + C03, B4},
                            {L2 + C03, B3},
                            {L3 + C03, B4},
                            {L2 + C03, B5},
                            {L1 + C03, B5},
                            {L1 + C03, B6},
                            {LN + C00, BN}
                        }, /* 1    C2             */
                        {
                            {L0 + C03, B2},
                            {L0 + C03, B0},
                            {L0 + C04, B7},
                            {L1 + C04, B7},
                            {L3 + C04, B7},
                            {L3 + C03, B1},
                            {L3 + C03, B2},
                            {L2 + C03, B2},
                            {L0 + C03, B1},
                            {L1 + C03, B0},
                            {L2 + C03, B0},
                            {L2 + C04, B7},
                            {L3 + C03, B0},
                            {L2 + C03, B1},
                            {L1 + C03, B1},
                            {L1 + C03, B2},
                            {LN + C00, BN}
                        }, /* 1    C3             */
                        {
                            {L0 + C04, B6},
                            {L0 + C04, B4},
                            {L0 + C04, B3},
                            {L1 + C04, B3},
                            {L3 + C04, B3},
                            {L3 + C04, B5},
                            {L3 + C04, B6},
                            {L2 + C04, B6},
                            {L0 + C04, B5},
                            {L1 + C04, B4},
                            {L2 + C04, B4},
                            {L2 + C04, B3},
                            {L3 + C04, B4},
                            {L2 + C04, B5},
                            {L1 + C04, B5},
                            {L1 + C04, B6},
                            {LN + C00, BN}
                        }, /* 1    C4             */
                        {
                            {L0 + C04, B2},
                            {L0 + C04, B0},
                            {L0 + C05, B7},
                            {L1 + C05, B7},
                            {L3 + C05, B7},
                            {L3 + C04, B1},
                            {L3 + C04, B2},
                            {L2 + C04, B2},
                            {L0 + C04, B1},
                            {L1 + C04, B0},
                            {L2 + C04, B0},
                            {L2 + C05, B7},
                            {L3 + C04, B0},
                            {L2 + C04, B1},
                            {L1 + C04, B1},
                            {L1 + C04, B2},
                            {LN + C00, BN}
                        }, /* 1    C5             */
                        {
                            {L0 + C05, B6},
                            {L0 + C05, B4},
                            {L0 + C05, B3},
                            {L1 + C05, B3},
                            {L3 + C05, B3},
                            {L3 + C05, B5},
                            {L3 + C05, B6},
                            {L2 + C05, B6},
                            {L0 + C05, B5},
                            {L1 + C05, B4},
                            {L2 + C05, B4},
                            {L2 + C05, B3},
                            {L3 + C05, B4},
                            {L2 + C05, B5},
                            {L1 + C05, B5},
                            {L1 + C05, B6},
                            {LN + C00, BN}
                        }, /* 1    C6             */
                        {
                            {L0 + C05, B2},
                            {L0 + C05, B0},
                            {L0 + C06, B7},
                            {L1 + C06, B7},
                            {L3 + C06, B7},
                            {L3 + C05, B1},
                            {L3 + C05, B2},
                            {L2 + C05, B2},
                            {L0 + C05, B1},
                            {L1 + C05, B0},
                            {L2 + C05, B0},
                            {L2 + C06, B7},
                            {L3 + C05, B0},
                            {L2 + C05, B1},
                            {L1 + C05, B1},
                            {L1 + C05, B2},
                            {LN + C00, BN}
                        }, /* 1    C7             */
                        {
                            {L0 + C06, B6},
                            {L0 + C06, B4},
                            {L0 + C06, B3},
                            {L1 + C06, B3},
                            {L3 + C06, B3},
                            {L3 + C06, B5},
                            {L3 + C06, B6},
                            {L2 + C06, B6},
                            {L0 + C06, B5},
                            {L1 + C06, B4},
                            {L2 + C06, B4},
                            {L2 + C06, B3},
                            {L3 + C06, B4},
                            {L2 + C06, B5},
                            {L1 + C06, B5},
                            {L1 + C06, B6},
                            {LN + C00, BN}
                        }, /* 1    C8             */
                        {
                            {L0 + C06, B2},
                            {L0 + C06, B0},
                            {L0 + C07, B7},
                            {L1 + C07, B7},
                            {L3 + C07, B7},
                            {L3 + C06, B1},
                            {L3 + C06, B2},
                            {L2 + C06, B2},
                            {L0 + C06, B1},
                            {L1 + C06, B0},
                            {L2 + C06, B0},
                            {L2 + C07, B7},
                            {L3 + C06, B0},
                            {L2 + C06, B1},
                            {L1 + C06, B1},
                            {L1 + C06, B2},
                            {LN + C00, BN}
                        }, /* 1    C9             */
                        {
                            {L0 + C07, B6},
                            {L0 + C07, B4},
                            {L0 + C07, B3},
                            {L1 + C07, B3},
                            {L3 + C07, B3},
                            {L3 + C07, B5},
                            {L3 + C07, B6},
                            {L2 + C07, B6},
                            {L0 + C07, B5},
                            {L1 + C07, B4},
                            {L2 + C07, B4},
                            {L2 + C07, B3},
                            {L3 + C07, B4},
                            {L2 + C07, B5},
                            {L1 + C07, B5},
                            {L1 + C07, B6},
                            {L4 + C02, B3}
                        }, /* 1    C10     F1     */
                        {
                            {L0 + C07, B2},
                            {L0 + C07, B0},
                            {L0 + C08, B7},
                            {L1 + C08, B7},
                            {L3 + C08, B7},
                            {L3 + C07, B1},
                            {L3 + C07, B2},
                            {L2 + C07, B2},
                            {L0 + C07, B1},
                            {L1 + C07, B0},
                            {L2 + C07, B0},
                            {L2 + C08, B7},
                            {L3 + C07, B0},
                            {L2 + C07, B1},
                            {L1 + C07, B1},
                            {L1 + C07, B2},
                            {L5 + C02, B3}
                        }, /* 1    C11     F2     */
                        {
                            {L0 + C08, B6},
                            {L0 + C08, B4},
                            {L0 + C08, B3},
                            {L1 + C08, B3},
                            {L3 + C08, B3},
                            {L3 + C08, B5},
                            {L3 + C08, B6},
                            {L2 + C08, B6},
                            {L0 + C08, B5},
                            {L1 + C08, B4},
                            {L2 + C08, B4},
                            {L2 + C08, B3},
                            {L3 + C08, B4},
                            {L2 + C08, B5},
                            {L1 + C08, B5},
                            {L1 + C08, B6},
                            {L6 + C02, B3}
                        }, /* 1    C12     F3     */
                        {
                            {L4 + C02, B2},
                            {L3 + C02, B4},
                            {L3 + C02, B5},
                            {L6 + C02, B5},
                            {L7 + C02, B4},
                            {L7 + C02, B1},
                            {L6 + C02, B2},
                            {L5 + C02, B2},
                            {L4 + C02, B4},
                            {L4 + C02, B5},
                            {L5 + C02, B5},
                            {L6 + C02, B4},
                            {L5 + C02, B4},
                            {L6 + C02, B1},
                            {L5 + C02, B1},
                            {L4 + C02, B1},
                            {L7 + C02, B5}
                        }, /* 2    C13     P1     */
                        {
                            {L4 + C02, B0},
                            {L3 + C02, B6},
                            {L3 + C02, B7},
                            {L6 + C02, B7},
                            {L7 + C02, B6},
                            {L7 + C03, B7},
                            {L6 + C02, B0},
                            {L5 + C02, B0},
                            {L4 + C02, B6},
                            {L4 + C02, B7},
                            {L5 + C02, B7},
                            {L6 + C02, B6},
                            {L5 + C02, B6},
                            {L6 + C03, B7},
                            {L5 + C03, B7},
                            {L4 + C03, B7},
                            {L7 + C02, B7}
                        }, /* 2    C14     P2     */
                        {
                            {L4 + C03, B6},
                            {L3 + C01, B0},
                            {L3 + C01, B1},
                            {L6 + C01, B1},
                            {L7 + C01, B0},
                            {L7 + C03, B5},
                            {L6 + C03, B6},
                            {L5 + C03, B6},
                            {L4 + C01, B0},
                            {L4 + C01, B1},
                            {L5 + C01, B1},
                            {L6 + C01, B0},
                            {L5 + C01, B0},
                            {L6 + C03, B5},
                            {L5 + C03, B5},
                            {L4 + C03, B5},
                            {L7 + C01, B1}
                        }, /* 2    C15     P3     */
                        {
                            {L4 + C03, B3},
                            {L3 + C01, B2},
                            {L3 + C01, B3},
                            {L6 + C01, B3},
                            {L7 + C01, B2},
                            {L7 + C03, B2},
                            {L6 + C03, B3},
                            {L5 + C03, B3},
                            {L4 + C01, B2},
                            {L4 + C01, B3},
                            {L5 + C01, B3},
                            {L6 + C01, B2},
                            {L5 + C01, B2},
                            {L6 + C03, B2},
                            {L5 + C03, B2},
                            {L4 + C03, B2},
                            {L7 + C01, B3}
                        }, /* 2    C16     P4     */
                        {
                            {L4 + C03, B1},
                            {L3 + C01, B4},
                            {L3 + C01, B5},
                            {L6 + C01, B5},
                            {L7 + C01, B4},
                            {L7 + C04, B7},
                            {L6 + C03, B1},
                            {L5 + C03, B1},
                            {L4 + C01, B4},
                            {L4 + C01, B5},
                            {L5 + C01, B5},
                            {L6 + C01, B4},
                            {L5 + C01, B4},
                            {L6 + C04, B7},
                            {L5 + C04, B7},
                            {L4 + C04, B7},
                            {L7 + C01, B5}
                        }, /* 2    C17     P5     */
                        {
                            {L4 + C04, B6},
                            {L3 + C01, B6},
                            {L3 + C01, B7},
                            {L6 + C01, B7},
                            {L7 + C01, B6},
                            {L7 + C04, B5},
                            {L6 + C04, B6},
                            {L5 + C04, B6},
                            {L4 + C01, B6},
                            {L4 + C01, B7},
                            {L5 + C01, B7},
                            {L6 + C01, B6},
                            {L5 + C01, B6},
                            {L6 + C04, B5},
                            {L5 + C04, B5},
                            {L4 + C04, B5},
                            {L7 + C01, B7}
                        }, /* 2    C18     P6     */
                        {
                            {L4 + C04, B4},
                            {L3 + C00, B0},
                            {L3 + C00, B1},
                            {L6 + C00, B1},
                            {L7 + C00, B0},
                            {L7 + C04, B3},
                            {L6 + C04, B4},
                            {L5 + C04, B4},
                            {L4 + C00, B0},
                            {L4 + C00, B1},
                            {L5 + C00, B1},
                            {L6 + C00, B0},
                            {L5 + C00, B0},
                            {L6 + C04, B3},
                            {L5 + C04, B3},
                            {L4 + C04, B3},
                            {L7 + C00, B1}
                        }, /* 2    C19     P7     */
                        {
                            {L4 + C04, B2},
                            {L3 + C00, B2},
                            {L3 + C00, B3},
                            {L6 + C00, B3},
                            {L7 + C00, B2},
                            {L7 + C04, B1},
                            {L6 + C04, B2},
                            {L5 + C04, B2},
                            {L4 + C00, B2},
                            {L4 + C00, B3},
                            {L5 + C00, B3},
                            {L6 + C00, B2},
                            {L5 + C00, B2},
                            {L6 + C04, B1},
                            {L5 + C04, B1},
                            {L4 + C04, B1},
                            {L7 + C00, B3}
                        }, /* 2    C20     P8     */
                        {
                            {L4 + C05, B7},
                            {L3 + C00, B4},
                            {L3 + C00, B5},
                            {L6 + C00, B5},
                            {L7 + C00, B4},
                            {L7 + C05, B6},
                            {L6 + C05, B7},
                            {L5 + C05, B7},
                            {L4 + C00, B4},
                            {L4 + C00, B5},
                            {L5 + C00, B5},
                            {L6 + C00, B4},
                            {L5 + C00, B4},
                            {L6 + C05, B6},
                            {L5 + C05, B6},
                            {L4 + C05, B6},
                            {L7 + C00, B5}
                        }, /* 2    C21     P9     */
                        {
                            {L4 + C05, B5},
                            {L3 + C00, B6},
                            {L3 + C00, B7},
                            {L6 + C00, B7},
                            {L7 + C00, B6},
                            {L7 + C05, B4},
                            {L6 + C05, B5},
                            {L5 + C05, B5},
                            {L4 + C00, B6},
                            {L4 + C00, B7},
                            {L5 + C00, B7},
                            {L6 + C00, B6},
                            {L5 + C00, B6},
                            {L6 + C05, B4},
                            {L5 + C05, B4},
                            {L4 + C05, B4},
                            {L7 + C00, B7}
                        }, /* 2    C22     P10    */
                        {
                            {L4 + C05, B3},
                            {L3 + C11, B7},
                            {L3 + C10, B0},
                            {L6 + C10, B0},
                            {L7 + C11, B7},
                            {L7 + C05, B2},
                            {L6 + C05, B3},
                            {L5 + C05, B3},
                            {L4 + C11, B7},
                            {L4 + C10, B0},
                            {L5 + C10, B0},
                            {L6 + C11, B7},
                            {L5 + C11, B7},
                            {L6 + C05, B2},
                            {L5 + C05, B2},
                            {L4 + C05, B2},
                            {L7 + C10, B0}
                        }, /* 2    C23     P11    */
                        {
                            {L4 + C05, B1},
                            {L3 + C10, B1},
                            {L3 + C10, B2},
                            {L6 + C10, B2},
                            {L7 + C10, B1},
                            {L7 + C05, B0},
                            {L6 + C05, B1},
                            {L5 + C05, B1},
                            {L4 + C10, B1},
                            {L4 + C10, B2},
                            {L5 + C10, B2},
                            {L6 + C10, B1},
                            {L5 + C10, B1},
                            {L6 + C05, B0},
                            {L5 + C05, B0},
                            {L4 + C05, B0},
                            {L7 + C10, B2}
                        }, /* 2    C24     P12    */
                        {
                            {L4 + C06, B7},
                            {L3 + C10, B3},
                            {L3 + C10, B4},
                            {L6 + C10, B4},
                            {L7 + C10, B3},
                            {L7 + C06, B6},
                            {L6 + C06, B7},
                            {L5 + C06, B7},
                            {L4 + C10, B3},
                            {L4 + C10, B4},
                            {L5 + C10, B4},
                            {L6 + C10, B3},
                            {L5 + C10, B3},
                            {L6 + C06, B6},
                            {L5 + C06, B6},
                            {L4 + C06, B6},
                            {L7 + C10, B4}
                        }, /* 2    C25     P13    */
                        {
                            {L4 + C06, B5},
                            {L3 + C10, B5},
                            {L3 + C10, B6},
                            {L6 + C10, B6},
                            {L7 + C10, B5},
                            {L7 + C06, B3},
                            {L6 + C06, B5},
                            {L5 + C06, B5},
                            {L4 + C10, B5},
                            {L4 + C10, B6},
                            {L5 + C10, B6},
                            {L6 + C10, B5},
                            {L5 + C10, B5},
                            {L6 + C06, B3},
                            {L5 + C06, B3},
                            {L4 + C06, B3},
                            {L7 + C10, B6}
                        }, /* 2    C26     P14    */
                        {
                            {L4 + C06, B2},
                            {L3 + C10, B7},
                            {L3 + C09, B0},
                            {L6 + C09, B0},
                            {L7 + C10, B7},
                            {L7 + C06, B1},
                            {L6 + C06, B2},
                            {L5 + C06, B2},
                            {L4 + C10, B7},
                            {L4 + C09, B0},
                            {L5 + C09, B0},
                            {L6 + C10, B7},
                            {L5 + C10, B7},
                            {L6 + C06, B1},
                            {L5 + C06, B1},
                            {L4 + C06, B1},
                            {L7 + C09, B0}
                        }, /* 2    C27     P15    */
                        {
                            {L4 + C07, B7},
                            {L3 + C09, B1},
                            {L3 + C09, B2},
                            {L6 + C09, B2},
                            {L7 + C09, B1},
                            {L7 + C07, B6},
                            {L6 + C07, B7},
                            {L5 + C07, B7},
                            {L4 + C09, B1},
                            {L4 + C09, B2},
                            {L5 + C09, B2},
                            {L6 + C09, B1},
                            {L5 + C09, B1},
                            {L6 + C07, B6},
                            {L5 + C07, B6},
                            {L4 + C07, B6},
                            {L7 + C09, B2}
                        }, /* 2    C28     P16    */
                        {
                            {L4 + C07, B5},
                            {L3 + C09, B3},
                            {L3 + C09, B4},
                            {L6 + C09, B4},
                            {L7 + C09, B3},
                            {L7 + C07, B3},
                            {L6 + C07, B5},
                            {L5 + C07, B5},
                            {L4 + C09, B3},
                            {L4 + C09, B4},
                            {L5 + C09, B4},
                            {L6 + C09, B3},
                            {L5 + C09, B3},
                            {L6 + C07, B3},
                            {L5 + C07, B3},
                            {L4 + C07, B3},
                            {L7 + C09, B4}
                        }, /* 2    C29     P17    */
                        {
                            {L4 + C07, B2},
                            {L3 + C09, B5},
                            {L3 + C09, B6},
                            {L6 + C09, B6},
                            {L7 + C09, B5},
                            {L7 + C07, B1},
                            {L6 + C07, B2},
                            {L5 + C07, B2},
                            {L4 + C09, B5},
                            {L4 + C09, B6},
                            {L5 + C09, B6},
                            {L6 + C09, B5},
                            {L5 + C09, B5},
                            {L6 + C07, B1},
                            {L5 + C07, B1},
                            {L4 + C07, B1},
                            {L7 + C09, B6}
                        }, /* 2    C30     P18    */
                        {
                            {L4 + C08, B7},
                            {L3 + C09, B7},
                            {L3 + C08, B0},
                            {L6 + C08, B0},
                            {L7 + C09, B7},
                            {L7 + C08, B6},
                            {L6 + C08, B7},
                            {L5 + C08, B7},
                            {L4 + C09, B7},
                            {L4 + C08, B0},
                            {L5 + C08, B0},
                            {L6 + C09, B7},
                            {L5 + C09, B7},
                            {L6 + C08, B6},
                            {L5 + C08, B6},
                            {L4 + C08, B6},
                            {L7 + C08, B0}
                        }, /* 2    C31     P19    */
                        {
                            {L4 + C08, B5},
                            {L3 + C08, B1},
                            {L3 + C08, B2},
                            {L6 + C08, B2},
                            {L7 + C08, B1},
                            {L7 + C08, B3},
                            {L6 + C08, B5},
                            {L5 + C08, B5},
                            {L4 + C08, B1},
                            {L4 + C08, B2},
                            {L5 + C08, B2},
                            {L6 + C08, B1},
                            {L5 + C08, B1},
                            {L6 + C08, B3},
                            {L5 + C08, B3},
                            {L4 + C08, B3},
                            {L7 + C08, B2}
                        } /* 2    C32     P20    */
                    };

                    //------------------------------ Variable Definition ------------------------//
                    // \var    tabUnicode
                    /// \brief  Table with the character UTF16 codes
                    static const uint16_t tabUnicode[SIZE_TAB] = {
                        0x0020, // Space
                        0x0022, // "
                        0x0024, // $
                        0x0025, // %
                        0x0027, // '
                        0x0028, // (
                        0x0029, // )
                        0x002A, // *
                        0x002B, // +
                        0x002D, // -
                        0x002E, // .
                        0x002F, // /
                        0x0030, // 0
                        0x0031, // 1
                        0x0032, // 2
                        0x0033, // 3
                        0x0034, // 4
                        0x0035, // 5
                        0x0036, // 6
                        0x0037, // 7
                        0x0038, // 8
                        0x0039, // 9
                        0x003C, // <
                        0x003D, // =
                        0x003E, // >
                        0x003F, // ?
                        0x0040, // @
                        0x0041, // A
                        0x0042, // B
                        0x0043, // C
                        0x0044, // D
                        0x0045, // E
                        0x0046, // F
                        0x0047, // G
                        0x0048, // H
                        0x0049, // I
                        0x004A, // J
                        0x004B, // K
                        0x004C, // L
                        0x004D, // M
                        0x004E, // N
                        0x004F, // O
                        0x0050, // P
                        0x0051, // Q
                        0x0052, // R
                        0x0053, // S
                        0x0054, // T
                        0x0055, // U
                        0x0056, // V
                        0x0057, // W
                        0x0058, // X
                        0x0059, // Y
                        0x005A, // Z
                        0x005B, // [
                        0x005C, /* \ */
                        0x005D, // ]
                        0x005E, // ^
                        0x005F, // _
                        0x0060, // `
                        0x0061, // a
                        0x0062, // b
                        0x0063, // c
                        0x0064, // d
                        0x0065, // e
                        0x0066, // f
                        0x0067, // g
                        0x0068, // h
                        0x0069, // i
                        0x006A, // j
                        0x006B, // k
                        0x006C, // l
                        0x006D, // m
                        0x006E, // n
                        0x006F, // o
                        0x0070, // p
                        0x0071, // q
                        0x0072, // r
                        0x0073, // s
                        0x0074, // t
                        0x0075, // u
                        0x0076, // v
                        0x0077, // w
                        0x0078, // x
                        0x0079, // y
                        0x007A, // z
                        0x007B, // {
                        0x007C, // |
                        0x007D, // }
                        0x0080, // S étroit de l'unité seconde
                        0x0081, // Autre petit K
                        0x0082, // Autre grand K
                        0x0083, // Autre petit s
                        0x00B0, // °
                        0x0410, // A pointu cyrillique
                        0x0411, // B tordu cyrillique
                        0x0412, // B cyrillique
                        0x0413, // Gamma cyrillique
                        0x0414, // Delta cyrillique
                        0x0415, // E cyrillique
                        0x0416, // X barre cyrillique
                        0x0417, // Sigma miroir cyri.llique
                        0x0418, // N miroir cyrillique
                        0x0419, // N barré miroir cyrillique
                        0x041A, // K cyrillique
                        0x041B, // A pointu sans barre cyrillique
                        0x041C, // M cyrillique
                        0x041D, // H cyrillique
                        0x041E, // O cyrillique
                        0x041F, // PI cyrillique
                        0x0420, // P cyrillique
                        0x0421, // C cyrillique
                        0x0422, // T cyrillique
                        0x0423, // Y étroit cyrillique
                        0x0424, // Phi cyrillique
                        0x0425, // X cyrillique
                        0x0426, // U cyrillique
                        0x0427, // 4 large cyrillique
                        0x0428, // W plat cyrillique
                        0x0429, // W, plat cyrillique
                        0x042A, // b barré cyrillique
                        0x042B, // U plein cyrillique
                        0x042C, // b large cyrillique
                        0x042D, // 3 large miroir cyrillique
                        0x042E, // KO cyrillique
                        0x042F, // R large miroir cyrillique
                        ',', // _
                        ',', // , virgule
                        '>', //pour remplacer la fleche haut
                        '<' //pour remplacer la fleche bas
                    };


                    // \var    tabSegments
                    /// \brief  Table with the binary codes of the segments to display
                    ///         for each character defined in tabUnicode.
                    static const uint32_t tabSegments[SIZE_TAB] = {
                        0, // Espace
                        A7 + A8, // "
                        A0 + A1 + A3 + A4 + A5 + A7 + A8 + A10 + A12 + A14, // $
                        A0 + A3 + A4 + A7 + A8 + A9 + A10 + A12 + A13 + A14, // %
                        A9, // '
                        A0 + A5 + A6 + A7, // (
                        A1 + A2 + A3 + A4, // )
                        A8 + A9 + A10 + A11 + A12 + A13 + A14 + A15, // *
                        A8 + A10 + A12 + A14, // +
                        A10 + A14, // -
                        A16, // .
                        A9 + A13, // /
                        //  A0+A1+A2+A3+A4+A5+A6+A7,                // 0
                        A1 + A2 + A3 + A4 + A8 + A12, // 0
                        A2 + A3 + A9, // 1
                        A0 + A1 + A2 + A4 + A5 + A6 + A10 + A14, // 2
                        A0 + A1 + A2 + A3 + A4 + A5 + A10 + A14, // 3
                        A2 + A3 + A7 + A10 + A14, // 4
                        A0 + A1 + A3 + A4 + A5 + A7 + A10 + A14, // 5
                        A0 + A1 + A3 + A4 + A5 + A6 + A7 + A10 + A14, // 6
                        A0 + A1 + A2 + A3, // 7
                        A0 + A1 + A2 + A3 + A4 + A5 + A6 + A7 + A10 + A14, // 8
                        A0 + A1 + A2 + A3 + A4 + A5 + A7 + A10 + A14, // 9
                        A9 + A11, // <
                        A4 + A5 + A10 + A14, // =
                        A13 + A15, // >
                        A0 + A1 + A2 + A10 + A12, // ?
                        A0 + A1 + A2 + A3 + A4 + A6 + A7 + A10 + A12, // @
                        A0 + A1 + A2 + A3 + A6 + A7 + A10 + A14, // A
                        A0 + A3 + A4 + A5 + A6 + A7 + A8 + A10 + A14, // B
                        A0 + A1 + A4 + A5 + A6 + A7, // C
                        A0 + A1 + A2 + A3 + A4 + A5 + A8 + A12, // D
                        A0 + A1 + A4 + A5 + A6 + A7 + A14, // E
                        A0 + A1 + A6 + A7 + A14, // F
                        A0 + A1 + A3 + A4 + A5 + A6 + A7 + A10, // G
                        A2 + A3 + A6 + A7 + A10 + A14, // H
                        A0 + A1 + A4 + A5 + A8 + A12, // I
                        A2 + A3 + A4 + A5 + A6, // J
                        A3 + A6 + A7 + A9 + A10 + A14, // K
                        A4 + A5 + A6 + A7, // L
                        A2 + A3 + A6 + A7 + A9 + A15, // M
                        A2 + A3 + A6 + A7 + A11 + A15, // N
                        A0 + A1 + A2 + A3 + A4 + A5 + A6 + A7, // O
                        A0 + A1 + A2 + A6 + A7 + A10 + A14, // P
                        A0 + A1 + A2 + A3 + A4 + A5 + A6 + A7 + A11, // Q
                        A0 + A1 + A2 + A6 + A7 + A10 + A11 + A14, // R
                        A0 + A1 + A3 + A4 + A5 + A10 + A15, // S
                        A0 + A1 + A8 + A12, // T
                        A2 + A3 + A4 + A5 + A6 + A7, // U
                        A6 + A7 + A9 + A13, // V
                        A2 + A3 + A6 + A7 + A11 + A13, // W
                        A9 + A11 + A13 + A15, // X
                        A9 + A12 + A15, // Y
                        A0 + A1 + A4 + A5 + A9 + A13, // Z
                        A0 + A5 + A6 + A7, // [
                        A11 + A15, /* \ */
                        A1 + A2 + A3 + A4, // ]
                        A11 + A13, // ^
                        A4 + A5, // _
                        A15, // `
                        A4 + A5 + A6 + A12 + A14, // a
                        A5 + A6 + A7 + A12 + A14, // b
                        A5 + A6 + A14, // c
                        A5 + A6 + A8 + A12 + A14, // d
                        A5 + A6 + A13 + A14, // e
                        A0 + A6 + A7 + A14, // f
                        A0 + A5 + A7 + A8 + A12 + A14, // g
                        A6 + A7 + A12 + A14, // h
                        A12, // i
                        A2 + A3 + A4, // j
                        A8 + A9 + A11 + A12, // k
                        A8 + A12, // l
                        A3 + A6 + A10 + A12 + A14, // m
                        A6 + A12 + A14, // n
                        A5 + A6 + A12 + A14, // o
                        A0 + A6 + A7 + A8 + A14, // p
                        A1 + A2 + A3 + A8 + A10, // q
                        A6 + A14, // r
                        A10 + A11 + A4, // s
                        A8 + A10 + A12, // t
                        A5 + A6 + A12, // u
                        A6 + A13, // v
                        A3 + A6 + A11 + A13, // w
                        A4 + A5 + A10 + A12 + A14, // x
                        A5 + A7 + A8 + A12 + A14, // y
                        A5 + A13 + A14, // z
                        A1 + A4 + A8 + A12 + A14, // {
                        A2 + A3, // |
                        A0 + A5 + A8 + A10 + A12, // }
                        A1 + A8 + A10 + A3 + A4, // S étroit de l'unité seconde
                        A8 + A9 + A10 + A3 + A12, // Autre petit k
                        A7 + A6 + A14 + A9 + A11, // autre grand K
                        A13 + A12 + A5, // Autre petit s
                        A0 + A8 + A14 + A7, // °
                        A0 + A1 + A2 + A3 + A6 + A7 + A10 + A14, // A pointu cyrillique
                        A0 + A1 + A3 + A4 + A5 + A6 + A7 + A10 + A14, // B tordu cyrillique
                        A0 + A3 + A4 + A5 + A6 + A7 + A8 + A10 + A14, // B cyrillique
                        A0 + A1 + A6 + A7, // Gamma cyrillique
                        A1 + A2 + A3 + A6 + A8 + A10 + A14, // Delta cyrillique
                        A0 + A1 + A4 + A5 + A6 + A7 + A14, // E cyrillique
                        A8 + A9 + A11 + A12 + A13 + A15, // X barre cyrillique
                        A0 + A1 + A4 + A5 + A9 + A11 + A14, // Sigma miroir cyrillique
                        A2 + A3 + A6 + A7 + A9 + A13, // N miroir cyrillique
                        A2 + A3 + A6 + A7 + A9 + A13, // N barré miroir cyrillique
                        A6 + A7 + A9 + A11 + A14, // K cyrillique
                        A1 + A2 + A3 + A8 + A13, // A pointu sans barre cyrillique
                        A2 + A3 + A6 + A7 + A9 + A15, // M cyrillique
                        A2 + A3 + A6 + A7 + A10 + A14, // H cyrillique
                        A0 + A1 + A2 + A3 + A4 + A5 + A6 + A7, // O cyrillique
                        A0 + A1 + A2 + A3 + A6 + A7, // PI cyrillique
                        A0 + A1 + A2 + A6 + A7 + A10 + A14, // P cyrillique
                        A0 + A1 + A4 + A5 + A6 + A7, // C cyrillique
                        A0 + A1 + A8 + A12, // T cyrillique
                        A2 + A3 + A4 + A5 + A7 + A10 + A14, // Y étroit cyrillique
                        A0 + A1 + A2 + A7 + A8 + A10 + A12 + A14, // Phi cyrillique
                        A9 + A11 + A13 + A15, // X cyrillique
                        A4 + A5 + A6 + A7 + A8 + A12, // U cyrillique
                        A2 + A3 + A7 + A10 + A14, // 4 large cyrillique
                        A2 + A3 + A4 + A5 + A6 + A7 + A8 + A12, // W plat cyrillique
                        A2 + A3 + A4 + A5 + A6 + A7 + A8 + A12 + A16, // W, plat cyrillique
                        A0 + A3 + A4 + A8 + A10 + A12, // b barré cyrillique
                        A2 + A3 + A5 + A6 + A7 + A12 + A14, // U plein cyrillique
                        A3 + A4 + A8 + A10 + A12, // b large cyrillique
                        A0 + A1 + A2 + A3 + A4 + A5 + A10, // 3 large miroir cyrillique
                        A1 + A2 + A3 + A4 + A6 + A7 + A8 + A12 + A14, // KO cyrillique
                        A0 + A1 + A2 + A3 + A7 + A10 + A13 + A14, // R large miroir cyrillique
                        A5, //pour coder le tiret
                        A13, //pour coder la virgule
                        A1 + A2 + A9 + A13, //pour remplacer la fleche haut
                        A3 + A4 + A11 + A15 //pour remplacer la fleche bas
                    };

                    class EM6124 : public sc_module {
                    public:

                        /* socket for spi management */
                        tlm_utils::simple_target_socket<EM6124> mosi; //master out slave in port
                        tlm_utils::simple_initiator_socket<EM6124> miso; //slave in master out port
                        /* socket for spi management */


                        typedef enum {
                            INIT_EM_6124, MAIN_JOB
                        } STATE;
                        STATE State;
                        uint8_t EM6124_Memory [ NB_FRAME * FRAME_SIZE];
                        uint8_t Byte2Receive, NbByte, cpt, buffer[64];
                        uint16_t buffer16[32];

                        void lcd_receive_frame(tlm::tlm_generic_payload& trans, sc_time& delay);
                        int ReadDeviceSegments(uint8_t *);


                        EM6124(const sc_module_name& name);
                        virtual ~EM6124();
                        uint8_t stream2send[7], stream2receive[7];

                    private:
                        char Full_Screen [32 + 1];

                    };
                } // end of namespace lcd_driver
            } // end of namespace signal_converter
        } // end of namespace tlm2
    } // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_TLM2_SIGNAL_CONVERTER_GENERIC_EM6124_EM6124_HH__

