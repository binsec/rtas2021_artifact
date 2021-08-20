set -ex
${ARMSEC} arm 0x40c8 0xe2a33a9c # adc	r3, r3, #638976
${ARMSEC} arm 0x40c8 0xe2b11912 # adcs	r1, r1, #294912
${ARMSEC} arm 0x40c8 0xe2a230cf # adc	r3, r2, #207
${ARMSEC} arm 0x40c8 0xe2b32259 # adcs	r2, r3, #-1879048187
${ARMSEC} arm 0x40c8 0xe2af311c # adc	r3, pc, #7
${ARMSEC} arm 0x40c8 0xe2bf2409 # adcs	r2, pc, #150994944
${ARMSEC} arm 0x40c8 0xe0a22c42 # adc	r2, r2, r2, asr #24
${ARMSEC} arm 0x40c8 0xe0b33ca3 # adcs	r3, r3, r3, lsr #25
${ARMSEC} arm 0x40c8 0xe0a03ee0 # adc	r3, r0, r0, ror #29
${ARMSEC} arm 0x40c8 0xe0b30ee3 # adcs	r0, r3, r3, ror #29
${ARMSEC} arm 0x40c8 0xe0a224a1 # adc	r2, r2, r1, lsr #9
${ARMSEC} arm 0x40c8 0xe0b22343 # adcs	r2, r2, r3, asr #6
${ARMSEC} arm 0x40c8 0xe0a01ce1 # adc	r1, r0, r1, ror #25
${ARMSEC} arm 0x40c8 0xe0b122c2 # adcs	r2, r1, r2, asr #5
${ARMSEC} arm 0x40c8 0xe0af35a3 # adc	r3, pc, r3, lsr #11
${ARMSEC} arm 0x40c8 0xe0bf0460 # adcs	r0, pc, r0, ror #8
${ARMSEC} arm 0x40c8 0xe0a3346f # adc	r3, r3, pc, ror #8
${ARMSEC} arm 0x40c8 0xe0b2206f # adcs	r2, r2, pc, rrx
${ARMSEC} arm 0x40c8 0xe0af01ef # adc	r0, pc, pc, ror #3
${ARMSEC} arm 0x40c8 0xe0bf16cf # adcs	r1, pc, pc, asr #13
${ARMSEC} arm 0x40c8 0xe0a01943 # adc	r1, r0, r3, asr #18
${ARMSEC} arm 0x40c8 0xe0b015e2 # adcs	r1, r0, r2, ror #11
${ARMSEC} arm 0x40c8 0xe0af2ce3 # adc	r2, pc, r3, ror #25
${ARMSEC} arm 0x40c8 0xe0bf1942 # adcs	r1, pc, r2, asr #18
${ARMSEC} arm 0x40c8 0xe0a3278f # adc	r2, r3, pc, lsl #15
${ARMSEC} arm 0x40c8 0xe0b20a8f # adcs	r0, r2, pc, lsl #21
${ARMSEC} arm 0x40c8 0xe0a22252 # adc	r2, r2, r2, asr r2
${ARMSEC} arm 0x40c8 0xe0b11111 # adcs	r1, r1, r1, lsl r1
${ARMSEC} arm 0x40c8 0xe0a30353 # adc	r0, r3, r3, asr r3
${ARMSEC} arm 0x40c8 0xe0b31333 # adcs	r1, r3, r3, lsr r3
${ARMSEC} arm 0x40c8 0xe0a00353 # adc	r0, r0, r3, asr r3
${ARMSEC} arm 0x40c8 0xe0b00171 # adcs	r0, r0, r1, ror r1
${ARMSEC} arm 0x40c8 0xe0a20032 # adc	r0, r2, r2, lsr r0
${ARMSEC} arm 0x40c8 0xe0b01130 # adcs	r1, r0, r0, lsr r1
${ARMSEC} arm 0x40c8 0xe0a11152 # adc	r1, r1, r2, asr r1
${ARMSEC} arm 0x40c8 0xe0b00071 # adcs	r0, r0, r1, ror r0
${ARMSEC} arm 0x40c8 0xe0a23373 # adc	r3, r2, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0b12212 # adcs	r2, r1, r2, lsl r2
${ARMSEC} arm 0x40c8 0xe0a00230 # adc	r0, r0, r0, lsr r2
${ARMSEC} arm 0x40c8 0xe0b33153 # adcs	r3, r3, r3, asr r1
${ARMSEC} arm 0x40c8 0xe0a12112 # adc	r2, r1, r2, lsl r1
${ARMSEC} arm 0x40c8 0xe0b32332 # adcs	r2, r3, r2, lsr r3
${ARMSEC} arm 0x40c8 0xe0a03212 # adc	r3, r0, r2, lsl r2
${ARMSEC} arm 0x40c8 0xe0b13030 # adcs	r3, r1, r0, lsr r0
${ARMSEC} arm 0x40c8 0xe0a02150 # adc	r2, r0, r0, asr r1
${ARMSEC} arm 0x40c8 0xe0b20372 # adcs	r0, r2, r2, ror r3
${ARMSEC} arm 0x40c8 0xe0a10133 # adc	r0, r1, r3, lsr r1
${ARMSEC} arm 0x40c8 0xe0b10132 # adcs	r0, r1, r2, lsr r1
${ARMSEC} arm 0x40c8 0xe0a11370 # adc	r1, r1, r0, ror r3
${ARMSEC} arm 0x40c8 0xe0b00233 # adcs	r0, r0, r3, lsr r2
${ARMSEC} arm 0x40c8 0xe0a02231 # adc	r2, r0, r1, lsr r2
${ARMSEC} arm 0x40c8 0xe0b10072 # adcs	r0, r1, r2, ror r0
${ARMSEC} arm 0x40c8 0xe0a23073 # adc	r3, r2, r3, ror r0
${ARMSEC} arm 0x40c8 0xe0b02112 # adcs	r2, r0, r2, lsl r1
${ARMSEC} arm 0x40c8 0xe0a03112 # adc	r3, r0, r2, lsl r1
${ARMSEC} arm 0x40c8 0xe0b10352 # adcs	r0, r1, r2, asr r3
${ARMSEC} arm 0x40c8 0xe28005d7 # add	r0, r0, #901775360
${ARMSEC} arm 0x40c8 0xe2911bdf # adds	r1, r1, #228352
${ARMSEC} arm 0x40c8 0xe28326c2 # add	r2, r3, #203423744
${ARMSEC} arm 0x40c8 0xe2920466 # adds	r0, r2, #1711276032
${ARMSEC} arm 0x40c8 0xe28f19a7 # add	r1, pc, #2736128
${ARMSEC} arm 0x40c8 0xe29f2ed3 # adds	r2, pc, #3376
${ARMSEC} arm 0x40c8 0xe0833423 # add	r3, r3, r3, lsr #8
${ARMSEC} arm 0x40c8 0xe0922e02 # adds	r2, r2, r2, lsl #28
${ARMSEC} arm 0x40c8 0xe0832e63 # add	r2, r3, r3, ror #28
${ARMSEC} arm 0x40c8 0xe0931583 # adds	r1, r3, r3, lsl #11
${ARMSEC} arm 0x40c8 0xe0822ba3 # add	r2, r2, r3, lsr #23
${ARMSEC} arm 0x40c8 0xe09119c0 # adds	r1, r1, r0, asr #19
${ARMSEC} arm 0x40c8 0xe0831401 # add	r1, r3, r1, lsl #8
${ARMSEC} arm 0x40c8 0xe0902202 # adds	r2, r0, r2, lsl #4
${ARMSEC} arm 0x40c8 0xe08f3f63 # add	r3, pc, r3, ror #30
${ARMSEC} arm 0x40c8 0xe09f3f63 # adds	r3, pc, r3, ror #30
${ARMSEC} arm 0x40c8 0xe08006cf # add	r0, r0, pc, asr #13
${ARMSEC} arm 0x40c8 0xe09339ef # adds	r3, r3, pc, ror #19
${ARMSEC} arm 0x40c8 0xe08f136f # add	r1, pc, pc, ror #6
${ARMSEC} arm 0x40c8 0xe09f0def # adds	r0, pc, pc, ror #27
${ARMSEC} arm 0x40c8 0xe0812c83 # add	r2, r1, r3, lsl #25
${ARMSEC} arm 0x40c8 0xe0912883 # adds	r2, r1, r3, lsl #17
${ARMSEC} arm 0x40c8 0xe08f3360 # add	r3, pc, r0, ror #6
${ARMSEC} arm 0x40c8 0xe09f19a2 # adds	r1, pc, r2, lsr #19
${ARMSEC} arm 0x40c8 0xe080372f # add	r3, r0, pc, lsr #14
${ARMSEC} arm 0x40c8 0xe09121cf # adds	r2, r1, pc, asr #3
${ARMSEC} arm 0x40c8 0xe0811171 # add	r1, r1, r1, ror r1
${ARMSEC} arm 0x40c8 0xe0900070 # adds	r0, r0, r0, ror r0
${ARMSEC} arm 0x40c8 0xe0830373 # add	r0, r3, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0901010 # adds	r1, r0, r0, lsl r0
${ARMSEC} arm 0x40c8 0xe0833131 # add	r3, r3, r1, lsr r1
${ARMSEC} arm 0x40c8 0xe0900373 # adds	r0, r0, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0830033 # add	r0, r3, r3, lsr r0
${ARMSEC} arm 0x40c8 0xe0903370 # adds	r3, r0, r0, ror r3
${ARMSEC} arm 0x40c8 0xe0800032 # add	r0, r0, r2, lsr r0
${ARMSEC} arm 0x40c8 0xe0900073 # adds	r0, r0, r3, ror r0
${ARMSEC} arm 0x40c8 0xe0823313 # add	r3, r2, r3, lsl r3
${ARMSEC} arm 0x40c8 0xe0932232 # adds	r2, r3, r2, lsr r2
${ARMSEC} arm 0x40c8 0xe0833233 # add	r3, r3, r3, lsr r2
${ARMSEC} arm 0x40c8 0xe0900170 # adds	r0, r0, r0, ror r1
${ARMSEC} arm 0x40c8 0xe0823253 # add	r3, r2, r3, asr r2
${ARMSEC} arm 0x40c8 0xe0921211 # adds	r1, r2, r1, lsl r2
${ARMSEC} arm 0x40c8 0xe0801272 # add	r1, r0, r2, ror r2
${ARMSEC} arm 0x40c8 0xe0903171 # adds	r3, r0, r1, ror r1
${ARMSEC} arm 0x40c8 0xe0820372 # add	r0, r2, r2, ror r3
${ARMSEC} arm 0x40c8 0xe0931013 # adds	r1, r3, r3, lsl r0
${ARMSEC} arm 0x40c8 0xe0812153 # add	r2, r1, r3, asr r1
${ARMSEC} arm 0x40c8 0xe0913150 # adds	r3, r1, r0, asr r1
${ARMSEC} arm 0x40c8 0xe0811230 # add	r1, r1, r0, lsr r2
${ARMSEC} arm 0x40c8 0xe0911072 # adds	r1, r1, r2, ror r0
${ARMSEC} arm 0x40c8 0xe0820073 # add	r0, r2, r3, ror r0
${ARMSEC} arm 0x40c8 0xe0932251 # adds	r2, r3, r1, asr r2
${ARMSEC} arm 0x40c8 0xe0820350 # add	r0, r2, r0, asr r3
${ARMSEC} arm 0x40c8 0xe0930170 # adds	r0, r3, r0, ror r1
${ARMSEC} arm 0x40c8 0xe0820331 # add	r0, r2, r1, lsr r3
${ARMSEC} arm 0x40c8 0xe0923031 # adds	r3, r2, r1, lsr r0
${ARMSEC} arm 0x40c8 0xe201196f # and	r1, r1, #1818624
${ARMSEC} arm 0x40c8 0xe21221e3 # ands	r2, r2, #-1073741768
${ARMSEC} arm 0x40c8 0xe20307c5 # and	r0, r3, #51642368
${ARMSEC} arm 0x40c8 0xe2121cd4 # ands	r1, r2, #54272
${ARMSEC} arm 0x40c8 0xe20f0619 # and	r0, pc, #26214400
${ARMSEC} arm 0x40c8 0xe21f29df # ands	r2, pc, #3653632
${ARMSEC} arm 0x40c8 0xe00001e0 # and	r0, r0, r0, ror #3
${ARMSEC} arm 0x40c8 0xe01009e0 # ands	r0, r0, r0, ror #19
${ARMSEC} arm 0x40c8 0xe0013601 # and	r3, r1, r1, lsl #12
${ARMSEC} arm 0x40c8 0xe0101860 # ands	r1, r0, r0, ror #16
${ARMSEC} arm 0x40c8 0xe0033a40 # and	r3, r3, r0, asr #20
${ARMSEC} arm 0x40c8 0xe0100a42 # ands	r0, r0, r2, asr #20
${ARMSEC} arm 0x40c8 0xe0032962 # and	r2, r3, r2, ror #18
${ARMSEC} arm 0x40c8 0xe01218c1 # ands	r1, r2, r1, asr #17
${ARMSEC} arm 0x40c8 0xe00f35e3 # and	r3, pc, r3, ror #11
${ARMSEC} arm 0x40c8 0xe01f15c1 # ands	r1, pc, r1, asr #11
${ARMSEC} arm 0x40c8 0xe0011f2f # and	r1, r1, pc, lsr #30
${ARMSEC} arm 0x40c8 0xe0122d0f # ands	r2, r2, pc, lsl #26
${ARMSEC} arm 0x40c8 0xe00f174f # and	r1, pc, pc, asr #14
${ARMSEC} arm 0x40c8 0xe01f3d8f # ands	r3, pc, pc, lsl #27
${ARMSEC} arm 0x40c8 0xe0012243 # and	r2, r1, r3, asr #4
${ARMSEC} arm 0x40c8 0xe0101d22 # ands	r1, r0, r2, lsr #26
${ARMSEC} arm 0x40c8 0xe00f1882 # and	r1, pc, r2, lsl #17
${ARMSEC} arm 0x40c8 0xe01f3ae0 # ands	r3, pc, r0, ror #21
${ARMSEC} arm 0x40c8 0xe003268f # and	r2, r3, pc, lsl #13
${ARMSEC} arm 0x40c8 0xe0110d4f # ands	r0, r1, pc, asr #26
${ARMSEC} arm 0x40c8 0xe0033373 # and	r3, r3, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0111151 # ands	r1, r1, r1, asr r1
${ARMSEC} arm 0x40c8 0xe0021272 # and	r1, r2, r2, ror r2
${ARMSEC} arm 0x40c8 0xe0113111 # ands	r3, r1, r1, lsl r1
${ARMSEC} arm 0x40c8 0xe0000373 # and	r0, r0, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0133272 # ands	r3, r3, r2, ror r2
${ARMSEC} arm 0x40c8 0xe0013371 # and	r3, r1, r1, ror r3
${ARMSEC} arm 0x40c8 0xe0121112 # ands	r1, r2, r2, lsl r1
${ARMSEC} arm 0x40c8 0xe0033372 # and	r3, r3, r2, ror r3
${ARMSEC} arm 0x40c8 0xe0111113 # ands	r1, r1, r3, lsl r1
${ARMSEC} arm 0x40c8 0xe0020030 # and	r0, r2, r0, lsr r0
${ARMSEC} arm 0x40c8 0xe0132232 # ands	r2, r3, r2, lsr r2
${ARMSEC} arm 0x40c8 0xe0033113 # and	r3, r3, r3, lsl r1
${ARMSEC} arm 0x40c8 0xe0111051 # ands	r1, r1, r1, asr r0
${ARMSEC} arm 0x40c8 0xe0010150 # and	r0, r1, r0, asr r1
${ARMSEC} arm 0x40c8 0xe0120230 # ands	r0, r2, r0, lsr r2
${ARMSEC} arm 0x40c8 0xe0013050 # and	r3, r1, r0, asr r0
${ARMSEC} arm 0x40c8 0xe0120151 # ands	r0, r2, r1, asr r1
${ARMSEC} arm 0x40c8 0xe0003230 # and	r3, r0, r0, lsr r2
${ARMSEC} arm 0x40c8 0xe0132073 # ands	r2, r3, r3, ror r0
${ARMSEC} arm 0x40c8 0xe0001052 # and	r1, r0, r2, asr r0
${ARMSEC} arm 0x40c8 0xe0123211 # ands	r3, r2, r1, lsl r2
${ARMSEC} arm 0x40c8 0xe0011032 # and	r1, r1, r2, lsr r0
${ARMSEC} arm 0x40c8 0xe0111350 # ands	r1, r1, r0, asr r3
${ARMSEC} arm 0x40c8 0xe0002253 # and	r2, r0, r3, asr r2
${ARMSEC} arm 0x40c8 0xe0110012 # ands	r0, r1, r2, lsl r0
${ARMSEC} arm 0x40c8 0xe0002352 # and	r2, r0, r2, asr r3
${ARMSEC} arm 0x40c8 0xe0120110 # ands	r0, r2, r0, lsl r1
${ARMSEC} arm 0x40c8 0xe0020331 # and	r0, r2, r1, lsr r3
${ARMSEC} arm 0x40c8 0xe0121013 # ands	r1, r2, r3, lsl r0
${ARMSEC} arm 0x40c8 0xe7d2119f # bfc	r1, #3, #16
${ARMSEC} arm 0x40c8 0xe7d82312 # bfi	r2, r2, #6, #19
${ARMSEC} arm 0x40c8 0xe7d60111 # bfi	r0, r1, #2, #21
${ARMSEC} arm 0x40c8 0xe3c22928 # bic	r2, r2, #655360
${ARMSEC} arm 0x40c8 0xe3d33190 # bics	r3, r3, #36
${ARMSEC} arm 0x40c8 0xe3c0198b # bic	r1, r0, #2277376
${ARMSEC} arm 0x40c8 0xe3d21c97 # bics	r1, r2, #38656
${ARMSEC} arm 0x40c8 0xe3cf1c8c # bic	r1, pc, #35840
${ARMSEC} arm 0x40c8 0xe3df328c # bics	r3, pc, #-1073741816
${ARMSEC} arm 0x40c8 0xe1c11ae1 # bic	r1, r1, r1, ror #21
${ARMSEC} arm 0x40c8 0xe1d009a0 # bics	r0, r0, r0, lsr #19
${ARMSEC} arm 0x40c8 0xe1c31d83 # bic	r1, r3, r3, lsl #27
${ARMSEC} arm 0x40c8 0xe1d03e80 # bics	r3, r0, r0, lsl #29
${ARMSEC} arm 0x40c8 0xe1c007c3 # bic	r0, r0, r3, asr #15
${ARMSEC} arm 0x40c8 0xe1d22043 # bics	r2, r2, r3, asr #32
${ARMSEC} arm 0x40c8 0xe1c21aa1 # bic	r1, r2, r1, lsr #21
${ARMSEC} arm 0x40c8 0xe1d21ce1 # bics	r1, r2, r1, ror #25
${ARMSEC} arm 0x40c8 0xe1cf0c20 # bic	r0, pc, r0, lsr #24
${ARMSEC} arm 0x40c8 0xe1df3f03 # bics	r3, pc, r3, lsl #30
${ARMSEC} arm 0x40c8 0xe1c2252f # bic	r2, r2, pc, lsr #10
${ARMSEC} arm 0x40c8 0xe1d22c6f # bics	r2, r2, pc, ror #24
${ARMSEC} arm 0x40c8 0xe1cf05ef # bic	r0, pc, pc, ror #11
${ARMSEC} arm 0x40c8 0xe1df280f # bics	r2, pc, pc, lsl #16
${ARMSEC} arm 0x40c8 0xe1c32fc1 # bic	r2, r3, r1, asr #31
${ARMSEC} arm 0x40c8 0xe1d20ec1 # bics	r0, r2, r1, asr #29
${ARMSEC} arm 0x40c8 0xe1cf0f22 # bic	r0, pc, r2, lsr #30
${ARMSEC} arm 0x40c8 0xe1df3d22 # bics	r3, pc, r2, lsr #26
${ARMSEC} arm 0x40c8 0xe1c1008f # bic	r0, r1, pc, lsl #1
${ARMSEC} arm 0x40c8 0xe1d3012f # bics	r0, r3, pc, lsr #2
${ARMSEC} arm 0x40c8 0xe1c11131 # bic	r1, r1, r1, lsr r1
${ARMSEC} arm 0x40c8 0xe1d00050 # bics	r0, r0, r0, asr r0
${ARMSEC} arm 0x40c8 0xe1c20252 # bic	r0, r2, r2, asr r2
${ARMSEC} arm 0x40c8 0xe1d10171 # bics	r0, r1, r1, ror r1
${ARMSEC} arm 0x40c8 0xe1c22030 # bic	r2, r2, r0, lsr r0
${ARMSEC} arm 0x40c8 0xe1d22313 # bics	r2, r2, r3, lsl r3
${ARMSEC} arm 0x40c8 0xe1c02250 # bic	r2, r0, r0, asr r2
${ARMSEC} arm 0x40c8 0xe1d03370 # bics	r3, r0, r0, ror r3
${ARMSEC} arm 0x40c8 0xe1c33332 # bic	r3, r3, r2, lsr r3
${ARMSEC} arm 0x40c8 0xe1d00032 # bics	r0, r0, r2, lsr r0
${ARMSEC} arm 0x40c8 0xe1c21131 # bic	r1, r2, r1, lsr r1
${ARMSEC} arm 0x40c8 0xe1d32212 # bics	r2, r3, r2, lsl r2
${ARMSEC} arm 0x40c8 0xe1c11371 # bic	r1, r1, r1, ror r3
${ARMSEC} arm 0x40c8 0xe1d11271 # bics	r1, r1, r1, ror r2
${ARMSEC} arm 0x40c8 0xe1c02052 # bic	r2, r0, r2, asr r0
${ARMSEC} arm 0x40c8 0xe1d31371 # bics	r1, r3, r1, ror r3
${ARMSEC} arm 0x40c8 0xe1c10313 # bic	r0, r1, r3, lsl r3
${ARMSEC} arm 0x40c8 0xe1d03252 # bics	r3, r0, r2, asr r2
${ARMSEC} arm 0x40c8 0xe1c23152 # bic	r3, r2, r2, asr r1
${ARMSEC} arm 0x40c8 0xe1d01250 # bics	r1, r0, r0, asr r2
${ARMSEC} arm 0x40c8 0xe1c03052 # bic	r3, r0, r2, asr r0
${ARMSEC} arm 0x40c8 0xe1d23250 # bics	r3, r2, r0, asr r2
${ARMSEC} arm 0x40c8 0xe1c11310 # bic	r1, r1, r0, lsl r3
${ARMSEC} arm 0x40c8 0xe1d00213 # bics	r0, r0, r3, lsl r2
${ARMSEC} arm 0x40c8 0xe1c21130 # bic	r1, r2, r0, lsr r1
${ARMSEC} arm 0x40c8 0xe1d10052 # bics	r0, r1, r2, asr r0
${ARMSEC} arm 0x40c8 0xe1c02312 # bic	r2, r0, r2, lsl r3
${ARMSEC} arm 0x40c8 0xe1d03233 # bics	r3, r0, r3, lsr r2
${ARMSEC} arm 0x40c8 0xe1c02371 # bic	r2, r0, r1, ror r3
${ARMSEC} arm 0x40c8 0xe1d13250 # bics	r3, r1, r0, asr r2
${ARMSEC} arm 0x40c8 0xe16f2f12 # clz	r2, r2
${ARMSEC} arm 0x40c8 0xe16f1f10 # clz	r1, r0
${ARMSEC} arm 0x40c8 0xe16f3f1f # clz	r3, pc
${ARMSEC} arm 0x40c8 0xe370070d # cmn	r0, #3407872
${ARMSEC} arm 0x40c8 0xe37f0e63 # cmn	pc, #1584
${ARMSEC} arm 0x40c8 0xe17004c0 # cmn	r0, r0, asr #9
${ARMSEC} arm 0x40c8 0xe17f020f # cmn	pc, pc, lsl #4
${ARMSEC} arm 0x40c8 0xe17201a1 # cmn	r2, r1, lsr #3
${ARMSEC} arm 0x40c8 0xe17f0423 # cmn	pc, r3, lsr #8
${ARMSEC} arm 0x40c8 0xe17101af # cmn	r1, pc, lsr #3
${ARMSEC} arm 0x40c8 0xe1700070 # cmn	r0, r0, ror r0
${ARMSEC} arm 0x40c8 0xe1710313 # cmn	r1, r3, lsl r3
${ARMSEC} arm 0x40c8 0xe1720352 # cmn	r2, r2, asr r3
${ARMSEC} arm 0x40c8 0xe1720231 # cmn	r2, r1, lsr r2
${ARMSEC} arm 0x40c8 0xe1730251 # cmn	r3, r1, asr r2
${ARMSEC} arm 0x40c8 0xe35205fa # cmp	r2, #1048576000
${ARMSEC} arm 0x40c8 0xe35f04a3 # cmp	pc, #-1560281088
${ARMSEC} arm 0x40c8 0xe15209c2 # cmp	r2, r2, asr #19
${ARMSEC} arm 0x40c8 0xe15f0f4f # cmp	pc, pc, asr #30
${ARMSEC} arm 0x40c8 0xe15300e2 # cmp	r3, r2, ror #1
${ARMSEC} arm 0x40c8 0xe15f07c3 # cmp	pc, r3, asr #15
${ARMSEC} arm 0x40c8 0xe152040f # cmp	r2, pc, lsl #8
${ARMSEC} arm 0x40c8 0xe1510151 # cmp	r1, r1, asr r1
${ARMSEC} arm 0x40c8 0xe1520050 # cmp	r2, r0, asr r0
${ARMSEC} arm 0x40c8 0xe1500130 # cmp	r0, r0, lsr r1
${ARMSEC} arm 0x40c8 0xe1500033 # cmp	r0, r3, lsr r0
${ARMSEC} arm 0x40c8 0xe1500251 # cmp	r0, r1, asr r2
${ARMSEC} arm 0x40c8 0xe2200441 # eor	r0, r0, #1090519040
${ARMSEC} arm 0x40c8 0xe23005c1 # eors	r0, r0, #809500672
${ARMSEC} arm 0x40c8 0xe222041e # eor	r0, r2, #503316480
${ARMSEC} arm 0x40c8 0xe2301f5d # eors	r1, r0, #372
${ARMSEC} arm 0x40c8 0xe22f342f # eor	r3, pc, #788529152
${ARMSEC} arm 0x40c8 0xe23f0460 # eors	r0, pc, #1610612736
${ARMSEC} arm 0x40c8 0xe0211181 # eor	r1, r1, r1, lsl #3
${ARMSEC} arm 0x40c8 0xe0300ea0 # eors	r0, r0, r0, lsr #29
${ARMSEC} arm 0x40c8 0xe0212a41 # eor	r2, r1, r1, asr #20
${ARMSEC} arm 0x40c8 0xe0310561 # eors	r0, r1, r1, ror #10
${ARMSEC} arm 0x40c8 0xe0200063 # eor	r0, r0, r3, rrx
${ARMSEC} arm 0x40c8 0xe0333d60 # eors	r3, r3, r0, ror #26
${ARMSEC} arm 0x40c8 0xe0201ea1 # eor	r1, r0, r1, lsr #29
${ARMSEC} arm 0x40c8 0xe0331861 # eors	r1, r3, r1, ror #16
${ARMSEC} arm 0x40c8 0xe02f1981 # eor	r1, pc, r1, lsl #19
${ARMSEC} arm 0x40c8 0xe03f00c0 # eors	r0, pc, r0, asr #1
${ARMSEC} arm 0x40c8 0xe02227ef # eor	r2, r2, pc, ror #15
${ARMSEC} arm 0x40c8 0xe03002af # eors	r0, r0, pc, lsr #5
${ARMSEC} arm 0x40c8 0xe02f22cf # eor	r2, pc, pc, asr #5
${ARMSEC} arm 0x40c8 0xe03f15af # eors	r1, pc, pc, lsr #11
${ARMSEC} arm 0x40c8 0xe0210143 # eor	r0, r1, r3, asr #2
${ARMSEC} arm 0x40c8 0xe0312960 # eors	r2, r1, r0, ror #18
${ARMSEC} arm 0x40c8 0xe02f06c2 # eor	r0, pc, r2, asr #13
${ARMSEC} arm 0x40c8 0xe03f0802 # eors	r0, pc, r2, lsl #16
${ARMSEC} arm 0x40c8 0xe020214f # eor	r2, r0, pc, asr #2
${ARMSEC} arm 0x40c8 0xe033144f # eors	r1, r3, pc, asr #8
${ARMSEC} arm 0x40c8 0xe0200050 # eor	r0, r0, r0, asr r0
${ARMSEC} arm 0x40c8 0xe0322232 # eors	r2, r2, r2, lsr r2
${ARMSEC} arm 0x40c8 0xe0230313 # eor	r0, r3, r3, lsl r3
${ARMSEC} arm 0x40c8 0xe0321272 # eors	r1, r2, r2, ror r2
${ARMSEC} arm 0x40c8 0xe0222111 # eor	r2, r2, r1, lsl r1
${ARMSEC} arm 0x40c8 0xe0322070 # eors	r2, r2, r0, ror r0
${ARMSEC} arm 0x40c8 0xe0213311 # eor	r3, r1, r1, lsl r3
${ARMSEC} arm 0x40c8 0xe0321152 # eors	r1, r2, r2, asr r1
${ARMSEC} arm 0x40c8 0xe0200052 # eor	r0, r0, r2, asr r0
${ARMSEC} arm 0x40c8 0xe0311172 # eors	r1, r1, r2, ror r1
${ARMSEC} arm 0x40c8 0xe0221171 # eor	r1, r2, r1, ror r1
${ARMSEC} arm 0x40c8 0xe0303333 # eors	r3, r0, r3, lsr r3
${ARMSEC} arm 0x40c8 0xe0200110 # eor	r0, r0, r0, lsl r1
${ARMSEC} arm 0x40c8 0xe0333253 # eors	r3, r3, r3, asr r2
${ARMSEC} arm 0x40c8 0xe0231351 # eor	r1, r3, r1, asr r3
${ARMSEC} arm 0x40c8 0xe0330350 # eors	r0, r3, r0, asr r3
${ARMSEC} arm 0x40c8 0xe0213212 # eor	r3, r1, r2, lsl r2
${ARMSEC} arm 0x40c8 0xe0310373 # eors	r0, r1, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0212311 # eor	r2, r1, r1, lsl r3
${ARMSEC} arm 0x40c8 0xe0310231 # eors	r0, r1, r1, lsr r2
${ARMSEC} arm 0x40c8 0xe0220273 # eor	r0, r2, r3, ror r2
${ARMSEC} arm 0x40c8 0xe0331332 # eors	r1, r3, r2, lsr r3
${ARMSEC} arm 0x40c8 0xe0211372 # eor	r1, r1, r2, ror r3
${ARMSEC} arm 0x40c8 0xe0300351 # eors	r0, r0, r1, asr r3
${ARMSEC} arm 0x40c8 0xe0223351 # eor	r3, r2, r1, asr r3
${ARMSEC} arm 0x40c8 0xe0320071 # eors	r0, r2, r1, ror r0
${ARMSEC} arm 0x40c8 0xe0210350 # eor	r0, r1, r0, asr r3
${ARMSEC} arm 0x40c8 0xe0302132 # eors	r2, r0, r2, lsr r1
${ARMSEC} arm 0x40c8 0xe0220331 # eor	r0, r2, r1, lsr r3
${ARMSEC} arm 0x40c8 0xe0313052 # eors	r3, r1, r2, asr r0
${ARMSEC} arm 0x40c8 0xe9300001 # ldmdb	r0!, {r0}
${ARMSEC} arm 0x40c8 0xe8910008 # ldmia	r1, {r3}
${ARMSEC} arm 0x40c8 0xe9b00005 # ldmib	r0!, {r0, r2}
${ARMSEC} arm 0x40c8 0xe9920006 # ldmib	r2, {r1, r2}
${ARMSEC} arm 0x40c8 0xe991000c # ldmib	r1, {r2, r3}
${ARMSEC} arm 0x40c8 0xe991000e # ldmib	r1, {r1, r2, r3}
${ARMSEC} arm 0x40c8 0xe832000d # ldmda	r2!, {r0, r2, r3}
${ARMSEC} arm 0x40c8 0xe9100007 # ldmdb	r0, {r0, r1, r2}
${ARMSEC} arm 0x40c8 0xe811000d # ldmda	r1, {r0, r2, r3}
${ARMSEC} arm 0x40c8 0xe811000f # ldmda	r1, {r0, r1, r2, r3}
${ARMSEC} arm 0x40c8 0xe930000e # ldmdb	r0!, {r1, r2, r3}
${ARMSEC} arm 0x40c8 0xe51119d6 # ldr	r1, [r1, #-2518]
${ARMSEC} arm 0x40c8 0xe5932855 # ldr	r2, [r3, #2133]
${ARMSEC} arm 0x40c8 0xe4921d07 # ldr	r1, [r2], #3335
${ARMSEC} arm 0x40c8 0xe7111021 # ldr	r1, [r1, -r1, lsr #32]
${ARMSEC} arm 0x40c8 0xe7101020 # ldr	r1, [r0, -r0, lsr #32]
${ARMSEC} arm 0x40c8 0xe7900301 # ldr	r0, [r0, r1, lsl #6]
${ARMSEC} arm 0x40c8 0xe7121221 # ldr	r1, [r2, -r1, lsr #4]
${ARMSEC} arm 0x40c8 0xe7103362 # ldr	r3, [r0, -r2, ror #6]
${ARMSEC} arm 0x40c8 0xe6103680 # ldr	r3, [r0], -r0, lsl #13
${ARMSEC} arm 0x40c8 0xe73200c0 # ldr	r0, [r2, -r0, asr #1]!
${ARMSEC} arm 0x40c8 0xe6901d82 # ldr	r1, [r0], r2, lsl #27
${ARMSEC} arm 0x40c8 0xe552271a # ldrb	r2, [r2, #-1818]
${ARMSEC} arm 0x40c8 0xe55230a1 # ldrb	r3, [r2, #-161]
${ARMSEC} arm 0x40c8 0xe4512e82 # ldrb	r2, [r1], #-3714
${ARMSEC} arm 0x40c8 0xe7533023 # ldrb	r3, [r3, -r3, lsr #32]
${ARMSEC} arm 0x40c8 0xe7d23022 # ldrb	r3, [r2, r2, lsr #32]
${ARMSEC} arm 0x40c8 0xe7533c00 # ldrb	r3, [r3, -r0, lsl #24]
${ARMSEC} arm 0x40c8 0xe75108c0 # ldrb	r0, [r1, -r0, asr #17]
${ARMSEC} arm 0x40c8 0xe7520903 # ldrb	r0, [r2, -r3, lsl #18]
${ARMSEC} arm 0x40c8 0xe6d32fa3 # ldrb	r2, [r3], r3, lsr #31
${ARMSEC} arm 0x40c8 0xe6503ae3 # ldrb	r3, [r0], -r3, ror #21
${ARMSEC} arm 0x40c8 0xe7703ce2 # ldrb	r3, [r0, -r2, ror #25]!
${ARMSEC} arm 0x40c8 0xe4f01be7 # ldrbt	r1, [r0], #3047
${ARMSEC} arm 0x40c8 0xe6f12c61 # ldrbt	r2, [r1], r1, ror #24
${ARMSEC} arm 0x40c8 0xe6732882 # ldrbt	r2, [r3], -r2, lsl #17
${ARMSEC} arm 0x40c8 0xe6731900 # ldrbt	r1, [r3], -r0, lsl #18
${ARMSEC} arm 0x40c8 0xe1c222d7 # ldrd	r2, [r2, #39]
${ARMSEC} arm 0x40c8 0xe14101d3 # ldrd	r0, [r1, #-19]
${ARMSEC} arm 0x40c8 0xe1c303df # ldrd	r0, [r3, #63]
${ARMSEC} arm 0x40c8 0xe04307d8 # ldrd	r0, [r3], #-120
${ARMSEC} arm 0x40c8 0xe10220d0 # ldrd	r2, [r2, -r0]
${ARMSEC} arm 0x40c8 0xe18100d2 # ldrd	r0, [r1, r2]
${ARMSEC} arm 0x40c8 0xe10200d3 # ldrd	r0, [r2, -r3]
${ARMSEC} arm 0x40c8 0xe08120d1 # ldrd	r2, [r1], r1
${ARMSEC} arm 0x40c8 0xe12120d0 # ldrd	r2, [r1, -r0]!
${ARMSEC} arm 0x40c8 0xe1d335bc # ldrh	r3, [r3, #92]
${ARMSEC} arm 0x40c8 0xe1d23abd # ldrh	r3, [r2, #173]
${ARMSEC} arm 0x40c8 0xe1f029be # ldrh	r2, [r0, #158]!
${ARMSEC} arm 0x40c8 0xe19220b3 # ldrh	r2, [r2, r3]
${ARMSEC} arm 0x40c8 0xe19130b3 # ldrh	r3, [r1, r3]
${ARMSEC} arm 0x40c8 0xe11130b2 # ldrh	r3, [r1, -r2]
${ARMSEC} arm 0x40c8 0xe09230b2 # ldrh	r3, [r2], r2
${ARMSEC} arm 0x40c8 0xe09200b0 # ldrh	r0, [r2], r0
${ARMSEC} arm 0x40c8 0xe1b320b1 # ldrh	r2, [r3, r1]!
${ARMSEC} arm 0x40c8 0xe0f128b4 # ldrht	r2, [r1], #132
${ARMSEC} arm 0x40c8 0xe03210b2 # ldrht	r1, [r2], -r2
${ARMSEC} arm 0x40c8 0xe0b310b1 # ldrht	r1, [r3], r1
${ARMSEC} arm 0x40c8 0xe03030b2 # ldrht	r3, [r0], -r2
${ARMSEC} arm 0x40c8 0xe1d116d6 # ldrsb	r1, [r1, #102]
${ARMSEC} arm 0x40c8 0xe1521dda # ldrsb	r1, [r2, #-218]
${ARMSEC} arm 0x40c8 0xe0d01fd4 # ldrsb	r1, [r0], #244
${ARMSEC} arm 0x40c8 0xe19000d1 # ldrsb	r0, [r0, r1]
${ARMSEC} arm 0x40c8 0xe11130d3 # ldrsb	r3, [r1, -r3]
${ARMSEC} arm 0x40c8 0xe11300d1 # ldrsb	r0, [r3, -r1]
${ARMSEC} arm 0x40c8 0xe01100d1 # ldrsb	r0, [r1], -r1
${ARMSEC} arm 0x40c8 0xe01310d1 # ldrsb	r1, [r3], -r1
${ARMSEC} arm 0x40c8 0xe1b210d3 # ldrsb	r1, [r2, r3]!
${ARMSEC} arm 0x40c8 0xe0732dd3 # ldrsbt	r2, [r3], #-211
${ARMSEC} arm 0x40c8 0xe0b100d1 # ldrsbt	r0, [r1], r1
${ARMSEC} arm 0x40c8 0xe0b130d3 # ldrsbt	r3, [r1], r3
${ARMSEC} arm 0x40c8 0xe03210d3 # ldrsbt	r1, [r2], -r3
${ARMSEC} arm 0x40c8 0xe1d008fa # ldrsh	r0, [r0, #138]
${ARMSEC} arm 0x40c8 0xe1d103fb # ldrsh	r0, [r1, #59]
${ARMSEC} arm 0x40c8 0xe1730df0 # ldrsh	r0, [r3, #-208]!
${ARMSEC} arm 0x40c8 0xe11110f0 # ldrsh	r1, [r1, -r0]
${ARMSEC} arm 0x40c8 0xe19310f1 # ldrsh	r1, [r3, r1]
${ARMSEC} arm 0x40c8 0xe19100f2 # ldrsh	r0, [r1, r2]
${ARMSEC} arm 0x40c8 0xe09300f3 # ldrsh	r0, [r3], r3
${ARMSEC} arm 0x40c8 0xe1b020f2 # ldrsh	r2, [r0, r2]!
${ARMSEC} arm 0x40c8 0xe1b200f1 # ldrsh	r0, [r2, r1]!
${ARMSEC} arm 0x40c8 0xe0f312f5 # ldrsht	r1, [r3], #37
${ARMSEC} arm 0x40c8 0xe0b320f3 # ldrsht	r2, [r3], r3
${ARMSEC} arm 0x40c8 0xe03030f3 # ldrsht	r3, [r0], -r3
${ARMSEC} arm 0x40c8 0xe0b210f3 # ldrsht	r1, [r2], r3
${ARMSEC} arm 0x40c8 0xe4330089 # ldrt	r0, [r3], #-137
${ARMSEC} arm 0x40c8 0xe6313421 # ldrt	r3, [r1], -r1, lsr #8
${ARMSEC} arm 0x40c8 0xe6b136c3 # ldrt	r3, [r1], r3, asr #13
${ARMSEC} arm 0x40c8 0xe63303e2 # ldrt	r0, [r3], -r2, ror #7
${ARMSEC} arm 0x40c8 0xe0211191 # mla	r1, r1, r1, r1
${ARMSEC} arm 0x40c8 0xe0333393 # mlas	r3, r3, r3, r3
${ARMSEC} arm 0x40c8 0xe0213393 # mla	r1, r3, r3, r3
${ARMSEC} arm 0x40c8 0xe0303393 # mlas	r0, r3, r3, r3
${ARMSEC} arm 0x40c8 0xe0201190 # mla	r0, r0, r1, r1
${ARMSEC} arm 0x40c8 0xe0332293 # mlas	r3, r3, r2, r2
${ARMSEC} arm 0x40c8 0xe0203093 # mla	r0, r3, r0, r3
${ARMSEC} arm 0x40c8 0xe0323293 # mlas	r2, r3, r2, r3
${ARMSEC} arm 0x40c8 0xe0231393 # mla	r3, r3, r3, r1
${ARMSEC} arm 0x40c8 0xe0312191 # mlas	r1, r1, r1, r2
${ARMSEC} arm 0x40c8 0xe0211192 # mla	r1, r2, r1, r1
${ARMSEC} arm 0x40c8 0xe0311192 # mlas	r1, r2, r1, r1
${ARMSEC} arm 0x40c8 0xe020009f # mla	r0, pc, r0, r0
${ARMSEC} arm 0x40c8 0xe033339f # mlas	r3, pc, r3, r3
${ARMSEC} arm 0x40c8 0xe0233293 # mla	r3, r3, r2, r3
${ARMSEC} arm 0x40c8 0xe0300290 # mlas	r0, r0, r2, r0
${ARMSEC} arm 0x40c8 0xe0233191 # mla	r3, r1, r1, r3
${ARMSEC} arm 0x40c8 0xe0322393 # mlas	r2, r3, r3, r2
${ARMSEC} arm 0x40c8 0xe0200f90 # mla	r0, r0, pc, r0
${ARMSEC} arm 0x40c8 0xe0311f91 # mlas	r1, r1, pc, r1
${ARMSEC} arm 0x40c8 0xe0222f9f # mla	r2, pc, pc, r2
${ARMSEC} arm 0x40c8 0xe0300f9f # mlas	r0, pc, pc, r0
${ARMSEC} arm 0x40c8 0xe022f292 # mla	r2, r2, r2, pc
${ARMSEC} arm 0x40c8 0xe031f191 # mlas	r1, r1, r1, pc
${ARMSEC} arm 0x40c8 0xe021f19f # mla	r1, pc, r1, pc
${ARMSEC} arm 0x40c8 0xe033f39f # mlas	r3, pc, r3, pc
${ARMSEC} arm 0x40c8 0xe021ff91 # mla	r1, r1, pc, pc
${ARMSEC} arm 0x40c8 0xe032ff92 # mlas	r2, r2, pc, pc
${ARMSEC} arm 0x40c8 0xe021ff9f # mla	r1, pc, pc, pc
${ARMSEC} arm 0x40c8 0xe033ff9f # mlas	r3, pc, pc, pc
${ARMSEC} arm 0x40c8 0xe0230092 # mla	r3, r2, r0, r0
${ARMSEC} arm 0x40c8 0xe0320091 # mlas	r2, r1, r0, r0
${ARMSEC} arm 0x40c8 0xe022339f # mla	r2, pc, r3, r3
${ARMSEC} arm 0x40c8 0xe031339f # mlas	r1, pc, r3, r3
${ARMSEC} arm 0x40c8 0xe0213093 # mla	r1, r3, r0, r3
${ARMSEC} arm 0x40c8 0xe0301291 # mlas	r0, r1, r2, r1
${ARMSEC} arm 0x40c8 0xe0230292 # mla	r3, r2, r2, r0
${ARMSEC} arm 0x40c8 0xe0310393 # mlas	r1, r3, r3, r0
${ARMSEC} arm 0x40c8 0xe0223092 # mla	r2, r2, r0, r3
${ARMSEC} arm 0x40c8 0xe0330193 # mlas	r3, r3, r1, r0
${ARMSEC} arm 0x40c8 0xe0230392 # mla	r3, r2, r3, r0
${ARMSEC} arm 0x40c8 0xe0321293 # mlas	r2, r3, r2, r1
${ARMSEC} arm 0x40c8 0xe023139f # mla	r3, pc, r3, r1
${ARMSEC} arm 0x40c8 0xe032029f # mlas	r2, pc, r2, r0
${ARMSEC} arm 0x40c8 0xe0230f90 # mla	r3, r0, pc, r0
${ARMSEC} arm 0x40c8 0xe0310f90 # mlas	r1, r0, pc, r0
${ARMSEC} arm 0x40c8 0xe0212f91 # mla	r1, r1, pc, r2
${ARMSEC} arm 0x40c8 0xe0302f90 # mlas	r0, r0, pc, r2
${ARMSEC} arm 0x40c8 0xe0202f9f # mla	r0, pc, pc, r2
${ARMSEC} arm 0x40c8 0xe0301f9f # mlas	r0, pc, pc, r1
${ARMSEC} arm 0x40c8 0xe0211093 # mla	r1, r3, r0, r1
${ARMSEC} arm 0x40c8 0xe0333190 # mlas	r3, r0, r1, r3
${ARMSEC} arm 0x40c8 0xe021139f # mla	r1, pc, r3, r1
${ARMSEC} arm 0x40c8 0xe031129f # mlas	r1, pc, r2, r1
${ARMSEC} arm 0x40c8 0xe0233f92 # mla	r3, r2, pc, r3
${ARMSEC} arm 0x40c8 0xe0311f92 # mlas	r1, r2, pc, r1
${ARMSEC} arm 0x40c8 0xe020f393 # mla	r0, r3, r3, pc
${ARMSEC} arm 0x40c8 0xe032f090 # mlas	r2, r0, r0, pc
${ARMSEC} arm 0x40c8 0xe023f193 # mla	r3, r3, r1, pc
${ARMSEC} arm 0x40c8 0xe033f193 # mlas	r3, r3, r1, pc
${ARMSEC} arm 0x40c8 0xe021f29f # mla	r1, pc, r2, pc
${ARMSEC} arm 0x40c8 0xe031f39f # mlas	r1, pc, r3, pc
${ARMSEC} arm 0x40c8 0xe022f290 # mla	r2, r0, r2, pc
${ARMSEC} arm 0x40c8 0xe033f391 # mlas	r3, r1, r3, pc
${ARMSEC} arm 0x40c8 0xe023ff92 # mla	r3, r2, pc, pc
${ARMSEC} arm 0x40c8 0xe032ff91 # mlas	r2, r1, pc, pc
${ARMSEC} arm 0x40c8 0xe0223190 # mla	r2, r0, r1, r3
${ARMSEC} arm 0x40c8 0xe0330291 # mlas	r3, r1, r2, r0
${ARMSEC} arm 0x40c8 0xe023209f # mla	r3, pc, r0, r2
${ARMSEC} arm 0x40c8 0xe033219f # mlas	r3, pc, r1, r2
${ARMSEC} arm 0x40c8 0xe0210f93 # mla	r1, r3, pc, r0
${ARMSEC} arm 0x40c8 0xe0330f91 # mlas	r3, r1, pc, r0
${ARMSEC} arm 0x40c8 0xe022f391 # mla	r2, r1, r3, pc
${ARMSEC} arm 0x40c8 0xe031f392 # mlas	r1, r2, r3, pc
${ARMSEC} arm 0x40c8 0xe0611191 # mls	r1, r1, r1, r1
${ARMSEC} arm 0x40c8 0xe0630090 # mls	r3, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe0600292 # mls	r0, r2, r2, r0
${ARMSEC} arm 0x40c8 0xe0603390 # mls	r0, r0, r3, r3
${ARMSEC} arm 0x40c8 0xe0622392 # mls	r2, r2, r3, r2
${ARMSEC} arm 0x40c8 0xe0602090 # mls	r0, r0, r0, r2
${ARMSEC} arm 0x40c8 0xe0611193 # mls	r1, r3, r1, r1
${ARMSEC} arm 0x40c8 0xe0631391 # mls	r3, r1, r3, r1
${ARMSEC} arm 0x40c8 0xe0612393 # mls	r1, r3, r3, r2
${ARMSEC} arm 0x40c8 0xe0601193 # mls	r0, r3, r1, r1
${ARMSEC} arm 0x40c8 0xe0602392 # mls	r0, r2, r3, r2
${ARMSEC} arm 0x40c8 0xe0600392 # mls	r0, r2, r3, r0
${ARMSEC} arm 0x40c8 0xe0601390 # mls	r0, r0, r3, r1
${ARMSEC} arm 0x40c8 0xe0631392 # mls	r3, r2, r3, r1
${ARMSEC} arm 0x40c8 0xe0623091 # mls	r2, r1, r0, r3
${ARMSEC} arm 0x40c8 0xe3a003f0 # mov	r0, #-1073741821
${ARMSEC} arm 0x40c8 0xe3b00e28 # movs	r0, #640
${ARMSEC} arm 0x40c8 0xe1a03403 # lsl	r3, r3, #8
${ARMSEC} arm 0x40c8 0xe1b03dc3 # asrs	r3, r3, #27
${ARMSEC} arm 0x40c8 0xe1a008e1 # ror	r0, r1, #17
${ARMSEC} arm 0x40c8 0xe1b01903 # lsls	r1, r3, #18
${ARMSEC} arm 0x40c8 0xe1a0354f # asr	r3, pc, #10
${ARMSEC} arm 0x40c8 0xe1b0168f # lsls	r1, pc, #13
${ARMSEC} arm 0x40c8 0xe1a00030 # lsr	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe1b00010 # lsls	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe1a00171 # ror	r0, r1, r1
${ARMSEC} arm 0x40c8 0xe1b00373 # rors	r0, r3, r3
${ARMSEC} arm 0x40c8 0xe1a03332 # lsr	r3, r2, r3
${ARMSEC} arm 0x40c8 0xe1b01172 # rors	r1, r2, r1
${ARMSEC} arm 0x40c8 0xe1a01271 # ror	r1, r1, r2
${ARMSEC} arm 0x40c8 0xe1b01371 # rors	r1, r1, r3
${ARMSEC} arm 0x40c8 0xe1a00113 # lsl	r0, r3, r1
${ARMSEC} arm 0x40c8 0xe1b01053 # asrs	r1, r3, r0
${ARMSEC} arm 0x40c8 0xe34c21ec # movt	r2, #49644
${ARMSEC} arm 0x40c8 0xe30a0d87 # movw	r0, #44423
${ARMSEC} arm 0x40c8 0xe0030393 # mul	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe0110191 # muls	r1, r1, r1
${ARMSEC} arm 0x40c8 0xe0030292 # mul	r3, r2, r2
${ARMSEC} arm 0x40c8 0xe0110090 # muls	r1, r0, r0
${ARMSEC} arm 0x40c8 0xe0000290 # mul	r0, r0, r2
${ARMSEC} arm 0x40c8 0xe0110091 # muls	r1, r1, r0
${ARMSEC} arm 0x40c8 0xe0020291 # mul	r2, r1, r2
${ARMSEC} arm 0x40c8 0xe0110192 # muls	r1, r2, r1
${ARMSEC} arm 0x40c8 0xe0010093 # mul	r1, r3, r0
${ARMSEC} arm 0x40c8 0xe0110390 # muls	r1, r0, r3
${ARMSEC} arm 0x40c8 0xe3e02561 # mvn	r2, #406847488
${ARMSEC} arm 0x40c8 0xe3f02beb # mvns	r2, #240640
${ARMSEC} arm 0x40c8 0xe1e02942 # mvn	r2, r2, asr #18
${ARMSEC} arm 0x40c8 0xe1f011a1 # mvns	r1, r1, lsr #3
${ARMSEC} arm 0x40c8 0xe1e02e21 # mvn	r2, r1, lsr #28
${ARMSEC} arm 0x40c8 0xe1f00742 # mvns	r0, r2, asr #14
${ARMSEC} arm 0x40c8 0xe1e02baf # mvn	r2, pc, lsr #23
${ARMSEC} arm 0x40c8 0xe1f004ef # mvns	r0, pc, ror #9
${ARMSEC} arm 0x40c8 0xe1e01131 # mvn	r1, r1, lsr r1
${ARMSEC} arm 0x40c8 0xe1f02252 # mvns	r2, r2, asr r2
${ARMSEC} arm 0x40c8 0xe1e03272 # mvn	r3, r2, ror r2
${ARMSEC} arm 0x40c8 0xe1f02050 # mvns	r2, r0, asr r0
${ARMSEC} arm 0x40c8 0xe1e01133 # mvn	r1, r3, lsr r1
${ARMSEC} arm 0x40c8 0xe1f01132 # mvns	r1, r2, lsr r1
${ARMSEC} arm 0x40c8 0xe1e02052 # mvn	r2, r2, asr r0
${ARMSEC} arm 0x40c8 0xe1f00230 # mvns	r0, r0, lsr r2
${ARMSEC} arm 0x40c8 0xe1e01213 # mvn	r1, r3, lsl r2
${ARMSEC} arm 0x40c8 0xe1f00152 # mvns	r0, r2, asr r1
${ARMSEC} arm 0x40c8 0xe3833c60 # orr	r3, r3, #24576
${ARMSEC} arm 0x40c8 0xe3900026 # orrs	r0, r0, #38
${ARMSEC} arm 0x40c8 0xe3813c8e # orr	r3, r1, #36352
${ARMSEC} arm 0x40c8 0xe3913ed3 # orrs	r3, r1, #3376
${ARMSEC} arm 0x40c8 0xe38f3a06 # orr	r3, pc, #24576
${ARMSEC} arm 0x40c8 0xe39f17d8 # orrs	r1, pc, #56623104
${ARMSEC} arm 0x40c8 0xe1833ea3 # orr	r3, r3, r3, lsr #29
${ARMSEC} arm 0x40c8 0xe1933b03 # orrs	r3, r3, r3, lsl #22
${ARMSEC} arm 0x40c8 0xe18015c0 # orr	r1, r0, r0, asr #11
${ARMSEC} arm 0x40c8 0xe1912941 # orrs	r2, r1, r1, asr #18
${ARMSEC} arm 0x40c8 0xe1822781 # orr	r2, r2, r1, lsl #15
${ARMSEC} arm 0x40c8 0xe19336e1 # orrs	r3, r3, r1, ror #13
${ARMSEC} arm 0x40c8 0xe1801d61 # orr	r1, r0, r1, ror #26
${ARMSEC} arm 0x40c8 0xe1912082 # orrs	r2, r1, r2, lsl #1
${ARMSEC} arm 0x40c8 0xe18f2302 # orr	r2, pc, r2, lsl #6
${ARMSEC} arm 0x40c8 0xe19f18a1 # orrs	r1, pc, r1, lsr #17
${ARMSEC} arm 0x40c8 0xe182228f # orr	r2, r2, pc, lsl #5
${ARMSEC} arm 0x40c8 0xe1911fef # orrs	r1, r1, pc, ror #31
${ARMSEC} arm 0x40c8 0xe18f122f # orr	r1, pc, pc, lsr #4
${ARMSEC} arm 0x40c8 0xe19f06ef # orrs	r0, pc, pc, ror #13
${ARMSEC} arm 0x40c8 0xe1823241 # orr	r3, r2, r1, asr #4
${ARMSEC} arm 0x40c8 0xe1932c20 # orrs	r2, r3, r0, lsr #24
${ARMSEC} arm 0x40c8 0xe18f3522 # orr	r3, pc, r2, lsr #10
${ARMSEC} arm 0x40c8 0xe19f33c2 # orrs	r3, pc, r2, asr #7
${ARMSEC} arm 0x40c8 0xe1801d0f # orr	r1, r0, pc, lsl #26
${ARMSEC} arm 0x40c8 0xe1930a6f # orrs	r0, r3, pc, ror #20
${ARMSEC} arm 0x40c8 0xe1800010 # orr	r0, r0, r0, lsl r0
${ARMSEC} arm 0x40c8 0xe1922212 # orrs	r2, r2, r2, lsl r2
${ARMSEC} arm 0x40c8 0xe1813171 # orr	r3, r1, r1, ror r1
${ARMSEC} arm 0x40c8 0xe1903010 # orrs	r3, r0, r0, lsl r0
${ARMSEC} arm 0x40c8 0xe1800151 # orr	r0, r0, r1, asr r1
${ARMSEC} arm 0x40c8 0xe1911010 # orrs	r1, r1, r0, lsl r0
${ARMSEC} arm 0x40c8 0xe1810071 # orr	r0, r1, r1, ror r0
${ARMSEC} arm 0x40c8 0xe1903310 # orrs	r3, r0, r0, lsl r3
${ARMSEC} arm 0x40c8 0xe1833372 # orr	r3, r3, r2, ror r3
${ARMSEC} arm 0x40c8 0xe1911150 # orrs	r1, r1, r0, asr r1
${ARMSEC} arm 0x40c8 0xe1823333 # orr	r3, r2, r3, lsr r3
${ARMSEC} arm 0x40c8 0xe1903353 # orrs	r3, r0, r3, asr r3
${ARMSEC} arm 0x40c8 0xe1800230 # orr	r0, r0, r0, lsr r2
${ARMSEC} arm 0x40c8 0xe1922152 # orrs	r2, r2, r2, asr r1
${ARMSEC} arm 0x40c8 0xe1821211 # orr	r1, r2, r1, lsl r2
${ARMSEC} arm 0x40c8 0xe1930310 # orrs	r0, r3, r0, lsl r3
${ARMSEC} arm 0x40c8 0xe1830272 # orr	r0, r3, r2, ror r2
${ARMSEC} arm 0x40c8 0xe1910232 # orrs	r0, r1, r2, lsr r2
${ARMSEC} arm 0x40c8 0xe1820132 # orr	r0, r2, r2, lsr r1
${ARMSEC} arm 0x40c8 0xe1931273 # orrs	r1, r3, r3, ror r2
${ARMSEC} arm 0x40c8 0xe1832350 # orr	r2, r3, r0, asr r3
${ARMSEC} arm 0x40c8 0xe1931310 # orrs	r1, r3, r0, lsl r3
${ARMSEC} arm 0x40c8 0xe1800311 # orr	r0, r0, r1, lsl r3
${ARMSEC} arm 0x40c8 0xe1900311 # orrs	r0, r0, r1, lsl r3
${ARMSEC} arm 0x40c8 0xe1802211 # orr	r2, r0, r1, lsl r2
${ARMSEC} arm 0x40c8 0xe1912273 # orrs	r2, r1, r3, ror r2
${ARMSEC} arm 0x40c8 0xe1813033 # orr	r3, r1, r3, lsr r0
${ARMSEC} arm 0x40c8 0xe1901371 # orrs	r1, r0, r1, ror r3
${ARMSEC} arm 0x40c8 0xe1802133 # orr	r2, r0, r3, lsr r1
${ARMSEC} arm 0x40c8 0xe1923130 # orrs	r3, r2, r0, lsr r1
${ARMSEC} arm 0x40c8 0xe6822792 # pkhbt	r2, r2, r2, lsl #15
${ARMSEC} arm 0x40c8 0xe68203d2 # pkhtb	r0, r2, r2, asr #7
${ARMSEC} arm 0x40c8 0xe6803a93 # pkhbt	r3, r0, r3, lsl #21
${ARMSEC} arm 0x40c8 0xe6833dd1 # pkhtb	r3, r3, r1, asr #27
${ARMSEC} arm 0x40c8 0xe6832c10 # pkhbt	r2, r3, r0, lsl #24
${ARMSEC} arm 0x40c8 0xe1000050 # qadd	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe1021052 # qadd	r1, r2, r2
${ARMSEC} arm 0x40c8 0xe1021051 # qadd	r1, r1, r2
${ARMSEC} arm 0x40c8 0xe1011053 # qadd	r1, r3, r1
${ARMSEC} arm 0x40c8 0xe1031050 # qadd	r1, r0, r3
${ARMSEC} arm 0x40c8 0xe6200f10 # qadd16	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe6213f11 # qadd16	r3, r1, r1
${ARMSEC} arm 0x40c8 0xe6210f10 # qadd16	r0, r1, r0
${ARMSEC} arm 0x40c8 0xe6233f12 # qadd16	r3, r3, r2
${ARMSEC} arm 0x40c8 0xe6231f10 # qadd16	r1, r3, r0
${ARMSEC} arm 0x40c8 0xe6211f91 # qadd8	r1, r1, r1
${ARMSEC} arm 0x40c8 0xe6203f90 # qadd8	r3, r0, r0
${ARMSEC} arm 0x40c8 0xe6203f93 # qadd8	r3, r0, r3
${ARMSEC} arm 0x40c8 0xe6233f91 # qadd8	r3, r3, r1
${ARMSEC} arm 0x40c8 0xe6203f92 # qadd8	r3, r0, r2
${ARMSEC} arm 0x40c8 0xe6222f32 # qasx	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6213f31 # qasx	r3, r1, r1
${ARMSEC} arm 0x40c8 0xe6223f33 # qasx	r3, r2, r3
${ARMSEC} arm 0x40c8 0xe6233f32 # qasx	r3, r3, r2
${ARMSEC} arm 0x40c8 0xe6202f33 # qasx	r2, r0, r3
${ARMSEC} arm 0x40c8 0xe1422052 # qdadd	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe1413051 # qdadd	r3, r1, r1
${ARMSEC} arm 0x40c8 0xe1413053 # qdadd	r3, r3, r1
${ARMSEC} arm 0x40c8 0xe1433050 # qdadd	r3, r0, r3
${ARMSEC} arm 0x40c8 0xe1420051 # qdadd	r0, r1, r2
${ARMSEC} arm 0x40c8 0xe1622052 # qdsub	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe1630053 # qdsub	r0, r3, r3
${ARMSEC} arm 0x40c8 0xe1620050 # qdsub	r0, r0, r2
${ARMSEC} arm 0x40c8 0xe1600051 # qdsub	r0, r1, r0
${ARMSEC} arm 0x40c8 0xe1620053 # qdsub	r0, r3, r2
${ARMSEC} arm 0x40c8 0xe6200f50 # qsax	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe6221f52 # qsax	r1, r2, r2
${ARMSEC} arm 0x40c8 0xe6220f50 # qsax	r0, r2, r0
${ARMSEC} arm 0x40c8 0xe6211f53 # qsax	r1, r1, r3
${ARMSEC} arm 0x40c8 0xe6231f50 # qsax	r1, r3, r0
${ARMSEC} arm 0x40c8 0xe1200050 # qsub	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe1203050 # qsub	r3, r0, r0
${ARMSEC} arm 0x40c8 0xe1200052 # qsub	r0, r2, r0
${ARMSEC} arm 0x40c8 0xe1210050 # qsub	r0, r0, r1
${ARMSEC} arm 0x40c8 0xe1230052 # qsub	r0, r2, r3
${ARMSEC} arm 0x40c8 0xe6211f71 # qsub16	r1, r1, r1
${ARMSEC} arm 0x40c8 0xe6210f71 # qsub16	r0, r1, r1
${ARMSEC} arm 0x40c8 0xe6213f73 # qsub16	r3, r1, r3
${ARMSEC} arm 0x40c8 0xe6200f73 # qsub16	r0, r0, r3
${ARMSEC} arm 0x40c8 0xe6220f71 # qsub16	r0, r2, r1
${ARMSEC} arm 0x40c8 0xe6233ff3 # qsub8	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6212ff1 # qsub8	r2, r1, r1
${ARMSEC} arm 0x40c8 0xe6202ff2 # qsub8	r2, r0, r2
${ARMSEC} arm 0x40c8 0xe6211ff0 # qsub8	r1, r1, r0
${ARMSEC} arm 0x40c8 0xe6202ff3 # qsub8	r2, r0, r3
${ARMSEC} arm 0x40c8 0xe6ff1f31 # rbit	r1, r1
${ARMSEC} arm 0x40c8 0xe6ff3f32 # rbit	r3, r2
${ARMSEC} arm 0x40c8 0xe6bf2f32 # rev	r2, r2
${ARMSEC} arm 0x40c8 0xe6bf0f31 # rev	r0, r1
${ARMSEC} arm 0x40c8 0xe6bf0fb0 # rev16	r0, r0
${ARMSEC} arm 0x40c8 0xe6bf3fb0 # rev16	r3, r0
${ARMSEC} arm 0x40c8 0xe6ff1fb1 # revsh	r1, r1
${ARMSEC} arm 0x40c8 0xe6ff1fb2 # revsh	r1, r2
${ARMSEC} arm 0x40c8 0xe2633937 # rsb	r3, r3, #901120
${ARMSEC} arm 0x40c8 0xe27116d5 # rsbs	r1, r1, #223346688
${ARMSEC} arm 0x40c8 0xe2612a55 # rsb	r2, r1, #348160
${ARMSEC} arm 0x40c8 0xe2730704 # rsbs	r0, r3, #1048576
${ARMSEC} arm 0x40c8 0xe26f0c6b # rsb	r0, pc, #27392
${ARMSEC} arm 0x40c8 0xe27f0802 # rsbs	r0, pc, #131072
${ARMSEC} arm 0x40c8 0xe0611181 # rsb	r1, r1, r1, lsl #3
${ARMSEC} arm 0x40c8 0xe0700360 # rsbs	r0, r0, r0, ror #6
${ARMSEC} arm 0x40c8 0xe06319c3 # rsb	r1, r3, r3, asr #19
${ARMSEC} arm 0x40c8 0xe0702e40 # rsbs	r2, r0, r0, asr #28
${ARMSEC} arm 0x40c8 0xe0600363 # rsb	r0, r0, r3, ror #6
${ARMSEC} arm 0x40c8 0xe07005a3 # rsbs	r0, r0, r3, lsr #11
${ARMSEC} arm 0x40c8 0xe0603843 # rsb	r3, r0, r3, asr #16
${ARMSEC} arm 0x40c8 0xe07132c3 # rsbs	r3, r1, r3, asr #5
${ARMSEC} arm 0x40c8 0xe06f21a2 # rsb	r2, pc, r2, lsr #3
${ARMSEC} arm 0x40c8 0xe07f3e63 # rsbs	r3, pc, r3, ror #28
${ARMSEC} arm 0x40c8 0xe06225ef # rsb	r2, r2, pc, ror #11
${ARMSEC} arm 0x40c8 0xe07116cf # rsbs	r1, r1, pc, asr #13
${ARMSEC} arm 0x40c8 0xe06f192f # rsb	r1, pc, pc, lsr #18
${ARMSEC} arm 0x40c8 0xe07f02ef # rsbs	r0, pc, pc, ror #5
${ARMSEC} arm 0x40c8 0xe06033e1 # rsb	r3, r0, r1, ror #7
${ARMSEC} arm 0x40c8 0xe07124c0 # rsbs	r2, r1, r0, asr #9
${ARMSEC} arm 0x40c8 0xe06f16c2 # rsb	r1, pc, r2, asr #13
${ARMSEC} arm 0x40c8 0xe07f0382 # rsbs	r0, pc, r2, lsl #7
${ARMSEC} arm 0x40c8 0xe0630c6f # rsb	r0, r3, pc, ror #24
${ARMSEC} arm 0x40c8 0xe073164f # rsbs	r1, r3, pc, asr #12
${ARMSEC} arm 0x40c8 0xe0600070 # rsb	r0, r0, r0, ror r0
${ARMSEC} arm 0x40c8 0xe0733313 # rsbs	r3, r3, r3, lsl r3
${ARMSEC} arm 0x40c8 0xe0630353 # rsb	r0, r3, r3, asr r3
${ARMSEC} arm 0x40c8 0xe0731353 # rsbs	r1, r3, r3, asr r3
${ARMSEC} arm 0x40c8 0xe0622333 # rsb	r2, r2, r3, lsr r3
${ARMSEC} arm 0x40c8 0xe0733151 # rsbs	r3, r3, r1, asr r1
${ARMSEC} arm 0x40c8 0xe0603370 # rsb	r3, r0, r0, ror r3
${ARMSEC} arm 0x40c8 0xe0732233 # rsbs	r2, r3, r3, lsr r2
${ARMSEC} arm 0x40c8 0xe0633372 # rsb	r3, r3, r2, ror r3
${ARMSEC} arm 0x40c8 0xe0733331 # rsbs	r3, r3, r1, lsr r3
${ARMSEC} arm 0x40c8 0xe0612232 # rsb	r2, r1, r2, lsr r2
${ARMSEC} arm 0x40c8 0xe0710010 # rsbs	r0, r1, r0, lsl r0
${ARMSEC} arm 0x40c8 0xe0633253 # rsb	r3, r3, r3, asr r2
${ARMSEC} arm 0x40c8 0xe0722332 # rsbs	r2, r2, r2, lsr r3
${ARMSEC} arm 0x40c8 0xe0613173 # rsb	r3, r1, r3, ror r1
${ARMSEC} arm 0x40c8 0xe0721271 # rsbs	r1, r2, r1, ror r2
${ARMSEC} arm 0x40c8 0xe0621333 # rsb	r1, r2, r3, lsr r3
${ARMSEC} arm 0x40c8 0xe0710373 # rsbs	r0, r1, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0631053 # rsb	r1, r3, r3, asr r0
${ARMSEC} arm 0x40c8 0xe0732173 # rsbs	r2, r3, r3, ror r1
${ARMSEC} arm 0x40c8 0xe0612113 # rsb	r2, r1, r3, lsl r1
${ARMSEC} arm 0x40c8 0xe0712150 # rsbs	r2, r1, r0, asr r1
${ARMSEC} arm 0x40c8 0xe0622351 # rsb	r2, r2, r1, asr r3
${ARMSEC} arm 0x40c8 0xe0722051 # rsbs	r2, r2, r1, asr r0
${ARMSEC} arm 0x40c8 0xe0613370 # rsb	r3, r1, r0, ror r3
${ARMSEC} arm 0x40c8 0xe0701172 # rsbs	r1, r0, r2, ror r1
${ARMSEC} arm 0x40c8 0xe0631011 # rsb	r1, r3, r1, lsl r0
${ARMSEC} arm 0x40c8 0xe0731051 # rsbs	r1, r3, r1, asr r0
${ARMSEC} arm 0x40c8 0xe0630211 # rsb	r0, r3, r1, lsl r2
${ARMSEC} arm 0x40c8 0xe0732170 # rsbs	r2, r3, r0, ror r1
${ARMSEC} arm 0x40c8 0xe2e22b69 # rsc	r2, r2, #107520
${ARMSEC} arm 0x40c8 0xe2f0078d # rscs	r0, r0, #36962304
${ARMSEC} arm 0x40c8 0xe2e20336 # rsc	r0, r2, #-671088640
${ARMSEC} arm 0x40c8 0xe2f120d1 # rscs	r2, r1, #209
${ARMSEC} arm 0x40c8 0xe2ef21bf # rsc	r2, pc, #-1073741777
${ARMSEC} arm 0x40c8 0xe2ff3a81 # rscs	r3, pc, #528384
${ARMSEC} arm 0x40c8 0xe0e11241 # rsc	r1, r1, r1, asr #4
${ARMSEC} arm 0x40c8 0xe0f00ca0 # rscs	r0, r0, r0, lsr #25
${ARMSEC} arm 0x40c8 0xe0e31e03 # rsc	r1, r3, r3, lsl #28
${ARMSEC} arm 0x40c8 0xe0f03500 # rscs	r3, r0, r0, lsl #10
${ARMSEC} arm 0x40c8 0xe0e00821 # rsc	r0, r0, r1, lsr #16
${ARMSEC} arm 0x40c8 0xe0f11d43 # rscs	r1, r1, r3, asr #26
${ARMSEC} arm 0x40c8 0xe0e01121 # rsc	r1, r0, r1, lsr #2
${ARMSEC} arm 0x40c8 0xe0f23743 # rscs	r3, r2, r3, asr #14
${ARMSEC} arm 0x40c8 0xe0ef3903 # rsc	r3, pc, r3, lsl #18
${ARMSEC} arm 0x40c8 0xe0ff0720 # rscs	r0, pc, r0, lsr #14
${ARMSEC} arm 0x40c8 0xe0e22b0f # rsc	r2, r2, pc, lsl #22
${ARMSEC} arm 0x40c8 0xe0f0086f # rscs	r0, r0, pc, ror #16
${ARMSEC} arm 0x40c8 0xe0ef0aef # rsc	r0, pc, pc, ror #21
${ARMSEC} arm 0x40c8 0xe0ff114f # rscs	r1, pc, pc, asr #2
${ARMSEC} arm 0x40c8 0xe0e033e2 # rsc	r3, r0, r2, ror #7
${ARMSEC} arm 0x40c8 0xe0f319a2 # rscs	r1, r3, r2, lsr #19
${ARMSEC} arm 0x40c8 0xe0ef0d83 # rsc	r0, pc, r3, lsl #27
${ARMSEC} arm 0x40c8 0xe0ff0523 # rscs	r0, pc, r3, lsr #10
${ARMSEC} arm 0x40c8 0xe0e0104f # rsc	r1, r0, pc, asr #32
${ARMSEC} arm 0x40c8 0xe0f01d6f # rscs	r1, r0, pc, ror #26
${ARMSEC} arm 0x40c8 0xe0e00010 # rsc	r0, r0, r0, lsl r0
${ARMSEC} arm 0x40c8 0xe0f22252 # rscs	r2, r2, r2, asr r2
${ARMSEC} arm 0x40c8 0xe0e03050 # rsc	r3, r0, r0, asr r0
${ARMSEC} arm 0x40c8 0xe0f03070 # rscs	r3, r0, r0, ror r0
${ARMSEC} arm 0x40c8 0xe0e00232 # rsc	r0, r0, r2, lsr r2
${ARMSEC} arm 0x40c8 0xe0f11353 # rscs	r1, r1, r3, asr r3
${ARMSEC} arm 0x40c8 0xe0e30073 # rsc	r0, r3, r3, ror r0
${ARMSEC} arm 0x40c8 0xe0f20032 # rscs	r0, r2, r2, lsr r0
${ARMSEC} arm 0x40c8 0xe0e22213 # rsc	r2, r2, r3, lsl r2
${ARMSEC} arm 0x40c8 0xe0f33370 # rscs	r3, r3, r0, ror r3
${ARMSEC} arm 0x40c8 0xe0e13333 # rsc	r3, r1, r3, lsr r3
${ARMSEC} arm 0x40c8 0xe0f13373 # rscs	r3, r1, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0e11271 # rsc	r1, r1, r1, ror r2
${ARMSEC} arm 0x40c8 0xe0f33053 # rscs	r3, r3, r3, asr r0
${ARMSEC} arm 0x40c8 0xe0e12112 # rsc	r2, r1, r2, lsl r1
${ARMSEC} arm 0x40c8 0xe0f32332 # rscs	r2, r3, r2, lsr r3
${ARMSEC} arm 0x40c8 0xe0e02111 # rsc	r2, r0, r1, lsl r1
${ARMSEC} arm 0x40c8 0xe0f10333 # rscs	r0, r1, r3, lsr r3
${ARMSEC} arm 0x40c8 0xe0e01230 # rsc	r1, r0, r0, lsr r2
${ARMSEC} arm 0x40c8 0xe0f21032 # rscs	r1, r2, r2, lsr r0
${ARMSEC} arm 0x40c8 0xe0e23250 # rsc	r3, r2, r0, asr r2
${ARMSEC} arm 0x40c8 0xe0f30371 # rscs	r0, r3, r1, ror r3
${ARMSEC} arm 0x40c8 0xe0e33211 # rsc	r3, r3, r1, lsl r2
${ARMSEC} arm 0x40c8 0xe0f00371 # rscs	r0, r0, r1, ror r3
${ARMSEC} arm 0x40c8 0xe0e01173 # rsc	r1, r0, r3, ror r1
${ARMSEC} arm 0x40c8 0xe0f02253 # rscs	r2, r0, r3, asr r2
${ARMSEC} arm 0x40c8 0xe0e21311 # rsc	r1, r2, r1, lsl r3
${ARMSEC} arm 0x40c8 0xe0f20110 # rscs	r0, r2, r0, lsl r1
${ARMSEC} arm 0x40c8 0xe0e23110 # rsc	r3, r2, r0, lsl r1
${ARMSEC} arm 0x40c8 0xe0f23031 # rscs	r3, r2, r1, lsr r0
${ARMSEC} arm 0x40c8 0xe6100f10 # sadd16	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe6130f13 # sadd16	r0, r3, r3
${ARMSEC} arm 0x40c8 0xe6103f13 # sadd16	r3, r0, r3
${ARMSEC} arm 0x40c8 0xe6133f11 # sadd16	r3, r3, r1
${ARMSEC} arm 0x40c8 0xe6102f11 # sadd16	r2, r0, r1
${ARMSEC} arm 0x40c8 0xe6133f93 # sadd8	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6131f93 # sadd8	r1, r3, r3
${ARMSEC} arm 0x40c8 0xe6123f93 # sadd8	r3, r2, r3
${ARMSEC} arm 0x40c8 0xe6111f90 # sadd8	r1, r1, r0
${ARMSEC} arm 0x40c8 0xe6123f90 # sadd8	r3, r2, r0
${ARMSEC} arm 0x40c8 0xe6100f30 # sasx	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe6120f32 # sasx	r0, r2, r2
${ARMSEC} arm 0x40c8 0xe6113f33 # sasx	r3, r1, r3
${ARMSEC} arm 0x40c8 0xe6122f31 # sasx	r2, r2, r1
${ARMSEC} arm 0x40c8 0xe6131f32 # sasx	r1, r3, r2
${ARMSEC} arm 0x40c8 0xe2c22bed # sbc	r2, r2, #242688
${ARMSEC} arm 0x40c8 0xe2d0009d # sbcs	r0, r0, #157
${ARMSEC} arm 0x40c8 0xe2c01d87 # sbc	r1, r0, #8640
${ARMSEC} arm 0x40c8 0xe2d02577 # sbcs	r2, r0, #499122176
${ARMSEC} arm 0x40c8 0xe2cf2d20 # sbc	r2, pc, #2048
${ARMSEC} arm 0x40c8 0xe2df05be # sbcs	r0, pc, #796917760
${ARMSEC} arm 0x40c8 0xe0c00ec0 # sbc	r0, r0, r0, asr #29
${ARMSEC} arm 0x40c8 0xe0d22542 # sbcs	r2, r2, r2, asr #10
${ARMSEC} arm 0x40c8 0xe0c019c0 # sbc	r1, r0, r0, asr #19
${ARMSEC} arm 0x40c8 0xe0d03ec0 # sbcs	r3, r0, r0, asr #29
${ARMSEC} arm 0x40c8 0xe0c006c1 # sbc	r0, r0, r1, asr #13
${ARMSEC} arm 0x40c8 0xe0d11360 # sbcs	r1, r1, r0, ror #6
${ARMSEC} arm 0x40c8 0xe0c12382 # sbc	r2, r1, r2, lsl #7
${ARMSEC} arm 0x40c8 0xe0d13e23 # sbcs	r3, r1, r3, lsr #28
${ARMSEC} arm 0x40c8 0xe0cf3b03 # sbc	r3, pc, r3, lsl #22
${ARMSEC} arm 0x40c8 0xe0df1ae1 # sbcs	r1, pc, r1, ror #21
${ARMSEC} arm 0x40c8 0xe0c11c8f # sbc	r1, r1, pc, lsl #25
${ARMSEC} arm 0x40c8 0xe0d11f2f # sbcs	r1, r1, pc, lsr #30
${ARMSEC} arm 0x40c8 0xe0cf18af # sbc	r1, pc, pc, lsr #17
${ARMSEC} arm 0x40c8 0xe0df3b8f # sbcs	r3, pc, pc, lsl #23
${ARMSEC} arm 0x40c8 0xe0c23b41 # sbc	r3, r2, r1, asr #22
${ARMSEC} arm 0x40c8 0xe0d31342 # sbcs	r1, r3, r2, asr #6
${ARMSEC} arm 0x40c8 0xe0cf0e02 # sbc	r0, pc, r2, lsl #28
${ARMSEC} arm 0x40c8 0xe0df0ca2 # sbcs	r0, pc, r2, lsr #25
${ARMSEC} arm 0x40c8 0xe0c13d4f # sbc	r3, r1, pc, asr #26
${ARMSEC} arm 0x40c8 0xe0d32c2f # sbcs	r2, r3, pc, lsr #24
${ARMSEC} arm 0x40c8 0xe0c00070 # sbc	r0, r0, r0, ror r0
${ARMSEC} arm 0x40c8 0xe0d00010 # sbcs	r0, r0, r0, lsl r0
${ARMSEC} arm 0x40c8 0xe0c21252 # sbc	r1, r2, r2, asr r2
${ARMSEC} arm 0x40c8 0xe0d13131 # sbcs	r3, r1, r1, lsr r1
${ARMSEC} arm 0x40c8 0xe0c00212 # sbc	r0, r0, r2, lsl r2
${ARMSEC} arm 0x40c8 0xe0d11252 # sbcs	r1, r1, r2, asr r2
${ARMSEC} arm 0x40c8 0xe0c10031 # sbc	r0, r1, r1, lsr r0
${ARMSEC} arm 0x40c8 0xe0d01110 # sbcs	r1, r0, r0, lsl r1
${ARMSEC} arm 0x40c8 0xe0c11132 # sbc	r1, r1, r2, lsr r1
${ARMSEC} arm 0x40c8 0xe0d11173 # sbcs	r1, r1, r3, ror r1
${ARMSEC} arm 0x40c8 0xe0c02272 # sbc	r2, r0, r2, ror r2
${ARMSEC} arm 0x40c8 0xe0d23333 # sbcs	r3, r2, r3, lsr r3
${ARMSEC} arm 0x40c8 0xe0c00210 # sbc	r0, r0, r0, lsl r2
${ARMSEC} arm 0x40c8 0xe0d00310 # sbcs	r0, r0, r0, lsl r3
${ARMSEC} arm 0x40c8 0xe0c03033 # sbc	r3, r0, r3, lsr r0
${ARMSEC} arm 0x40c8 0xe0d30370 # sbcs	r0, r3, r0, ror r3
${ARMSEC} arm 0x40c8 0xe0c10373 # sbc	r0, r1, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0d20171 # sbcs	r0, r2, r1, ror r1
${ARMSEC} arm 0x40c8 0xe0c10371 # sbc	r0, r1, r1, ror r3
${ARMSEC} arm 0x40c8 0xe0d23052 # sbcs	r3, r2, r2, asr r0
${ARMSEC} arm 0x40c8 0xe0c02051 # sbc	r2, r0, r1, asr r0
${ARMSEC} arm 0x40c8 0xe0d23270 # sbcs	r3, r2, r0, ror r2
${ARMSEC} arm 0x40c8 0xe0c11213 # sbc	r1, r1, r3, lsl r2
${ARMSEC} arm 0x40c8 0xe0d11312 # sbcs	r1, r1, r2, lsl r3
${ARMSEC} arm 0x40c8 0xe0c21133 # sbc	r1, r2, r3, lsr r1
${ARMSEC} arm 0x40c8 0xe0d23371 # sbcs	r3, r2, r1, ror r3
${ARMSEC} arm 0x40c8 0xe0c23073 # sbc	r3, r2, r3, ror r0
${ARMSEC} arm 0x40c8 0xe0d13073 # sbcs	r3, r1, r3, ror r0
${ARMSEC} arm 0x40c8 0xe0c03271 # sbc	r3, r0, r1, ror r2
${ARMSEC} arm 0x40c8 0xe0d01233 # sbcs	r1, r0, r3, lsr r2
${ARMSEC} arm 0x40c8 0xe7a235d3 # sbfx	r3, r3, #11, #3
${ARMSEC} arm 0x40c8 0xe7b00652 # sbfx	r0, r2, #12, #17
${ARMSEC} arm 0x40c8 0xe7a41bdf # sbfx	r1, pc, #23, #5
${ARMSEC} arm 0x40c8 0xe6800fb0 # sel	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe6832fb3 # sel	r2, r3, r3
${ARMSEC} arm 0x40c8 0xe6813fb3 # sel	r3, r1, r3
${ARMSEC} arm 0x40c8 0xe6811fb3 # sel	r1, r1, r3
${ARMSEC} arm 0x40c8 0xe6823fb1 # sel	r3, r2, r1
${ARMSEC} arm 0x40c8 0xe6322f12 # shadd16	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6323f12 # shadd16	r3, r2, r2
${ARMSEC} arm 0x40c8 0xe6332f12 # shadd16	r2, r3, r2
${ARMSEC} arm 0x40c8 0xe6300f13 # shadd16	r0, r0, r3
${ARMSEC} arm 0x40c8 0xe6332f10 # shadd16	r2, r3, r0
${ARMSEC} arm 0x40c8 0xe6333f93 # shadd8	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6301f90 # shadd8	r1, r0, r0
${ARMSEC} arm 0x40c8 0xe6303f93 # shadd8	r3, r0, r3
${ARMSEC} arm 0x40c8 0xe6311f93 # shadd8	r1, r1, r3
${ARMSEC} arm 0x40c8 0xe6301f92 # shadd8	r1, r0, r2
${ARMSEC} arm 0x40c8 0xe6322f32 # shasx	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6323f32 # shasx	r3, r2, r2
${ARMSEC} arm 0x40c8 0xe6312f32 # shasx	r2, r1, r2
${ARMSEC} arm 0x40c8 0xe6333f31 # shasx	r3, r3, r1
${ARMSEC} arm 0x40c8 0xe6320f31 # shasx	r0, r2, r1
${ARMSEC} arm 0x40c8 0xe6322f52 # shsax	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6310f51 # shsax	r0, r1, r1
${ARMSEC} arm 0x40c8 0xe6321f51 # shsax	r1, r2, r1
${ARMSEC} arm 0x40c8 0xe6322f51 # shsax	r2, r2, r1
${ARMSEC} arm 0x40c8 0xe6313f52 # shsax	r3, r1, r2
${ARMSEC} arm 0x40c8 0xe6300f70 # shsub16	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe6330f73 # shsub16	r0, r3, r3
${ARMSEC} arm 0x40c8 0xe6303f73 # shsub16	r3, r0, r3
${ARMSEC} arm 0x40c8 0xe6333f71 # shsub16	r3, r3, r1
${ARMSEC} arm 0x40c8 0xe6330f71 # shsub16	r0, r3, r1
${ARMSEC} arm 0x40c8 0xe6322ff2 # shsub8	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6313ff1 # shsub8	r3, r1, r1
${ARMSEC} arm 0x40c8 0xe6323ff3 # shsub8	r3, r2, r3
${ARMSEC} arm 0x40c8 0xe6333ff2 # shsub8	r3, r3, r2
${ARMSEC} arm 0x40c8 0xe6302ff3 # shsub8	r2, r0, r3
${ARMSEC} arm 0x40c8 0xe7000030 # smladx	r0, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe7001111 # smlad	r0, r1, r1, r1
${ARMSEC} arm 0x40c8 0xe7022333 # smladx	r2, r3, r3, r2
${ARMSEC} arm 0x40c8 0xe7030330 # smladx	r3, r0, r3, r0
${ARMSEC} arm 0x40c8 0xe7000012 # smlad	r0, r2, r0, r0
${ARMSEC} arm 0x40c8 0xe7002030 # smladx	r0, r0, r0, r2
${ARMSEC} arm 0x40c8 0xe7011331 # smladx	r1, r1, r3, r1
${ARMSEC} arm 0x40c8 0xe7001130 # smladx	r0, r0, r1, r1
${ARMSEC} arm 0x40c8 0xe7020111 # smlad	r2, r1, r1, r0
${ARMSEC} arm 0x40c8 0xe7012012 # smlad	r1, r2, r0, r2
${ARMSEC} arm 0x40c8 0xe7020031 # smladx	r2, r1, r0, r0
${ARMSEC} arm 0x40c8 0xe7022033 # smladx	r2, r3, r0, r2
${ARMSEC} arm 0x40c8 0xe7030331 # smladx	r3, r1, r3, r0
${ARMSEC} arm 0x40c8 0xe7021312 # smlad	r2, r2, r3, r1
${ARMSEC} arm 0x40c8 0xe7003132 # smladx	r0, r2, r1, r3
${ARMSEC} arm 0x40c8 0xe0e01191 # smlal	r1, r0, r1, r1
${ARMSEC} arm 0x40c8 0xe0f30090 # smlals	r0, r3, r0, r0
${ARMSEC} arm 0x40c8 0xe0e02090 # smlal	r2, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe0f20292 # smlals	r0, r2, r2, r2
${ARMSEC} arm 0x40c8 0xe0e31391 # smlal	r1, r3, r1, r3
${ARMSEC} arm 0x40c8 0xe0f03093 # smlals	r3, r0, r3, r0
${ARMSEC} arm 0x40c8 0xe0e03390 # smlal	r3, r0, r0, r3
${ARMSEC} arm 0x40c8 0xe0f03390 # smlals	r3, r0, r0, r3
${ARMSEC} arm 0x40c8 0xe0e20191 # smlal	r0, r2, r1, r1
${ARMSEC} arm 0x40c8 0xe0f03191 # smlals	r3, r0, r1, r1
${ARMSEC} arm 0x40c8 0xe0e13390 # smlal	r3, r1, r0, r3
${ARMSEC} arm 0x40c8 0xe0f01193 # smlals	r1, r0, r3, r1
${ARMSEC} arm 0x40c8 0xe0e31392 # smlal	r1, r3, r2, r3
${ARMSEC} arm 0x40c8 0xe0f30391 # smlals	r0, r3, r1, r3
${ARMSEC} arm 0x40c8 0xe0e10290 # smlal	r0, r1, r0, r2
${ARMSEC} arm 0x40c8 0xe0f03193 # smlals	r3, r0, r3, r1
${ARMSEC} arm 0x40c8 0xe0e01390 # smlal	r1, r0, r0, r3
${ARMSEC} arm 0x40c8 0xe0f32193 # smlals	r2, r3, r3, r1
${ARMSEC} arm 0x40c8 0xe0e23091 # smlal	r3, r2, r1, r0
${ARMSEC} arm 0x40c8 0xe0f20193 # smlals	r0, r2, r3, r1
${ARMSEC} arm 0x40c8 0xe7423313 # smlald	r3, r2, r3, r3
${ARMSEC} arm 0x40c8 0xe7403030 # smlaldx	r3, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe7412112 # smlald	r2, r1, r2, r1
${ARMSEC} arm 0x40c8 0xe7430033 # smlaldx	r0, r3, r3, r0
${ARMSEC} arm 0x40c8 0xe7430131 # smlaldx	r0, r3, r1, r1
${ARMSEC} arm 0x40c8 0xe7401113 # smlald	r1, r0, r3, r1
${ARMSEC} arm 0x40c8 0xe7413132 # smlaldx	r3, r1, r2, r1
${ARMSEC} arm 0x40c8 0xe7402332 # smlaldx	r2, r0, r2, r3
${ARMSEC} arm 0x40c8 0xe7402110 # smlald	r2, r0, r0, r1
${ARMSEC} arm 0x40c8 0xe7403211 # smlald	r3, r0, r1, r2
${ARMSEC} arm 0x40c8 0xe1413383 # smlalbb	r3, r1, r3, r3
${ARMSEC} arm 0x40c8 0xe14020a0 # smlaltb	r2, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe14020e2 # smlaltt	r2, r0, r2, r0
${ARMSEC} arm 0x40c8 0xe14322e3 # smlaltt	r2, r3, r3, r2
${ARMSEC} arm 0x40c8 0xe1421383 # smlalbb	r1, r2, r3, r3
${ARMSEC} arm 0x40c8 0xe1423380 # smlalbb	r3, r2, r0, r3
${ARMSEC} arm 0x40c8 0xe14121a3 # smlaltb	r2, r1, r3, r1
${ARMSEC} arm 0x40c8 0xe1432082 # smlalbb	r2, r3, r2, r0
${ARMSEC} arm 0x40c8 0xe1412081 # smlalbb	r2, r1, r1, r0
${ARMSEC} arm 0x40c8 0xe14032a1 # smlaltb	r3, r0, r1, r2
${ARMSEC} arm 0x40c8 0xe1233383 # smlawb	r3, r3, r3, r3
${ARMSEC} arm 0x40c8 0xe1230080 # smlawb	r3, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe12331c1 # smlawt	r3, r1, r1, r3
${ARMSEC} arm 0x40c8 0xe12202c0 # smlawt	r2, r0, r2, r0
${ARMSEC} arm 0x40c8 0xe1200082 # smlawb	r0, r2, r0, r0
${ARMSEC} arm 0x40c8 0xe1202080 # smlawb	r0, r0, r0, r2
${ARMSEC} arm 0x40c8 0xe12330c3 # smlawt	r3, r3, r0, r3
${ARMSEC} arm 0x40c8 0xe1213381 # smlawb	r1, r1, r3, r3
${ARMSEC} arm 0x40c8 0xe1210383 # smlawb	r1, r3, r3, r0
${ARMSEC} arm 0x40c8 0xe12123c2 # smlawt	r1, r2, r3, r2
${ARMSEC} arm 0x40c8 0xe1223380 # smlawb	r2, r0, r3, r3
${ARMSEC} arm 0x40c8 0xe1233081 # smlawb	r3, r1, r0, r3
${ARMSEC} arm 0x40c8 0xe1202083 # smlawb	r0, r3, r0, r2
${ARMSEC} arm 0x40c8 0xe12230c2 # smlawt	r2, r2, r0, r3
${ARMSEC} arm 0x40c8 0xe12102c3 # smlawt	r1, r3, r2, r0
${ARMSEC} arm 0x40c8 0xe10222a2 # smlatb	r2, r2, r2, r2
${ARMSEC} arm 0x40c8 0xe10100a0 # smlatb	r1, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe10330e0 # smlatt	r3, r0, r0, r3
${ARMSEC} arm 0x40c8 0xe10020e2 # smlatt	r0, r2, r0, r2
${ARMSEC} arm 0x40c8 0xe1022283 # smlabb	r2, r3, r2, r2
${ARMSEC} arm 0x40c8 0xe10020c0 # smlabt	r0, r0, r0, r2
${ARMSEC} arm 0x40c8 0xe10221c2 # smlabt	r2, r2, r1, r2
${ARMSEC} arm 0x40c8 0xe1013381 # smlabb	r1, r1, r3, r3
${ARMSEC} arm 0x40c8 0xe10021c1 # smlabt	r0, r1, r1, r2
${ARMSEC} arm 0x40c8 0xe10023e2 # smlatt	r0, r2, r3, r2
${ARMSEC} arm 0x40c8 0xe10322a0 # smlatb	r3, r0, r2, r2
${ARMSEC} arm 0x40c8 0xe1022180 # smlabb	r2, r0, r1, r2
${ARMSEC} arm 0x40c8 0xe10131c2 # smlabt	r1, r2, r1, r3
${ARMSEC} arm 0x40c8 0xe10210e2 # smlatt	r2, r2, r0, r1
${ARMSEC} arm 0x40c8 0xe10210c3 # smlabt	r2, r3, r0, r1
${ARMSEC} arm 0x40c8 0xe7033373 # smlsdx	r3, r3, r3, r3
${ARMSEC} arm 0x40c8 0xe7020070 # smlsdx	r2, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe7033070 # smlsdx	r3, r0, r0, r3
${ARMSEC} arm 0x40c8 0xe7002052 # smlsd	r0, r2, r0, r2
${ARMSEC} arm 0x40c8 0xe7033350 # smlsd	r3, r0, r3, r3
${ARMSEC} arm 0x40c8 0xe7032373 # smlsdx	r3, r3, r3, r2
${ARMSEC} arm 0x40c8 0xe7022172 # smlsdx	r2, r2, r1, r2
${ARMSEC} arm 0x40c8 0xe7013371 # smlsdx	r1, r1, r3, r3
${ARMSEC} arm 0x40c8 0xe7023050 # smlsd	r2, r0, r0, r3
${ARMSEC} arm 0x40c8 0xe7012072 # smlsdx	r1, r2, r0, r2
${ARMSEC} arm 0x40c8 0xe7030071 # smlsdx	r3, r1, r0, r0
${ARMSEC} arm 0x40c8 0xe7000251 # smlsd	r0, r1, r2, r0
${ARMSEC} arm 0x40c8 0xe7023271 # smlsdx	r2, r1, r2, r3
${ARMSEC} arm 0x40c8 0xe7002370 # smlsdx	r0, r0, r3, r2
${ARMSEC} arm 0x40c8 0xe7003251 # smlsd	r0, r1, r2, r3
${ARMSEC} arm 0x40c8 0xe7423373 # smlsldx	r3, r2, r3, r3
${ARMSEC} arm 0x40c8 0xe7432373 # smlsldx	r2, r3, r3, r3
${ARMSEC} arm 0x40c8 0xe7420270 # smlsldx	r0, r2, r0, r2
${ARMSEC} arm 0x40c8 0xe7431153 # smlsld	r1, r3, r3, r1
${ARMSEC} arm 0x40c8 0xe7431272 # smlsldx	r1, r3, r2, r2
${ARMSEC} arm 0x40c8 0xe7401172 # smlsldx	r1, r0, r2, r1
${ARMSEC} arm 0x40c8 0xe7401072 # smlsldx	r1, r0, r2, r0
${ARMSEC} arm 0x40c8 0xe7421071 # smlsldx	r1, r2, r1, r0
${ARMSEC} arm 0x40c8 0xe7431253 # smlsld	r1, r3, r3, r2
${ARMSEC} arm 0x40c8 0xe7401352 # smlsld	r1, r0, r2, r3
${ARMSEC} arm 0x40c8 0xe7500010 # smmla	r0, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe7502212 # smmla	r0, r2, r2, r2
${ARMSEC} arm 0x40c8 0xe7531331 # smmlar	r3, r1, r3, r1
${ARMSEC} arm 0x40c8 0xe7520032 # smmlar	r2, r2, r0, r0
${ARMSEC} arm 0x40c8 0xe7510111 # smmla	r1, r1, r1, r0
${ARMSEC} arm 0x40c8 0xe7533213 # smmla	r3, r3, r2, r3
${ARMSEC} arm 0x40c8 0xe7511113 # smmla	r1, r3, r1, r1
${ARMSEC} arm 0x40c8 0xe7511030 # smmlar	r1, r0, r0, r1
${ARMSEC} arm 0x40c8 0xe7501231 # smmlar	r0, r1, r2, r1
${ARMSEC} arm 0x40c8 0xe7520013 # smmla	r2, r3, r0, r0
${ARMSEC} arm 0x40c8 0xe7523111 # smmla	r2, r1, r1, r3
${ARMSEC} arm 0x40c8 0xe7510112 # smmla	r1, r2, r1, r0
${ARMSEC} arm 0x40c8 0xe7521312 # smmla	r2, r2, r3, r1
${ARMSEC} arm 0x40c8 0xe7511310 # smmla	r1, r0, r3, r1
${ARMSEC} arm 0x40c8 0xe7502113 # smmla	r0, r3, r1, r2
${ARMSEC} arm 0x40c8 0xe75222f2 # smmlsr	r2, r2, r2, r2
${ARMSEC} arm 0x40c8 0xe75300d0 # smmls	r3, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe75030f3 # smmlsr	r0, r3, r0, r3
${ARMSEC} arm 0x40c8 0xe75133d1 # smmls	r1, r1, r3, r3
${ARMSEC} arm 0x40c8 0xe75131f1 # smmlsr	r1, r1, r1, r3
${ARMSEC} arm 0x40c8 0xe75331f3 # smmlsr	r3, r3, r1, r3
${ARMSEC} arm 0x40c8 0xe75222f0 # smmlsr	r2, r0, r2, r2
${ARMSEC} arm 0x40c8 0xe75221f1 # smmlsr	r2, r1, r1, r2
${ARMSEC} arm 0x40c8 0xe75210f1 # smmlsr	r2, r1, r0, r1
${ARMSEC} arm 0x40c8 0xe75300d1 # smmls	r3, r1, r0, r0
${ARMSEC} arm 0x40c8 0xe75320d0 # smmls	r3, r0, r0, r2
${ARMSEC} arm 0x40c8 0xe75101d2 # smmls	r1, r2, r1, r0
${ARMSEC} arm 0x40c8 0xe75302d3 # smmls	r3, r3, r2, r0
${ARMSEC} arm 0x40c8 0xe75332f1 # smmlsr	r3, r1, r2, r3
${ARMSEC} arm 0x40c8 0xe75201f3 # smmlsr	r2, r3, r1, r0
${ARMSEC} arm 0x40c8 0xe753f333 # smmulr	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe752f333 # smmulr	r2, r3, r3
${ARMSEC} arm 0x40c8 0xe750f033 # smmulr	r0, r3, r0
${ARMSEC} arm 0x40c8 0xe753f033 # smmulr	r3, r3, r0
${ARMSEC} arm 0x40c8 0xe751f210 # smmul	r1, r0, r2
${ARMSEC} arm 0x40c8 0xe702f232 # smuadx	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe703f232 # smuadx	r3, r2, r2
${ARMSEC} arm 0x40c8 0xe703f330 # smuadx	r3, r0, r3
${ARMSEC} arm 0x40c8 0xe702f112 # smuad	r2, r2, r1
${ARMSEC} arm 0x40c8 0xe702f311 # smuad	r2, r1, r3
${ARMSEC} arm 0x40c8 0xe0c02292 # smull	r2, r0, r2, r2
${ARMSEC} arm 0x40c8 0xe0d20090 # smulls	r0, r2, r0, r0
${ARMSEC} arm 0x40c8 0xe0c30393 # smull	r0, r3, r3, r3
${ARMSEC} arm 0x40c8 0xe0d13191 # smulls	r3, r1, r1, r1
${ARMSEC} arm 0x40c8 0xe0c01190 # smull	r1, r0, r0, r1
${ARMSEC} arm 0x40c8 0xe0d32293 # smulls	r2, r3, r3, r2
${ARMSEC} arm 0x40c8 0xe0c32392 # smull	r2, r3, r2, r3
${ARMSEC} arm 0x40c8 0xe0d21291 # smulls	r1, r2, r1, r2
${ARMSEC} arm 0x40c8 0xe0c10393 # smull	r0, r1, r3, r3
${ARMSEC} arm 0x40c8 0xe0d03191 # smulls	r3, r0, r1, r1
${ARMSEC} arm 0x40c8 0xe0c23093 # smull	r3, r2, r3, r0
${ARMSEC} arm 0x40c8 0xe0d02192 # smulls	r2, r0, r2, r1
${ARMSEC} arm 0x40c8 0xe0c10291 # smull	r0, r1, r1, r2
${ARMSEC} arm 0x40c8 0xe0d10291 # smulls	r0, r1, r1, r2
${ARMSEC} arm 0x40c8 0xe0c01192 # smull	r1, r0, r2, r1
${ARMSEC} arm 0x40c8 0xe0d12293 # smulls	r2, r1, r3, r2
${ARMSEC} arm 0x40c8 0xe0c21293 # smull	r1, r2, r3, r2
${ARMSEC} arm 0x40c8 0xe0d13190 # smulls	r3, r1, r0, r1
${ARMSEC} arm 0x40c8 0xe0c30291 # smull	r0, r3, r1, r2
${ARMSEC} arm 0x40c8 0xe0d31290 # smulls	r1, r3, r0, r2
${ARMSEC} arm 0x40c8 0xe12303e3 # smulwt	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe12300a0 # smulwb	r3, r0, r0
${ARMSEC} arm 0x40c8 0xe12302e3 # smulwt	r3, r3, r2
${ARMSEC} arm 0x40c8 0xe12202a0 # smulwb	r2, r0, r2
${ARMSEC} arm 0x40c8 0xe12203a1 # smulwb	r2, r1, r3
${ARMSEC} arm 0x40c8 0xe16101c1 # smulbt	r1, r1, r1
${ARMSEC} arm 0x40c8 0xe1630282 # smulbb	r3, r2, r2
${ARMSEC} arm 0x40c8 0xe16000e1 # smultt	r0, r1, r0
${ARMSEC} arm 0x40c8 0xe1610081 # smulbb	r1, r1, r0
${ARMSEC} arm 0x40c8 0xe1600381 # smulbb	r0, r1, r3
${ARMSEC} arm 0x40c8 0xe702f252 # smusd	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe700f272 # smusdx	r0, r2, r2
${ARMSEC} arm 0x40c8 0xe700f072 # smusdx	r0, r2, r0
${ARMSEC} arm 0x40c8 0xe701f271 # smusdx	r1, r1, r2
${ARMSEC} arm 0x40c8 0xe702f370 # smusdx	r2, r0, r3
${ARMSEC} arm 0x40c8 0xe6b42452 # ssat	r2, #21, r2, asr #8
${ARMSEC} arm 0x40c8 0xe6b03b90 # ssat	r3, #17, r0, lsl #23
${ARMSEC} arm 0x40c8 0xe6a02f32 # ssat16	r2, #1, r2
${ARMSEC} arm 0x40c8 0xe6ae1f30 # ssat16	r1, #15, r0
${ARMSEC} arm 0x40c8 0xe6133f53 # ssax	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6131f53 # ssax	r1, r3, r3
${ARMSEC} arm 0x40c8 0xe6110f50 # ssax	r0, r1, r0
${ARMSEC} arm 0x40c8 0xe6122f50 # ssax	r2, r2, r0
${ARMSEC} arm 0x40c8 0xe6101f53 # ssax	r1, r0, r3
${ARMSEC} arm 0x40c8 0xe6133f73 # ssub16	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6123f72 # ssub16	r3, r2, r2
${ARMSEC} arm 0x40c8 0xe6123f73 # ssub16	r3, r2, r3
${ARMSEC} arm 0x40c8 0xe6100f72 # ssub16	r0, r0, r2
${ARMSEC} arm 0x40c8 0xe6112f70 # ssub16	r2, r1, r0
${ARMSEC} arm 0x40c8 0xe6122ff2 # ssub8	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6132ff3 # ssub8	r2, r3, r3
${ARMSEC} arm 0x40c8 0xe6113ff3 # ssub8	r3, r1, r3
${ARMSEC} arm 0x40c8 0xe6133ff2 # ssub8	r3, r3, r2
${ARMSEC} arm 0x40c8 0xe6130ff2 # ssub8	r0, r3, r2
${ARMSEC} arm 0x40c8 0xe8030008 # stmda	r3, {r3}
${ARMSEC} arm 0x40c8 0xe8020001 # stmda	r2, {r0}
${ARMSEC} arm 0x40c8 0xe9018000 # stmdb	r1, {pc}
${ARMSEC} arm 0x40c8 0xe9200001 # stmdb	r0!, {r0}
${ARMSEC} arm 0x40c8 0xe803000c # stmda	r3, {r2, r3}
${ARMSEC} arm 0x40c8 0xe9808001 # stmib	r0, {r0, pc}
${ARMSEC} arm 0x40c8 0xe801000a # stmda	r1, {r1, r3}
${ARMSEC} arm 0x40c8 0xe9008001 # stmdb	r0, {r0, pc}
${ARMSEC} arm 0x40c8 0xe9210004 # stmdb	r1!, {r2}
${ARMSEC} arm 0x40c8 0xe9238000 # stmdb	r3!, {pc}
${ARMSEC} arm 0x40c8 0xe981000c # stmib	r1, {r2, r3}
${ARMSEC} arm 0x40c8 0xe9818001 # stmib	r1, {r0, pc}
${ARMSEC} arm 0x40c8 0xe8028001 # stmda	r2, {r0, pc}
${ARMSEC} arm 0x40c8 0xe9a10006 # stmib	r1!, {r1, r2}
${ARMSEC} arm 0x40c8 0xe9a08001 # stmib	r0!, {r0, pc}
${ARMSEC} arm 0x40c8 0xe821000a # stmda	r1!, {r1, r3}
${ARMSEC} arm 0x40c8 0xe981000e # stmib	r1, {r1, r2, r3}
${ARMSEC} arm 0x40c8 0xe8808005 # stmia	r0, {r0, r2, pc}
${ARMSEC} arm 0x40c8 0xe882000d # stmia	r2, {r0, r2, r3}
${ARMSEC} arm 0x40c8 0xe883800c # stmia	r3, {r2, r3, pc}
${ARMSEC} arm 0x40c8 0xe900000d # stmdb	r0, {r0, r2, r3}
${ARMSEC} arm 0x40c8 0xe803800c # stmda	r3, {r2, r3, pc}
${ARMSEC} arm 0x40c8 0xe9008005 # stmdb	r0, {r0, r2, pc}
${ARMSEC} arm 0x40c8 0xe921000c # stmdb	r1!, {r2, r3}
${ARMSEC} arm 0x40c8 0xe8a28001 # stmia	r2!, {r0, pc}
${ARMSEC} arm 0x40c8 0xe8238002 # stmda	r3!, {r1, pc}
${ARMSEC} arm 0x40c8 0xe8830007 # stmia	r3, {r0, r1, r2}
${ARMSEC} arm 0x40c8 0xe980800a # stmib	r0, {r1, r3, pc}
${ARMSEC} arm 0x40c8 0xe9028003 # stmdb	r2, {r0, r1, pc}
${ARMSEC} arm 0x40c8 0xe9a0000b # stmib	r0!, {r0, r1, r3}
${ARMSEC} arm 0x40c8 0xe9a08003 # stmib	r0!, {r0, r1, pc}
${ARMSEC} arm 0x40c8 0xe8200007 # stmda	r0!, {r0, r1, r2}
${ARMSEC} arm 0x40c8 0xe8208003 # stmda	r0!, {r0, r1, pc}
${ARMSEC} arm 0x40c8 0xe880800b # stmia	r0, {r0, r1, r3, pc}
${ARMSEC} arm 0x40c8 0xe981000f # stmib	r1, {r0, r1, r2, r3}
${ARMSEC} arm 0x40c8 0xe9818007 # stmib	r1, {r0, r1, r2, pc}
${ARMSEC} arm 0x40c8 0xe982000f # stmib	r2, {r0, r1, r2, r3}
${ARMSEC} arm 0x40c8 0xe883800e # stmia	r3, {r1, r2, r3, pc}
${ARMSEC} arm 0x40c8 0xe903800d # stmdb	r3, {r0, r2, r3, pc}
${ARMSEC} arm 0x40c8 0xe801800b # stmda	r1, {r0, r1, r3, pc}
${ARMSEC} arm 0x40c8 0xe900800b # stmdb	r0, {r0, r1, r3, pc}
${ARMSEC} arm 0x40c8 0xe822000b # stmda	r2!, {r0, r1, r3}
${ARMSEC} arm 0x40c8 0xe9a08006 # stmib	r0!, {r1, r2, pc}
${ARMSEC} arm 0x40c8 0xe920800c # stmdb	r0!, {r2, r3, pc}
${ARMSEC} arm 0x40c8 0xe980800e # stmib	r0, {r1, r2, r3, pc}
${ARMSEC} arm 0x40c8 0xe9a0800b # stmib	r0!, {r0, r1, r3, pc}
${ARMSEC} arm 0x40c8 0xe920800d # stmdb	r0!, {r0, r2, r3, pc}
${ARMSEC} arm 0x40c8 0xe9a0800e # stmib	r0!, {r1, r2, r3, pc}
${ARMSEC} arm 0x40c8 0xe50006d2 # str	r0, [r0, #-1746]
${ARMSEC} arm 0x40c8 0xe502041e # str	r0, [r2, #-1054]
${ARMSEC} arm 0x40c8 0xe503f9c1 # str	pc, [r3, #-2497]
${ARMSEC} arm 0x40c8 0xe4802351 # str	r2, [r0], #849
${ARMSEC} arm 0x40c8 0xe403f79c # str	pc, [r3], #-1948
${ARMSEC} arm 0x40c8 0xe7011021 # str	r1, [r1, -r1, lsr #32]
${ARMSEC} arm 0x40c8 0xe7012021 # str	r2, [r1, -r1, lsr #32]
${ARMSEC} arm 0x40c8 0xe782f022 # str	pc, [r2, r2, lsr #32]
${ARMSEC} arm 0x40c8 0xe7812602 # str	r2, [r1, r2, lsl #12]
${ARMSEC} arm 0x40c8 0xe7822bc3 # str	r2, [r2, r3, asr #23]
${ARMSEC} arm 0x40c8 0xe7813242 # str	r3, [r1, r2, asr #4]
${ARMSEC} arm 0x40c8 0xe781ff83 # str	pc, [r1, r3, lsl #31]
${ARMSEC} arm 0x40c8 0xe6010980 # str	r0, [r1], -r0, lsl #19
${ARMSEC} arm 0x40c8 0xe60105a1 # str	r0, [r1], -r1, lsr #11
${ARMSEC} arm 0x40c8 0xe683fb03 # str	pc, [r3], r3, lsl #22
${ARMSEC} arm 0x40c8 0xe7221120 # str	r1, [r2, -r0, lsr #2]!
${ARMSEC} arm 0x40c8 0xe7a2f963 # str	pc, [r2, r3, ror #18]!
${ARMSEC} arm 0x40c8 0xe5422fb3 # strb	r2, [r2, #-4019]
${ARMSEC} arm 0x40c8 0xe5c21299 # strb	r1, [r2, #665]
${ARMSEC} arm 0x40c8 0xe5e012a9 # strb	r1, [r0, #681]!
${ARMSEC} arm 0x40c8 0xe7400020 # strb	r0, [r0, -r0, lsr #32]
${ARMSEC} arm 0x40c8 0xe7430023 # strb	r0, [r3, -r3, lsr #32]
${ARMSEC} arm 0x40c8 0xe74206c0 # strb	r0, [r2, -r0, asr #13]
${ARMSEC} arm 0x40c8 0xe7c33282 # strb	r3, [r3, r2, lsl #5]
${ARMSEC} arm 0x40c8 0xe7c01423 # strb	r1, [r0, r3, lsr #8]
${ARMSEC} arm 0x40c8 0xe7601d41 # strb	r1, [r0, -r1, asr #26]!
${ARMSEC} arm 0x40c8 0xe6413de1 # strb	r3, [r1], -r1, ror #27
${ARMSEC} arm 0x40c8 0xe6420861 # strb	r0, [r2], -r1, ror #16
${ARMSEC} arm 0x40c8 0xe4e21507 # strbt	r1, [r2], #1287
${ARMSEC} arm 0x40c8 0xe66232e3 # strbt	r3, [r2], -r3, ror #5
${ARMSEC} arm 0x40c8 0xe6e02600 # strbt	r2, [r0], r0, lsl #12
${ARMSEC} arm 0x40c8 0xe66035e1 # strbt	r3, [r0], -r1, ror #11
${ARMSEC} arm 0x40c8 0xe1c001f4 # strd	r0, [r0, #20]
${ARMSEC} arm 0x40c8 0xe1c109f7 # strd	r0, [r1, #151]
${ARMSEC} arm 0x40c8 0xe14121fe # strd	r2, [r1, #-30]
${ARMSEC} arm 0x40c8 0xe0c20ff2 # strd	r0, [r2], #242
${ARMSEC} arm 0x40c8 0xe10100f0 # strd	r0, [r1, -r0]
${ARMSEC} arm 0x40c8 0xe18220f3 # strd	r2, [r2, r3]
${ARMSEC} arm 0x40c8 0xe18300f0 # strd	r0, [r3, r0]
${ARMSEC} arm 0x40c8 0xe18000f3 # strd	r0, [r0, r3]
${ARMSEC} arm 0x40c8 0xe18120f3 # strd	r2, [r1, r3]
${ARMSEC} arm 0x40c8 0xe18320f0 # strd	r2, [r3, r0]
${ARMSEC} arm 0x40c8 0xe10300f2 # strd	r0, [r3, -r2]
${ARMSEC} arm 0x40c8 0xe12200f0 # strd	r0, [r2, -r0]!
${ARMSEC} arm 0x40c8 0xe08200f1 # strd	r0, [r2], r1
${ARMSEC} arm 0x40c8 0xe00300f3 # strd	r0, [r3], -r3
${ARMSEC} arm 0x40c8 0xe08020f1 # strd	r2, [r0], r1
${ARMSEC} arm 0x40c8 0xe1c11bb4 # strh	r1, [r1, #180]
${ARMSEC} arm 0x40c8 0xe14120b1 # strh	r2, [r1, #-1]
${ARMSEC} arm 0x40c8 0xe1e314b6 # strh	r1, [r3, #70]!
${ARMSEC} arm 0x40c8 0xe18310b1 # strh	r1, [r3, r1]
${ARMSEC} arm 0x40c8 0xe10330b0 # strh	r3, [r3, -r0]
${ARMSEC} arm 0x40c8 0xe18130b0 # strh	r3, [r1, r0]
${ARMSEC} arm 0x40c8 0xe08320b2 # strh	r2, [r3], r2
${ARMSEC} arm 0x40c8 0xe08030b0 # strh	r3, [r0], r0
${ARMSEC} arm 0x40c8 0xe00320b1 # strh	r2, [r3], -r1
${ARMSEC} arm 0x40c8 0xe0e12db1 # strht	r2, [r1], #209
${ARMSEC} arm 0x40c8 0xe02200b0 # strht	r0, [r2], -r0
${ARMSEC} arm 0x40c8 0xe02130b1 # strht	r3, [r1], -r1
${ARMSEC} arm 0x40c8 0xe02100b2 # strht	r0, [r1], -r2
${ARMSEC} arm 0x40c8 0xe4a10c67 # strt	r0, [r1], #3175
${ARMSEC} arm 0x40c8 0xe420f881 # strt	pc, [r0], #-2177
${ARMSEC} arm 0x40c8 0xe6a32942 # strt	r2, [r3], r2, asr #18
${ARMSEC} arm 0x40c8 0xe6232be3 # strt	r2, [r3], -r3, ror #23
${ARMSEC} arm 0x40c8 0xe620fe20 # strt	pc, [r0], -r0, lsr #28
${ARMSEC} arm 0x40c8 0xe6a02a21 # strt	r2, [r0], r1, lsr #20
${ARMSEC} arm 0x40c8 0xe622f920 # strt	pc, [r2], -r0, lsr #18
${ARMSEC} arm 0x40c8 0xe24221b4 # sub	r2, r2, #45
${ARMSEC} arm 0x40c8 0xe25335ea # subs	r3, r3, #981467136
${ARMSEC} arm 0x40c8 0xe2412201 # sub	r2, r1, #268435456
${ARMSEC} arm 0x40c8 0xe2513c7d # subs	r3, r1, #32000
${ARMSEC} arm 0x40c8 0xe24f3caa # sub	r3, pc, #43520
${ARMSEC} arm 0x40c8 0xe25f162f # subs	r1, pc, #49283072
${ARMSEC} arm 0x40c8 0xe04222e2 # sub	r2, r2, r2, ror #5
${ARMSEC} arm 0x40c8 0xe05228c2 # subs	r2, r2, r2, asr #17
${ARMSEC} arm 0x40c8 0xe04014e0 # sub	r1, r0, r0, ror #9
${ARMSEC} arm 0x40c8 0xe05324c3 # subs	r2, r3, r3, asr #9
${ARMSEC} arm 0x40c8 0xe0433522 # sub	r3, r3, r2, lsr #10
${ARMSEC} arm 0x40c8 0xe0500be3 # subs	r0, r0, r3, ror #23
${ARMSEC} arm 0x40c8 0xe0401441 # sub	r1, r0, r1, asr #8
${ARMSEC} arm 0x40c8 0xe05102c0 # subs	r0, r1, r0, asr #5
${ARMSEC} arm 0x40c8 0xe04f0520 # sub	r0, pc, r0, lsr #10
${ARMSEC} arm 0x40c8 0xe05f0120 # subs	r0, pc, r0, lsr #2
${ARMSEC} arm 0x40c8 0xe042266f # sub	r2, r2, pc, ror #12
${ARMSEC} arm 0x40c8 0xe0533c6f # subs	r3, r3, pc, ror #24
${ARMSEC} arm 0x40c8 0xe04f36cf # sub	r3, pc, pc, asr #13
${ARMSEC} arm 0x40c8 0xe05f07ef # subs	r0, pc, pc, ror #15
${ARMSEC} arm 0x40c8 0xe0431d02 # sub	r1, r3, r2, lsl #26
${ARMSEC} arm 0x40c8 0xe0510b22 # subs	r0, r1, r2, lsr #22
${ARMSEC} arm 0x40c8 0xe04f0f63 # sub	r0, pc, r3, ror #30
${ARMSEC} arm 0x40c8 0xe05f1a00 # subs	r1, pc, r0, lsl #20
${ARMSEC} arm 0x40c8 0xe04021ef # sub	r2, r0, pc, ror #3
${ARMSEC} arm 0x40c8 0xe05204af # subs	r0, r2, pc, lsr #9
${ARMSEC} arm 0x40c8 0xe0433373 # sub	r3, r3, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0500030 # subs	r0, r0, r0, lsr r0
${ARMSEC} arm 0x40c8 0xe0420212 # sub	r0, r2, r2, lsl r2
${ARMSEC} arm 0x40c8 0xe0520232 # subs	r0, r2, r2, lsr r2
${ARMSEC} arm 0x40c8 0xe0422353 # sub	r2, r2, r3, asr r3
${ARMSEC} arm 0x40c8 0xe0522171 # subs	r2, r2, r1, ror r1
${ARMSEC} arm 0x40c8 0xe0431113 # sub	r1, r3, r3, lsl r1
${ARMSEC} arm 0x40c8 0xe0501130 # subs	r1, r0, r0, lsr r1
${ARMSEC} arm 0x40c8 0xe0400033 # sub	r0, r0, r3, lsr r0
${ARMSEC} arm 0x40c8 0xe0533350 # subs	r3, r3, r0, asr r3
${ARMSEC} arm 0x40c8 0xe0403313 # sub	r3, r0, r3, lsl r3
${ARMSEC} arm 0x40c8 0xe0532212 # subs	r2, r3, r2, lsl r2
${ARMSEC} arm 0x40c8 0xe0433213 # sub	r3, r3, r3, lsl r2
${ARMSEC} arm 0x40c8 0xe0533113 # subs	r3, r3, r3, lsl r1
${ARMSEC} arm 0x40c8 0xe0403053 # sub	r3, r0, r3, asr r0
${ARMSEC} arm 0x40c8 0xe0530350 # subs	r0, r3, r0, asr r3
${ARMSEC} arm 0x40c8 0xe0420353 # sub	r0, r2, r3, asr r3
${ARMSEC} arm 0x40c8 0xe0512373 # subs	r2, r1, r3, ror r3
${ARMSEC} arm 0x40c8 0xe0403230 # sub	r3, r0, r0, lsr r2
${ARMSEC} arm 0x40c8 0xe0523152 # subs	r3, r2, r2, asr r1
${ARMSEC} arm 0x40c8 0xe0423251 # sub	r3, r2, r1, asr r2
${ARMSEC} arm 0x40c8 0xe0532330 # subs	r2, r3, r0, lsr r3
${ARMSEC} arm 0x40c8 0xe0433271 # sub	r3, r3, r1, ror r2
${ARMSEC} arm 0x40c8 0xe0511253 # subs	r1, r1, r3, asr r2
${ARMSEC} arm 0x40c8 0xe0410053 # sub	r0, r1, r3, asr r0
${ARMSEC} arm 0x40c8 0xe0520013 # subs	r0, r2, r3, lsl r0
${ARMSEC} arm 0x40c8 0xe0402112 # sub	r2, r0, r2, lsl r1
${ARMSEC} arm 0x40c8 0xe0502112 # subs	r2, r0, r2, lsl r1
${ARMSEC} arm 0x40c8 0xe0402371 # sub	r2, r0, r1, ror r3
${ARMSEC} arm 0x40c8 0xe0523051 # subs	r3, r2, r1, asr r0
${ARMSEC} arm 0x40c8 0xe1000090 # swp	r0, r0, [r0]
${ARMSEC} arm 0x40c8 0xe1013091 # swp	r3, r1, [r1]
${ARMSEC} arm 0x40c8 0xe1033091 # swp	r3, r1, [r3]
${ARMSEC} arm 0x40c8 0xe1020090 # swp	r0, r0, [r2]
${ARMSEC} arm 0x40c8 0xe102209f # swp	r2, pc, [r2]
${ARMSEC} arm 0x40c8 0xe1020091 # swp	r0, r1, [r2]
${ARMSEC} arm 0x40c8 0xe102309f # swp	r3, pc, [r2]
${ARMSEC} arm 0x40c8 0xe1411091 # swpb	r1, r1, [r1]
${ARMSEC} arm 0x40c8 0xe1402090 # swpb	r2, r0, [r0]
${ARMSEC} arm 0x40c8 0xe1422090 # swpb	r2, r0, [r2]
${ARMSEC} arm 0x40c8 0xe1410090 # swpb	r0, r0, [r1]
${ARMSEC} arm 0x40c8 0xe141109f # swpb	r1, pc, [r1]
${ARMSEC} arm 0x40c8 0xe1430092 # swpb	r0, r2, [r3]
${ARMSEC} arm 0x40c8 0xe141009f # swpb	r0, pc, [r1]
${ARMSEC} arm 0x40c8 0xe6a11c71 # sxtab	r1, r1, r1, ror #24
${ARMSEC} arm 0x40c8 0xe6a20872 # sxtab	r0, r2, r2, ror #16
${ARMSEC} arm 0x40c8 0xe6a32072 # sxtab	r2, r3, r2
${ARMSEC} arm 0x40c8 0xe6a33872 # sxtab	r3, r3, r2, ror #16
${ARMSEC} arm 0x40c8 0xe6a32c70 # sxtab	r2, r3, r0, ror #24
${ARMSEC} arm 0x40c8 0xe6800870 # sxtab16	r0, r0, r0, ror #16
${ARMSEC} arm 0x40c8 0xe6830873 # sxtab16	r0, r3, r3, ror #16
${ARMSEC} arm 0x40c8 0xe6820470 # sxtab16	r0, r2, r0, ror #8
${ARMSEC} arm 0x40c8 0xe6833c70 # sxtab16	r3, r3, r0, ror #24
${ARMSEC} arm 0x40c8 0xe6802471 # sxtab16	r2, r0, r1, ror #8
${ARMSEC} arm 0x40c8 0xe6b22c72 # sxtah	r2, r2, r2, ror #24
${ARMSEC} arm 0x40c8 0xe6b32073 # sxtah	r2, r3, r3
${ARMSEC} arm 0x40c8 0xe6b32072 # sxtah	r2, r3, r2
${ARMSEC} arm 0x40c8 0xe6b33471 # sxtah	r3, r3, r1, ror #8
${ARMSEC} arm 0x40c8 0xe6b02071 # sxtah	r2, r0, r1
${ARMSEC} arm 0x40c8 0xe6af0470 # sxtb	r0, r0, ror #8
${ARMSEC} arm 0x40c8 0xe6af1470 # sxtb	r1, r0, ror #8
${ARMSEC} arm 0x40c8 0xe68f2c72 # sxtb16	r2, r2, ror #24
${ARMSEC} arm 0x40c8 0xe68f2870 # sxtb16	r2, r0, ror #16
${ARMSEC} arm 0x40c8 0xe6bf0470 # sxth	r0, r0, ror #8
${ARMSEC} arm 0x40c8 0xe6bf2873 # sxth	r2, r3, ror #16
${ARMSEC} arm 0x40c8 0xe3310eed # teq	r1, #3792
${ARMSEC} arm 0x40c8 0xe33f02ce # teq	pc, #-536870900
${ARMSEC} arm 0x40c8 0xe1320ec2 # teq	r2, r2, asr #29
${ARMSEC} arm 0x40c8 0xe13f050f # teq	pc, pc, lsl #10
${ARMSEC} arm 0x40c8 0xe1320623 # teq	r2, r3, lsr #12
${ARMSEC} arm 0x40c8 0xe13f0581 # teq	pc, r1, lsl #11
${ARMSEC} arm 0x40c8 0xe1310b8f # teq	r1, pc, lsl #23
${ARMSEC} arm 0x40c8 0xe1310131 # teq	r1, r1, lsr r1
${ARMSEC} arm 0x40c8 0xe1330232 # teq	r3, r2, lsr r2
${ARMSEC} arm 0x40c8 0xe1300170 # teq	r0, r0, ror r1
${ARMSEC} arm 0x40c8 0xe1330331 # teq	r3, r1, lsr r3
${ARMSEC} arm 0x40c8 0xe1300233 # teq	r0, r3, lsr r2
${ARMSEC} arm 0x40c8 0xe31204b9 # tst	r2, #-1191182336
${ARMSEC} arm 0x40c8 0xe31f01ba # tst	pc, #-2147483602
${ARMSEC} arm 0x40c8 0xe1130083 # tst	r3, r3, lsl #1
${ARMSEC} arm 0x40c8 0xe11f0a0f # tst	pc, pc, lsl #20
${ARMSEC} arm 0x40c8 0xe1110243 # tst	r1, r3, asr #4
${ARMSEC} arm 0x40c8 0xe11005af # tst	r0, pc, lsr #11
${ARMSEC} arm 0x40c8 0xe11f0e00 # tst	pc, r0, lsl #28
${ARMSEC} arm 0x40c8 0xe1110111 # tst	r1, r1, lsl r1
${ARMSEC} arm 0x40c8 0xe1120131 # tst	r2, r1, lsr r1
${ARMSEC} arm 0x40c8 0xe1100170 # tst	r0, r0, ror r1
${ARMSEC} arm 0x40c8 0xe1120231 # tst	r2, r1, lsr r2
${ARMSEC} arm 0x40c8 0xe1110033 # tst	r1, r3, lsr r0
${ARMSEC} arm 0x40c8 0xe6533f13 # uadd16	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6521f12 # uadd16	r1, r2, r2
${ARMSEC} arm 0x40c8 0xe6513f13 # uadd16	r3, r1, r3
${ARMSEC} arm 0x40c8 0xe6500f12 # uadd16	r0, r0, r2
${ARMSEC} arm 0x40c8 0xe6502f13 # uadd16	r2, r0, r3
${ARMSEC} arm 0x40c8 0xe6522f92 # uadd8	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6512f91 # uadd8	r2, r1, r1
${ARMSEC} arm 0x40c8 0xe6512f92 # uadd8	r2, r1, r2
${ARMSEC} arm 0x40c8 0xe6533f92 # uadd8	r3, r3, r2
${ARMSEC} arm 0x40c8 0xe6523f90 # uadd8	r3, r2, r0
${ARMSEC} arm 0x40c8 0xe6522f32 # uasx	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6523f32 # uasx	r3, r2, r2
${ARMSEC} arm 0x40c8 0xe6532f32 # uasx	r2, r3, r2
${ARMSEC} arm 0x40c8 0xe6500f33 # uasx	r0, r0, r3
${ARMSEC} arm 0x40c8 0xe6532f30 # uasx	r2, r3, r0
${ARMSEC} arm 0x40c8 0xe7f91051 # ubfx	r1, r1, #0, #26
${ARMSEC} arm 0x40c8 0xe7e309d3 # ubfx	r0, r3, #19, #4
${ARMSEC} arm 0x40c8 0xe7e901df # ubfx	r0, pc, #3, #10
${ARMSEC} arm 0x40c8 0xe6700f10 # uhadd16	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe6720f12 # uhadd16	r0, r2, r2
${ARMSEC} arm 0x40c8 0xe6713f13 # uhadd16	r3, r1, r3
${ARMSEC} arm 0x40c8 0xe6722f11 # uhadd16	r2, r2, r1
${ARMSEC} arm 0x40c8 0xe6731f12 # uhadd16	r1, r3, r2
${ARMSEC} arm 0x40c8 0xe6733f93 # uhadd8	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6731f93 # uhadd8	r1, r3, r3
${ARMSEC} arm 0x40c8 0xe6710f90 # uhadd8	r0, r1, r0
${ARMSEC} arm 0x40c8 0xe6722f90 # uhadd8	r2, r2, r0
${ARMSEC} arm 0x40c8 0xe6701f93 # uhadd8	r1, r0, r3
${ARMSEC} arm 0x40c8 0xe6700f30 # uhasx	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe6730f33 # uhasx	r0, r3, r3
${ARMSEC} arm 0x40c8 0xe6703f33 # uhasx	r3, r0, r3
${ARMSEC} arm 0x40c8 0xe6733f31 # uhasx	r3, r3, r1
${ARMSEC} arm 0x40c8 0xe6730f31 # uhasx	r0, r3, r1
${ARMSEC} arm 0x40c8 0xe6722f52 # uhsax	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6713f51 # uhsax	r3, r1, r1
${ARMSEC} arm 0x40c8 0xe6723f53 # uhsax	r3, r2, r3
${ARMSEC} arm 0x40c8 0xe6733f52 # uhsax	r3, r3, r2
${ARMSEC} arm 0x40c8 0xe6702f53 # uhsax	r2, r0, r3
${ARMSEC} arm 0x40c8 0xe6722f72 # uhsub16	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6723f72 # uhsub16	r3, r2, r2
${ARMSEC} arm 0x40c8 0xe6712f72 # uhsub16	r2, r1, r2
${ARMSEC} arm 0x40c8 0xe6733f71 # uhsub16	r3, r3, r1
${ARMSEC} arm 0x40c8 0xe6720f71 # uhsub16	r0, r2, r1
${ARMSEC} arm 0x40c8 0xe6722ff2 # uhsub8	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6710ff1 # uhsub8	r0, r1, r1
${ARMSEC} arm 0x40c8 0xe6721ff1 # uhsub8	r1, r2, r1
${ARMSEC} arm 0x40c8 0xe6722ff1 # uhsub8	r2, r2, r1
${ARMSEC} arm 0x40c8 0xe6713ff2 # uhsub8	r3, r1, r2
${ARMSEC} arm 0x40c8 0xe0423292 # umaal	r3, r2, r2, r2
${ARMSEC} arm 0x40c8 0xe0423293 # umaal	r3, r2, r3, r2
${ARMSEC} arm 0x40c8 0xe0413391 # umaal	r3, r1, r1, r3
${ARMSEC} arm 0x40c8 0xe0430090 # umaal	r0, r3, r0, r0
${ARMSEC} arm 0x40c8 0xe0412193 # umaal	r2, r1, r3, r1
${ARMSEC} arm 0x40c8 0xe0413392 # umaal	r3, r1, r2, r3
${ARMSEC} arm 0x40c8 0xe0410391 # umaal	r0, r1, r1, r3
${ARMSEC} arm 0x40c8 0xe0403193 # umaal	r3, r0, r3, r1
${ARMSEC} arm 0x40c8 0xe0410292 # umaal	r0, r1, r2, r2
${ARMSEC} arm 0x40c8 0xe0402193 # umaal	r2, r0, r3, r1
${ARMSEC} arm 0x40c8 0xe0a32393 # umlal	r2, r3, r3, r3
${ARMSEC} arm 0x40c8 0xe0b03090 # umlals	r3, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe0a02092 # umlal	r2, r0, r2, r0
${ARMSEC} arm 0x40c8 0xe0b01091 # umlals	r1, r0, r1, r0
${ARMSEC} arm 0x40c8 0xe0a10091 # umlal	r0, r1, r1, r0
${ARMSEC} arm 0x40c8 0xe0b12291 # umlals	r2, r1, r1, r2
${ARMSEC} arm 0x40c8 0xe0a01191 # umlal	r1, r0, r1, r1
${ARMSEC} arm 0x40c8 0xe0b20090 # umlals	r0, r2, r0, r0
${ARMSEC} arm 0x40c8 0xe0a23291 # umlal	r3, r2, r1, r2
${ARMSEC} arm 0x40c8 0xe0b10193 # umlals	r0, r1, r3, r1
${ARMSEC} arm 0x40c8 0xe0a21190 # umlal	r1, r2, r0, r1
${ARMSEC} arm 0x40c8 0xe0b32290 # umlals	r2, r3, r0, r2
${ARMSEC} arm 0x40c8 0xe0a21092 # umlal	r1, r2, r2, r0
${ARMSEC} arm 0x40c8 0xe0b20392 # umlals	r0, r2, r2, r3
${ARMSEC} arm 0x40c8 0xe0a30290 # umlal	r0, r3, r0, r2
${ARMSEC} arm 0x40c8 0xe0b30290 # umlals	r0, r3, r0, r2
${ARMSEC} arm 0x40c8 0xe0a03292 # umlal	r3, r0, r2, r2
${ARMSEC} arm 0x40c8 0xe0b21393 # umlals	r1, r2, r3, r3
${ARMSEC} arm 0x40c8 0xe0a12390 # umlal	r2, r1, r0, r3
${ARMSEC} arm 0x40c8 0xe0b20193 # umlals	r0, r2, r3, r1
${ARMSEC} arm 0x40c8 0xe0820090 # umull	r0, r2, r0, r0
${ARMSEC} arm 0x40c8 0xe0931191 # umulls	r1, r3, r1, r1
${ARMSEC} arm 0x40c8 0xe0803090 # umull	r3, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe0920292 # umulls	r0, r2, r2, r2
${ARMSEC} arm 0x40c8 0xe0810091 # umull	r0, r1, r1, r0
${ARMSEC} arm 0x40c8 0xe0930093 # umulls	r0, r3, r3, r0
${ARMSEC} arm 0x40c8 0xe0813193 # umull	r3, r1, r3, r1
${ARMSEC} arm 0x40c8 0xe0902092 # umulls	r2, r0, r2, r0
${ARMSEC} arm 0x40c8 0xe0830191 # umull	r0, r3, r1, r1
${ARMSEC} arm 0x40c8 0xe0932191 # umulls	r2, r3, r1, r1
${ARMSEC} arm 0x40c8 0xe0801291 # umull	r1, r0, r1, r2
${ARMSEC} arm 0x40c8 0xe0901291 # umulls	r1, r0, r1, r2
${ARMSEC} arm 0x40c8 0xe0803290 # umull	r3, r0, r0, r2
${ARMSEC} arm 0x40c8 0xe0901290 # umulls	r1, r0, r0, r2
${ARMSEC} arm 0x40c8 0xe0823391 # umull	r3, r2, r1, r3
${ARMSEC} arm 0x40c8 0xe0921193 # umulls	r1, r2, r3, r1
${ARMSEC} arm 0x40c8 0xe0820291 # umull	r0, r2, r1, r2
${ARMSEC} arm 0x40c8 0xe0930391 # umulls	r0, r3, r1, r3
${ARMSEC} arm 0x40c8 0xe0831092 # umull	r1, r3, r2, r0
${ARMSEC} arm 0x40c8 0xe0923190 # umulls	r3, r2, r0, r1
${ARMSEC} arm 0x40c8 0xe6611f11 # uqadd16	r1, r1, r1
${ARMSEC} arm 0x40c8 0xe6621f12 # uqadd16	r1, r2, r2
${ARMSEC} arm 0x40c8 0xe6601f11 # uqadd16	r1, r0, r1
${ARMSEC} arm 0x40c8 0xe6611f12 # uqadd16	r1, r1, r2
${ARMSEC} arm 0x40c8 0xe6613f10 # uqadd16	r3, r1, r0
${ARMSEC} arm 0x40c8 0xe6622f92 # uqadd8	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6612f91 # uqadd8	r2, r1, r1
${ARMSEC} arm 0x40c8 0xe6612f92 # uqadd8	r2, r1, r2
${ARMSEC} arm 0x40c8 0xe6633f92 # uqadd8	r3, r3, r2
${ARMSEC} arm 0x40c8 0xe6623f90 # uqadd8	r3, r2, r0
${ARMSEC} arm 0x40c8 0xe6600f30 # uqasx	r0, r0, r0
${ARMSEC} arm 0x40c8 0xe6601f30 # uqasx	r1, r0, r0
${ARMSEC} arm 0x40c8 0xe6612f32 # uqasx	r2, r1, r2
${ARMSEC} arm 0x40c8 0xe6633f30 # uqasx	r3, r3, r0
${ARMSEC} arm 0x40c8 0xe6612f30 # uqasx	r2, r1, r0
${ARMSEC} arm 0x40c8 0xe6633f53 # uqsax	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6613f51 # uqsax	r3, r1, r1
${ARMSEC} arm 0x40c8 0xe6631f51 # uqsax	r1, r3, r1
${ARMSEC} arm 0x40c8 0xe6600f51 # uqsax	r0, r0, r1
${ARMSEC} arm 0x40c8 0xe6601f53 # uqsax	r1, r0, r3
${ARMSEC} arm 0x40c8 0xe6633f73 # uqsub16	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6632f73 # uqsub16	r2, r3, r3
${ARMSEC} arm 0x40c8 0xe6632f72 # uqsub16	r2, r3, r2
${ARMSEC} arm 0x40c8 0xe6611f73 # uqsub16	r1, r1, r3
${ARMSEC} arm 0x40c8 0xe6623f70 # uqsub16	r3, r2, r0
${ARMSEC} arm 0x40c8 0xe6622ff2 # uqsub8	r2, r2, r2
${ARMSEC} arm 0x40c8 0xe6613ff1 # uqsub8	r3, r1, r1
${ARMSEC} arm 0x40c8 0xe6623ff3 # uqsub8	r3, r2, r3
${ARMSEC} arm 0x40c8 0xe6633ff2 # uqsub8	r3, r3, r2
${ARMSEC} arm 0x40c8 0xe6602ff3 # uqsub8	r2, r0, r3
${ARMSEC} arm 0x40c8 0xe783f313 # usad8	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe783f010 # usad8	r3, r0, r0
${ARMSEC} arm 0x40c8 0xe781f113 # usad8	r1, r3, r1
${ARMSEC} arm 0x40c8 0xe781f311 # usad8	r1, r1, r3
${ARMSEC} arm 0x40c8 0xe783f011 # usad8	r3, r1, r0
${ARMSEC} arm 0x40c8 0xe7800010 # usada8	r0, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe7820010 # usada8	r2, r0, r0, r0
${ARMSEC} arm 0x40c8 0xe7811212 # usada8	r1, r2, r2, r1
${ARMSEC} arm 0x40c8 0xe7810110 # usada8	r1, r0, r1, r0
${ARMSEC} arm 0x40c8 0xe7822213 # usada8	r2, r3, r2, r2
${ARMSEC} arm 0x40c8 0xe7832313 # usada8	r3, r3, r3, r2
${ARMSEC} arm 0x40c8 0xe7811311 # usada8	r1, r1, r3, r1
${ARMSEC} arm 0x40c8 0xe7823312 # usada8	r2, r2, r3, r3
${ARMSEC} arm 0x40c8 0xe7820111 # usada8	r2, r1, r1, r0
${ARMSEC} arm 0x40c8 0xe7802112 # usada8	r0, r2, r1, r2
${ARMSEC} arm 0x40c8 0xe7831112 # usada8	r3, r2, r1, r1
${ARMSEC} arm 0x40c8 0xe7822310 # usada8	r2, r0, r3, r2
${ARMSEC} arm 0x40c8 0xe7830311 # usada8	r3, r1, r3, r0
${ARMSEC} arm 0x40c8 0xe7820112 # usada8	r2, r2, r1, r0
${ARMSEC} arm 0x40c8 0xe7820113 # usada8	r2, r3, r1, r0
${ARMSEC} arm 0x40c8 0xe6e03453 # usat	r3, #0, r3, asr #8
${ARMSEC} arm 0x40c8 0xe6e00291 # usat	r0, #0, r1, lsl #5
${ARMSEC} arm 0x40c8 0xe6e40052 # usat	r0, #4, r2, asr #32
${ARMSEC} arm 0x40c8 0xe6e52f32 # usat16	r2, #5, r2
${ARMSEC} arm 0x40c8 0xe6e20f33 # usat16	r0, #2, r3
${ARMSEC} arm 0x40c8 0xe6533f53 # usax	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6501f50 # usax	r1, r0, r0
${ARMSEC} arm 0x40c8 0xe6503f53 # usax	r3, r0, r3
${ARMSEC} arm 0x40c8 0xe6511f53 # usax	r1, r1, r3
${ARMSEC} arm 0x40c8 0xe6501f52 # usax	r1, r0, r2
${ARMSEC} arm 0x40c8 0xe6511f71 # usub16	r1, r1, r1
${ARMSEC} arm 0x40c8 0xe6510f71 # usub16	r0, r1, r1
${ARMSEC} arm 0x40c8 0xe6523f73 # usub16	r3, r2, r3
${ARMSEC} arm 0x40c8 0xe6511f72 # usub16	r1, r1, r2
${ARMSEC} arm 0x40c8 0xe6520f73 # usub16	r0, r2, r3
${ARMSEC} arm 0x40c8 0xe6533ff3 # usub8	r3, r3, r3
${ARMSEC} arm 0x40c8 0xe6532ff3 # usub8	r2, r3, r3
${ARMSEC} arm 0x40c8 0xe6532ff2 # usub8	r2, r3, r2
${ARMSEC} arm 0x40c8 0xe6511ff3 # usub8	r1, r1, r3
${ARMSEC} arm 0x40c8 0xe6523ff0 # usub8	r3, r2, r0
${ARMSEC} arm 0x40c8 0xe6e11c71 # uxtab	r1, r1, r1, ror #24
${ARMSEC} arm 0x40c8 0xe6e13c71 # uxtab	r3, r1, r1, ror #24
${ARMSEC} arm 0x40c8 0xe6e32872 # uxtab	r2, r3, r2, ror #16
${ARMSEC} arm 0x40c8 0xe6e00072 # uxtab	r0, r0, r2
${ARMSEC} arm 0x40c8 0xe6e13470 # uxtab	r3, r1, r0, ror #8
${ARMSEC} arm 0x40c8 0xe6c00c70 # uxtab16	r0, r0, r0, ror #24
${ARMSEC} arm 0x40c8 0xe6c01470 # uxtab16	r1, r0, r0, ror #8
${ARMSEC} arm 0x40c8 0xe6c03c73 # uxtab16	r3, r0, r3, ror #24
${ARMSEC} arm 0x40c8 0xe6c11872 # uxtab16	r1, r1, r2, ror #16
${ARMSEC} arm 0x40c8 0xe6c10c73 # uxtab16	r0, r1, r3, ror #24
${ARMSEC} arm 0x40c8 0xe6f11471 # uxtah	r1, r1, r1, ror #8
${ARMSEC} arm 0x40c8 0xe6f13071 # uxtah	r3, r1, r1
${ARMSEC} arm 0x40c8 0xe6f23073 # uxtah	r3, r2, r3
${ARMSEC} arm 0x40c8 0xe6f22c71 # uxtah	r2, r2, r1, ror #24
${ARMSEC} arm 0x40c8 0xe6f20071 # uxtah	r0, r2, r1
${ARMSEC} arm 0x40c8 0xe6ef0c70 # uxtb	r0, r0, ror #24
${ARMSEC} arm 0x40c8 0xe6ef3c70 # uxtb	r3, r0, ror #24
${ARMSEC} arm 0x40c8 0xe6ef307f # uxtb	r3, pc
${ARMSEC} arm 0x40c8 0xe6cf1871 # uxtb16	r1, r1, ror #16
${ARMSEC} arm 0x40c8 0xe6cf2471 # uxtb16	r2, r1, ror #8
${ARMSEC} arm 0x40c8 0xe6ff0c70 # uxth	r0, r0, ror #24
${ARMSEC} arm 0x40c8 0xe6ff3c70 # uxth	r3, r0, ror #24
