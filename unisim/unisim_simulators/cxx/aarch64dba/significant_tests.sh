set -x

${EXE} aarch64 0x4005e8 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x4005ec 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x4005f0 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x4005f4 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x4005f8 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x4005fc 0x52800c20; echo # mov	w0, #0x61                       // #97
${EXE} aarch64 0x400600 0x4a000260; echo # eor	w0, w19, w0
${EXE} aarch64 0x400604 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400608 0x7101727f; echo # cmp	w19, #0x5c
${EXE} aarch64 0x40060c 0x54000060; echo # b.eq	400618 <check_char_0+0x30>
${EXE} aarch64 0x400610 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x400614 0x94001721; echo # bl	406298 <exit>
${EXE} aarch64 0x400618 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x40061c 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x400620 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x400624 0xd65f03c0; echo # ret
${EXE} aarch64 0x400628 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x40062c 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x400630 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x400634 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x400638 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x40063c 0x52800d60; echo # mov	w0, #0x6b                       // #107
${EXE} aarch64 0x400640 0x4a000260; echo # eor	w0, w19, w0
${EXE} aarch64 0x400644 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400648 0x11010e60; echo # add	w0, w19, #0x43
${EXE} aarch64 0x40064c 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400650 0x7101a67f; echo # cmp	w19, #0x69
${EXE} aarch64 0x400654 0x54000060; echo # b.eq	400660 <check_char_1+0x38>
${EXE} aarch64 0x400658 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x40065c 0x9400170f; echo # bl	406298 <exit>
${EXE} aarch64 0x400660 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x400664 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x400668 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x40066c 0xd65f03c0; echo # ret
${EXE} aarch64 0x400670 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x400674 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x400678 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x40067c 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x400680 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x400684 0x1100f660; echo # add	w0, w19, #0x3d
${EXE} aarch64 0x400688 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x40068c 0x2a1303e0; echo # mov	w0, w19
${EXE} aarch64 0x400690 0x0b000000; echo # add	w0, w0, w0
${EXE} aarch64 0x400694 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400698 0x7103f27f; echo # cmp	w19, #0xfc
${EXE} aarch64 0x40069c 0x54000060; echo # b.eq	4006a8 <check_char_2+0x38>
${EXE} aarch64 0x4006a0 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x4006a4 0x940016fd; echo # bl	406298 <exit>
${EXE} aarch64 0x4006a8 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x4006ac 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x4006b0 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x4006b4 0xd65f03c0; echo # ret
${EXE} aarch64 0x4006b8 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x4006bc 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x4006c0 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x4006c4 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x4006c8 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x4006cc 0x12800d40; echo # mov	w0, #0xffffff95                 // #-107
${EXE} aarch64 0x4006d0 0x4a000260; echo # eor	w0, w19, w0
${EXE} aarch64 0x4006d4 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x4006d8 0x71036e7f; echo # cmp	w19, #0xdb
${EXE} aarch64 0x4006dc 0x54000060; echo # b.eq	4006e8 <check_char_3+0x30>
${EXE} aarch64 0x4006e0 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x4006e4 0x940016ed; echo # bl	406298 <exit>
${EXE} aarch64 0x4006e8 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x4006ec 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x4006f0 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x4006f4 0xd65f03c0; echo # ret
${EXE} aarch64 0x4006f8 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x4006fc 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x400700 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x400704 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x400708 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x40070c 0x52800260; echo # mov	w0, #0x13                       // #19
${EXE} aarch64 0x400710 0x4a000260; echo # eor	w0, w19, w0
${EXE} aarch64 0x400714 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400718 0x2a1303e0; echo # mov	w0, w19
${EXE} aarch64 0x40071c 0x0b000000; echo # add	w0, w0, w0
${EXE} aarch64 0x400720 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400724 0x71023a7f; echo # cmp	w19, #0x8e
${EXE} aarch64 0x400728 0x54000060; echo # b.eq	400734 <check_char_4+0x3c>
${EXE} aarch64 0x40072c 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x400730 0x940016da; echo # bl	406298 <exit>
${EXE} aarch64 0x400734 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x400738 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x40073c 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x400740 0xd65f03c0; echo # ret
${EXE} aarch64 0x400744 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x400748 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x40074c 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x400750 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x400754 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x400758 0x528000a0; echo # mov	w0, #0x5                        // #5
${EXE} aarch64 0x40075c 0x4a000260; echo # eor	w0, w19, w0
${EXE} aarch64 0x400760 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400764 0x2a1303e1; echo # mov	w1, w19
${EXE} aarch64 0x400768 0x2a0103e0; echo # mov	w0, w1
${EXE} aarch64 0x40076c 0x531e1400; echo # ubfiz	w0, w0, #2, #6
${EXE} aarch64 0x400770 0x4b010000; echo # sub	w0, w0, w1
${EXE} aarch64 0x400774 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400778 0x7103927f; echo # cmp	w19, #0xe4
${EXE} aarch64 0x40077c 0x54000060; echo # b.eq	400788 <check_char_5+0x44>
${EXE} aarch64 0x400780 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x400784 0x940016c5; echo # bl	406298 <exit>
${EXE} aarch64 0x400788 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x40078c 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x400790 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x400794 0xd65f03c0; echo # ret
${EXE} aarch64 0x400798 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x40079c 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x4007a0 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x4007a4 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x4007a8 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x4007ac 0x11011e60; echo # add	w0, w19, #0x47
${EXE} aarch64 0x4007b0 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x4007b4 0x71022a7f; echo # cmp	w19, #0x8a
${EXE} aarch64 0x4007b8 0x54000060; echo # b.eq	4007c4 <check_char_6+0x2c>
${EXE} aarch64 0x4007bc 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x4007c0 0x940016b6; echo # bl	406298 <exit>
${EXE} aarch64 0x4007c4 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x4007c8 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x4007cc 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x4007d0 0xd65f03c0; echo # ret
${EXE} aarch64 0x4007d4 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x4007d8 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x4007dc 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x4007e0 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x4007e4 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x4007e8 0x52800520; echo # mov	w0, #0x29                       // #41
${EXE} aarch64 0x4007ec 0x4a000260; echo # eor	w0, w19, w0
${EXE} aarch64 0x4007f0 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x4007f4 0x71019a7f; echo # cmp	w19, #0x66
${EXE} aarch64 0x4007f8 0x54000060; echo # b.eq	400804 <check_char_7+0x30>
${EXE} aarch64 0x4007fc 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x400800 0x940016a6; echo # bl	406298 <exit>
${EXE} aarch64 0x400804 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x400808 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x40080c 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x400810 0xd65f03c0; echo # ret
${EXE} aarch64 0x400814 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x400818 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x40081c 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x400820 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x400824 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x400828 0x1100a660; echo # add	w0, w19, #0x29
${EXE} aarch64 0x40082c 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400830 0x1100d660; echo # add	w0, w19, #0x35
${EXE} aarch64 0x400834 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400838 0x7102c27f; echo # cmp	w19, #0xb0
${EXE} aarch64 0x40083c 0x54000060; echo # b.eq	400848 <check_char_8+0x34>
${EXE} aarch64 0x400840 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x400844 0x94001695; echo # bl	406298 <exit>
${EXE} aarch64 0x400848 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x40084c 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x400850 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x400854 0xd65f03c0; echo # ret
${EXE} aarch64 0x400858 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x40085c 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x400860 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x400864 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x400868 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x40086c 0x528007a0; echo # mov	w0, #0x3d                       // #61
${EXE} aarch64 0x400870 0x4a000260; echo # eor	w0, w19, w0
${EXE} aarch64 0x400874 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400878 0x1100a660; echo # add	w0, w19, #0x29
${EXE} aarch64 0x40087c 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400880 0x11002e60; echo # add	w0, w19, #0xb
${EXE} aarch64 0x400884 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x400888 0x7102b27f; echo # cmp	w19, #0xac
${EXE} aarch64 0x40088c 0x54000060; echo # b.eq	400898 <check_char_9+0x40>
${EXE} aarch64 0x400890 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x400894 0x94001681; echo # bl	406298 <exit>
${EXE} aarch64 0x400898 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x40089c 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x4008a0 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x4008a4 0xd65f03c0; echo # ret
${EXE} aarch64 0x4008a8 0xa9bd7bfd; echo # stp	x29, x30, [sp,#-48]!
${EXE} aarch64 0x4008ac 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x4008b0 0xf9000bf3; echo # str	x19, [sp,#16]
${EXE} aarch64 0x4008b4 0x3900bfa0; echo # strb	w0, [x29,#47]
${EXE} aarch64 0x4008b8 0x3940bfb3; echo # ldrb	w19, [x29,#47]
${EXE} aarch64 0x4008bc 0x528005e0; echo # mov	w0, #0x2f                       // #47
${EXE} aarch64 0x4008c0 0x4a000260; echo # eor	w0, w19, w0
${EXE} aarch64 0x4008c4 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x4008c8 0x11007660; echo # add	w0, w19, #0x1d
${EXE} aarch64 0x4008cc 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x4008d0 0x11010e60; echo # add	w0, w19, #0x43
${EXE} aarch64 0x4008d4 0x53001c13; echo # uxtb	w19, w0
${EXE} aarch64 0x4008d8 0x7101ca7f; echo # cmp	w19, #0x72
${EXE} aarch64 0x4008dc 0x54000060; echo # b.eq	4008e8 <check_char_10+0x40>
${EXE} aarch64 0x4008e0 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x4008e4 0x9400166d; echo # bl	406298 <exit>
${EXE} aarch64 0x4008e8 0x52800020; echo # mov	w0, #0x1                        // #1
${EXE} aarch64 0x4008ec 0xf9400bf3; echo # ldr	x19, [sp,#16]
${EXE} aarch64 0x4008f0 0xa8c37bfd; echo # ldp	x29, x30, [sp],#48
${EXE} aarch64 0x4008f4 0xd65f03c0; echo # ret
${EXE} aarch64 0x4008f8 0xa9be7bfd; echo # stp	x29, x30, [sp,#-32]!
${EXE} aarch64 0x4008fc 0x910003fd; echo # mov	x29, sp
${EXE} aarch64 0x400900 0xf9000fa0; echo # str	x0, [x29,#24]
${EXE} aarch64 0x400904 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400908 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40090c 0x97ffff37; echo # bl	4005e8 <check_char_0>
${EXE} aarch64 0x400910 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400914 0x91000400; echo # add	x0, x0, #0x1
${EXE} aarch64 0x400918 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40091c 0x97ffff43; echo # bl	400628 <check_char_1>
${EXE} aarch64 0x400920 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400924 0x91000800; echo # add	x0, x0, #0x2
${EXE} aarch64 0x400928 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40092c 0x97ffff51; echo # bl	400670 <check_char_2>
${EXE} aarch64 0x400930 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400934 0x91000c00; echo # add	x0, x0, #0x3
${EXE} aarch64 0x400938 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40093c 0x97ffff5f; echo # bl	4006b8 <check_char_3>
${EXE} aarch64 0x400940 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400944 0x91001000; echo # add	x0, x0, #0x4
${EXE} aarch64 0x400948 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40094c 0x97ffff6b; echo # bl	4006f8 <check_char_4>
${EXE} aarch64 0x400950 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400954 0x91001400; echo # add	x0, x0, #0x5
${EXE} aarch64 0x400958 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40095c 0x97ffff7a; echo # bl	400744 <check_char_5>
${EXE} aarch64 0x400960 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400964 0x91001800; echo # add	x0, x0, #0x6
${EXE} aarch64 0x400968 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40096c 0x97ffff8b; echo # bl	400798 <check_char_6>
${EXE} aarch64 0x400970 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400974 0x91001c00; echo # add	x0, x0, #0x7
${EXE} aarch64 0x400978 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40097c 0x97ffff96; echo # bl	4007d4 <check_char_7>
${EXE} aarch64 0x400980 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400984 0x91002000; echo # add	x0, x0, #0x8
${EXE} aarch64 0x400988 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40098c 0x97ffffa2; echo # bl	400814 <check_char_8>
${EXE} aarch64 0x400990 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x400994 0x91002400; echo # add	x0, x0, #0x9
${EXE} aarch64 0x400998 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x40099c 0x97ffffaf; echo # bl	400858 <check_char_9>
${EXE} aarch64 0x4009a0 0xf9400fa0; echo # ldr	x0, [x29,#24]
${EXE} aarch64 0x4009a4 0x91002800; echo # add	x0, x0, #0xa
${EXE} aarch64 0x4009a8 0x39400000; echo # ldrb	w0, [x0]
${EXE} aarch64 0x4009ac 0x97ffffbf; echo # bl	4008a8 <check_char_10>
${EXE} aarch64 0x4009b0 0x52800020; echo # mov	w0, #0x1
${EXE} aarch64 0x4009b4 0xa8c27bfd; echo # ldp	x29, x30, [sp],#32
${EXE} aarch64 0x4009b8 0xd65f03c0; echo # ret
