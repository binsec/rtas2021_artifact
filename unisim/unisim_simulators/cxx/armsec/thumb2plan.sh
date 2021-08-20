set -ex
${ARMSEC} thumb 0x40c8 0x4149 # adcs	r1, r1
${ARMSEC} thumb 0x40ca 0x4149 # adcs	r1, r1
${ARMSEC} thumb 0x40c8 0x4143 # adcs	r3, r0
${ARMSEC} thumb 0x40ca 0x4143 # adcs	r3, r0
${ARMSEC} thumb 0x40c8 0x1141eb41 # adc.w	r1, r1, r1, lsl #5
${ARMSEC} thumb 0x40ca 0x1141eb41 # adc.w	r1, r1, r1, lsl #5
${ARMSEC} thumb 0x40c8 0x30b0eb50 # adcs.w	r0, r0, r0, ror #14
${ARMSEC} thumb 0x40ca 0x30b0eb50 # adcs.w	r0, r0, r0, ror #14
${ARMSEC} thumb 0x40c8 0x50e3eb43 # adc.w	r0, r3, r3, asr #23
${ARMSEC} thumb 0x40ca 0x50e3eb43 # adc.w	r0, r3, r3, asr #23
${ARMSEC} thumb 0x40c8 0x4041eb51 # adcs.w	r0, r1, r1, lsl #17
${ARMSEC} thumb 0x40ca 0x4041eb51 # adcs.w	r0, r1, r1, lsl #17
${ARMSEC} thumb 0x40c8 0x52d3eb42 # adc.w	r2, r2, r3, lsr #23
${ARMSEC} thumb 0x40ca 0x52d3eb42 # adc.w	r2, r2, r3, lsr #23
${ARMSEC} thumb 0x40c8 0x3290eb52 # adcs.w	r2, r2, r0, lsr #14
${ARMSEC} thumb 0x40ca 0x3290eb52 # adcs.w	r2, r2, r0, lsr #14
${ARMSEC} thumb 0x40c8 0x3353eb40 # adc.w	r3, r0, r3, lsr #13
${ARMSEC} thumb 0x40ca 0x3353eb40 # adc.w	r3, r0, r3, lsr #13
${ARMSEC} thumb 0x40c8 0x1f1eb50 # adcs.w	r1, r0, r1, ror #3
${ARMSEC} thumb 0x40ca 0x1f1eb50 # adcs.w	r1, r0, r1, ror #3
${ARMSEC} thumb 0x40c8 0x1301eb40 # adc.w	r3, r0, r1, lsl #4
${ARMSEC} thumb 0x40ca 0x1301eb40 # adc.w	r3, r0, r1, lsl #4
${ARMSEC} thumb 0x40c8 0x4380eb52 # adcs.w	r3, r2, r0, lsl #18
${ARMSEC} thumb 0x40ca 0x4380eb52 # adcs.w	r3, r2, r0, lsl #18
${ARMSEC} thumb 0x40c8 0x5191f541 # adc.w	r1, r1, #4640
${ARMSEC} thumb 0x40ca 0x5191f541 # adc.w	r1, r1, #4640
${ARMSEC} thumb 0x40c8 0x5061f150 # adcs.w	r0, r0, #943718400
${ARMSEC} thumb 0x40ca 0x5061f150 # adcs.w	r0, r0, #943718400
${ARMSEC} thumb 0x40c8 0x7ff541 # adc.w	r0, r1, #16711680
${ARMSEC} thumb 0x40ca 0x7ff541 # adc.w	r0, r1, #16711680
${ARMSEC} thumb 0x40c8 0x1126f552 # adcs.w	r1, r2, #2719744
${ARMSEC} thumb 0x40ca 0x1126f552 # adcs.w	r1, r2, #2719744
${ARMSEC} thumb 0x40c8 0x1d92 # adds	r2, r2, #6
${ARMSEC} thumb 0x40ca 0x1d92 # adds	r2, r2, #6
${ARMSEC} thumb 0x40c8 0x1c10 # adds	r0, r2, #0
${ARMSEC} thumb 0x40ca 0x1c10 # adds	r0, r2, #0
${ARMSEC} thumb 0x40c8 0x33b6 # adds	r3, #182
${ARMSEC} thumb 0x40ca 0x33b6 # adds	r3, #182
${ARMSEC} thumb 0x40c8 0x1892 # adds	r2, r2, r2
${ARMSEC} thumb 0x40ca 0x1892 # adds	r2, r2, r2
${ARMSEC} thumb 0x40c8 0x18d9 # adds	r1, r3, r3
${ARMSEC} thumb 0x40ca 0x18d9 # adds	r1, r3, r3
${ARMSEC} thumb 0x40c8 0x1810 # adds	r0, r2, r0
${ARMSEC} thumb 0x40ca 0x1810 # adds	r0, r2, r0
${ARMSEC} thumb 0x40c8 0x1852 # adds	r2, r2, r1
${ARMSEC} thumb 0x40ca 0x1852 # adds	r2, r2, r1
${ARMSEC} thumb 0x40c8 0x18d0 # adds	r0, r2, r3
${ARMSEC} thumb 0x40ca 0x18d0 # adds	r0, r2, r3
${ARMSEC} thumb 0x40c8 0x4400 # add	r0, r0
${ARMSEC} thumb 0x40ca 0x4400 # add	r0, r0
${ARMSEC} thumb 0x40c8 0x4413 # add	r3, r2
${ARMSEC} thumb 0x40ca 0x4413 # add	r3, r2
${ARMSEC} thumb 0x40c8 0x4479 # add	r1, pc
${ARMSEC} thumb 0x40ca 0x4479 # add	r1, pc
${ARMSEC} thumb 0x40c8 0x4181eb01 # add.w	r1, r1, r1, lsl #18
${ARMSEC} thumb 0x40ca 0x4181eb01 # add.w	r1, r1, r1, lsl #18
${ARMSEC} thumb 0x40c8 0x22a2eb12 # adds.w	r2, r2, r2, asr #10
${ARMSEC} thumb 0x40ca 0x22a2eb12 # adds.w	r2, r2, r2, asr #10
${ARMSEC} thumb 0x40c8 0x7130eb00 # add.w	r1, r0, r0, ror #28
${ARMSEC} thumb 0x40ca 0x7130eb00 # add.w	r1, r0, r0, ror #28
${ARMSEC} thumb 0x40c8 0x102eb12 # adds.w	r1, r2, r2
${ARMSEC} thumb 0x40ca 0x102eb12 # adds.w	r1, r2, r2
${ARMSEC} thumb 0x40c8 0x5060eb03 # add.w	r0, r3, r0, asr #21
${ARMSEC} thumb 0x40ca 0x5060eb03 # add.w	r0, r3, r0, asr #21
${ARMSEC} thumb 0x40c8 0x202eb11 # adds.w	r2, r1, r2
${ARMSEC} thumb 0x40ca 0x202eb11 # adds.w	r2, r1, r2
${ARMSEC} thumb 0x40c8 0xe2eb00 # add.w	r0, r0, r2, asr #3
${ARMSEC} thumb 0x40ca 0xe2eb00 # add.w	r0, r0, r2, asr #3
${ARMSEC} thumb 0x40c8 0x70a3eb10 # adds.w	r0, r0, r3, asr #30
${ARMSEC} thumb 0x40ca 0x70a3eb10 # adds.w	r0, r0, r3, asr #30
${ARMSEC} thumb 0x40c8 0x2003eb01 # add.w	r0, r1, r3, lsl #8
${ARMSEC} thumb 0x40ca 0x2003eb01 # add.w	r0, r1, r3, lsl #8
${ARMSEC} thumb 0x40c8 0x4062eb13 # adds.w	r0, r3, r2, asr #17
${ARMSEC} thumb 0x40ca 0x4062eb13 # adds.w	r0, r3, r2, asr #17
${ARMSEC} thumb 0x40c8 0x411af101 # add.w	r1, r1, #-1711276032
${ARMSEC} thumb 0x40ca 0x411af101 # add.w	r1, r1, #-1711276032
${ARMSEC} thumb 0x40c8 0x51f0f111 # adds.w	r1, r1, #503316480
${ARMSEC} thumb 0x40ca 0x51f0f111 # adds.w	r1, r1, #503316480
${ARMSEC} thumb 0x40c8 0x78f102 # add.w	r0, r2, #120
${ARMSEC} thumb 0x40ca 0x78f102 # add.w	r0, r2, #120
${ARMSEC} thumb 0x40c8 0x5229f111 # adds.w	r2, r1, #708837376
${ARMSEC} thumb 0x40ca 0x5229f111 # adds.w	r2, r1, #708837376
${ARMSEC} thumb 0x40c8 0x3155f201 # addw	r1, r1, #853
${ARMSEC} thumb 0x40ca 0x3155f201 # addw	r1, r1, #853
${ARMSEC} thumb 0x40c8 0x1f601 # addw	r0, r1, #2049
${ARMSEC} thumb 0x40ca 0x1f601 # addw	r0, r1, #2049
${ARMSEC} thumb 0x40c8 0xa38e # add	r3, pc, #568
${ARMSEC} thumb 0x40ca 0xa38e # add	r3, pc, #568
${ARMSEC} thumb 0x40c8 0x31eff60f # addw	r1, pc, #3055
${ARMSEC} thumb 0x40ca 0x31eff60f # addw	r1, pc, #3055
${ARMSEC} thumb 0x40c8 0x3320f2af # subw	r3, pc, #800
${ARMSEC} thumb 0x40ca 0x3320f2af # subw	r3, pc, #800
${ARMSEC} thumb 0x40c8 0x4012 # ands	r2, r2
${ARMSEC} thumb 0x40ca 0x4012 # ands	r2, r2
${ARMSEC} thumb 0x40c8 0x401a # ands	r2, r3
${ARMSEC} thumb 0x40ca 0x401a # ands	r2, r3
${ARMSEC} thumb 0x40c8 0x5363ea03 # and.w	r3, r3, r3, asr #21
${ARMSEC} thumb 0x40ca 0x5363ea03 # and.w	r3, r3, r3, asr #21
${ARMSEC} thumb 0x40c8 0x73d3ea13 # ands.w	r3, r3, r3, lsr #31
${ARMSEC} thumb 0x40ca 0x73d3ea13 # ands.w	r3, r3, r3, lsr #31
${ARMSEC} thumb 0x40c8 0x2390ea00 # and.w	r3, r0, r0, lsr #10
${ARMSEC} thumb 0x40ca 0x2390ea00 # and.w	r3, r0, r0, lsr #10
${ARMSEC} thumb 0x40c8 0x7013ea13 # ands.w	r0, r3, r3, lsr #28
${ARMSEC} thumb 0x40ca 0x7013ea13 # ands.w	r0, r3, r3, lsr #28
${ARMSEC} thumb 0x40c8 0x6011ea00 # and.w	r0, r0, r1, lsr #24
${ARMSEC} thumb 0x40ca 0x6011ea00 # and.w	r0, r0, r1, lsr #24
${ARMSEC} thumb 0x40c8 0x72d1ea12 # ands.w	r2, r2, r1, lsr #31
${ARMSEC} thumb 0x40ca 0x72d1ea12 # ands.w	r2, r2, r1, lsr #31
${ARMSEC} thumb 0x40c8 0x62b2ea00 # and.w	r2, r0, r2, ror #26
${ARMSEC} thumb 0x40ca 0x62b2ea00 # and.w	r2, r0, r2, ror #26
${ARMSEC} thumb 0x40c8 0x2373ea10 # ands.w	r3, r0, r3, ror #9
${ARMSEC} thumb 0x40ca 0x2373ea10 # ands.w	r3, r0, r3, ror #9
${ARMSEC} thumb 0x40c8 0x3b0ea02 # and.w	r3, r2, r0, ror #2
${ARMSEC} thumb 0x40ca 0x3b0ea02 # and.w	r3, r2, r0, ror #2
${ARMSEC} thumb 0x40c8 0x143ea10 # ands.w	r1, r0, r3, lsl #1
${ARMSEC} thumb 0x40ca 0x143ea10 # ands.w	r1, r0, r3, lsl #1
${ARMSEC} thumb 0x40c8 0x128ff002 # and.w	r2, r2, #9371791
${ARMSEC} thumb 0x40ca 0x128ff002 # and.w	r2, r2, #9371791
${ARMSEC} thumb 0x40c8 0x712af011 # ands.w	r1, r1, #44564480
${ARMSEC} thumb 0x40ca 0x712af011 # ands.w	r1, r1, #44564480
${ARMSEC} thumb 0x40c8 0x1287f003 # and.w	r2, r3, #8847495
${ARMSEC} thumb 0x40ca 0x1287f003 # and.w	r2, r3, #8847495
${ARMSEC} thumb 0x40c8 0x30cf012 # ands.w	r3, r2, #12
${ARMSEC} thumb 0x40ca 0x30cf012 # ands.w	r3, r2, #12
${ARMSEC} thumb 0x40c8 0x411b # asrs	r3, r3
${ARMSEC} thumb 0x40ca 0x411b # asrs	r3, r3
${ARMSEC} thumb 0x40c8 0x4108 # asrs	r0, r1
${ARMSEC} thumb 0x40ca 0x4108 # asrs	r0, r1
${ARMSEC} thumb 0x40c8 0x5014f36f # bfc	r0, #20, #1
${ARMSEC} thumb 0x40ca 0x5014f36f # bfc	r0, #20, #1
${ARMSEC} thumb 0x40c8 0xdff360 # bfi	r0, r0, #3, #29
${ARMSEC} thumb 0x40ca 0xdff360 # bfi	r0, r0, #3, #29
${ARMSEC} thumb 0x40c8 0x3213f363 # bfi	r2, r3, #12, #8
${ARMSEC} thumb 0x40ca 0x3213f363 # bfi	r2, r3, #12, #8
${ARMSEC} thumb 0x40c8 0x4389 # bics	r1, r1
${ARMSEC} thumb 0x40ca 0x4389 # bics	r1, r1
${ARMSEC} thumb 0x40c8 0x4383 # bics	r3, r0
${ARMSEC} thumb 0x40ca 0x4383 # bics	r3, r0
${ARMSEC} thumb 0x40c8 0x7202ea22 # bic.w	r2, r2, r2, lsl #28
${ARMSEC} thumb 0x40ca 0x7202ea22 # bic.w	r2, r2, r2, lsl #28
${ARMSEC} thumb 0x40c8 0x6373ea33 # bics.w	r3, r3, r3, ror #25
${ARMSEC} thumb 0x40ca 0x6373ea33 # bics.w	r3, r3, r3, ror #25
${ARMSEC} thumb 0x40c8 0x1381ea21 # bic.w	r3, r1, r1, lsl #6
${ARMSEC} thumb 0x40ca 0x1381ea21 # bic.w	r3, r1, r1, lsl #6
${ARMSEC} thumb 0x40c8 0x71c0ea30 # bics.w	r1, r0, r0, lsl #31
${ARMSEC} thumb 0x40ca 0x71c0ea30 # bics.w	r1, r0, r0, lsl #31
${ARMSEC} thumb 0x40c8 0x6153ea21 # bic.w	r1, r1, r3, lsr #25
${ARMSEC} thumb 0x40ca 0x6153ea21 # bic.w	r1, r1, r3, lsr #25
${ARMSEC} thumb 0x40c8 0x1391ea33 # bics.w	r3, r3, r1, lsr #6
${ARMSEC} thumb 0x40ca 0x1391ea33 # bics.w	r3, r3, r1, lsr #6
${ARMSEC} thumb 0x40c8 0x5232ea21 # bic.w	r2, r1, r2, ror #20
${ARMSEC} thumb 0x40ca 0x5232ea21 # bic.w	r2, r1, r2, ror #20
${ARMSEC} thumb 0x40c8 0x4232ea30 # bics.w	r2, r0, r2, ror #16
${ARMSEC} thumb 0x40ca 0x4232ea30 # bics.w	r2, r0, r2, ror #16
${ARMSEC} thumb 0x40c8 0x3033ea21 # bic.w	r0, r1, r3, ror #12
${ARMSEC} thumb 0x40ca 0x3033ea21 # bic.w	r0, r1, r3, ror #12
${ARMSEC} thumb 0x40c8 0x6052ea33 # bics.w	r0, r3, r2, lsr #25
${ARMSEC} thumb 0x40ca 0x6052ea33 # bics.w	r0, r3, r2, lsr #25
${ARMSEC} thumb 0x40c8 0x7241f422 # bic.w	r2, r2, #772
${ARMSEC} thumb 0x40ca 0x7241f422 # bic.w	r2, r2, #772
${ARMSEC} thumb 0x40c8 0x52adf432 # bics.w	r2, r2, #5536
${ARMSEC} thumb 0x40ca 0x52adf432 # bics.w	r2, r2, #5536
${ARMSEC} thumb 0x40c8 0x390f021 # bic.w	r3, r1, #144
${ARMSEC} thumb 0x40ca 0x390f021 # bic.w	r3, r1, #144
${ARMSEC} thumb 0x40c8 0x175f030 # bics.w	r1, r0, #117
${ARMSEC} thumb 0x40ca 0x175f030 # bics.w	r1, r0, #117
${ARMSEC} thumb 0x40c8 0xf181fab1 # clz	r1, r1
${ARMSEC} thumb 0x40ca 0xf181fab1 # clz	r1, r1
${ARMSEC} thumb 0x40c8 0xf381fab1 # clz	r3, r1
${ARMSEC} thumb 0x40ca 0xf381fab1 # clz	r3, r1
${ARMSEC} thumb 0x40c8 0x42d2 # cmn	r2, r2
${ARMSEC} thumb 0x40ca 0x42d2 # cmn	r2, r2
${ARMSEC} thumb 0x40c8 0x42c8 # cmn	r0, r1
${ARMSEC} thumb 0x40ca 0x42c8 # cmn	r0, r1
${ARMSEC} thumb 0x40c8 0x6f61eb11 # cmn.w	r1, r1, asr #25
${ARMSEC} thumb 0x40ca 0x6f61eb11 # cmn.w	r1, r1, asr #25
${ARMSEC} thumb 0x40c8 0x7f02eb10 # cmn.w	r0, r2, lsl #28
${ARMSEC} thumb 0x40ca 0x7f02eb10 # cmn.w	r0, r2, lsl #28
${ARMSEC} thumb 0x40c8 0x4fd7f112 # cmn.w	r2, #1803550720
${ARMSEC} thumb 0x40ca 0x4fd7f112 # cmn.w	r2, #1803550720
${ARMSEC} thumb 0x40c8 0x2bce # cmp	r3, #206
${ARMSEC} thumb 0x40ca 0x2bce # cmp	r3, #206
${ARMSEC} thumb 0x40c8 0x4280 # cmp	r0, r0
${ARMSEC} thumb 0x40ca 0x4280 # cmp	r0, r0
${ARMSEC} thumb 0x40c8 0x4299 # cmp	r1, r3
${ARMSEC} thumb 0x40ca 0x4299 # cmp	r1, r3
${ARMSEC} thumb 0x40c8 0x6fe1ebb1 # cmp.w	r1, r1, asr #27
${ARMSEC} thumb 0x40ca 0x6fe1ebb1 # cmp.w	r1, r1, asr #27
${ARMSEC} thumb 0x40c8 0x1f90ebb1 # cmp.w	r1, r0, lsr #6
${ARMSEC} thumb 0x40ca 0x1f90ebb1 # cmp.w	r1, r0, lsr #6
${ARMSEC} thumb 0x40c8 0x6f09f5b3 # cmp.w	r3, #2192
${ARMSEC} thumb 0x40ca 0x6f09f5b3 # cmp.w	r3, #2192
${ARMSEC} thumb 0x40c8 0x4040 # eors	r0, r0
${ARMSEC} thumb 0x40ca 0x4040 # eors	r0, r0
${ARMSEC} thumb 0x40c8 0x4059 # eors	r1, r3
${ARMSEC} thumb 0x40ca 0x4059 # eors	r1, r3
${ARMSEC} thumb 0x40c8 0x5000ea80 # eor.w	r0, r0, r0, lsl #20
${ARMSEC} thumb 0x40ca 0x5000ea80 # eor.w	r0, r0, r0, lsl #20
${ARMSEC} thumb 0x40c8 0x22a2ea92 # eors.w	r2, r2, r2, asr #10
${ARMSEC} thumb 0x40ca 0x22a2ea92 # eors.w	r2, r2, r2, asr #10
${ARMSEC} thumb 0x40c8 0x11e3ea83 # eor.w	r1, r3, r3, asr #7
${ARMSEC} thumb 0x40ca 0x11e3ea83 # eor.w	r1, r3, r3, asr #7
${ARMSEC} thumb 0x40c8 0x60f2ea92 # eors.w	r0, r2, r2, ror #27
${ARMSEC} thumb 0x40ca 0x60f2ea92 # eors.w	r0, r2, r2, ror #27
${ARMSEC} thumb 0x40c8 0x3f2ea83 # eor.w	r3, r3, r2, ror #3
${ARMSEC} thumb 0x40ca 0x3f2ea83 # eor.w	r3, r3, r2, ror #3
${ARMSEC} thumb 0x40c8 0x5182ea91 # eors.w	r1, r1, r2, lsl #22
${ARMSEC} thumb 0x40ca 0x5182ea91 # eors.w	r1, r1, r2, lsl #22
${ARMSEC} thumb 0x40c8 0x5020ea82 # eor.w	r0, r2, r0, asr #20
${ARMSEC} thumb 0x40ca 0x5020ea82 # eor.w	r0, r2, r0, asr #20
${ARMSEC} thumb 0x40c8 0x7101ea90 # eors.w	r1, r0, r1, lsl #28
${ARMSEC} thumb 0x40ca 0x7101ea90 # eors.w	r1, r0, r1, lsl #28
${ARMSEC} thumb 0x40c8 0x20e3ea81 # eor.w	r0, r1, r3, asr #11
${ARMSEC} thumb 0x40ca 0x20e3ea81 # eor.w	r0, r1, r3, asr #11
${ARMSEC} thumb 0x40c8 0x2250ea91 # eors.w	r2, r1, r0, lsr #9
${ARMSEC} thumb 0x40ca 0x2250ea91 # eors.w	r2, r1, r0, lsr #9
${ARMSEC} thumb 0x40c8 0x32d0f082 # eor.w	r2, r2, #-791621424
${ARMSEC} thumb 0x40ca 0x32d0f082 # eor.w	r2, r2, #-791621424
${ARMSEC} thumb 0x40c8 0x6242f092 # eors.w	r2, r2, #203423744
${ARMSEC} thumb 0x40ca 0x6242f092 # eors.w	r2, r2, #203423744
${ARMSEC} thumb 0x40c8 0x30f5f083 # eor.w	r0, r3, #-168430091
${ARMSEC} thumb 0x40ca 0x30f5f083 # eor.w	r0, r3, #-168430091
${ARMSEC} thumb 0x40c8 0x216ef492 # eors.w	r1, r2, #974848
${ARMSEC} thumb 0x40ca 0x216ef492 # eors.w	r1, r2, #974848
${ARMSEC} thumb 0x40c8 0x5e912 # ldmdb	r2, {r0, r2}
${ARMSEC} thumb 0x40ca 0x5e912 # ldmdb	r2, {r0, r2}
${ARMSEC} thumb 0x40c8 0x9e910 # ldmdb	r0, {r0, r3}
${ARMSEC} thumb 0x40ca 0x9e910 # ldmdb	r0, {r0, r3}
${ARMSEC} thumb 0x40c8 0x9e912 # ldmdb	r2, {r0, r3}
${ARMSEC} thumb 0x40ca 0x9e912 # ldmdb	r2, {r0, r3}
${ARMSEC} thumb 0x40c8 0xde913 # ldmdb	r3, {r0, r2, r3}
${ARMSEC} thumb 0x40ca 0xde913 # ldmdb	r3, {r0, r2, r3}
${ARMSEC} thumb 0x40c8 0xde912 # ldmdb	r2, {r0, r2, r3}
${ARMSEC} thumb 0x40ca 0xde912 # ldmdb	r2, {r0, r2, r3}
${ARMSEC} thumb 0x40c8 0xae930 # ldmdb	r0!, {r1, r3}
${ARMSEC} thumb 0x40ca 0xae930 # ldmdb	r0!, {r1, r3}
${ARMSEC} thumb 0x40c8 0xee910 # ldmdb	r0, {r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xee910 # ldmdb	r0, {r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe913 # ldmdb	r3, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe913 # ldmdb	r3, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe911 # ldmdb	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe911 # ldmdb	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe910 # ldmdb	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe910 # ldmdb	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xc902 # ldmia	r1, {r1}
${ARMSEC} thumb 0x40ca 0xc902 # ldmia	r1, {r1}
${ARMSEC} thumb 0x40c8 0xca0c # ldmia	r2, {r2, r3}
${ARMSEC} thumb 0x40ca 0xca0c # ldmia	r2, {r2, r3}
${ARMSEC} thumb 0x40c8 0xc802 # ldmia	r0!, {r1}
${ARMSEC} thumb 0x40ca 0xc802 # ldmia	r0!, {r1}
${ARMSEC} thumb 0x40c8 0xc90e # ldmia	r1, {r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xc90e # ldmia	r1, {r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xca0e # ldmia	r2, {r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xca0e # ldmia	r2, {r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xcb03 # ldmia	r3!, {r0, r1}
${ARMSEC} thumb 0x40ca 0xcb03 # ldmia	r3!, {r0, r1}
${ARMSEC} thumb 0x40c8 0xc80f # ldmia	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xc80f # ldmia	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xc90f # ldmia	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xc90f # ldmia	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xca0f # ldmia	r2, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xca0f # ldmia	r2, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xca0b # ldmia	r2!, {r0, r1, r3}
${ARMSEC} thumb 0x40ca 0xca0b # ldmia	r2!, {r0, r1, r3}
${ARMSEC} thumb 0x40c8 0x9e890 # ldmia.w	r0, {r0, r3}
${ARMSEC} thumb 0x40ca 0x9e890 # ldmia.w	r0, {r0, r3}
${ARMSEC} thumb 0x40c8 0xae893 # ldmia.w	r3, {r1, r3}
${ARMSEC} thumb 0x40ca 0xae893 # ldmia.w	r3, {r1, r3}
${ARMSEC} thumb 0x40c8 0x9e891 # ldmia.w	r1, {r0, r3}
${ARMSEC} thumb 0x40ca 0x9e891 # ldmia.w	r1, {r0, r3}
${ARMSEC} thumb 0x40c8 0xde892 # ldmia.w	r2, {r0, r2, r3}
${ARMSEC} thumb 0x40ca 0xde892 # ldmia.w	r2, {r0, r2, r3}
${ARMSEC} thumb 0x40c8 0x5e8b1 # ldmia.w	r1!, {r0, r2}
${ARMSEC} thumb 0x40ca 0x5e8b1 # ldmia.w	r1!, {r0, r2}
${ARMSEC} thumb 0x40c8 0xbe892 # ldmia.w	r2, {r0, r1, r3}
${ARMSEC} thumb 0x40ca 0xbe892 # ldmia.w	r2, {r0, r1, r3}
${ARMSEC} thumb 0x40c8 0xfe890 # ldmia.w	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe890 # ldmia.w	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe891 # ldmia.w	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe891 # ldmia.w	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe892 # ldmia.w	r2, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe892 # ldmia.w	r2, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xde8b1 # ldmia.w	r1!, {r0, r2, r3}
${ARMSEC} thumb 0x40ca 0xde8b1 # ldmia.w	r1!, {r0, r2, r3}
${ARMSEC} thumb 0x40c8 0x6800 # ldr	r0, [r0, #0]
${ARMSEC} thumb 0x40ca 0x6800 # ldr	r0, [r0, #0]
${ARMSEC} thumb 0x40c8 0x6d99 # ldr	r1, [r3, #88]
${ARMSEC} thumb 0x40ca 0x6d99 # ldr	r1, [r3, #88]
${ARMSEC} thumb 0x40c8 0x5812 # ldr	r2, [r2, r0]
${ARMSEC} thumb 0x40ca 0x5812 # ldr	r2, [r2, r0]
${ARMSEC} thumb 0x40c8 0x5818 # ldr	r0, [r3, r0]
${ARMSEC} thumb 0x40ca 0x5818 # ldr	r0, [r3, r0]
${ARMSEC} thumb 0x40c8 0x5898 # ldr	r0, [r3, r2]
${ARMSEC} thumb 0x40ca 0x5898 # ldr	r0, [r3, r2]
${ARMSEC} thumb 0x40c8 0x2c9ef852 # ldr.w	r2, [r2, #-158]
${ARMSEC} thumb 0x40ca 0x2c9ef852 # ldr.w	r2, [r2, #-158]
${ARMSEC} thumb 0x40c8 0xce7f852 # ldr.w	r0, [r2, #-231]
${ARMSEC} thumb 0x40ca 0xce7f852 # ldr.w	r0, [r2, #-231]
${ARMSEC} thumb 0x40c8 0x3d60f850 # ldr.w	r3, [r0, #-96]!
${ARMSEC} thumb 0x40ca 0x3d60f850 # ldr.w	r3, [r0, #-96]!
${ARMSEC} thumb 0x40c8 0x28bcf8d2 # ldr.w	r2, [r2, #2236]
${ARMSEC} thumb 0x40ca 0x28bcf8d2 # ldr.w	r2, [r2, #2236]
${ARMSEC} thumb 0x40c8 0x3a2cf8d0 # ldr.w	r3, [r0, #2604]
${ARMSEC} thumb 0x40ca 0x3a2cf8d0 # ldr.w	r3, [r0, #2604]
${ARMSEC} thumb 0x40c8 0x3002f853 # ldr.w	r3, [r3, r2]
${ARMSEC} thumb 0x40ca 0x3002f853 # ldr.w	r3, [r3, r2]
${ARMSEC} thumb 0x40c8 0x1011f853 # ldr.w	r1, [r3, r1, lsl #1]
${ARMSEC} thumb 0x40ca 0x1011f853 # ldr.w	r1, [r3, r1, lsl #1]
${ARMSEC} thumb 0x40c8 0x3001f850 # ldr.w	r3, [r0, r1]
${ARMSEC} thumb 0x40ca 0x3001f850 # ldr.w	r3, [r0, r1]
${ARMSEC} thumb 0x40c8 0x79db # ldrb	r3, [r3, #7]
${ARMSEC} thumb 0x40ca 0x79db # ldrb	r3, [r3, #7]
${ARMSEC} thumb 0x40c8 0x7d9a # ldrb	r2, [r3, #22]
${ARMSEC} thumb 0x40ca 0x7d9a # ldrb	r2, [r3, #22]
${ARMSEC} thumb 0x40c8 0x5c40 # ldrb	r0, [r0, r1]
${ARMSEC} thumb 0x40ca 0x5c40 # ldrb	r0, [r0, r1]
${ARMSEC} thumb 0x40c8 0x5c08 # ldrb	r0, [r1, r0]
${ARMSEC} thumb 0x40ca 0x5c08 # ldrb	r0, [r1, r0]
${ARMSEC} thumb 0x40c8 0x5cd0 # ldrb	r0, [r2, r3]
${ARMSEC} thumb 0x40ca 0x5cd0 # ldrb	r0, [r2, r3]
${ARMSEC} thumb 0x40c8 0xc93f810 # ldrb.w	r0, [r0, #-147]
${ARMSEC} thumb 0x40ca 0xc93f810 # ldrb.w	r0, [r0, #-147]
${ARMSEC} thumb 0x40c8 0x2ca3f811 # ldrb.w	r2, [r1, #-163]
${ARMSEC} thumb 0x40ca 0x2ca3f811 # ldrb.w	r2, [r1, #-163]
${ARMSEC} thumb 0x40c8 0x3f34f810 # ldrb.w	r3, [r0, #52]!
${ARMSEC} thumb 0x40ca 0x3f34f810 # ldrb.w	r3, [r0, #52]!
${ARMSEC} thumb 0x40c8 0x157ff891 # ldrb.w	r1, [r1, #1407]
${ARMSEC} thumb 0x40ca 0x157ff891 # ldrb.w	r1, [r1, #1407]
${ARMSEC} thumb 0x40c8 0x18d5f893 # ldrb.w	r1, [r3, #2261]
${ARMSEC} thumb 0x40ca 0x18d5f893 # ldrb.w	r1, [r3, #2261]
${ARMSEC} thumb 0x40c8 0x1002f811 # ldrb.w	r1, [r1, r2]
${ARMSEC} thumb 0x40ca 0x1002f811 # ldrb.w	r1, [r1, r2]
${ARMSEC} thumb 0x40c8 0x10f812 # ldrb.w	r0, [r2, r0, lsl #1]
${ARMSEC} thumb 0x40ca 0x10f812 # ldrb.w	r0, [r2, r0, lsl #1]
${ARMSEC} thumb 0x40c8 0x2020f813 # ldrb.w	r2, [r3, r0, lsl #2]
${ARMSEC} thumb 0x40ca 0x2020f813 # ldrb.w	r2, [r3, r0, lsl #2]
${ARMSEC} thumb 0x40c8 0x2e2af812 # ldrbt	r2, [r2, #42]
${ARMSEC} thumb 0x40ca 0x2e2af812 # ldrbt	r2, [r2, #42]
${ARMSEC} thumb 0x40c8 0x2e8bf810 # ldrbt	r2, [r0, #139]
${ARMSEC} thumb 0x40ca 0x2e8bf810 # ldrbt	r2, [r0, #139]
${ARMSEC} thumb 0x40c8 0x9e9d0 # ldrd	r0, r0, [r0, #36]
${ARMSEC} thumb 0x40ca 0x9e9d0 # ldrd	r0, r0, [r0, #36]
${ARMSEC} thumb 0x40c8 0x22d5e953 # ldrd	r2, r2, [r3, #-852]
${ARMSEC} thumb 0x40ca 0x22d5e953 # ldrd	r2, r2, [r3, #-852]
${ARMSEC} thumb 0x40c8 0x3020e953 # ldrd	r3, r0, [r3, #-128]
${ARMSEC} thumb 0x40ca 0x3020e953 # ldrd	r3, r0, [r3, #-128]
${ARMSEC} thumb 0x40c8 0x3048e950 # ldrd	r3, r0, [r0, #-288]
${ARMSEC} thumb 0x40ca 0x3048e950 # ldrd	r3, r0, [r0, #-288]
${ARMSEC} thumb 0x40c8 0x319ae9d2 # ldrd	r3, r1, [r2, #616]
${ARMSEC} thumb 0x40ca 0x319ae9d2 # ldrd	r3, r1, [r2, #616]
${ARMSEC} thumb 0x40c8 0x12e972 # ldrd	r0, r0, [r2, #-72]!
${ARMSEC} thumb 0x40ca 0x12e972 # ldrd	r0, r0, [r2, #-72]!
${ARMSEC} thumb 0x40c8 0x2149e870 # ldrd	r2, r1, [r0], #-292
${ARMSEC} thumb 0x40ca 0x2149e870 # ldrd	r2, r1, [r0], #-292
${ARMSEC} thumb 0x40c8 0x8980 # ldrh	r0, [r0, #12]
${ARMSEC} thumb 0x40ca 0x8980 # ldrh	r0, [r0, #12]
${ARMSEC} thumb 0x40c8 0x8981 # ldrh	r1, [r0, #12]
${ARMSEC} thumb 0x40ca 0x8981 # ldrh	r1, [r0, #12]
${ARMSEC} thumb 0x40c8 0x5a80 # ldrh	r0, [r0, r2]
${ARMSEC} thumb 0x40ca 0x5a80 # ldrh	r0, [r0, r2]
${ARMSEC} thumb 0x40c8 0x5a82 # ldrh	r2, [r0, r2]
${ARMSEC} thumb 0x40ca 0x5a82 # ldrh	r2, [r0, r2]
${ARMSEC} thumb 0x40c8 0x5a0a # ldrh	r2, [r1, r0]
${ARMSEC} thumb 0x40ca 0x5a0a # ldrh	r2, [r1, r0]
${ARMSEC} thumb 0x40c8 0x3cfbf833 # ldrh.w	r3, [r3, #-251]
${ARMSEC} thumb 0x40ca 0x3cfbf833 # ldrh.w	r3, [r3, #-251]
${ARMSEC} thumb 0x40c8 0xcabf833 # ldrh.w	r0, [r3, #-171]
${ARMSEC} thumb 0x40ca 0xcabf833 # ldrh.w	r0, [r3, #-171]
${ARMSEC} thumb 0x40c8 0x1b1af832 # ldrh.w	r1, [r2], #26
${ARMSEC} thumb 0x40ca 0x1b1af832 # ldrh.w	r1, [r2], #26
${ARMSEC} thumb 0x40c8 0x3ebef8b3 # ldrh.w	r3, [r3, #3774]
${ARMSEC} thumb 0x40ca 0x3ebef8b3 # ldrh.w	r3, [r3, #3774]
${ARMSEC} thumb 0x40c8 0x1ecff8b0 # ldrh.w	r1, [r0, #3791]
${ARMSEC} thumb 0x40ca 0x1ecff8b0 # ldrh.w	r1, [r0, #3791]
${ARMSEC} thumb 0x40c8 0x3022f833 # ldrh.w	r3, [r3, r2, lsl #2]
${ARMSEC} thumb 0x40ca 0x3022f833 # ldrh.w	r3, [r3, r2, lsl #2]
${ARMSEC} thumb 0x40c8 0x1011f833 # ldrh.w	r1, [r3, r1, lsl #1]
${ARMSEC} thumb 0x40ca 0x1011f833 # ldrh.w	r1, [r3, r1, lsl #1]
${ARMSEC} thumb 0x40c8 0x1032f833 # ldrh.w	r1, [r3, r2, lsl #3]
${ARMSEC} thumb 0x40ca 0x1032f833 # ldrh.w	r1, [r3, r2, lsl #3]
${ARMSEC} thumb 0x40c8 0x2e97f832 # ldrht	r2, [r2, #151]
${ARMSEC} thumb 0x40ca 0x2e97f832 # ldrht	r2, [r2, #151]
${ARMSEC} thumb 0x40c8 0x3e6cf831 # ldrht	r3, [r1, #108]
${ARMSEC} thumb 0x40ca 0x3e6cf831 # ldrht	r3, [r1, #108]
${ARMSEC} thumb 0x40c8 0x5612 # ldrsb	r2, [r2, r0]
${ARMSEC} thumb 0x40ca 0x5612 # ldrsb	r2, [r2, r0]
${ARMSEC} thumb 0x40c8 0x5610 # ldrsb	r0, [r2, r0]
${ARMSEC} thumb 0x40ca 0x5610 # ldrsb	r0, [r2, r0]
${ARMSEC} thumb 0x40c8 0x56c1 # ldrsb	r1, [r0, r3]
${ARMSEC} thumb 0x40ca 0x56c1 # ldrsb	r1, [r0, r3]
${ARMSEC} thumb 0x40c8 0xc7af910 # ldrsb.w	r0, [r0, #-122]
${ARMSEC} thumb 0x40ca 0xc7af910 # ldrsb.w	r0, [r0, #-122]
${ARMSEC} thumb 0x40c8 0xc60f913 # ldrsb.w	r0, [r3, #-96]
${ARMSEC} thumb 0x40ca 0xc60f913 # ldrsb.w	r0, [r3, #-96]
${ARMSEC} thumb 0x40c8 0x1b99f913 # ldrsb.w	r1, [r3], #153
${ARMSEC} thumb 0x40ca 0x1b99f913 # ldrsb.w	r1, [r3], #153
${ARMSEC} thumb 0x40c8 0x36fff993 # ldrsb.w	r3, [r3, #1791]
${ARMSEC} thumb 0x40ca 0x36fff993 # ldrsb.w	r3, [r3, #1791]
${ARMSEC} thumb 0x40c8 0x19df993 # ldrsb.w	r0, [r3, #413]
${ARMSEC} thumb 0x40ca 0x19df993 # ldrsb.w	r0, [r3, #413]
${ARMSEC} thumb 0x40c8 0x2021f912 # ldrsb.w	r2, [r2, r1, lsl #2]
${ARMSEC} thumb 0x40ca 0x2021f912 # ldrsb.w	r2, [r2, r1, lsl #2]
${ARMSEC} thumb 0x40c8 0x1011f910 # ldrsb.w	r1, [r0, r1, lsl #1]
${ARMSEC} thumb 0x40ca 0x1011f910 # ldrsb.w	r1, [r0, r1, lsl #1]
${ARMSEC} thumb 0x40c8 0x3021f910 # ldrsb.w	r3, [r0, r1, lsl #2]
${ARMSEC} thumb 0x40ca 0x3021f910 # ldrsb.w	r3, [r0, r1, lsl #2]
${ARMSEC} thumb 0x40c8 0x3ee6f913 # ldrsbt	r3, [r3, #230]
${ARMSEC} thumb 0x40ca 0x3ee6f913 # ldrsbt	r3, [r3, #230]
${ARMSEC} thumb 0x40c8 0x2e88f913 # ldrsbt	r2, [r3, #136]
${ARMSEC} thumb 0x40ca 0x2e88f913 # ldrsbt	r2, [r3, #136]
${ARMSEC} thumb 0x40c8 0x5e5b # ldrsh	r3, [r3, r1]
${ARMSEC} thumb 0x40ca 0x5e5b # ldrsh	r3, [r3, r1]
${ARMSEC} thumb 0x40c8 0x5e41 # ldrsh	r1, [r0, r1]
${ARMSEC} thumb 0x40ca 0x5e41 # ldrsh	r1, [r0, r1]
${ARMSEC} thumb 0x40c8 0x5e13 # ldrsh	r3, [r2, r0]
${ARMSEC} thumb 0x40ca 0x5e13 # ldrsh	r3, [r2, r0]
${ARMSEC} thumb 0x40c8 0x3cd6f933 # ldrsh.w	r3, [r3, #-214]
${ARMSEC} thumb 0x40ca 0x3cd6f933 # ldrsh.w	r3, [r3, #-214]
${ARMSEC} thumb 0x40c8 0xc61f932 # ldrsh.w	r0, [r2, #-97]
${ARMSEC} thumb 0x40ca 0xc61f932 # ldrsh.w	r0, [r2, #-97]
${ARMSEC} thumb 0x40c8 0x392ef931 # ldrsh.w	r3, [r1], #-46
${ARMSEC} thumb 0x40ca 0x392ef931 # ldrsh.w	r3, [r1], #-46
${ARMSEC} thumb 0x40c8 0x35b4f9b3 # ldrsh.w	r3, [r3, #1460]
${ARMSEC} thumb 0x40ca 0x35b4f9b3 # ldrsh.w	r3, [r3, #1460]
${ARMSEC} thumb 0x40c8 0x2952f9b1 # ldrsh.w	r2, [r1, #2386]
${ARMSEC} thumb 0x40ca 0x2952f9b1 # ldrsh.w	r2, [r1, #2386]
${ARMSEC} thumb 0x40c8 0x2031f932 # ldrsh.w	r2, [r2, r1, lsl #3]
${ARMSEC} thumb 0x40ca 0x2031f932 # ldrsh.w	r2, [r2, r1, lsl #3]
${ARMSEC} thumb 0x40c8 0x3023f931 # ldrsh.w	r3, [r1, r3, lsl #2]
${ARMSEC} thumb 0x40ca 0x3023f931 # ldrsh.w	r3, [r1, r3, lsl #2]
${ARMSEC} thumb 0x40c8 0x1030f932 # ldrsh.w	r1, [r2, r0, lsl #3]
${ARMSEC} thumb 0x40ca 0x1030f932 # ldrsh.w	r1, [r2, r0, lsl #3]
${ARMSEC} thumb 0x40c8 0xef7f930 # ldrsht	r0, [r0, #247]
${ARMSEC} thumb 0x40ca 0xef7f930 # ldrsht	r0, [r0, #247]
${ARMSEC} thumb 0x40c8 0x3eaef931 # ldrsht	r3, [r1, #174]
${ARMSEC} thumb 0x40ca 0x3eaef931 # ldrsht	r3, [r1, #174]
${ARMSEC} thumb 0x40c8 0x1ec9f851 # ldrt	r1, [r1, #201]
${ARMSEC} thumb 0x40ca 0x1ec9f851 # ldrt	r1, [r1, #201]
${ARMSEC} thumb 0x40c8 0x1e52f853 # ldrt	r1, [r3, #82]
${ARMSEC} thumb 0x40ca 0x1e52f853 # ldrt	r1, [r3, #82]
${ARMSEC} thumb 0x40c8 0x4080 # lsls	r0, r0
${ARMSEC} thumb 0x40ca 0x4080 # lsls	r0, r0
${ARMSEC} thumb 0x40c8 0x408a # lsls	r2, r1
${ARMSEC} thumb 0x40ca 0x408a # lsls	r2, r1
${ARMSEC} thumb 0x40c8 0x40db # lsrs	r3, r3
${ARMSEC} thumb 0x40ca 0x40db # lsrs	r3, r3
${ARMSEC} thumb 0x40c8 0x40c1 # lsrs	r1, r0
${ARMSEC} thumb 0x40ca 0x40c1 # lsrs	r1, r0
${ARMSEC} thumb 0x40c8 0x3303fb03 # mla	r3, r3, r3, r3
${ARMSEC} thumb 0x40ca 0x3303fb03 # mla	r3, r3, r3, r3
${ARMSEC} thumb 0x40c8 0x1301fb01 # mla	r3, r1, r1, r1
${ARMSEC} thumb 0x40ca 0x1301fb01 # mla	r3, r1, r1, r1
${ARMSEC} thumb 0x40c8 0x1fb01 # mla	r0, r1, r1, r0
${ARMSEC} thumb 0x40ca 0x1fb01 # mla	r0, r1, r1, r0
${ARMSEC} thumb 0x40c8 0x1201fb02 # mla	r2, r2, r1, r1
${ARMSEC} thumb 0x40ca 0x1201fb02 # mla	r2, r2, r1, r1
${ARMSEC} thumb 0x40c8 0x1fb00 # mla	r0, r0, r1, r0
${ARMSEC} thumb 0x40ca 0x1fb00 # mla	r0, r0, r1, r0
${ARMSEC} thumb 0x40c8 0x3202fb02 # mla	r2, r2, r2, r3
${ARMSEC} thumb 0x40ca 0x3202fb02 # mla	r2, r2, r2, r3
${ARMSEC} thumb 0x40c8 0x3303fb00 # mla	r3, r0, r3, r3
${ARMSEC} thumb 0x40ca 0x3303fb00 # mla	r3, r0, r3, r3
${ARMSEC} thumb 0x40c8 0x1303fb01 # mla	r3, r1, r3, r1
${ARMSEC} thumb 0x40ca 0x1303fb01 # mla	r3, r1, r3, r1
${ARMSEC} thumb 0x40c8 0x2003fb03 # mla	r0, r3, r3, r2
${ARMSEC} thumb 0x40ca 0x2003fb03 # mla	r0, r3, r3, r2
${ARMSEC} thumb 0x40c8 0x100fb02 # mla	r1, r2, r0, r0
${ARMSEC} thumb 0x40ca 0x100fb02 # mla	r1, r2, r0, r0
${ARMSEC} thumb 0x40c8 0x203fb00 # mla	r2, r0, r3, r0
${ARMSEC} thumb 0x40ca 0x203fb00 # mla	r2, r0, r3, r0
${ARMSEC} thumb 0x40c8 0x3300fb02 # mla	r3, r2, r0, r3
${ARMSEC} thumb 0x40ca 0x3300fb02 # mla	r3, r2, r0, r3
${ARMSEC} thumb 0x40c8 0x102fb01 # mla	r1, r1, r2, r0
${ARMSEC} thumb 0x40ca 0x102fb01 # mla	r1, r1, r2, r0
${ARMSEC} thumb 0x40c8 0x3000fb02 # mla	r0, r2, r0, r3
${ARMSEC} thumb 0x40ca 0x3000fb02 # mla	r0, r2, r0, r3
${ARMSEC} thumb 0x40c8 0x2003fb01 # mla	r0, r1, r3, r2
${ARMSEC} thumb 0x40ca 0x2003fb01 # mla	r0, r1, r3, r2
${ARMSEC} thumb 0x40c8 0x1111fb01 # mls	r1, r1, r1, r1
${ARMSEC} thumb 0x40ca 0x1111fb01 # mls	r1, r1, r1, r1
${ARMSEC} thumb 0x40c8 0x110fb00 # mls	r1, r0, r0, r0
${ARMSEC} thumb 0x40ca 0x110fb00 # mls	r1, r0, r0, r0
${ARMSEC} thumb 0x40c8 0x3310fb00 # mls	r3, r0, r0, r3
${ARMSEC} thumb 0x40ca 0x3310fb00 # mls	r3, r0, r0, r3
${ARMSEC} thumb 0x40c8 0x3213fb02 # mls	r2, r2, r3, r3
${ARMSEC} thumb 0x40ca 0x3213fb02 # mls	r2, r2, r3, r3
${ARMSEC} thumb 0x40c8 0x12fb00 # mls	r0, r0, r2, r0
${ARMSEC} thumb 0x40ca 0x12fb00 # mls	r0, r0, r2, r0
${ARMSEC} thumb 0x40c8 0x2010fb00 # mls	r0, r0, r0, r2
${ARMSEC} thumb 0x40ca 0x2010fb00 # mls	r0, r0, r0, r2
${ARMSEC} thumb 0x40c8 0x1111fb02 # mls	r1, r2, r1, r1
${ARMSEC} thumb 0x40ca 0x1111fb02 # mls	r1, r2, r1, r1
${ARMSEC} thumb 0x40c8 0x212fb00 # mls	r2, r0, r2, r0
${ARMSEC} thumb 0x40ca 0x212fb00 # mls	r2, r0, r2, r0
${ARMSEC} thumb 0x40c8 0x1210fb00 # mls	r2, r0, r0, r1
${ARMSEC} thumb 0x40ca 0x1210fb00 # mls	r2, r0, r0, r1
${ARMSEC} thumb 0x40c8 0x3113fb00 # mls	r1, r0, r3, r3
${ARMSEC} thumb 0x40ca 0x3113fb00 # mls	r1, r0, r3, r3
${ARMSEC} thumb 0x40c8 0x2011fb02 # mls	r0, r2, r1, r2
${ARMSEC} thumb 0x40ca 0x2011fb02 # mls	r0, r2, r1, r2
${ARMSEC} thumb 0x40c8 0x1113fb00 # mls	r1, r0, r3, r1
${ARMSEC} thumb 0x40ca 0x1113fb00 # mls	r1, r0, r3, r1
${ARMSEC} thumb 0x40c8 0x3210fb02 # mls	r2, r2, r0, r3
${ARMSEC} thumb 0x40ca 0x3210fb02 # mls	r2, r2, r0, r3
${ARMSEC} thumb 0x40c8 0x2111fb03 # mls	r1, r3, r1, r2
${ARMSEC} thumb 0x40ca 0x2111fb03 # mls	r1, r3, r1, r2
${ARMSEC} thumb 0x40c8 0x2113fb00 # mls	r1, r0, r3, r2
${ARMSEC} thumb 0x40ca 0x2113fb00 # mls	r1, r0, r3, r2
${ARMSEC} thumb 0x40c8 0x2382 # movs	r3, #130
${ARMSEC} thumb 0x40ca 0x2382 # movs	r3, #130
${ARMSEC} thumb 0x40c8 0x4612 # mov	r2, r2
${ARMSEC} thumb 0x40ca 0x4612 # mov	r2, r2
${ARMSEC} thumb 0x40c8 0x4601 # mov	r1, r0
${ARMSEC} thumb 0x40ca 0x4601 # mov	r1, r0
${ARMSEC} thumb 0x40c8 0x467b # mov	r3, pc
${ARMSEC} thumb 0x40ca 0x467b # mov	r3, pc
${ARMSEC} thumb 0x40c8 0xc00 # lsrs	r0, r0, #16
${ARMSEC} thumb 0x40ca 0xc00 # lsrs	r0, r0, #16
${ARMSEC} thumb 0x40c8 0x1559 # asrs	r1, r3, #21
${ARMSEC} thumb 0x40ca 0x1559 # asrs	r1, r3, #21
${ARMSEC} thumb 0x40c8 0x50f0ea4f # mov.w	r0, r0, ror #23
${ARMSEC} thumb 0x40ca 0x50f0ea4f # mov.w	r0, r0, ror #23
${ARMSEC} thumb 0x40c8 0x3e3ea5f # movs.w	r3, r3, asr #3
${ARMSEC} thumb 0x40ca 0x3e3ea5f # movs.w	r3, r3, asr #3
${ARMSEC} thumb 0x40c8 0x7051ea4f # mov.w	r0, r1, lsr #29
${ARMSEC} thumb 0x40ca 0x7051ea4f # mov.w	r0, r1, lsr #29
${ARMSEC} thumb 0x40c8 0x1211ea5f # movs.w	r2, r1, lsr #4
${ARMSEC} thumb 0x40ca 0x1211ea5f # movs.w	r2, r1, lsr #4
${ARMSEC} thumb 0x40c8 0x1264f44f # mov.w	r2, #3735552
${ARMSEC} thumb 0x40ca 0x1264f44f # mov.w	r2, #3735552
${ARMSEC} thumb 0x40c8 0x21aef45f # movs.w	r1, #356352
${ARMSEC} thumb 0x40ca 0x21aef45f # movs.w	r1, #356352
${ARMSEC} thumb 0x40c8 0x3003f6cf # movt	r0, #64259
${ARMSEC} thumb 0x40ca 0x3003f6cf # movt	r0, #64259
${ARMSEC} thumb 0x40c8 0xbf64b # movw	r0, #47115
${ARMSEC} thumb 0x40ca 0xbf64b # movw	r0, #47115
${ARMSEC} thumb 0x40c8 0x4349 # muls	r1, r1
${ARMSEC} thumb 0x40ca 0x4349 # muls	r1, r1
${ARMSEC} thumb 0x40c8 0x434a # muls	r2, r1
${ARMSEC} thumb 0x40ca 0x434a # muls	r2, r1
${ARMSEC} thumb 0x40c8 0xf000fb00 # mul.w	r0, r0, r0
${ARMSEC} thumb 0x40ca 0xf000fb00 # mul.w	r0, r0, r0
${ARMSEC} thumb 0x40c8 0xf302fb02 # mul.w	r3, r2, r2
${ARMSEC} thumb 0x40ca 0xf302fb02 # mul.w	r3, r2, r2
${ARMSEC} thumb 0x40c8 0xf200fb02 # mul.w	r2, r2, r0
${ARMSEC} thumb 0x40ca 0xf200fb02 # mul.w	r2, r2, r0
${ARMSEC} thumb 0x40c8 0xf202fb03 # mul.w	r2, r3, r2
${ARMSEC} thumb 0x40ca 0xf202fb03 # mul.w	r2, r3, r2
${ARMSEC} thumb 0x40c8 0xf301fb02 # mul.w	r3, r2, r1
${ARMSEC} thumb 0x40ca 0xf301fb02 # mul.w	r3, r2, r1
${ARMSEC} thumb 0x40c8 0x43c0 # mvns	r0, r0
${ARMSEC} thumb 0x40ca 0x43c0 # mvns	r0, r0
${ARMSEC} thumb 0x40c8 0x43d0 # mvns	r0, r2
${ARMSEC} thumb 0x40ca 0x43d0 # mvns	r0, r2
${ARMSEC} thumb 0x40c8 0x43d3ea6f # mvn.w	r3, r3, lsr #19
${ARMSEC} thumb 0x40ca 0x43d3ea6f # mvn.w	r3, r3, lsr #19
${ARMSEC} thumb 0x40c8 0x3010ea7f # mvns.w	r0, r0, lsr #12
${ARMSEC} thumb 0x40ca 0x3010ea7f # mvns.w	r0, r0, lsr #12
${ARMSEC} thumb 0x40c8 0x4100ea6f # mvn.w	r1, r0, lsl #16
${ARMSEC} thumb 0x40ca 0x4100ea6f # mvn.w	r1, r0, lsl #16
${ARMSEC} thumb 0x40c8 0x7203ea7f # mvns.w	r2, r3, lsl #28
${ARMSEC} thumb 0x40ca 0x7203ea7f # mvns.w	r2, r3, lsl #28
${ARMSEC} thumb 0x40c8 0x31daf06f # mvn.w	r1, #-623191334
${ARMSEC} thumb 0x40ca 0x31daf06f # mvn.w	r1, #-623191334
${ARMSEC} thumb 0x40c8 0x130ef47f # mvns.w	r3, #2326528
${ARMSEC} thumb 0x40ca 0x130ef47f # mvns.w	r3, #2326528
${ARMSEC} thumb 0x40c8 0x4240 # negs	r0, r0
${ARMSEC} thumb 0x40ca 0x4240 # negs	r0, r0
${ARMSEC} thumb 0x40c8 0x4253 # negs	r3, r2
${ARMSEC} thumb 0x40ca 0x4253 # negs	r3, r2
${ARMSEC} thumb 0x40c8 0x4191ea61 # orn	r1, r1, r1, lsr #18
${ARMSEC} thumb 0x40ca 0x4191ea61 # orn	r1, r1, r1, lsr #18
${ARMSEC} thumb 0x40c8 0x5222ea72 # orns	r2, r2, r2, asr #20
${ARMSEC} thumb 0x40ca 0x5222ea72 # orns	r2, r2, r2, asr #20
${ARMSEC} thumb 0x40c8 0x3112ea62 # orn	r1, r2, r2, lsr #12
${ARMSEC} thumb 0x40ca 0x3112ea62 # orn	r1, r2, r2, lsr #12
${ARMSEC} thumb 0x40c8 0x4350ea70 # orns	r3, r0, r0, lsr #17
${ARMSEC} thumb 0x40ca 0x4350ea70 # orns	r3, r0, r0, lsr #17
${ARMSEC} thumb 0x40c8 0x5122ea61 # orn	r1, r1, r2, asr #20
${ARMSEC} thumb 0x40ca 0x5122ea61 # orn	r1, r1, r2, asr #20
${ARMSEC} thumb 0x40c8 0x3053ea70 # orns	r0, r0, r3, lsr #13
${ARMSEC} thumb 0x40ca 0x3053ea70 # orns	r0, r0, r3, lsr #13
${ARMSEC} thumb 0x40c8 0x1171ea62 # orn	r1, r2, r1, ror #5
${ARMSEC} thumb 0x40ca 0x1171ea62 # orn	r1, r2, r1, ror #5
${ARMSEC} thumb 0x40c8 0x5323ea71 # orns	r3, r1, r3, asr #20
${ARMSEC} thumb 0x40ca 0x5323ea71 # orns	r3, r1, r3, asr #20
${ARMSEC} thumb 0x40c8 0x3092ea61 # orn	r0, r1, r2, lsr #14
${ARMSEC} thumb 0x40ca 0x3092ea61 # orn	r0, r1, r2, lsr #14
${ARMSEC} thumb 0x40c8 0x5380ea71 # orns	r3, r1, r0, lsl #22
${ARMSEC} thumb 0x40ca 0x5380ea71 # orns	r3, r1, r0, lsl #22
${ARMSEC} thumb 0x40c8 0x3310f463 # orn	r3, r3, #147456
${ARMSEC} thumb 0x40ca 0x3310f463 # orn	r3, r3, #147456
${ARMSEC} thumb 0x40c8 0x1e4f071 # orns	r1, r1, #228
${ARMSEC} thumb 0x40ca 0x1e4f071 # orns	r1, r1, #228
${ARMSEC} thumb 0x40c8 0x63bcf062 # orn	r3, r2, #98566144
${ARMSEC} thumb 0x40ca 0x63bcf062 # orn	r3, r2, #98566144
${ARMSEC} thumb 0x40c8 0x71adf072 # orns	r1, r2, #22675456
${ARMSEC} thumb 0x40ca 0x71adf072 # orns	r1, r2, #22675456
${ARMSEC} thumb 0x40c8 0x431b # orrs	r3, r3
${ARMSEC} thumb 0x40ca 0x431b # orrs	r3, r3
${ARMSEC} thumb 0x40c8 0x4318 # orrs	r0, r3
${ARMSEC} thumb 0x40ca 0x4318 # orrs	r0, r3
${ARMSEC} thumb 0x40c8 0x6353ea43 # orr.w	r3, r3, r3, lsr #25
${ARMSEC} thumb 0x40ca 0x6353ea43 # orr.w	r3, r3, r3, lsr #25
${ARMSEC} thumb 0x40c8 0x90ea50 # orrs.w	r0, r0, r0, lsr #2
${ARMSEC} thumb 0x40ca 0x90ea50 # orrs.w	r0, r0, r0, lsr #2
${ARMSEC} thumb 0x40c8 0x71e0ea40 # orr.w	r1, r0, r0, asr #31
${ARMSEC} thumb 0x40ca 0x71e0ea40 # orr.w	r1, r0, r0, asr #31
${ARMSEC} thumb 0x40c8 0x5073ea53 # orrs.w	r0, r3, r3, ror #21
${ARMSEC} thumb 0x40ca 0x5073ea53 # orrs.w	r0, r3, r3, ror #21
${ARMSEC} thumb 0x40c8 0x4023ea40 # orr.w	r0, r0, r3, asr #16
${ARMSEC} thumb 0x40ca 0x4023ea40 # orr.w	r0, r0, r3, asr #16
${ARMSEC} thumb 0x40c8 0x6223ea52 # orrs.w	r2, r2, r3, asr #24
${ARMSEC} thumb 0x40ca 0x6223ea52 # orrs.w	r2, r2, r3, asr #24
${ARMSEC} thumb 0x40c8 0x7373ea42 # orr.w	r3, r2, r3, ror #29
${ARMSEC} thumb 0x40ca 0x7373ea42 # orr.w	r3, r2, r3, ror #29
${ARMSEC} thumb 0x40c8 0x33e3ea50 # orrs.w	r3, r0, r3, asr #15
${ARMSEC} thumb 0x40ca 0x33e3ea50 # orrs.w	r3, r0, r3, asr #15
${ARMSEC} thumb 0x40c8 0x13c2ea40 # orr.w	r3, r0, r2, lsl #7
${ARMSEC} thumb 0x40ca 0x13c2ea40 # orr.w	r3, r0, r2, lsl #7
${ARMSEC} thumb 0x40c8 0x7361ea50 # orrs.w	r3, r0, r1, asr #29
${ARMSEC} thumb 0x40ca 0x7361ea50 # orrs.w	r3, r0, r1, asr #29
${ARMSEC} thumb 0x40c8 0x109ff040 # orr.w	r0, r0, #10420383
${ARMSEC} thumb 0x40ca 0x109ff040 # orr.w	r0, r0, #10420383
${ARMSEC} thumb 0x40c8 0x5141f451 # orrs.w	r1, r1, #12352
${ARMSEC} thumb 0x40ca 0x5141f451 # orrs.w	r1, r1, #12352
${ARMSEC} thumb 0x40c8 0x1080f442 # orr.w	r0, r2, #1048576
${ARMSEC} thumb 0x40ca 0x1080f442 # orr.w	r0, r2, #1048576
${ARMSEC} thumb 0x40c8 0x418ff450 # orrs.w	r1, r0, #18304
${ARMSEC} thumb 0x40ca 0x418ff450 # orrs.w	r1, r0, #18304
${ARMSEC} thumb 0x40c8 0x1141eac1 # pkhbt	r1, r1, r1, lsl #5
${ARMSEC} thumb 0x40ca 0x1141eac1 # pkhbt	r1, r1, r1, lsl #5
${ARMSEC} thumb 0x40c8 0x73e0eac0 # pkhtb	r3, r0, r0, asr #31
${ARMSEC} thumb 0x40ca 0x73e0eac0 # pkhtb	r3, r0, r0, asr #31
${ARMSEC} thumb 0x40c8 0x73e3eac0 # pkhtb	r3, r0, r3, asr #31
${ARMSEC} thumb 0x40ca 0x73e3eac0 # pkhtb	r3, r0, r3, asr #31
${ARMSEC} thumb 0x40c8 0x5361eac3 # pkhtb	r3, r3, r1, asr #21
${ARMSEC} thumb 0x40ca 0x5361eac3 # pkhtb	r3, r3, r1, asr #21
${ARMSEC} thumb 0x40c8 0x5162eac3 # pkhtb	r1, r3, r2, asr #21
${ARMSEC} thumb 0x40ca 0x5162eac3 # pkhtb	r1, r3, r2, asr #21
${ARMSEC} thumb 0x40c8 0xf181fa81 # qadd	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf181fa81 # qadd	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf280fa80 # qadd	r2, r0, r0
${ARMSEC} thumb 0x40ca 0xf280fa80 # qadd	r2, r0, r0
${ARMSEC} thumb 0x40c8 0xf181fa83 # qadd	r1, r1, r3
${ARMSEC} thumb 0x40ca 0xf181fa83 # qadd	r1, r1, r3
${ARMSEC} thumb 0x40c8 0xf380fa83 # qadd	r3, r0, r3
${ARMSEC} thumb 0x40ca 0xf380fa83 # qadd	r3, r0, r3
${ARMSEC} thumb 0x40c8 0xf382fa80 # qadd	r3, r2, r0
${ARMSEC} thumb 0x40ca 0xf382fa80 # qadd	r3, r2, r0
${ARMSEC} thumb 0x40c8 0xf313fa93 # qadd16	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf313fa93 # qadd16	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf312fa92 # qadd16	r3, r2, r2
${ARMSEC} thumb 0x40ca 0xf312fa92 # qadd16	r3, r2, r2
${ARMSEC} thumb 0x40c8 0xf111fa90 # qadd16	r1, r0, r1
${ARMSEC} thumb 0x40ca 0xf111fa90 # qadd16	r1, r0, r1
${ARMSEC} thumb 0x40c8 0xf211fa92 # qadd16	r2, r2, r1
${ARMSEC} thumb 0x40ca 0xf211fa92 # qadd16	r2, r2, r1
${ARMSEC} thumb 0x40c8 0xf012fa93 # qadd16	r0, r3, r2
${ARMSEC} thumb 0x40ca 0xf012fa93 # qadd16	r0, r3, r2
${ARMSEC} thumb 0x40c8 0xf313fa83 # qadd8	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf313fa83 # qadd8	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf211fa81 # qadd8	r2, r1, r1
${ARMSEC} thumb 0x40ca 0xf211fa81 # qadd8	r2, r1, r1
${ARMSEC} thumb 0x40c8 0xf010fa83 # qadd8	r0, r3, r0
${ARMSEC} thumb 0x40ca 0xf010fa83 # qadd8	r0, r3, r0
${ARMSEC} thumb 0x40c8 0xf013fa80 # qadd8	r0, r0, r3
${ARMSEC} thumb 0x40ca 0xf013fa80 # qadd8	r0, r0, r3
${ARMSEC} thumb 0x40c8 0xf113fa80 # qadd8	r1, r0, r3
${ARMSEC} thumb 0x40ca 0xf113fa80 # qadd8	r1, r0, r3
${ARMSEC} thumb 0x40c8 0xf111faa1 # qasx	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf111faa1 # qasx	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf311faa1 # qasx	r3, r1, r1
${ARMSEC} thumb 0x40ca 0xf311faa1 # qasx	r3, r1, r1
${ARMSEC} thumb 0x40c8 0xf111faa3 # qasx	r1, r3, r1
${ARMSEC} thumb 0x40ca 0xf111faa3 # qasx	r1, r3, r1
${ARMSEC} thumb 0x40c8 0xf312faa3 # qasx	r3, r3, r2
${ARMSEC} thumb 0x40ca 0xf312faa3 # qasx	r3, r3, r2
${ARMSEC} thumb 0x40c8 0xf210faa3 # qasx	r2, r3, r0
${ARMSEC} thumb 0x40ca 0xf210faa3 # qasx	r2, r3, r0
${ARMSEC} thumb 0x40c8 0xf292fa82 # qdadd	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf292fa82 # qdadd	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf390fa80 # qdadd	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf390fa80 # qdadd	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf191fa80 # qdadd	r1, r1, r0
${ARMSEC} thumb 0x40ca 0xf191fa80 # qdadd	r1, r1, r0
${ARMSEC} thumb 0x40c8 0xf391fa83 # qdadd	r3, r1, r3
${ARMSEC} thumb 0x40ca 0xf391fa83 # qdadd	r3, r1, r3
${ARMSEC} thumb 0x40c8 0xf093fa82 # qdadd	r0, r3, r2
${ARMSEC} thumb 0x40ca 0xf093fa82 # qdadd	r0, r3, r2
${ARMSEC} thumb 0x40c8 0xf2b2fa82 # qdsub	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf2b2fa82 # qdsub	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf2b1fa81 # qdsub	r2, r1, r1
${ARMSEC} thumb 0x40ca 0xf2b1fa81 # qdsub	r2, r1, r1
${ARMSEC} thumb 0x40c8 0xf0b0fa81 # qdsub	r0, r0, r1
${ARMSEC} thumb 0x40ca 0xf0b0fa81 # qdsub	r0, r0, r1
${ARMSEC} thumb 0x40c8 0xf2b1fa82 # qdsub	r2, r1, r2
${ARMSEC} thumb 0x40ca 0xf2b1fa82 # qdsub	r2, r1, r2
${ARMSEC} thumb 0x40c8 0xf1b2fa83 # qdsub	r1, r2, r3
${ARMSEC} thumb 0x40ca 0xf1b2fa83 # qdsub	r1, r2, r3
${ARMSEC} thumb 0x40c8 0xf111fae1 # qsax	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf111fae1 # qsax	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf013fae3 # qsax	r0, r3, r3
${ARMSEC} thumb 0x40ca 0xf013fae3 # qsax	r0, r3, r3
${ARMSEC} thumb 0x40c8 0xf111fae0 # qsax	r1, r0, r1
${ARMSEC} thumb 0x40ca 0xf111fae0 # qsax	r1, r0, r1
${ARMSEC} thumb 0x40c8 0xf013fae0 # qsax	r0, r0, r3
${ARMSEC} thumb 0x40ca 0xf013fae0 # qsax	r0, r0, r3
${ARMSEC} thumb 0x40c8 0xf211fae0 # qsax	r2, r0, r1
${ARMSEC} thumb 0x40ca 0xf211fae0 # qsax	r2, r0, r1
${ARMSEC} thumb 0x40c8 0xf3a3fa83 # qsub	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf3a3fa83 # qsub	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf3a0fa80 # qsub	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf3a0fa80 # qsub	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf2a3fa82 # qsub	r2, r3, r2
${ARMSEC} thumb 0x40ca 0xf2a3fa82 # qsub	r2, r3, r2
${ARMSEC} thumb 0x40c8 0xf0a0fa82 # qsub	r0, r0, r2
${ARMSEC} thumb 0x40ca 0xf0a0fa82 # qsub	r0, r0, r2
${ARMSEC} thumb 0x40c8 0xf3a1fa80 # qsub	r3, r1, r0
${ARMSEC} thumb 0x40ca 0xf3a1fa80 # qsub	r3, r1, r0
${ARMSEC} thumb 0x40c8 0xf010fad0 # qsub16	r0, r0, r0
${ARMSEC} thumb 0x40ca 0xf010fad0 # qsub16	r0, r0, r0
${ARMSEC} thumb 0x40c8 0xf011fad1 # qsub16	r0, r1, r1
${ARMSEC} thumb 0x40ca 0xf011fad1 # qsub16	r0, r1, r1
${ARMSEC} thumb 0x40c8 0xf313fad2 # qsub16	r3, r2, r3
${ARMSEC} thumb 0x40ca 0xf313fad2 # qsub16	r3, r2, r3
${ARMSEC} thumb 0x40c8 0xf011fad0 # qsub16	r0, r0, r1
${ARMSEC} thumb 0x40ca 0xf011fad0 # qsub16	r0, r0, r1
${ARMSEC} thumb 0x40c8 0xf311fad2 # qsub16	r3, r2, r1
${ARMSEC} thumb 0x40ca 0xf311fad2 # qsub16	r3, r2, r1
${ARMSEC} thumb 0x40c8 0xf010fac0 # qsub8	r0, r0, r0
${ARMSEC} thumb 0x40ca 0xf010fac0 # qsub8	r0, r0, r0
${ARMSEC} thumb 0x40c8 0xf011fac1 # qsub8	r0, r1, r1
${ARMSEC} thumb 0x40ca 0xf011fac1 # qsub8	r0, r1, r1
${ARMSEC} thumb 0x40c8 0xf313fac2 # qsub8	r3, r2, r3
${ARMSEC} thumb 0x40ca 0xf313fac2 # qsub8	r3, r2, r3
${ARMSEC} thumb 0x40c8 0xf011fac0 # qsub8	r0, r0, r1
${ARMSEC} thumb 0x40ca 0xf011fac0 # qsub8	r0, r0, r1
${ARMSEC} thumb 0x40c8 0xf311fac2 # qsub8	r3, r2, r1
${ARMSEC} thumb 0x40ca 0xf311fac2 # qsub8	r3, r2, r1
${ARMSEC} thumb 0x40c8 0xf1a1fa91 # rbit	r1, r1
${ARMSEC} thumb 0x40ca 0xf1a1fa91 # rbit	r1, r1
${ARMSEC} thumb 0x40c8 0xf0a2fa92 # rbit	r0, r2
${ARMSEC} thumb 0x40ca 0xf0a2fa92 # rbit	r0, r2
${ARMSEC} thumb 0x40c8 0xba09 # rev	r1, r1
${ARMSEC} thumb 0x40ca 0xba09 # rev	r1, r1
${ARMSEC} thumb 0x40c8 0xba18 # rev	r0, r3
${ARMSEC} thumb 0x40ca 0xba18 # rev	r0, r3
${ARMSEC} thumb 0x40c8 0xba5b # rev16	r3, r3
${ARMSEC} thumb 0x40ca 0xba5b # rev16	r3, r3
${ARMSEC} thumb 0x40c8 0xba43 # rev16	r3, r0
${ARMSEC} thumb 0x40ca 0xba43 # rev16	r3, r0
${ARMSEC} thumb 0x40c8 0xf393fa93 # rev16.w	r3, r3
${ARMSEC} thumb 0x40ca 0xf393fa93 # rev16.w	r3, r3
${ARMSEC} thumb 0x40c8 0xf190fa90 # rev16.w	r1, r0
${ARMSEC} thumb 0x40ca 0xf190fa90 # rev16.w	r1, r0
${ARMSEC} thumb 0x40c8 0xf383fa93 # rev.w	r3, r3
${ARMSEC} thumb 0x40ca 0xf383fa93 # rev.w	r3, r3
${ARMSEC} thumb 0x40c8 0xf380fa90 # rev.w	r3, r0
${ARMSEC} thumb 0x40ca 0xf380fa90 # rev.w	r3, r0
${ARMSEC} thumb 0x40c8 0xbac0 # revsh	r0, r0
${ARMSEC} thumb 0x40ca 0xbac0 # revsh	r0, r0
${ARMSEC} thumb 0x40c8 0xbac8 # revsh	r0, r1
${ARMSEC} thumb 0x40ca 0xbac8 # revsh	r0, r1
${ARMSEC} thumb 0x40c8 0xf3b3fa93 # revsh.w	r3, r3
${ARMSEC} thumb 0x40ca 0xf3b3fa93 # revsh.w	r3, r3
${ARMSEC} thumb 0x40c8 0xf3b2fa92 # revsh.w	r3, r2
${ARMSEC} thumb 0x40ca 0xf3b2fa92 # revsh.w	r3, r2
${ARMSEC} thumb 0x40c8 0x41c9 # rors	r1, r1
${ARMSEC} thumb 0x40ca 0x41c9 # rors	r1, r1
${ARMSEC} thumb 0x40c8 0x41c2 # rors	r2, r0
${ARMSEC} thumb 0x40ca 0x41c2 # rors	r2, r0
${ARMSEC} thumb 0x40c8 0x1b1ebc1 # rsb	r1, r1, r1, ror #2
${ARMSEC} thumb 0x40ca 0x1b1ebc1 # rsb	r1, r1, r1, ror #2
${ARMSEC} thumb 0x40c8 0x20e0ebd0 # rsbs	r0, r0, r0, asr #11
${ARMSEC} thumb 0x40ca 0x20e0ebd0 # rsbs	r0, r0, r0, asr #11
${ARMSEC} thumb 0x40c8 0x2132ebc2 # rsb	r1, r2, r2, ror #8
${ARMSEC} thumb 0x40ca 0x2132ebc2 # rsb	r1, r2, r2, ror #8
${ARMSEC} thumb 0x40c8 0x130ebd0 # rsbs	r1, r0, r0, rrx
${ARMSEC} thumb 0x40ca 0x130ebd0 # rsbs	r1, r0, r0, rrx
${ARMSEC} thumb 0x40c8 0x3241ebc2 # rsb	r2, r2, r1, lsl #13
${ARMSEC} thumb 0x40ca 0x3241ebc2 # rsb	r2, r2, r1, lsl #13
${ARMSEC} thumb 0x40c8 0x13a1ebd3 # rsbs	r3, r3, r1, asr #6
${ARMSEC} thumb 0x40ca 0x13a1ebd3 # rsbs	r3, r3, r1, asr #6
${ARMSEC} thumb 0x40c8 0x5080ebc2 # rsb	r0, r2, r0, lsl #22
${ARMSEC} thumb 0x40ca 0x5080ebc2 # rsb	r0, r2, r0, lsl #22
${ARMSEC} thumb 0x40c8 0x72e2ebd3 # rsbs	r2, r3, r2, asr #31
${ARMSEC} thumb 0x40ca 0x72e2ebd3 # rsbs	r2, r3, r2, asr #31
${ARMSEC} thumb 0x40c8 0x3351ebc2 # rsb	r3, r2, r1, lsr #13
${ARMSEC} thumb 0x40ca 0x3351ebc2 # rsb	r3, r2, r1, lsr #13
${ARMSEC} thumb 0x40c8 0x60c1ebd3 # rsbs	r0, r3, r1, lsl #27
${ARMSEC} thumb 0x40ca 0x60c1ebd3 # rsbs	r0, r3, r1, lsl #27
${ARMSEC} thumb 0x40c8 0x4245f1c2 # rsb	r2, r2, #-989855744
${ARMSEC} thumb 0x40ca 0x4245f1c2 # rsb	r2, r2, #-989855744
${ARMSEC} thumb 0x40c8 0x4140f5d1 # rsbs	r1, r1, #49152
${ARMSEC} thumb 0x40ca 0x4140f5d1 # rsbs	r1, r1, #49152
${ARMSEC} thumb 0x40c8 0x1125f5c3 # rsb	r1, r3, #2703360
${ARMSEC} thumb 0x40ca 0x1125f5c3 # rsb	r1, r3, #2703360
${ARMSEC} thumb 0x40c8 0x12baf5d3 # rsbs	r2, r3, #1523712
${ARMSEC} thumb 0x40ca 0x12baf5d3 # rsbs	r2, r3, #1523712
${ARMSEC} thumb 0x40c8 0xf303fa93 # sadd16	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf303fa93 # sadd16	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf300fa90 # sadd16	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf300fa90 # sadd16	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf000fa92 # sadd16	r0, r2, r0
${ARMSEC} thumb 0x40ca 0xf000fa92 # sadd16	r0, r2, r0
${ARMSEC} thumb 0x40c8 0xf203fa92 # sadd16	r2, r2, r3
${ARMSEC} thumb 0x40ca 0xf203fa92 # sadd16	r2, r2, r3
${ARMSEC} thumb 0x40c8 0xf301fa90 # sadd16	r3, r0, r1
${ARMSEC} thumb 0x40ca 0xf301fa90 # sadd16	r3, r0, r1
${ARMSEC} thumb 0x40c8 0xf202fa82 # sadd8	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf202fa82 # sadd8	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf300fa80 # sadd8	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf300fa80 # sadd8	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf101fa80 # sadd8	r1, r0, r1
${ARMSEC} thumb 0x40ca 0xf101fa80 # sadd8	r1, r0, r1
${ARMSEC} thumb 0x40c8 0xf301fa83 # sadd8	r3, r3, r1
${ARMSEC} thumb 0x40ca 0xf301fa83 # sadd8	r3, r3, r1
${ARMSEC} thumb 0x40c8 0xf003fa82 # sadd8	r0, r2, r3
${ARMSEC} thumb 0x40ca 0xf003fa82 # sadd8	r0, r2, r3
${ARMSEC} thumb 0x40c8 0xf303faa3 # sasx	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf303faa3 # sasx	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf300faa0 # sasx	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf300faa0 # sasx	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf000faa2 # sasx	r0, r2, r0
${ARMSEC} thumb 0x40ca 0xf000faa2 # sasx	r0, r2, r0
${ARMSEC} thumb 0x40c8 0xf203faa2 # sasx	r2, r2, r3
${ARMSEC} thumb 0x40ca 0xf203faa2 # sasx	r2, r2, r3
${ARMSEC} thumb 0x40c8 0xf301faa0 # sasx	r3, r0, r1
${ARMSEC} thumb 0x40ca 0xf301faa0 # sasx	r3, r0, r1
${ARMSEC} thumb 0x40c8 0x4180 # sbcs	r0, r0
${ARMSEC} thumb 0x40ca 0x4180 # sbcs	r0, r0
${ARMSEC} thumb 0x40c8 0x4183 # sbcs	r3, r0
${ARMSEC} thumb 0x40ca 0x4183 # sbcs	r3, r0
${ARMSEC} thumb 0x40c8 0x4030eb60 # sbc.w	r0, r0, r0, ror #16
${ARMSEC} thumb 0x40ca 0x4030eb60 # sbc.w	r0, r0, r0, ror #16
${ARMSEC} thumb 0x40c8 0x1262eb72 # sbcs.w	r2, r2, r2, asr #5
${ARMSEC} thumb 0x40ca 0x1262eb72 # sbcs.w	r2, r2, r2, asr #5
${ARMSEC} thumb 0x40c8 0x7381eb61 # sbc.w	r3, r1, r1, lsl #30
${ARMSEC} thumb 0x40ca 0x7381eb61 # sbc.w	r3, r1, r1, lsl #30
${ARMSEC} thumb 0x40c8 0x6183eb73 # sbcs.w	r1, r3, r3, lsl #26
${ARMSEC} thumb 0x40ca 0x6183eb73 # sbcs.w	r1, r3, r3, lsl #26
${ARMSEC} thumb 0x40c8 0x3141eb60 # sbc.w	r1, r0, r1, lsl #13
${ARMSEC} thumb 0x40ca 0x3141eb60 # sbc.w	r1, r0, r1, lsl #13
${ARMSEC} thumb 0x40c8 0x13a3eb71 # sbcs.w	r3, r1, r3, asr #6
${ARMSEC} thumb 0x40ca 0x13a3eb71 # sbcs.w	r3, r1, r3, asr #6
${ARMSEC} thumb 0x40c8 0x6073eb60 # sbc.w	r0, r0, r3, ror #25
${ARMSEC} thumb 0x40ca 0x6073eb60 # sbc.w	r0, r0, r3, ror #25
${ARMSEC} thumb 0x40c8 0x1103eb71 # sbcs.w	r1, r1, r3, lsl #4
${ARMSEC} thumb 0x40ca 0x1103eb71 # sbcs.w	r1, r1, r3, lsl #4
${ARMSEC} thumb 0x40c8 0x4110eb63 # sbc.w	r1, r3, r0, lsr #16
${ARMSEC} thumb 0x40ca 0x4110eb63 # sbc.w	r1, r3, r0, lsr #16
${ARMSEC} thumb 0x40c8 0x102eb73 # sbcs.w	r1, r3, r2
${ARMSEC} thumb 0x40ca 0x102eb73 # sbcs.w	r1, r3, r2
${ARMSEC} thumb 0x40c8 0x3121f561 # sbc.w	r1, r1, #164864
${ARMSEC} thumb 0x40ca 0x3121f561 # sbc.w	r1, r1, #164864
${ARMSEC} thumb 0x40c8 0x63e1f573 # sbcs.w	r3, r3, #1800
${ARMSEC} thumb 0x40ca 0x63e1f573 # sbcs.w	r3, r3, #1800
${ARMSEC} thumb 0x40c8 0x21fbf562 # sbc.w	r1, r2, #514048
${ARMSEC} thumb 0x40ca 0x21fbf562 # sbc.w	r1, r2, #514048
${ARMSEC} thumb 0x40c8 0x400af573 # sbcs.w	r0, r3, #35328
${ARMSEC} thumb 0x40ca 0x400af573 # sbcs.w	r0, r3, #35328
${ARMSEC} thumb 0x40c8 0x6141f341 # sbfx	r1, r1, #25, #2
${ARMSEC} thumb 0x40ca 0x6141f341 # sbfx	r1, r1, #25, #2
${ARMSEC} thumb 0x40c8 0x434bf340 # sbfx	r3, r0, #17, #12
${ARMSEC} thumb 0x40ca 0x434bf340 # sbfx	r3, r0, #17, #12
${ARMSEC} thumb 0x40c8 0xf282faa2 # sel	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf282faa2 # sel	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf381faa1 # sel	r3, r1, r1
${ARMSEC} thumb 0x40ca 0xf381faa1 # sel	r3, r1, r1
${ARMSEC} thumb 0x40c8 0xf282faa3 # sel	r2, r3, r2
${ARMSEC} thumb 0x40ca 0xf282faa3 # sel	r2, r3, r2
${ARMSEC} thumb 0x40c8 0xf280faa2 # sel	r2, r2, r0
${ARMSEC} thumb 0x40ca 0xf280faa2 # sel	r2, r2, r0
${ARMSEC} thumb 0x40c8 0xf381faa2 # sel	r3, r2, r1
${ARMSEC} thumb 0x40ca 0xf381faa2 # sel	r3, r2, r1
${ARMSEC} thumb 0x40c8 0xf222fa92 # shadd16	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf222fa92 # shadd16	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf221fa91 # shadd16	r2, r1, r1
${ARMSEC} thumb 0x40ca 0xf221fa91 # shadd16	r2, r1, r1
${ARMSEC} thumb 0x40c8 0xf020fa91 # shadd16	r0, r1, r0
${ARMSEC} thumb 0x40ca 0xf020fa91 # shadd16	r0, r1, r0
${ARMSEC} thumb 0x40c8 0xf221fa92 # shadd16	r2, r2, r1
${ARMSEC} thumb 0x40ca 0xf221fa92 # shadd16	r2, r2, r1
${ARMSEC} thumb 0x40c8 0xf122fa93 # shadd16	r1, r3, r2
${ARMSEC} thumb 0x40ca 0xf122fa93 # shadd16	r1, r3, r2
${ARMSEC} thumb 0x40c8 0xf323fa83 # shadd8	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf323fa83 # shadd8	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf320fa80 # shadd8	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf320fa80 # shadd8	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf222fa80 # shadd8	r2, r0, r2
${ARMSEC} thumb 0x40ca 0xf222fa80 # shadd8	r2, r0, r2
${ARMSEC} thumb 0x40c8 0xf220fa82 # shadd8	r2, r2, r0
${ARMSEC} thumb 0x40ca 0xf220fa82 # shadd8	r2, r2, r0
${ARMSEC} thumb 0x40c8 0xf223fa80 # shadd8	r2, r0, r3
${ARMSEC} thumb 0x40ca 0xf223fa80 # shadd8	r2, r0, r3
${ARMSEC} thumb 0x40c8 0xf222faa2 # shasx	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf222faa2 # shasx	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf321faa1 # shasx	r3, r1, r1
${ARMSEC} thumb 0x40ca 0xf321faa1 # shasx	r3, r1, r1
${ARMSEC} thumb 0x40c8 0xf222faa3 # shasx	r2, r3, r2
${ARMSEC} thumb 0x40ca 0xf222faa3 # shasx	r2, r3, r2
${ARMSEC} thumb 0x40c8 0xf220faa2 # shasx	r2, r2, r0
${ARMSEC} thumb 0x40ca 0xf220faa2 # shasx	r2, r2, r0
${ARMSEC} thumb 0x40c8 0xf321faa2 # shasx	r3, r2, r1
${ARMSEC} thumb 0x40ca 0xf321faa2 # shasx	r3, r2, r1
${ARMSEC} thumb 0x40c8 0xf101fa21 # lsr.w	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf101fa21 # lsr.w	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf000fa70 # rors.w	r0, r0, r0
${ARMSEC} thumb 0x40ca 0xf000fa70 # rors.w	r0, r0, r0
${ARMSEC} thumb 0x40c8 0xf002fa62 # ror.w	r0, r2, r2
${ARMSEC} thumb 0x40ca 0xf002fa62 # ror.w	r0, r2, r2
${ARMSEC} thumb 0x40c8 0xf300fa10 # lsls.w	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf300fa10 # lsls.w	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf202fa21 # lsr.w	r2, r1, r2
${ARMSEC} thumb 0x40ca 0xf202fa21 # lsr.w	r2, r1, r2
${ARMSEC} thumb 0x40c8 0xf101fa13 # lsls.w	r1, r3, r1
${ARMSEC} thumb 0x40ca 0xf101fa13 # lsls.w	r1, r3, r1
${ARMSEC} thumb 0x40c8 0xf201fa22 # lsr.w	r2, r2, r1
${ARMSEC} thumb 0x40ca 0xf201fa22 # lsr.w	r2, r2, r1
${ARMSEC} thumb 0x40c8 0xf100fa31 # lsrs.w	r1, r1, r0
${ARMSEC} thumb 0x40ca 0xf100fa31 # lsrs.w	r1, r1, r0
${ARMSEC} thumb 0x40c8 0xf103fa42 # asr.w	r1, r2, r3
${ARMSEC} thumb 0x40ca 0xf103fa42 # asr.w	r1, r2, r3
${ARMSEC} thumb 0x40c8 0xf003fa72 # rors.w	r0, r2, r3
${ARMSEC} thumb 0x40ca 0xf003fa72 # rors.w	r0, r2, r3
${ARMSEC} thumb 0x40c8 0xf222fae2 # shsax	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf222fae2 # shsax	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf123fae3 # shsax	r1, r3, r3
${ARMSEC} thumb 0x40ca 0xf123fae3 # shsax	r1, r3, r3
${ARMSEC} thumb 0x40c8 0xf323fae1 # shsax	r3, r1, r3
${ARMSEC} thumb 0x40ca 0xf323fae1 # shsax	r3, r1, r3
${ARMSEC} thumb 0x40c8 0xf322fae3 # shsax	r3, r3, r2
${ARMSEC} thumb 0x40ca 0xf322fae3 # shsax	r3, r3, r2
${ARMSEC} thumb 0x40c8 0xf120fae3 # shsax	r1, r3, r0
${ARMSEC} thumb 0x40ca 0xf120fae3 # shsax	r1, r3, r0
${ARMSEC} thumb 0x40c8 0xf323fad3 # shsub16	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf323fad3 # shsub16	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf022fad2 # shsub16	r0, r2, r2
${ARMSEC} thumb 0x40ca 0xf022fad2 # shsub16	r0, r2, r2
${ARMSEC} thumb 0x40c8 0xf222fad0 # shsub16	r2, r0, r2
${ARMSEC} thumb 0x40ca 0xf222fad0 # shsub16	r2, r0, r2
${ARMSEC} thumb 0x40c8 0xf322fad3 # shsub16	r3, r3, r2
${ARMSEC} thumb 0x40ca 0xf322fad3 # shsub16	r3, r3, r2
${ARMSEC} thumb 0x40c8 0xf221fad0 # shsub16	r2, r0, r1
${ARMSEC} thumb 0x40ca 0xf221fad0 # shsub16	r2, r0, r1
${ARMSEC} thumb 0x40c8 0xf121fac1 # shsub8	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf121fac1 # shsub8	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf221fac1 # shsub8	r2, r1, r1
${ARMSEC} thumb 0x40ca 0xf221fac1 # shsub8	r2, r1, r1
${ARMSEC} thumb 0x40c8 0xf020fac3 # shsub8	r0, r3, r0
${ARMSEC} thumb 0x40ca 0xf020fac3 # shsub8	r0, r3, r0
${ARMSEC} thumb 0x40c8 0xf120fac1 # shsub8	r1, r1, r0
${ARMSEC} thumb 0x40ca 0xf120fac1 # shsub8	r1, r1, r0
${ARMSEC} thumb 0x40c8 0xf021fac3 # shsub8	r0, r3, r1
${ARMSEC} thumb 0x40ca 0xf021fac3 # shsub8	r0, r3, r1
${ARMSEC} thumb 0x40c8 0x3313fb23 # smladx	r3, r3, r3, r3
${ARMSEC} thumb 0x40ca 0x3313fb23 # smladx	r3, r3, r3, r3
${ARMSEC} thumb 0x40c8 0x3103fb23 # smlad	r1, r3, r3, r3
${ARMSEC} thumb 0x40ca 0x3103fb23 # smlad	r1, r3, r3, r3
${ARMSEC} thumb 0x40c8 0x2200fb20 # smlad	r2, r0, r0, r2
${ARMSEC} thumb 0x40ca 0x2200fb20 # smlad	r2, r0, r0, r2
${ARMSEC} thumb 0x40c8 0x111fb20 # smladx	r1, r0, r1, r0
${ARMSEC} thumb 0x40ca 0x111fb20 # smladx	r1, r0, r1, r0
${ARMSEC} thumb 0x40c8 0x3303fb22 # smlad	r3, r2, r3, r3
${ARMSEC} thumb 0x40ca 0x3303fb22 # smlad	r3, r2, r3, r3
${ARMSEC} thumb 0x40c8 0x3111fb21 # smladx	r1, r1, r1, r3
${ARMSEC} thumb 0x40ca 0x3111fb21 # smladx	r1, r1, r1, r3
${ARMSEC} thumb 0x40c8 0x2211fb22 # smladx	r2, r2, r1, r2
${ARMSEC} thumb 0x40ca 0x2211fb22 # smladx	r2, r2, r1, r2
${ARMSEC} thumb 0x40c8 0x1211fb22 # smladx	r2, r2, r1, r1
${ARMSEC} thumb 0x40ca 0x1211fb22 # smladx	r2, r2, r1, r1
${ARMSEC} thumb 0x40c8 0x3011fb21 # smladx	r0, r1, r1, r3
${ARMSEC} thumb 0x40ca 0x3011fb21 # smladx	r0, r1, r1, r3
${ARMSEC} thumb 0x40c8 0x203fb20 # smlad	r2, r0, r3, r0
${ARMSEC} thumb 0x40ca 0x203fb20 # smlad	r2, r0, r3, r0
${ARMSEC} thumb 0x40c8 0x3103fb22 # smlad	r1, r2, r3, r3
${ARMSEC} thumb 0x40ca 0x3103fb22 # smlad	r1, r2, r3, r3
${ARMSEC} thumb 0x40c8 0x3302fb20 # smlad	r3, r0, r2, r3
${ARMSEC} thumb 0x40ca 0x3302fb20 # smlad	r3, r0, r2, r3
${ARMSEC} thumb 0x40c8 0x1313fb22 # smladx	r3, r2, r3, r1
${ARMSEC} thumb 0x40ca 0x1313fb22 # smladx	r3, r2, r3, r1
${ARMSEC} thumb 0x40c8 0x3200fb22 # smlad	r2, r2, r0, r3
${ARMSEC} thumb 0x40ca 0x3200fb22 # smlad	r2, r2, r0, r3
${ARMSEC} thumb 0x40c8 0x3011fb22 # smladx	r0, r2, r1, r3
${ARMSEC} thumb 0x40ca 0x3011fb22 # smladx	r0, r2, r1, r3
${ARMSEC} thumb 0x40c8 0x3003fbc3 # smlal	r3, r0, r3, r3
${ARMSEC} thumb 0x40ca 0x3003fbc3 # smlal	r3, r0, r3, r3
${ARMSEC} thumb 0x40c8 0x1303fbc3 # smlal	r1, r3, r3, r3
${ARMSEC} thumb 0x40ca 0x1303fbc3 # smlal	r1, r3, r3, r3
${ARMSEC} thumb 0x40c8 0x202fbc0 # smlal	r0, r2, r0, r2
${ARMSEC} thumb 0x40ca 0x202fbc0 # smlal	r0, r2, r0, r2
${ARMSEC} thumb 0x40c8 0x2102fbc1 # smlal	r2, r1, r1, r2
${ARMSEC} thumb 0x40ca 0x2102fbc1 # smlal	r2, r1, r1, r2
${ARMSEC} thumb 0x40c8 0x301fbc1 # smlal	r0, r3, r1, r1
${ARMSEC} thumb 0x40ca 0x301fbc1 # smlal	r0, r3, r1, r1
${ARMSEC} thumb 0x40c8 0x2102fbc0 # smlal	r2, r1, r0, r2
${ARMSEC} thumb 0x40ca 0x2102fbc0 # smlal	r2, r1, r0, r2
${ARMSEC} thumb 0x40c8 0x303fbc1 # smlal	r0, r3, r1, r3
${ARMSEC} thumb 0x40ca 0x303fbc1 # smlal	r0, r3, r1, r3
${ARMSEC} thumb 0x40c8 0x1002fbc1 # smlal	r1, r0, r1, r2
${ARMSEC} thumb 0x40ca 0x1002fbc1 # smlal	r1, r0, r1, r2
${ARMSEC} thumb 0x40c8 0x3002fbc0 # smlal	r3, r0, r0, r2
${ARMSEC} thumb 0x40ca 0x3002fbc0 # smlal	r3, r0, r0, r2
${ARMSEC} thumb 0x40c8 0x2103fbc0 # smlal	r2, r1, r0, r3
${ARMSEC} thumb 0x40ca 0x2103fbc0 # smlal	r2, r1, r0, r3
${ARMSEC} thumb 0x40c8 0x1d0fbc0 # smlaldx	r0, r1, r0, r0
${ARMSEC} thumb 0x40ca 0x1d0fbc0 # smlaldx	r0, r1, r0, r0
${ARMSEC} thumb 0x40c8 0x30c0fbc0 # smlald	r3, r0, r0, r0
${ARMSEC} thumb 0x40ca 0x30c0fbc0 # smlald	r3, r0, r0, r0
${ARMSEC} thumb 0x40c8 0x32d2fbc3 # smlaldx	r3, r2, r3, r2
${ARMSEC} thumb 0x40ca 0x32d2fbc3 # smlaldx	r3, r2, r3, r2
${ARMSEC} thumb 0x40c8 0x23c2fbc3 # smlald	r2, r3, r3, r2
${ARMSEC} thumb 0x40ca 0x23c2fbc3 # smlald	r2, r3, r3, r2
${ARMSEC} thumb 0x40c8 0x12c3fbc3 # smlald	r1, r2, r3, r3
${ARMSEC} thumb 0x40ca 0x12c3fbc3 # smlald	r1, r2, r3, r3
${ARMSEC} thumb 0x40c8 0x13c1fbc2 # smlald	r1, r3, r2, r1
${ARMSEC} thumb 0x40ca 0x13c1fbc2 # smlald	r1, r3, r2, r1
${ARMSEC} thumb 0x40c8 0x1c1fbc2 # smlald	r0, r1, r2, r1
${ARMSEC} thumb 0x40ca 0x1c1fbc2 # smlald	r0, r1, r2, r1
${ARMSEC} thumb 0x40c8 0x30c1fbc3 # smlald	r3, r0, r3, r1
${ARMSEC} thumb 0x40ca 0x30c1fbc3 # smlald	r3, r0, r3, r1
${ARMSEC} thumb 0x40c8 0x32c0fbc2 # smlald	r3, r2, r2, r0
${ARMSEC} thumb 0x40ca 0x32c0fbc2 # smlald	r3, r2, r2, r0
${ARMSEC} thumb 0x40c8 0x32c1fbc0 # smlald	r3, r2, r0, r1
${ARMSEC} thumb 0x40ca 0x32c1fbc0 # smlald	r3, r2, r0, r1
${ARMSEC} thumb 0x40c8 0x21a2fbc2 # smlaltb	r2, r1, r2, r2
${ARMSEC} thumb 0x40ca 0x21a2fbc2 # smlaltb	r2, r1, r2, r2
${ARMSEC} thumb 0x40c8 0x12b2fbc2 # smlaltt	r1, r2, r2, r2
${ARMSEC} thumb 0x40ca 0x12b2fbc2 # smlaltt	r1, r2, r2, r2
${ARMSEC} thumb 0x40c8 0x2181fbc2 # smlalbb	r2, r1, r2, r1
${ARMSEC} thumb 0x40ca 0x2181fbc2 # smlalbb	r2, r1, r2, r1
${ARMSEC} thumb 0x40c8 0x2b0fbc2 # smlaltt	r0, r2, r2, r0
${ARMSEC} thumb 0x40ca 0x2b0fbc2 # smlaltt	r0, r2, r2, r0
${ARMSEC} thumb 0x40c8 0x3190fbc0 # smlalbt	r3, r1, r0, r0
${ARMSEC} thumb 0x40ca 0x3190fbc0 # smlalbt	r3, r1, r0, r0
${ARMSEC} thumb 0x40c8 0x1391fbc0 # smlalbt	r1, r3, r0, r1
${ARMSEC} thumb 0x40ca 0x1391fbc0 # smlalbt	r1, r3, r0, r1
${ARMSEC} thumb 0x40c8 0x30a0fbc1 # smlaltb	r3, r0, r1, r0
${ARMSEC} thumb 0x40ca 0x30a0fbc1 # smlaltb	r3, r0, r1, r0
${ARMSEC} thumb 0x40c8 0x392fbc0 # smlalbt	r0, r3, r0, r2
${ARMSEC} thumb 0x40ca 0x392fbc0 # smlalbt	r0, r3, r0, r2
${ARMSEC} thumb 0x40c8 0x10a3fbc0 # smlaltb	r1, r0, r0, r3
${ARMSEC} thumb 0x40ca 0x10a3fbc0 # smlaltb	r1, r0, r0, r3
${ARMSEC} thumb 0x40c8 0x1380fbc2 # smlalbb	r1, r3, r2, r0
${ARMSEC} thumb 0x40ca 0x1380fbc2 # smlalbb	r1, r3, r2, r0
${ARMSEC} thumb 0x40c8 0xfb30 # smlawb	r0, r0, r0, r0
${ARMSEC} thumb 0x40ca 0xfb30 # smlawb	r0, r0, r0, r0
${ARMSEC} thumb 0x40c8 0x2102fb32 # smlawb	r1, r2, r2, r2
${ARMSEC} thumb 0x40ca 0x2102fb32 # smlawb	r1, r2, r2, r2
${ARMSEC} thumb 0x40c8 0x13fb33 # smlawt	r0, r3, r3, r0
${ARMSEC} thumb 0x40ca 0x13fb33 # smlawt	r0, r3, r3, r0
${ARMSEC} thumb 0x40c8 0x2101fb32 # smlawb	r1, r2, r1, r2
${ARMSEC} thumb 0x40ca 0x2101fb32 # smlawb	r1, r2, r1, r2
${ARMSEC} thumb 0x40c8 0xfb32 # smlawb	r0, r2, r0, r0
${ARMSEC} thumb 0x40ca 0xfb32 # smlawb	r0, r2, r0, r0
${ARMSEC} thumb 0x40c8 0x2010fb30 # smlawt	r0, r0, r0, r2
${ARMSEC} thumb 0x40ca 0x2010fb30 # smlawt	r0, r0, r0, r2
${ARMSEC} thumb 0x40c8 0x3311fb33 # smlawt	r3, r3, r1, r3
${ARMSEC} thumb 0x40ca 0x3311fb33 # smlawt	r3, r3, r1, r3
${ARMSEC} thumb 0x40c8 0x1301fb33 # smlawb	r3, r3, r1, r1
${ARMSEC} thumb 0x40ca 0x1301fb33 # smlawb	r3, r3, r1, r1
${ARMSEC} thumb 0x40c8 0x1013fb33 # smlawt	r0, r3, r3, r1
${ARMSEC} thumb 0x40ca 0x1013fb33 # smlawt	r0, r3, r3, r1
${ARMSEC} thumb 0x40c8 0x2011fb32 # smlawt	r0, r2, r1, r2
${ARMSEC} thumb 0x40ca 0x2011fb32 # smlawt	r0, r2, r1, r2
${ARMSEC} thumb 0x40c8 0x3113fb32 # smlawt	r1, r2, r3, r3
${ARMSEC} thumb 0x40ca 0x3113fb32 # smlawt	r1, r2, r3, r3
${ARMSEC} thumb 0x40c8 0x1113fb30 # smlawt	r1, r0, r3, r1
${ARMSEC} thumb 0x40ca 0x1113fb30 # smlawt	r1, r0, r3, r1
${ARMSEC} thumb 0x40c8 0x2000fb31 # smlawb	r0, r1, r0, r2
${ARMSEC} thumb 0x40ca 0x2000fb31 # smlawb	r0, r1, r0, r2
${ARMSEC} thumb 0x40c8 0x1200fb32 # smlawb	r2, r2, r0, r1
${ARMSEC} thumb 0x40ca 0x1200fb32 # smlawb	r2, r2, r0, r1
${ARMSEC} thumb 0x40c8 0x103fb32 # smlawb	r1, r2, r3, r0
${ARMSEC} thumb 0x40ca 0x103fb32 # smlawb	r1, r2, r3, r0
${ARMSEC} thumb 0x40c8 0x30fb10 # smlatt	r0, r0, r0, r0
${ARMSEC} thumb 0x40ca 0x30fb10 # smlatt	r0, r0, r0, r0
${ARMSEC} thumb 0x40c8 0x2032fb12 # smlatt	r0, r2, r2, r2
${ARMSEC} thumb 0x40ca 0x2032fb12 # smlatt	r0, r2, r2, r2
${ARMSEC} thumb 0x40c8 0x3320fb10 # smlatb	r3, r0, r0, r3
${ARMSEC} thumb 0x40ca 0x3320fb10 # smlatb	r3, r0, r0, r3
${ARMSEC} thumb 0x40c8 0x2010fb12 # smlabt	r0, r2, r0, r2
${ARMSEC} thumb 0x40ca 0x2010fb12 # smlabt	r0, r2, r0, r2
${ARMSEC} thumb 0x40c8 0x2212fb13 # smlabt	r2, r3, r2, r2
${ARMSEC} thumb 0x40ca 0x2212fb13 # smlabt	r2, r3, r2, r2
${ARMSEC} thumb 0x40c8 0x3232fb12 # smlatt	r2, r2, r2, r3
${ARMSEC} thumb 0x40ca 0x3232fb12 # smlatt	r2, r2, r2, r3
${ARMSEC} thumb 0x40c8 0x1113fb11 # smlabt	r1, r1, r3, r1
${ARMSEC} thumb 0x40ca 0x1113fb11 # smlabt	r1, r1, r3, r1
${ARMSEC} thumb 0x40c8 0x320fb13 # smlatb	r3, r3, r0, r0
${ARMSEC} thumb 0x40ca 0x320fb13 # smlatb	r3, r3, r0, r0
${ARMSEC} thumb 0x40c8 0x3221fb11 # smlatb	r2, r1, r1, r3
${ARMSEC} thumb 0x40ca 0x3221fb11 # smlatb	r2, r1, r1, r3
${ARMSEC} thumb 0x40c8 0x3011fb13 # smlabt	r0, r3, r1, r3
${ARMSEC} thumb 0x40ca 0x3011fb13 # smlabt	r0, r3, r1, r3
${ARMSEC} thumb 0x40c8 0x1031fb12 # smlatt	r0, r2, r1, r1
${ARMSEC} thumb 0x40ca 0x1031fb12 # smlatt	r0, r2, r1, r1
${ARMSEC} thumb 0x40c8 0x1110fb12 # smlabt	r1, r2, r0, r1
${ARMSEC} thumb 0x40ca 0x1110fb12 # smlabt	r1, r2, r0, r1
${ARMSEC} thumb 0x40c8 0x2121fb13 # smlatb	r1, r3, r1, r2
${ARMSEC} thumb 0x40ca 0x2121fb13 # smlatb	r1, r3, r1, r2
${ARMSEC} thumb 0x40c8 0x2321fb13 # smlatb	r3, r3, r1, r2
${ARMSEC} thumb 0x40ca 0x2321fb13 # smlatb	r3, r3, r1, r2
${ARMSEC} thumb 0x40c8 0x302fb11 # smlabb	r3, r1, r2, r0
${ARMSEC} thumb 0x40ca 0x302fb11 # smlabb	r3, r1, r2, r0
${ARMSEC} thumb 0x40c8 0x1111fb41 # smlsdx	r1, r1, r1, r1
${ARMSEC} thumb 0x40ca 0x1111fb41 # smlsdx	r1, r1, r1, r1
${ARMSEC} thumb 0x40c8 0x3113fb43 # smlsdx	r1, r3, r3, r3
${ARMSEC} thumb 0x40ca 0x3113fb43 # smlsdx	r1, r3, r3, r3
${ARMSEC} thumb 0x40c8 0x3310fb40 # smlsdx	r3, r0, r0, r3
${ARMSEC} thumb 0x40ca 0x3310fb40 # smlsdx	r3, r0, r0, r3
${ARMSEC} thumb 0x40c8 0x1303fb41 # smlsd	r3, r1, r3, r1
${ARMSEC} thumb 0x40ca 0x1303fb41 # smlsd	r3, r1, r3, r1
${ARMSEC} thumb 0x40c8 0x10fb43 # smlsdx	r0, r3, r0, r0
${ARMSEC} thumb 0x40ca 0x10fb43 # smlsdx	r0, r3, r0, r0
${ARMSEC} thumb 0x40c8 0x3101fb41 # smlsd	r1, r1, r1, r3
${ARMSEC} thumb 0x40ca 0x3101fb41 # smlsd	r1, r1, r1, r3
${ARMSEC} thumb 0x40c8 0x2fb40 # smlsd	r0, r0, r2, r0
${ARMSEC} thumb 0x40ca 0x2fb40 # smlsd	r0, r0, r2, r0
${ARMSEC} thumb 0x40c8 0x1211fb42 # smlsdx	r2, r2, r1, r1
${ARMSEC} thumb 0x40ca 0x1211fb42 # smlsdx	r2, r2, r1, r1
${ARMSEC} thumb 0x40c8 0x302fb42 # smlsd	r3, r2, r2, r0
${ARMSEC} thumb 0x40ca 0x302fb42 # smlsd	r3, r2, r2, r0
${ARMSEC} thumb 0x40c8 0x3211fb43 # smlsdx	r2, r3, r1, r3
${ARMSEC} thumb 0x40ca 0x3211fb43 # smlsdx	r2, r3, r1, r3
${ARMSEC} thumb 0x40c8 0x1201fb43 # smlsd	r2, r3, r1, r1
${ARMSEC} thumb 0x40ca 0x1201fb43 # smlsd	r2, r3, r1, r1
${ARMSEC} thumb 0x40c8 0x2201fb40 # smlsd	r2, r0, r1, r2
${ARMSEC} thumb 0x40ca 0x2201fb40 # smlsd	r2, r0, r1, r2
${ARMSEC} thumb 0x40c8 0x1202fb43 # smlsd	r2, r3, r2, r1
${ARMSEC} thumb 0x40ca 0x1202fb43 # smlsd	r2, r3, r2, r1
${ARMSEC} thumb 0x40c8 0x3210fb42 # smlsdx	r2, r2, r0, r3
${ARMSEC} thumb 0x40ca 0x3210fb42 # smlsdx	r2, r2, r0, r3
${ARMSEC} thumb 0x40c8 0x3112fb40 # smlsdx	r1, r0, r2, r3
${ARMSEC} thumb 0x40ca 0x3112fb40 # smlsdx	r1, r0, r2, r3
${ARMSEC} thumb 0x40c8 0x31c3fbd3 # smlsld	r3, r1, r3, r3
${ARMSEC} thumb 0x40ca 0x31c3fbd3 # smlsld	r3, r1, r3, r3
${ARMSEC} thumb 0x40c8 0x31d1fbd1 # smlsldx	r3, r1, r1, r1
${ARMSEC} thumb 0x40ca 0x31d1fbd1 # smlsldx	r3, r1, r1, r1
${ARMSEC} thumb 0x40c8 0x21c1fbd2 # smlsld	r2, r1, r2, r1
${ARMSEC} thumb 0x40ca 0x21c1fbd2 # smlsld	r2, r1, r2, r1
${ARMSEC} thumb 0x40c8 0x12d1fbd2 # smlsldx	r1, r2, r2, r1
${ARMSEC} thumb 0x40ca 0x12d1fbd2 # smlsldx	r1, r2, r2, r1
${ARMSEC} thumb 0x40c8 0x13c2fbd2 # smlsld	r1, r3, r2, r2
${ARMSEC} thumb 0x40ca 0x13c2fbd2 # smlsld	r1, r3, r2, r2
${ARMSEC} thumb 0x40c8 0x12d1fbd0 # smlsldx	r1, r2, r0, r1
${ARMSEC} thumb 0x40ca 0x12d1fbd0 # smlsldx	r1, r2, r0, r1
${ARMSEC} thumb 0x40c8 0x12c2fbd3 # smlsld	r1, r2, r3, r2
${ARMSEC} thumb 0x40ca 0x12c2fbd3 # smlsld	r1, r2, r3, r2
${ARMSEC} thumb 0x40c8 0x13c2fbd1 # smlsld	r1, r3, r1, r2
${ARMSEC} thumb 0x40ca 0x13c2fbd1 # smlsld	r1, r3, r1, r2
${ARMSEC} thumb 0x40c8 0x10d3fbd0 # smlsldx	r1, r0, r0, r3
${ARMSEC} thumb 0x40ca 0x10d3fbd0 # smlsldx	r1, r0, r0, r3
${ARMSEC} thumb 0x40c8 0x13c2fbd0 # smlsld	r1, r3, r0, r2
${ARMSEC} thumb 0x40ca 0x13c2fbd0 # smlsld	r1, r3, r0, r2
${ARMSEC} thumb 0x40c8 0x2202fb52 # smmla	r2, r2, r2, r2
${ARMSEC} thumb 0x40ca 0x2202fb52 # smmla	r2, r2, r2, r2
${ARMSEC} thumb 0x40c8 0x310fb50 # smmlar	r3, r0, r0, r0
${ARMSEC} thumb 0x40ca 0x310fb50 # smmlar	r3, r0, r0, r0
${ARMSEC} thumb 0x40c8 0x2303fb52 # smmla	r3, r2, r3, r2
${ARMSEC} thumb 0x40ca 0x2303fb52 # smmla	r3, r2, r3, r2
${ARMSEC} thumb 0x40c8 0x1201fb52 # smmla	r2, r2, r1, r1
${ARMSEC} thumb 0x40ca 0x1201fb52 # smmla	r2, r2, r1, r1
${ARMSEC} thumb 0x40c8 0x3000fb50 # smmla	r0, r0, r0, r3
${ARMSEC} thumb 0x40ca 0x3000fb50 # smmla	r0, r0, r0, r3
${ARMSEC} thumb 0x40c8 0x2fb50 # smmla	r0, r0, r2, r0
${ARMSEC} thumb 0x40ca 0x2fb50 # smmla	r0, r0, r2, r0
${ARMSEC} thumb 0x40c8 0x3303fb50 # smmla	r3, r0, r3, r3
${ARMSEC} thumb 0x40ca 0x3303fb50 # smmla	r3, r0, r3, r3
${ARMSEC} thumb 0x40c8 0x2fb52 # smmla	r0, r2, r2, r0
${ARMSEC} thumb 0x40ca 0x2fb52 # smmla	r0, r2, r2, r0
${ARMSEC} thumb 0x40c8 0x312fb50 # smmlar	r3, r0, r2, r0
${ARMSEC} thumb 0x40ca 0x312fb50 # smmlar	r3, r0, r2, r0
${ARMSEC} thumb 0x40c8 0x1201fb50 # smmla	r2, r0, r1, r1
${ARMSEC} thumb 0x40ca 0x1201fb50 # smmla	r2, r0, r1, r1
${ARMSEC} thumb 0x40c8 0x103fb53 # smmla	r1, r3, r3, r0
${ARMSEC} thumb 0x40ca 0x103fb53 # smmla	r1, r3, r3, r0
${ARMSEC} thumb 0x40c8 0x1212fb50 # smmlar	r2, r0, r2, r1
${ARMSEC} thumb 0x40ca 0x1212fb50 # smmlar	r2, r0, r2, r1
${ARMSEC} thumb 0x40c8 0x213fb52 # smmlar	r2, r2, r3, r0
${ARMSEC} thumb 0x40ca 0x213fb52 # smmlar	r2, r2, r3, r0
${ARMSEC} thumb 0x40c8 0x1102fb53 # smmla	r1, r3, r2, r1
${ARMSEC} thumb 0x40ca 0x1102fb53 # smmla	r1, r3, r2, r1
${ARMSEC} thumb 0x40c8 0x2013fb51 # smmlar	r0, r1, r3, r2
${ARMSEC} thumb 0x40ca 0x2013fb51 # smmlar	r0, r1, r3, r2
${ARMSEC} thumb 0x40c8 0x1101fb61 # smmls	r1, r1, r1, r1
${ARMSEC} thumb 0x40ca 0x1101fb61 # smmls	r1, r1, r1, r1
${ARMSEC} thumb 0x40c8 0x3113fb63 # smmlsr	r1, r3, r3, r3
${ARMSEC} thumb 0x40ca 0x3113fb63 # smmlsr	r1, r3, r3, r3
${ARMSEC} thumb 0x40c8 0x2303fb62 # smmls	r3, r2, r3, r2
${ARMSEC} thumb 0x40ca 0x2303fb62 # smmls	r3, r2, r3, r2
${ARMSEC} thumb 0x40c8 0x200fb62 # smmls	r2, r2, r0, r0
${ARMSEC} thumb 0x40ca 0x200fb62 # smmls	r2, r2, r0, r0
${ARMSEC} thumb 0x40c8 0x3000fb60 # smmls	r0, r0, r0, r3
${ARMSEC} thumb 0x40ca 0x3000fb60 # smmls	r0, r0, r0, r3
${ARMSEC} thumb 0x40c8 0x1112fb61 # smmlsr	r1, r1, r2, r1
${ARMSEC} thumb 0x40ca 0x1112fb61 # smmlsr	r1, r1, r2, r1
${ARMSEC} thumb 0x40c8 0x10fb63 # smmlsr	r0, r3, r0, r0
${ARMSEC} thumb 0x40ca 0x10fb63 # smmlsr	r0, r3, r0, r0
${ARMSEC} thumb 0x40c8 0x2201fb61 # smmls	r2, r1, r1, r2
${ARMSEC} thumb 0x40ca 0x2201fb61 # smmls	r2, r1, r1, r2
${ARMSEC} thumb 0x40c8 0x203fb60 # smmls	r2, r0, r3, r0
${ARMSEC} thumb 0x40ca 0x203fb60 # smmls	r2, r0, r3, r0
${ARMSEC} thumb 0x40c8 0x2302fb61 # smmls	r3, r1, r2, r2
${ARMSEC} thumb 0x40ca 0x2302fb61 # smmls	r3, r1, r2, r2
${ARMSEC} thumb 0x40c8 0x2001fb61 # smmls	r0, r1, r1, r2
${ARMSEC} thumb 0x40ca 0x2001fb61 # smmls	r0, r1, r1, r2
${ARMSEC} thumb 0x40c8 0x2111fb60 # smmlsr	r1, r0, r1, r2
${ARMSEC} thumb 0x40ca 0x2111fb60 # smmlsr	r1, r0, r1, r2
${ARMSEC} thumb 0x40c8 0x1213fb62 # smmlsr	r2, r2, r3, r1
${ARMSEC} thumb 0x40ca 0x1213fb62 # smmlsr	r2, r2, r3, r1
${ARMSEC} thumb 0x40c8 0x1112fb63 # smmlsr	r1, r3, r2, r1
${ARMSEC} thumb 0x40ca 0x1112fb63 # smmlsr	r1, r3, r2, r1
${ARMSEC} thumb 0x40c8 0x3112fb60 # smmlsr	r1, r0, r2, r3
${ARMSEC} thumb 0x40ca 0x3112fb60 # smmlsr	r1, r0, r2, r3
${ARMSEC} thumb 0x40c8 0xf111fb51 # smmulr	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf111fb51 # smmulr	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf311fb51 # smmulr	r3, r1, r1
${ARMSEC} thumb 0x40ca 0xf311fb51 # smmulr	r3, r1, r1
${ARMSEC} thumb 0x40c8 0xf313fb52 # smmulr	r3, r2, r3
${ARMSEC} thumb 0x40ca 0xf313fb52 # smmulr	r3, r2, r3
${ARMSEC} thumb 0x40c8 0xf200fb52 # smmul	r2, r2, r0
${ARMSEC} thumb 0x40ca 0xf200fb52 # smmul	r2, r2, r0
${ARMSEC} thumb 0x40c8 0xf103fb52 # smmul	r1, r2, r3
${ARMSEC} thumb 0x40ca 0xf103fb52 # smmul	r1, r2, r3
${ARMSEC} thumb 0x40c8 0xf303fb23 # smuad	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf303fb23 # smuad	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf213fb23 # smuadx	r2, r3, r3
${ARMSEC} thumb 0x40ca 0xf213fb23 # smuadx	r2, r3, r3
${ARMSEC} thumb 0x40c8 0xf202fb20 # smuad	r2, r0, r2
${ARMSEC} thumb 0x40ca 0xf202fb20 # smuad	r2, r0, r2
${ARMSEC} thumb 0x40c8 0xf012fb20 # smuadx	r0, r0, r2
${ARMSEC} thumb 0x40ca 0xf012fb20 # smuadx	r0, r0, r2
${ARMSEC} thumb 0x40c8 0xf310fb22 # smuadx	r3, r2, r0
${ARMSEC} thumb 0x40ca 0xf310fb22 # smuadx	r3, r2, r0
${ARMSEC} thumb 0x40c8 0x3203fb83 # smull	r3, r2, r3, r3
${ARMSEC} thumb 0x40ca 0x3203fb83 # smull	r3, r2, r3, r3
${ARMSEC} thumb 0x40c8 0x1202fb82 # smull	r1, r2, r2, r2
${ARMSEC} thumb 0x40ca 0x1202fb82 # smull	r1, r2, r2, r2
${ARMSEC} thumb 0x40c8 0x3203fb82 # smull	r3, r2, r2, r3
${ARMSEC} thumb 0x40ca 0x3203fb82 # smull	r3, r2, r2, r3
${ARMSEC} thumb 0x40c8 0x1202fb81 # smull	r1, r2, r1, r2
${ARMSEC} thumb 0x40ca 0x1202fb81 # smull	r1, r2, r1, r2
${ARMSEC} thumb 0x40c8 0x3002fb82 # smull	r3, r0, r2, r2
${ARMSEC} thumb 0x40ca 0x3002fb82 # smull	r3, r0, r2, r2
${ARMSEC} thumb 0x40c8 0x1300fb81 # smull	r1, r3, r1, r0
${ARMSEC} thumb 0x40ca 0x1300fb81 # smull	r1, r3, r1, r0
${ARMSEC} thumb 0x40c8 0x2301fb83 # smull	r2, r3, r3, r1
${ARMSEC} thumb 0x40ca 0x2301fb83 # smull	r2, r3, r3, r1
${ARMSEC} thumb 0x40c8 0x100fb83 # smull	r0, r1, r3, r0
${ARMSEC} thumb 0x40ca 0x100fb83 # smull	r0, r1, r3, r0
${ARMSEC} thumb 0x40c8 0x2000fb81 # smull	r2, r0, r1, r0
${ARMSEC} thumb 0x40ca 0x2000fb81 # smull	r2, r0, r1, r0
${ARMSEC} thumb 0x40c8 0x3100fb82 # smull	r3, r1, r2, r0
${ARMSEC} thumb 0x40ca 0x3100fb82 # smull	r3, r1, r2, r0
${ARMSEC} thumb 0x40c8 0xf212fb32 # smulwt	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf212fb32 # smulwt	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf013fb33 # smulwt	r0, r3, r3
${ARMSEC} thumb 0x40ca 0xf013fb33 # smulwt	r0, r3, r3
${ARMSEC} thumb 0x40c8 0xf202fb33 # smulwb	r2, r3, r2
${ARMSEC} thumb 0x40ca 0xf202fb33 # smulwb	r2, r3, r2
${ARMSEC} thumb 0x40c8 0xf203fb32 # smulwb	r2, r2, r3
${ARMSEC} thumb 0x40ca 0xf203fb32 # smulwb	r2, r2, r3
${ARMSEC} thumb 0x40c8 0xf302fb30 # smulwb	r3, r0, r2
${ARMSEC} thumb 0x40ca 0xf302fb30 # smulwb	r3, r0, r2
${ARMSEC} thumb 0x40c8 0xf000fb10 # smulbb	r0, r0, r0
${ARMSEC} thumb 0x40ca 0xf000fb10 # smulbb	r0, r0, r0
${ARMSEC} thumb 0x40c8 0xf311fb11 # smulbt	r3, r1, r1
${ARMSEC} thumb 0x40ca 0xf311fb11 # smulbt	r3, r1, r1
${ARMSEC} thumb 0x40c8 0xf303fb12 # smulbb	r3, r2, r3
${ARMSEC} thumb 0x40ca 0xf303fb12 # smulbb	r3, r2, r3
${ARMSEC} thumb 0x40c8 0xf311fb13 # smulbt	r3, r3, r1
${ARMSEC} thumb 0x40ca 0xf311fb13 # smulbt	r3, r3, r1
${ARMSEC} thumb 0x40c8 0xf022fb11 # smultb	r0, r1, r2
${ARMSEC} thumb 0x40ca 0xf022fb11 # smultb	r0, r1, r2
${ARMSEC} thumb 0x40c8 0xf212fb42 # smusdx	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf212fb42 # smusdx	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf102fb42 # smusd	r1, r2, r2
${ARMSEC} thumb 0x40ca 0xf102fb42 # smusd	r1, r2, r2
${ARMSEC} thumb 0x40c8 0xf010fb43 # smusdx	r0, r3, r0
${ARMSEC} thumb 0x40ca 0xf010fb43 # smusdx	r0, r3, r0
${ARMSEC} thumb 0x40c8 0xf100fb41 # smusd	r1, r1, r0
${ARMSEC} thumb 0x40ca 0xf100fb41 # smusd	r1, r1, r0
${ARMSEC} thumb 0x40c8 0xf002fb43 # smusd	r0, r3, r2
${ARMSEC} thumb 0x40ca 0xf002fb43 # smusd	r0, r3, r2
${ARMSEC} thumb 0x40c8 0x239df303 # ssat	r3, #29, r3, lsl #10
${ARMSEC} thumb 0x40ca 0x239df303 # ssat	r3, #29, r3, lsl #10
${ARMSEC} thumb 0x40c8 0x738af320 # ssat	r3, #10, r0, asr #30
${ARMSEC} thumb 0x40ca 0x738af320 # ssat	r3, #10, r0, asr #30
${ARMSEC} thumb 0x40c8 0x20bf322 # ssat16	r2, #11, r2
${ARMSEC} thumb 0x40ca 0x20bf322 # ssat16	r2, #11, r2
${ARMSEC} thumb 0x40c8 0x5f322 # ssat16	r0, #5, r2
${ARMSEC} thumb 0x40ca 0x5f322 # ssat16	r0, #5, r2
${ARMSEC} thumb 0x40c8 0xf202fae2 # ssax	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf202fae2 # ssax	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf300fae0 # ssax	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf300fae0 # ssax	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf101fae0 # ssax	r1, r0, r1
${ARMSEC} thumb 0x40ca 0xf101fae0 # ssax	r1, r0, r1
${ARMSEC} thumb 0x40c8 0xf301fae3 # ssax	r3, r3, r1
${ARMSEC} thumb 0x40ca 0xf301fae3 # ssax	r3, r3, r1
${ARMSEC} thumb 0x40c8 0xf003fae2 # ssax	r0, r2, r3
${ARMSEC} thumb 0x40ca 0xf003fae2 # ssax	r0, r2, r3
${ARMSEC} thumb 0x40c8 0xf202fad2 # ssub16	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf202fad2 # ssub16	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf201fad1 # ssub16	r2, r1, r1
${ARMSEC} thumb 0x40ca 0xf201fad1 # ssub16	r2, r1, r1
${ARMSEC} thumb 0x40c8 0xf000fad1 # ssub16	r0, r1, r0
${ARMSEC} thumb 0x40ca 0xf000fad1 # ssub16	r0, r1, r0
${ARMSEC} thumb 0x40c8 0xf201fad2 # ssub16	r2, r2, r1
${ARMSEC} thumb 0x40ca 0xf201fad2 # ssub16	r2, r2, r1
${ARMSEC} thumb 0x40c8 0xf102fad3 # ssub16	r1, r3, r2
${ARMSEC} thumb 0x40ca 0xf102fad3 # ssub16	r1, r3, r2
${ARMSEC} thumb 0x40c8 0xf303fac3 # ssub8	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf303fac3 # ssub8	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf300fac0 # ssub8	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf300fac0 # ssub8	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf202fac0 # ssub8	r2, r0, r2
${ARMSEC} thumb 0x40ca 0xf202fac0 # ssub8	r2, r0, r2
${ARMSEC} thumb 0x40c8 0xf200fac2 # ssub8	r2, r2, r0
${ARMSEC} thumb 0x40ca 0xf200fac2 # ssub8	r2, r2, r0
${ARMSEC} thumb 0x40c8 0xf203fac0 # ssub8	r2, r0, r3
${ARMSEC} thumb 0x40ca 0xf203fac0 # ssub8	r2, r0, r3
${ARMSEC} thumb 0x40c8 0x6e902 # stmdb	r2, {r1, r2}
${ARMSEC} thumb 0x40ca 0x6e902 # stmdb	r2, {r1, r2}
${ARMSEC} thumb 0x40c8 0x6e901 # stmdb	r1, {r1, r2}
${ARMSEC} thumb 0x40ca 0x6e901 # stmdb	r1, {r1, r2}
${ARMSEC} thumb 0x40c8 0xae900 # stmdb	r0, {r1, r3}
${ARMSEC} thumb 0x40ca 0xae900 # stmdb	r0, {r1, r3}
${ARMSEC} thumb 0x40c8 0xde903 # stmdb	r3, {r0, r2, r3}
${ARMSEC} thumb 0x40ca 0xde903 # stmdb	r3, {r0, r2, r3}
${ARMSEC} thumb 0x40c8 0xbe901 # stmdb	r1, {r0, r1, r3}
${ARMSEC} thumb 0x40ca 0xbe901 # stmdb	r1, {r0, r1, r3}
${ARMSEC} thumb 0x40c8 0xde900 # stmdb	r0, {r0, r2, r3}
${ARMSEC} thumb 0x40ca 0xde900 # stmdb	r0, {r0, r2, r3}
${ARMSEC} thumb 0x40c8 0xae922 # stmdb	r2!, {r1, r3}
${ARMSEC} thumb 0x40ca 0xae922 # stmdb	r2!, {r1, r3}
${ARMSEC} thumb 0x40c8 0x7e903 # stmdb	r3, {r0, r1, r2}
${ARMSEC} thumb 0x40ca 0x7e903 # stmdb	r3, {r0, r1, r2}
${ARMSEC} thumb 0x40c8 0xfe903 # stmdb	r3, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe903 # stmdb	r3, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe901 # stmdb	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe901 # stmdb	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe900 # stmdb	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe900 # stmdb	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xde921 # stmdb	r1!, {r0, r2, r3}
${ARMSEC} thumb 0x40ca 0xde921 # stmdb	r1!, {r0, r2, r3}
${ARMSEC} thumb 0x40c8 0xc102 # stmia	r1!, {r1}
${ARMSEC} thumb 0x40ca 0xc102 # stmia	r1!, {r1}
${ARMSEC} thumb 0x40c8 0xc108 # stmia	r1!, {r3}
${ARMSEC} thumb 0x40ca 0xc108 # stmia	r1!, {r3}
${ARMSEC} thumb 0x40c8 0xc005 # stmia	r0!, {r0, r2}
${ARMSEC} thumb 0x40ca 0xc005 # stmia	r0!, {r0, r2}
${ARMSEC} thumb 0x40c8 0xc105 # stmia	r1!, {r0, r2}
${ARMSEC} thumb 0x40ca 0xc105 # stmia	r1!, {r0, r2}
${ARMSEC} thumb 0x40c8 0xc10e # stmia	r1!, {r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xc10e # stmia	r1!, {r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xc00e # stmia	r0!, {r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xc00e # stmia	r0!, {r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xc00f # stmia	r0!, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xc00f # stmia	r0!, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0x6e881 # stmia.w	r1, {r1, r2}
${ARMSEC} thumb 0x40ca 0x6e881 # stmia.w	r1, {r1, r2}
${ARMSEC} thumb 0x40c8 0xce883 # stmia.w	r3, {r2, r3}
${ARMSEC} thumb 0x40ca 0xce883 # stmia.w	r3, {r2, r3}
${ARMSEC} thumb 0x40c8 0xae880 # stmia.w	r0, {r1, r3}
${ARMSEC} thumb 0x40ca 0xae880 # stmia.w	r0, {r1, r3}
${ARMSEC} thumb 0x40c8 0xde880 # stmia.w	r0, {r0, r2, r3}
${ARMSEC} thumb 0x40ca 0xde880 # stmia.w	r0, {r0, r2, r3}
${ARMSEC} thumb 0x40c8 0xee882 # stmia.w	r2, {r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xee882 # stmia.w	r2, {r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xbe883 # stmia.w	r3, {r0, r1, r3}
${ARMSEC} thumb 0x40ca 0xbe883 # stmia.w	r3, {r0, r1, r3}
${ARMSEC} thumb 0x40c8 0xce8a0 # stmia.w	r0!, {r2, r3}
${ARMSEC} thumb 0x40ca 0xce8a0 # stmia.w	r0!, {r2, r3}
${ARMSEC} thumb 0x40c8 0xbe882 # stmia.w	r2, {r0, r1, r3}
${ARMSEC} thumb 0x40ca 0xbe882 # stmia.w	r2, {r0, r1, r3}
${ARMSEC} thumb 0x40c8 0xfe880 # stmia.w	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe880 # stmia.w	r0, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe881 # stmia.w	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe881 # stmia.w	r1, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe882 # stmia.w	r2, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe882 # stmia.w	r2, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0xfe883 # stmia.w	r3, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40ca 0xfe883 # stmia.w	r3, {r0, r1, r2, r3}
${ARMSEC} thumb 0x40c8 0x7e8a3 # stmia.w	r3!, {r0, r1, r2}
${ARMSEC} thumb 0x40ca 0x7e8a3 # stmia.w	r3!, {r0, r1, r2}
${ARMSEC} thumb 0x40c8 0x6352 # str	r2, [r2, #52]
${ARMSEC} thumb 0x40ca 0x6352 # str	r2, [r2, #52]
${ARMSEC} thumb 0x40c8 0x6683 # str	r3, [r0, #104]
${ARMSEC} thumb 0x40ca 0x6683 # str	r3, [r0, #104]
${ARMSEC} thumb 0x40c8 0x5082 # str	r2, [r0, r2]
${ARMSEC} thumb 0x40ca 0x5082 # str	r2, [r0, r2]
${ARMSEC} thumb 0x40c8 0x5012 # str	r2, [r2, r0]
${ARMSEC} thumb 0x40ca 0x5012 # str	r2, [r2, r0]
${ARMSEC} thumb 0x40c8 0x5099 # str	r1, [r3, r2]
${ARMSEC} thumb 0x40ca 0x5099 # str	r1, [r3, r2]
${ARMSEC} thumb 0x40c8 0x2c9ff842 # str.w	r2, [r2, #-159]
${ARMSEC} thumb 0x40ca 0x2c9ff842 # str.w	r2, [r2, #-159]
${ARMSEC} thumb 0x40c8 0x2c97f841 # str.w	r2, [r1, #-151]
${ARMSEC} thumb 0x40ca 0x2c97f841 # str.w	r2, [r1, #-151]
${ARMSEC} thumb 0x40c8 0x2ddaf841 # str.w	r2, [r1, #-218]!
${ARMSEC} thumb 0x40ca 0x2ddaf841 # str.w	r2, [r1, #-218]!
${ARMSEC} thumb 0x40c8 0xb72f8c0 # str.w	r0, [r0, #2930]
${ARMSEC} thumb 0x40ca 0xb72f8c0 # str.w	r0, [r0, #2930]
${ARMSEC} thumb 0x40c8 0x3cd7f8c1 # str.w	r3, [r1, #3287]
${ARMSEC} thumb 0x40ca 0x3cd7f8c1 # str.w	r3, [r1, #3287]
${ARMSEC} thumb 0x40c8 0x30f842 # str.w	r0, [r2, r0, lsl #3]
${ARMSEC} thumb 0x40ca 0x30f842 # str.w	r0, [r2, r0, lsl #3]
${ARMSEC} thumb 0x40c8 0x3031f843 # str.w	r3, [r3, r1, lsl #3]
${ARMSEC} thumb 0x40ca 0x3031f843 # str.w	r3, [r3, r1, lsl #3]
${ARMSEC} thumb 0x40c8 0x1f842 # str.w	r0, [r2, r1]
${ARMSEC} thumb 0x40ca 0x1f842 # str.w	r0, [r2, r1]
${ARMSEC} thumb 0x40c8 0x73d2 # strb	r2, [r2, #15]
${ARMSEC} thumb 0x40ca 0x73d2 # strb	r2, [r2, #15]
${ARMSEC} thumb 0x40c8 0x74d8 # strb	r0, [r3, #19]
${ARMSEC} thumb 0x40ca 0x74d8 # strb	r0, [r3, #19]
${ARMSEC} thumb 0x40c8 0x5408 # strb	r0, [r1, r0]
${ARMSEC} thumb 0x40ca 0x5408 # strb	r0, [r1, r0]
${ARMSEC} thumb 0x40c8 0x5480 # strb	r0, [r0, r2]
${ARMSEC} thumb 0x40ca 0x5480 # strb	r0, [r0, r2]
${ARMSEC} thumb 0x40c8 0x540b # strb	r3, [r1, r0]
${ARMSEC} thumb 0x40ca 0x540b # strb	r3, [r1, r0]
${ARMSEC} thumb 0x40c8 0x3c80f803 # strb.w	r3, [r3, #-128]
${ARMSEC} thumb 0x40ca 0x3c80f803 # strb.w	r3, [r3, #-128]
${ARMSEC} thumb 0x40c8 0x1cd3f802 # strb.w	r1, [r2, #-211]
${ARMSEC} thumb 0x40ca 0x1cd3f802 # strb.w	r1, [r2, #-211]
${ARMSEC} thumb 0x40c8 0x996f803 # strb.w	r0, [r3], #-150
${ARMSEC} thumb 0x40ca 0x996f803 # strb.w	r0, [r3], #-150
${ARMSEC} thumb 0x40c8 0x15f1f881 # strb.w	r1, [r1, #1521]
${ARMSEC} thumb 0x40ca 0x15f1f881 # strb.w	r1, [r1, #1521]
${ARMSEC} thumb 0x40c8 0xb8af883 # strb.w	r0, [r3, #2954]
${ARMSEC} thumb 0x40ca 0xb8af883 # strb.w	r0, [r3, #2954]
${ARMSEC} thumb 0x40c8 0x2032f801 # strb.w	r2, [r1, r2, lsl #3]
${ARMSEC} thumb 0x40ca 0x2032f801 # strb.w	r2, [r1, r2, lsl #3]
${ARMSEC} thumb 0x40c8 0x1010f801 # strb.w	r1, [r1, r0, lsl #1]
${ARMSEC} thumb 0x40ca 0x1010f801 # strb.w	r1, [r1, r0, lsl #1]
${ARMSEC} thumb 0x40c8 0x1022f800 # strb.w	r1, [r0, r2, lsl #2]
${ARMSEC} thumb 0x40ca 0x1022f800 # strb.w	r1, [r0, r2, lsl #2]
${ARMSEC} thumb 0x40c8 0x3eacf803 # strbt	r3, [r3, #172]
${ARMSEC} thumb 0x40ca 0x3eacf803 # strbt	r3, [r3, #172]
${ARMSEC} thumb 0x40c8 0xe82f802 # strbt	r0, [r2, #130]
${ARMSEC} thumb 0x40ca 0xe82f802 # strbt	r0, [r2, #130]
${ARMSEC} thumb 0x40c8 0x2227e9c2 # strd	r2, r2, [r2, #156]
${ARMSEC} thumb 0x40ca 0x2227e9c2 # strd	r2, r2, [r2, #156]
${ARMSEC} thumb 0x40c8 0x1384e941 # strd	r1, r3, [r1, #-528]
${ARMSEC} thumb 0x40ca 0x1384e941 # strd	r1, r3, [r1, #-528]
${ARMSEC} thumb 0x40c8 0x3ade9c3 # strd	r0, r3, [r3, #692]
${ARMSEC} thumb 0x40ca 0x3ade9c3 # strd	r0, r3, [r3, #692]
${ARMSEC} thumb 0x40c8 0x33a2e9c2 # strd	r3, r3, [r2, #648]
${ARMSEC} thumb 0x40ca 0x33a2e9c2 # strd	r3, r3, [r2, #648]
${ARMSEC} thumb 0x40c8 0x1217e943 # strd	r1, r2, [r3, #-92]
${ARMSEC} thumb 0x40ca 0x1217e943 # strd	r1, r2, [r3, #-92]
${ARMSEC} thumb 0x40c8 0x228fe8e3 # strd	r2, r2, [r3], #572
${ARMSEC} thumb 0x40ca 0x228fe8e3 # strd	r2, r2, [r3], #572
${ARMSEC} thumb 0x40c8 0x135ae862 # strd	r1, r3, [r2], #-360
${ARMSEC} thumb 0x40ca 0x135ae862 # strd	r1, r3, [r2], #-360
${ARMSEC} thumb 0x40c8 0x849b # strh	r3, [r3, #36]
${ARMSEC} thumb 0x40ca 0x849b # strh	r3, [r3, #36]
${ARMSEC} thumb 0x40c8 0x8293 # strh	r3, [r2, #20]
${ARMSEC} thumb 0x40ca 0x8293 # strh	r3, [r2, #20]
${ARMSEC} thumb 0x40c8 0x5251 # strh	r1, [r2, r1]
${ARMSEC} thumb 0x40ca 0x5251 # strh	r1, [r2, r1]
${ARMSEC} thumb 0x40c8 0x5280 # strh	r0, [r0, r2]
${ARMSEC} thumb 0x40ca 0x5280 # strh	r0, [r0, r2]
${ARMSEC} thumb 0x40c8 0x52ca # strh	r2, [r1, r3]
${ARMSEC} thumb 0x40ca 0x52ca # strh	r2, [r1, r3]
${ARMSEC} thumb 0x40c8 0x1c22f821 # strh.w	r1, [r1, #-34]
${ARMSEC} thumb 0x40ca 0x1c22f821 # strh.w	r1, [r1, #-34]
${ARMSEC} thumb 0x40c8 0xca6f823 # strh.w	r0, [r3, #-166]
${ARMSEC} thumb 0x40ca 0xca6f823 # strh.w	r0, [r3, #-166]
${ARMSEC} thumb 0x40c8 0x1bf6f820 # strh.w	r1, [r0], #246
${ARMSEC} thumb 0x40ca 0x1bf6f820 # strh.w	r1, [r0], #246
${ARMSEC} thumb 0x40c8 0x136ef8a1 # strh.w	r1, [r1, #878]
${ARMSEC} thumb 0x40ca 0x136ef8a1 # strh.w	r1, [r1, #878]
${ARMSEC} thumb 0x40c8 0xbe1f8a3 # strh.w	r0, [r3, #3041]
${ARMSEC} thumb 0x40ca 0xbe1f8a3 # strh.w	r0, [r3, #3041]
${ARMSEC} thumb 0x40c8 0x1031f823 # strh.w	r1, [r3, r1, lsl #3]
${ARMSEC} thumb 0x40ca 0x1031f823 # strh.w	r1, [r3, r1, lsl #3]
${ARMSEC} thumb 0x40c8 0x1032f821 # strh.w	r1, [r1, r2, lsl #3]
${ARMSEC} thumb 0x40ca 0x1032f821 # strh.w	r1, [r1, r2, lsl #3]
${ARMSEC} thumb 0x40c8 0x2031f820 # strh.w	r2, [r0, r1, lsl #3]
${ARMSEC} thumb 0x40ca 0x2031f820 # strh.w	r2, [r0, r1, lsl #3]
${ARMSEC} thumb 0x40c8 0x1e7ff821 # strht	r1, [r1, #127]
${ARMSEC} thumb 0x40ca 0x1e7ff821 # strht	r1, [r1, #127]
${ARMSEC} thumb 0x40c8 0x2ebdf823 # strht	r2, [r3, #189]
${ARMSEC} thumb 0x40ca 0x2ebdf823 # strht	r2, [r3, #189]
${ARMSEC} thumb 0x40c8 0x3ebef843 # strt	r3, [r3, #190]
${ARMSEC} thumb 0x40ca 0x3ebef843 # strt	r3, [r3, #190]
${ARMSEC} thumb 0x40c8 0x2e2bf843 # strt	r2, [r3, #43]
${ARMSEC} thumb 0x40ca 0x2e2bf843 # strt	r2, [r3, #43]
${ARMSEC} thumb 0x40c8 0x1f40 # subs	r0, r0, #5
${ARMSEC} thumb 0x40ca 0x1f40 # subs	r0, r0, #5
${ARMSEC} thumb 0x40c8 0x1f5a # subs	r2, r3, #5
${ARMSEC} thumb 0x40ca 0x1f5a # subs	r2, r3, #5
${ARMSEC} thumb 0x40c8 0x39cd # subs	r1, #205
${ARMSEC} thumb 0x40ca 0x39cd # subs	r1, #205
${ARMSEC} thumb 0x40c8 0x1adb # subs	r3, r3, r3
${ARMSEC} thumb 0x40ca 0x1adb # subs	r3, r3, r3
${ARMSEC} thumb 0x40c8 0x1a03 # subs	r3, r0, r0
${ARMSEC} thumb 0x40ca 0x1a03 # subs	r3, r0, r0
${ARMSEC} thumb 0x40c8 0x1a10 # subs	r0, r2, r0
${ARMSEC} thumb 0x40ca 0x1a10 # subs	r0, r2, r0
${ARMSEC} thumb 0x40c8 0x1a1b # subs	r3, r3, r0
${ARMSEC} thumb 0x40ca 0x1a1b # subs	r3, r3, r0
${ARMSEC} thumb 0x40c8 0x1a98 # subs	r0, r3, r2
${ARMSEC} thumb 0x40ca 0x1a98 # subs	r0, r3, r2
${ARMSEC} thumb 0x40c8 0x51c1eba1 # sub.w	r1, r1, r1, lsl #23
${ARMSEC} thumb 0x40ca 0x51c1eba1 # sub.w	r1, r1, r1, lsl #23
${ARMSEC} thumb 0x40c8 0x3050ebb0 # subs.w	r0, r0, r0, lsr #13
${ARMSEC} thumb 0x40ca 0x3050ebb0 # subs.w	r0, r0, r0, lsr #13
${ARMSEC} thumb 0x40c8 0x6332eba2 # sub.w	r3, r2, r2, ror #24
${ARMSEC} thumb 0x40ca 0x6332eba2 # sub.w	r3, r2, r2, ror #24
${ARMSEC} thumb 0x40c8 0x2123ebb3 # subs.w	r1, r3, r3, asr #8
${ARMSEC} thumb 0x40ca 0x2123ebb3 # subs.w	r1, r3, r3, asr #8
${ARMSEC} thumb 0x40c8 0x72b2eba0 # sub.w	r2, r0, r2, ror #30
${ARMSEC} thumb 0x40ca 0x72b2eba0 # sub.w	r2, r0, r2, ror #30
${ARMSEC} thumb 0x40c8 0x2030ebb3 # subs.w	r0, r3, r0, ror #8
${ARMSEC} thumb 0x40ca 0x2030ebb3 # subs.w	r0, r3, r0, ror #8
${ARMSEC} thumb 0x40c8 0x61d3eba1 # sub.w	r1, r1, r3, lsr #27
${ARMSEC} thumb 0x40ca 0x61d3eba1 # sub.w	r1, r1, r3, lsr #27
${ARMSEC} thumb 0x40c8 0x1372ebb3 # subs.w	r3, r3, r2, ror #5
${ARMSEC} thumb 0x40ca 0x1372ebb3 # subs.w	r3, r3, r2, ror #5
${ARMSEC} thumb 0x40c8 0x2240eba3 # sub.w	r2, r3, r0, lsl #9
${ARMSEC} thumb 0x40ca 0x2240eba3 # sub.w	r2, r3, r0, lsl #9
${ARMSEC} thumb 0x40c8 0x3ebb2 # subs.w	r0, r2, r3
${ARMSEC} thumb 0x40ca 0x3ebb2 # subs.w	r0, r2, r3
${ARMSEC} thumb 0x40c8 0x20aaf1a0 # sub.w	r0, r0, #-1442797056
${ARMSEC} thumb 0x40ca 0x20aaf1a0 # sub.w	r0, r0, #-1442797056
${ARMSEC} thumb 0x40c8 0x2389f1b3 # subs.w	r3, r3, #-1996453632
${ARMSEC} thumb 0x40ca 0x2389f1b3 # subs.w	r3, r3, #-1996453632
${ARMSEC} thumb 0x40c8 0x732ef1a0 # sub.w	r3, r0, #45613056
${ARMSEC} thumb 0x40ca 0x732ef1a0 # sub.w	r3, r0, #45613056
${ARMSEC} thumb 0x40c8 0x290f5b1 # subs.w	r2, r1, #4718592
${ARMSEC} thumb 0x40ca 0x290f5b1 # subs.w	r2, r1, #4718592
${ARMSEC} thumb 0x40c8 0x1272f2a2 # subw	r2, r2, #370
${ARMSEC} thumb 0x40ca 0x1272f2a2 # subw	r2, r2, #370
${ARMSEC} thumb 0x40c8 0x235ef2a2 # subw	r3, r2, #606
${ARMSEC} thumb 0x40ca 0x235ef2a2 # subw	r3, r2, #606
${ARMSEC} thumb 0x40c8 0xf282fa42 # sxtab	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf282fa42 # sxtab	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf2b3fa43 # sxtab	r2, r3, r3, ror #24
${ARMSEC} thumb 0x40ca 0xf2b3fa43 # sxtab	r2, r3, r3, ror #24
${ARMSEC} thumb 0x40c8 0xf181fa40 # sxtab	r1, r0, r1
${ARMSEC} thumb 0x40ca 0xf181fa40 # sxtab	r1, r0, r1
${ARMSEC} thumb 0x40c8 0xf1a3fa41 # sxtab	r1, r1, r3, ror #16
${ARMSEC} thumb 0x40ca 0xf1a3fa41 # sxtab	r1, r1, r3, ror #16
${ARMSEC} thumb 0x40c8 0xf081fa43 # sxtab	r0, r3, r1
${ARMSEC} thumb 0x40ca 0xf081fa43 # sxtab	r0, r3, r1
${ARMSEC} thumb 0x40c8 0xf3b3fa23 # sxtab16	r3, r3, r3, ror #24
${ARMSEC} thumb 0x40ca 0xf3b3fa23 # sxtab16	r3, r3, r3, ror #24
${ARMSEC} thumb 0x40c8 0xf1b3fa23 # sxtab16	r1, r3, r3, ror #24
${ARMSEC} thumb 0x40ca 0xf1b3fa23 # sxtab16	r1, r3, r3, ror #24
${ARMSEC} thumb 0x40c8 0xf282fa21 # sxtab16	r2, r1, r2
${ARMSEC} thumb 0x40ca 0xf282fa21 # sxtab16	r2, r1, r2
${ARMSEC} thumb 0x40c8 0xf2b3fa22 # sxtab16	r2, r2, r3, ror #24
${ARMSEC} thumb 0x40ca 0xf2b3fa22 # sxtab16	r2, r2, r3, ror #24
${ARMSEC} thumb 0x40c8 0xf2b0fa23 # sxtab16	r2, r3, r0, ror #24
${ARMSEC} thumb 0x40ca 0xf2b0fa23 # sxtab16	r2, r3, r0, ror #24
${ARMSEC} thumb 0x40c8 0xf3b3fa03 # sxtah	r3, r3, r3, ror #24
${ARMSEC} thumb 0x40ca 0xf3b3fa03 # sxtah	r3, r3, r3, ror #24
${ARMSEC} thumb 0x40c8 0xf082fa02 # sxtah	r0, r2, r2
${ARMSEC} thumb 0x40ca 0xf082fa02 # sxtah	r0, r2, r2
${ARMSEC} thumb 0x40c8 0xf282fa03 # sxtah	r2, r3, r2
${ARMSEC} thumb 0x40ca 0xf282fa03 # sxtah	r2, r3, r2
${ARMSEC} thumb 0x40c8 0xf081fa00 # sxtah	r0, r0, r1
${ARMSEC} thumb 0x40ca 0xf081fa00 # sxtah	r0, r0, r1
${ARMSEC} thumb 0x40c8 0xf091fa03 # sxtah	r0, r3, r1, ror #8
${ARMSEC} thumb 0x40ca 0xf091fa03 # sxtah	r0, r3, r1, ror #8
${ARMSEC} thumb 0x40c8 0xb240 # sxtb	r0, r0
${ARMSEC} thumb 0x40ca 0xb240 # sxtb	r0, r0
${ARMSEC} thumb 0x40c8 0xb241 # sxtb	r1, r0
${ARMSEC} thumb 0x40ca 0xb241 # sxtb	r1, r0
${ARMSEC} thumb 0x40c8 0xf1b1fa2f # sxtb16	r1, r1, ror #24
${ARMSEC} thumb 0x40ca 0xf1b1fa2f # sxtb16	r1, r1, ror #24
${ARMSEC} thumb 0x40c8 0xf1a3fa2f # sxtb16	r1, r3, ror #16
${ARMSEC} thumb 0x40ca 0xf1a3fa2f # sxtb16	r1, r3, ror #16
${ARMSEC} thumb 0x40c8 0xf3b3fa4f # sxtb.w	r3, r3, ror #24
${ARMSEC} thumb 0x40ca 0xf3b3fa4f # sxtb.w	r3, r3, ror #24
${ARMSEC} thumb 0x40c8 0xf3a1fa4f # sxtb.w	r3, r1, ror #16
${ARMSEC} thumb 0x40ca 0xf3a1fa4f # sxtb.w	r3, r1, ror #16
${ARMSEC} thumb 0x40c8 0xb200 # sxth	r0, r0
${ARMSEC} thumb 0x40ca 0xb200 # sxth	r0, r0
${ARMSEC} thumb 0x40c8 0xb213 # sxth	r3, r2
${ARMSEC} thumb 0x40ca 0xb213 # sxth	r3, r2
${ARMSEC} thumb 0x40c8 0xf0b0fa0f # sxth.w	r0, r0, ror #24
${ARMSEC} thumb 0x40ca 0xf0b0fa0f # sxth.w	r0, r0, ror #24
${ARMSEC} thumb 0x40c8 0xf092fa0f # sxth.w	r0, r2, ror #8
${ARMSEC} thumb 0x40ca 0xf092fa0f # sxth.w	r0, r2, ror #8
${ARMSEC} thumb 0x40c8 0x5f83ea93 # teq	r3, r3, lsl #22
${ARMSEC} thumb 0x40ca 0x5f83ea93 # teq	r3, r3, lsl #22
${ARMSEC} thumb 0x40c8 0x2f22ea90 # teq	r0, r2, asr #8
${ARMSEC} thumb 0x40ca 0x2f22ea90 # teq	r0, r2, asr #8
${ARMSEC} thumb 0x40c8 0x7f1cf092 # teq	r2, #40894464
${ARMSEC} thumb 0x40ca 0x7f1cf092 # teq	r2, #40894464
${ARMSEC} thumb 0x40c8 0x4209 # tst	r1, r1
${ARMSEC} thumb 0x40ca 0x4209 # tst	r1, r1
${ARMSEC} thumb 0x40c8 0x4210 # tst	r0, r2
${ARMSEC} thumb 0x40ca 0x4210 # tst	r0, r2
${ARMSEC} thumb 0x40c8 0xf50ea10 # tst.w	r0, r0, lsr #1
${ARMSEC} thumb 0x40ca 0xf50ea10 # tst.w	r0, r0, lsr #1
${ARMSEC} thumb 0x40c8 0x3fc1ea13 # tst.w	r3, r1, lsl #15
${ARMSEC} thumb 0x40ca 0x3fc1ea13 # tst.w	r3, r1, lsl #15
${ARMSEC} thumb 0x40c8 0x5feef412 # tst.w	r2, #7616
${ARMSEC} thumb 0x40ca 0x5feef412 # tst.w	r2, #7616
${ARMSEC} thumb 0x40c8 0xf040fa90 # uadd16	r0, r0, r0
${ARMSEC} thumb 0x40ca 0xf040fa90 # uadd16	r0, r0, r0
${ARMSEC} thumb 0x40c8 0xf142fa92 # uadd16	r1, r2, r2
${ARMSEC} thumb 0x40ca 0xf142fa92 # uadd16	r1, r2, r2
${ARMSEC} thumb 0x40c8 0xf141fa93 # uadd16	r1, r3, r1
${ARMSEC} thumb 0x40ca 0xf141fa93 # uadd16	r1, r3, r1
${ARMSEC} thumb 0x40c8 0xf143fa91 # uadd16	r1, r1, r3
${ARMSEC} thumb 0x40ca 0xf143fa91 # uadd16	r1, r1, r3
${ARMSEC} thumb 0x40c8 0xf043fa92 # uadd16	r0, r2, r3
${ARMSEC} thumb 0x40ca 0xf043fa92 # uadd16	r0, r2, r3
${ARMSEC} thumb 0x40c8 0xf242fa82 # uadd8	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf242fa82 # uadd8	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf143fa83 # uadd8	r1, r3, r3
${ARMSEC} thumb 0x40ca 0xf143fa83 # uadd8	r1, r3, r3
${ARMSEC} thumb 0x40c8 0xf141fa80 # uadd8	r1, r0, r1
${ARMSEC} thumb 0x40ca 0xf141fa80 # uadd8	r1, r0, r1
${ARMSEC} thumb 0x40c8 0xf342fa83 # uadd8	r3, r3, r2
${ARMSEC} thumb 0x40ca 0xf342fa83 # uadd8	r3, r3, r2
${ARMSEC} thumb 0x40c8 0xf140fa83 # uadd8	r1, r3, r0
${ARMSEC} thumb 0x40ca 0xf140fa83 # uadd8	r1, r3, r0
${ARMSEC} thumb 0x40c8 0xf242faa2 # uasx	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf242faa2 # uasx	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf241faa1 # uasx	r2, r1, r1
${ARMSEC} thumb 0x40ca 0xf241faa1 # uasx	r2, r1, r1
${ARMSEC} thumb 0x40c8 0xf040faa1 # uasx	r0, r1, r0
${ARMSEC} thumb 0x40ca 0xf040faa1 # uasx	r0, r1, r0
${ARMSEC} thumb 0x40c8 0xf241faa2 # uasx	r2, r2, r1
${ARMSEC} thumb 0x40ca 0xf241faa2 # uasx	r2, r2, r1
${ARMSEC} thumb 0x40c8 0xf142faa3 # uasx	r1, r3, r2
${ARMSEC} thumb 0x40ca 0xf142faa3 # uasx	r1, r3, r2
${ARMSEC} thumb 0x40c8 0x5182f3c1 # ubfx	r1, r1, #22, #3
${ARMSEC} thumb 0x40ca 0x5182f3c1 # ubfx	r1, r1, #22, #3
${ARMSEC} thumb 0x40c8 0x11c0f3c0 # ubfx	r1, r0, #7, #1
${ARMSEC} thumb 0x40ca 0x11c0f3c0 # ubfx	r1, r0, #7, #1
${ARMSEC} thumb 0x40c8 0xf363fa93 # uhadd16	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf363fa93 # uhadd16	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf360fa90 # uhadd16	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf360fa90 # uhadd16	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf060fa92 # uhadd16	r0, r2, r0
${ARMSEC} thumb 0x40ca 0xf060fa92 # uhadd16	r0, r2, r0
${ARMSEC} thumb 0x40c8 0xf263fa92 # uhadd16	r2, r2, r3
${ARMSEC} thumb 0x40ca 0xf263fa92 # uhadd16	r2, r2, r3
${ARMSEC} thumb 0x40c8 0xf361fa90 # uhadd16	r3, r0, r1
${ARMSEC} thumb 0x40ca 0xf361fa90 # uhadd16	r3, r0, r1
${ARMSEC} thumb 0x40c8 0xf262fa82 # uhadd8	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf262fa82 # uhadd8	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf360fa80 # uhadd8	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf360fa80 # uhadd8	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf161fa80 # uhadd8	r1, r0, r1
${ARMSEC} thumb 0x40ca 0xf161fa80 # uhadd8	r1, r0, r1
${ARMSEC} thumb 0x40c8 0xf361fa83 # uhadd8	r3, r3, r1
${ARMSEC} thumb 0x40ca 0xf361fa83 # uhadd8	r3, r3, r1
${ARMSEC} thumb 0x40c8 0xf063fa82 # uhadd8	r0, r2, r3
${ARMSEC} thumb 0x40ca 0xf063fa82 # uhadd8	r0, r2, r3
${ARMSEC} thumb 0x40c8 0xf363faa3 # uhasx	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf363faa3 # uhasx	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf062faa2 # uhasx	r0, r2, r2
${ARMSEC} thumb 0x40ca 0xf062faa2 # uhasx	r0, r2, r2
${ARMSEC} thumb 0x40c8 0xf262faa0 # uhasx	r2, r0, r2
${ARMSEC} thumb 0x40ca 0xf262faa0 # uhasx	r2, r0, r2
${ARMSEC} thumb 0x40c8 0xf362faa3 # uhasx	r3, r3, r2
${ARMSEC} thumb 0x40ca 0xf362faa3 # uhasx	r3, r3, r2
${ARMSEC} thumb 0x40c8 0xf261faa0 # uhasx	r2, r0, r1
${ARMSEC} thumb 0x40ca 0xf261faa0 # uhasx	r2, r0, r1
${ARMSEC} thumb 0x40c8 0xf161fae1 # uhsax	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf161fae1 # uhsax	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf261fae1 # uhsax	r2, r1, r1
${ARMSEC} thumb 0x40ca 0xf261fae1 # uhsax	r2, r1, r1
${ARMSEC} thumb 0x40c8 0xf060fae3 # uhsax	r0, r3, r0
${ARMSEC} thumb 0x40ca 0xf060fae3 # uhsax	r0, r3, r0
${ARMSEC} thumb 0x40c8 0xf160fae1 # uhsax	r1, r1, r0
${ARMSEC} thumb 0x40ca 0xf160fae1 # uhsax	r1, r1, r0
${ARMSEC} thumb 0x40c8 0xf061fae3 # uhsax	r0, r3, r1
${ARMSEC} thumb 0x40ca 0xf061fae3 # uhsax	r0, r3, r1
${ARMSEC} thumb 0x40c8 0xf262fad2 # uhsub16	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf262fad2 # uhsub16	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf361fad1 # uhsub16	r3, r1, r1
${ARMSEC} thumb 0x40ca 0xf361fad1 # uhsub16	r3, r1, r1
${ARMSEC} thumb 0x40c8 0xf262fad3 # uhsub16	r2, r3, r2
${ARMSEC} thumb 0x40ca 0xf262fad3 # uhsub16	r2, r3, r2
${ARMSEC} thumb 0x40c8 0xf260fad2 # uhsub16	r2, r2, r0
${ARMSEC} thumb 0x40ca 0xf260fad2 # uhsub16	r2, r2, r0
${ARMSEC} thumb 0x40c8 0xf361fad2 # uhsub16	r3, r2, r1
${ARMSEC} thumb 0x40ca 0xf361fad2 # uhsub16	r3, r2, r1
${ARMSEC} thumb 0x40c8 0xf262fac2 # uhsub8	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf262fac2 # uhsub8	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf163fac3 # uhsub8	r1, r3, r3
${ARMSEC} thumb 0x40ca 0xf163fac3 # uhsub8	r1, r3, r3
${ARMSEC} thumb 0x40c8 0xf363fac1 # uhsub8	r3, r1, r3
${ARMSEC} thumb 0x40ca 0xf363fac1 # uhsub8	r3, r1, r3
${ARMSEC} thumb 0x40c8 0xf362fac3 # uhsub8	r3, r3, r2
${ARMSEC} thumb 0x40ca 0xf362fac3 # uhsub8	r3, r3, r2
${ARMSEC} thumb 0x40c8 0xf160fac3 # uhsub8	r1, r3, r0
${ARMSEC} thumb 0x40ca 0xf160fac3 # uhsub8	r1, r3, r0
${ARMSEC} thumb 0x40c8 0x1060fbe0 # umaal	r1, r0, r0, r0
${ARMSEC} thumb 0x40ca 0x1060fbe0 # umaal	r1, r0, r0, r0
${ARMSEC} thumb 0x40c8 0x1262fbe1 # umaal	r1, r2, r1, r2
${ARMSEC} thumb 0x40ca 0x1262fbe1 # umaal	r1, r2, r1, r2
${ARMSEC} thumb 0x40c8 0x260fbe2 # umaal	r0, r2, r2, r0
${ARMSEC} thumb 0x40ca 0x260fbe2 # umaal	r0, r2, r2, r0
${ARMSEC} thumb 0x40c8 0x160fbe0 # umaal	r0, r1, r0, r0
${ARMSEC} thumb 0x40ca 0x160fbe0 # umaal	r0, r1, r0, r0
${ARMSEC} thumb 0x40c8 0x1363fbe2 # umaal	r1, r3, r2, r3
${ARMSEC} thumb 0x40ca 0x1363fbe2 # umaal	r1, r3, r2, r3
${ARMSEC} thumb 0x40c8 0x1061fbe2 # umaal	r1, r0, r2, r1
${ARMSEC} thumb 0x40ca 0x1061fbe2 # umaal	r1, r0, r2, r1
${ARMSEC} thumb 0x40c8 0x2063fbe0 # umaal	r2, r0, r0, r3
${ARMSEC} thumb 0x40ca 0x2063fbe0 # umaal	r2, r0, r0, r3
${ARMSEC} thumb 0x40c8 0x3261fbe3 # umaal	r3, r2, r3, r1
${ARMSEC} thumb 0x40ca 0x3261fbe3 # umaal	r3, r2, r3, r1
${ARMSEC} thumb 0x40c8 0x2061fbe1 # umaal	r2, r0, r1, r1
${ARMSEC} thumb 0x40ca 0x2061fbe1 # umaal	r2, r0, r1, r1
${ARMSEC} thumb 0x40c8 0x1263fbe0 # umaal	r1, r2, r0, r3
${ARMSEC} thumb 0x40ca 0x1263fbe0 # umaal	r1, r2, r0, r3
${ARMSEC} thumb 0x40c8 0x303fbe3 # umlal	r0, r3, r3, r3
${ARMSEC} thumb 0x40ca 0x303fbe3 # umlal	r0, r3, r3, r3
${ARMSEC} thumb 0x40c8 0x1000fbe1 # umlal	r1, r0, r1, r0
${ARMSEC} thumb 0x40ca 0x1000fbe1 # umlal	r1, r0, r1, r0
${ARMSEC} thumb 0x40c8 0x3003fbe0 # umlal	r3, r0, r0, r3
${ARMSEC} thumb 0x40ca 0x3003fbe0 # umlal	r3, r0, r0, r3
${ARMSEC} thumb 0x40c8 0x2302fbe2 # umlal	r2, r3, r2, r2
${ARMSEC} thumb 0x40ca 0x2302fbe2 # umlal	r2, r3, r2, r2
${ARMSEC} thumb 0x40c8 0x3000fbe2 # umlal	r3, r0, r2, r0
${ARMSEC} thumb 0x40ca 0x3000fbe2 # umlal	r3, r0, r2, r0
${ARMSEC} thumb 0x40c8 0x2302fbe0 # umlal	r2, r3, r0, r2
${ARMSEC} thumb 0x40ca 0x2302fbe0 # umlal	r2, r3, r0, r2
${ARMSEC} thumb 0x40c8 0x2300fbe3 # umlal	r2, r3, r3, r0
${ARMSEC} thumb 0x40ca 0x2300fbe3 # umlal	r2, r3, r3, r0
${ARMSEC} thumb 0x40c8 0x1200fbe1 # umlal	r1, r2, r1, r0
${ARMSEC} thumb 0x40ca 0x1200fbe1 # umlal	r1, r2, r1, r0
${ARMSEC} thumb 0x40c8 0x1200fbe0 # umlal	r1, r2, r0, r0
${ARMSEC} thumb 0x40ca 0x1200fbe0 # umlal	r1, r2, r0, r0
${ARMSEC} thumb 0x40c8 0x3002fbe1 # umlal	r3, r0, r1, r2
${ARMSEC} thumb 0x40ca 0x3002fbe1 # umlal	r3, r0, r1, r2
${ARMSEC} thumb 0x40c8 0x300fba0 # umull	r0, r3, r0, r0
${ARMSEC} thumb 0x40ca 0x300fba0 # umull	r0, r3, r0, r0
${ARMSEC} thumb 0x40c8 0x2000fba0 # umull	r2, r0, r0, r0
${ARMSEC} thumb 0x40ca 0x2000fba0 # umull	r2, r0, r0, r0
${ARMSEC} thumb 0x40c8 0x3103fba1 # umull	r3, r1, r1, r3
${ARMSEC} thumb 0x40ca 0x3103fba1 # umull	r3, r1, r1, r3
${ARMSEC} thumb 0x40c8 0x3000fba3 # umull	r3, r0, r3, r0
${ARMSEC} thumb 0x40ca 0x3000fba3 # umull	r3, r0, r3, r0
${ARMSEC} thumb 0x40c8 0x2103fba3 # umull	r2, r1, r3, r3
${ARMSEC} thumb 0x40ca 0x2103fba3 # umull	r2, r1, r3, r3
${ARMSEC} thumb 0x40c8 0x103fba0 # umull	r0, r1, r0, r3
${ARMSEC} thumb 0x40ca 0x103fba0 # umull	r0, r1, r0, r3
${ARMSEC} thumb 0x40c8 0x3102fba1 # umull	r3, r1, r1, r2
${ARMSEC} thumb 0x40ca 0x3102fba1 # umull	r3, r1, r1, r2
${ARMSEC} thumb 0x40c8 0x2102fba3 # umull	r2, r1, r3, r2
${ARMSEC} thumb 0x40ca 0x2102fba3 # umull	r2, r1, r3, r2
${ARMSEC} thumb 0x40c8 0x3202fba0 # umull	r3, r2, r0, r2
${ARMSEC} thumb 0x40ca 0x3202fba0 # umull	r3, r2, r0, r2
${ARMSEC} thumb 0x40c8 0x302fba1 # umull	r0, r3, r1, r2
${ARMSEC} thumb 0x40ca 0x302fba1 # umull	r0, r3, r1, r2
${ARMSEC} thumb 0x40c8 0xf252fa92 # uqadd16	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf252fa92 # uqadd16	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf351fa91 # uqadd16	r3, r1, r1
${ARMSEC} thumb 0x40ca 0xf351fa91 # uqadd16	r3, r1, r1
${ARMSEC} thumb 0x40c8 0xf252fa93 # uqadd16	r2, r3, r2
${ARMSEC} thumb 0x40ca 0xf252fa93 # uqadd16	r2, r3, r2
${ARMSEC} thumb 0x40c8 0xf250fa92 # uqadd16	r2, r2, r0
${ARMSEC} thumb 0x40ca 0xf250fa92 # uqadd16	r2, r2, r0
${ARMSEC} thumb 0x40c8 0xf351fa92 # uqadd16	r3, r2, r1
${ARMSEC} thumb 0x40ca 0xf351fa92 # uqadd16	r3, r2, r1
${ARMSEC} thumb 0x40c8 0xf252fa82 # uqadd8	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf252fa82 # uqadd8	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf153fa83 # uqadd8	r1, r3, r3
${ARMSEC} thumb 0x40ca 0xf153fa83 # uqadd8	r1, r3, r3
${ARMSEC} thumb 0x40c8 0xf353fa81 # uqadd8	r3, r1, r3
${ARMSEC} thumb 0x40ca 0xf353fa81 # uqadd8	r3, r1, r3
${ARMSEC} thumb 0x40c8 0xf352fa83 # uqadd8	r3, r3, r2
${ARMSEC} thumb 0x40ca 0xf352fa83 # uqadd8	r3, r3, r2
${ARMSEC} thumb 0x40c8 0xf150fa83 # uqadd8	r1, r3, r0
${ARMSEC} thumb 0x40ca 0xf150fa83 # uqadd8	r1, r3, r0
${ARMSEC} thumb 0x40c8 0xf252faa2 # uqasx	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf252faa2 # uqasx	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf053faa3 # uqasx	r0, r3, r3
${ARMSEC} thumb 0x40ca 0xf053faa3 # uqasx	r0, r3, r3
${ARMSEC} thumb 0x40c8 0xf252faa3 # uqasx	r2, r3, r2
${ARMSEC} thumb 0x40ca 0xf252faa3 # uqasx	r2, r3, r2
${ARMSEC} thumb 0x40c8 0xf253faa2 # uqasx	r2, r2, r3
${ARMSEC} thumb 0x40ca 0xf253faa2 # uqasx	r2, r2, r3
${ARMSEC} thumb 0x40c8 0xf352faa0 # uqasx	r3, r0, r2
${ARMSEC} thumb 0x40ca 0xf352faa0 # uqasx	r3, r0, r2
${ARMSEC} thumb 0x40c8 0xf151fae1 # uqsax	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf151fae1 # uqsax	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf250fae0 # uqsax	r2, r0, r0
${ARMSEC} thumb 0x40ca 0xf250fae0 # uqsax	r2, r0, r0
${ARMSEC} thumb 0x40c8 0xf151fae3 # uqsax	r1, r3, r1
${ARMSEC} thumb 0x40ca 0xf151fae3 # uqsax	r1, r3, r1
${ARMSEC} thumb 0x40c8 0xf350fae3 # uqsax	r3, r3, r0
${ARMSEC} thumb 0x40ca 0xf350fae3 # uqsax	r3, r3, r0
${ARMSEC} thumb 0x40c8 0xf352fae0 # uqsax	r3, r0, r2
${ARMSEC} thumb 0x40ca 0xf352fae0 # uqsax	r3, r0, r2
${ARMSEC} thumb 0x40c8 0xf353fad3 # uqsub16	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf353fad3 # uqsub16	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf250fad0 # uqsub16	r2, r0, r0
${ARMSEC} thumb 0x40ca 0xf250fad0 # uqsub16	r2, r0, r0
${ARMSEC} thumb 0x40c8 0xf151fad3 # uqsub16	r1, r3, r1
${ARMSEC} thumb 0x40ca 0xf151fad3 # uqsub16	r1, r3, r1
${ARMSEC} thumb 0x40c8 0xf351fad3 # uqsub16	r3, r3, r1
${ARMSEC} thumb 0x40ca 0xf351fad3 # uqsub16	r3, r3, r1
${ARMSEC} thumb 0x40c8 0xf053fad2 # uqsub16	r0, r2, r3
${ARMSEC} thumb 0x40ca 0xf053fad2 # uqsub16	r0, r2, r3
${ARMSEC} thumb 0x40c8 0xf353fac3 # uqsub8	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf353fac3 # uqsub8	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf052fac2 # uqsub8	r0, r2, r2
${ARMSEC} thumb 0x40ca 0xf052fac2 # uqsub8	r0, r2, r2
${ARMSEC} thumb 0x40c8 0xf252fac0 # uqsub8	r2, r0, r2
${ARMSEC} thumb 0x40ca 0xf252fac0 # uqsub8	r2, r0, r2
${ARMSEC} thumb 0x40c8 0xf352fac3 # uqsub8	r3, r3, r2
${ARMSEC} thumb 0x40ca 0xf352fac3 # uqsub8	r3, r3, r2
${ARMSEC} thumb 0x40c8 0xf251fac0 # uqsub8	r2, r0, r1
${ARMSEC} thumb 0x40ca 0xf251fac0 # uqsub8	r2, r0, r1
${ARMSEC} thumb 0x40c8 0xf101fb71 # usad8	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf101fb71 # usad8	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf201fb71 # usad8	r2, r1, r1
${ARMSEC} thumb 0x40ca 0xf201fb71 # usad8	r2, r1, r1
${ARMSEC} thumb 0x40c8 0xf000fb73 # usad8	r0, r3, r0
${ARMSEC} thumb 0x40ca 0xf000fb73 # usad8	r0, r3, r0
${ARMSEC} thumb 0x40c8 0xf100fb71 # usad8	r1, r1, r0
${ARMSEC} thumb 0x40ca 0xf100fb71 # usad8	r1, r1, r0
${ARMSEC} thumb 0x40c8 0xf001fb73 # usad8	r0, r3, r1
${ARMSEC} thumb 0x40ca 0xf001fb73 # usad8	r0, r3, r1
${ARMSEC} thumb 0x40c8 0x2202fb72 # usada8	r2, r2, r2, r2
${ARMSEC} thumb 0x40ca 0x2202fb72 # usada8	r2, r2, r2, r2
${ARMSEC} thumb 0x40c8 0x3203fb73 # usada8	r2, r3, r3, r3
${ARMSEC} thumb 0x40ca 0x3203fb73 # usada8	r2, r3, r3, r3
${ARMSEC} thumb 0x40c8 0x2fb72 # usada8	r0, r2, r2, r0
${ARMSEC} thumb 0x40ca 0x2fb72 # usada8	r0, r2, r2, r0
${ARMSEC} thumb 0x40c8 0x1303fb71 # usada8	r3, r1, r3, r1
${ARMSEC} thumb 0x40ca 0x1303fb71 # usada8	r3, r1, r3, r1
${ARMSEC} thumb 0x40c8 0xfb72 # usada8	r0, r2, r0, r0
${ARMSEC} thumb 0x40ca 0xfb72 # usada8	r0, r2, r0, r0
${ARMSEC} thumb 0x40c8 0x2303fb73 # usada8	r3, r3, r3, r2
${ARMSEC} thumb 0x40ca 0x2303fb73 # usada8	r3, r3, r3, r2
${ARMSEC} thumb 0x40c8 0x2201fb72 # usada8	r2, r2, r1, r2
${ARMSEC} thumb 0x40ca 0x2201fb72 # usada8	r2, r2, r1, r2
${ARMSEC} thumb 0x40c8 0x1301fb73 # usada8	r3, r3, r1, r1
${ARMSEC} thumb 0x40ca 0x1301fb73 # usada8	r3, r3, r1, r1
${ARMSEC} thumb 0x40c8 0x2001fb71 # usada8	r0, r1, r1, r2
${ARMSEC} thumb 0x40ca 0x2001fb71 # usada8	r0, r1, r1, r2
${ARMSEC} thumb 0x40c8 0x302fb70 # usada8	r3, r0, r2, r0
${ARMSEC} thumb 0x40ca 0x302fb70 # usada8	r3, r0, r2, r0
${ARMSEC} thumb 0x40c8 0x2102fb70 # usada8	r1, r0, r2, r2
${ARMSEC} thumb 0x40ca 0x2102fb70 # usada8	r1, r0, r2, r2
${ARMSEC} thumb 0x40c8 0x1100fb73 # usada8	r1, r3, r0, r1
${ARMSEC} thumb 0x40ca 0x1100fb73 # usada8	r1, r3, r0, r1
${ARMSEC} thumb 0x40c8 0x202fb73 # usada8	r2, r3, r2, r0
${ARMSEC} thumb 0x40ca 0x202fb73 # usada8	r2, r3, r2, r0
${ARMSEC} thumb 0x40c8 0x2301fb73 # usada8	r3, r3, r1, r2
${ARMSEC} thumb 0x40ca 0x2301fb73 # usada8	r3, r3, r1, r2
${ARMSEC} thumb 0x40c8 0x2300fb71 # usada8	r3, r1, r0, r2
${ARMSEC} thumb 0x40ca 0x2300fb71 # usada8	r3, r1, r0, r2
${ARMSEC} thumb 0x40c8 0x153f3a1 # usat	r1, #19, r1, asr #1
${ARMSEC} thumb 0x40ca 0x153f3a1 # usat	r1, #19, r1, asr #1
${ARMSEC} thumb 0x40c8 0x4058f381 # usat	r0, #24, r1, lsl #17
${ARMSEC} thumb 0x40ca 0x4058f381 # usat	r0, #24, r1, lsl #17
${ARMSEC} thumb 0x40c8 0x103f3a1 # usat16	r1, #3, r1
${ARMSEC} thumb 0x40ca 0x103f3a1 # usat16	r1, #3, r1
${ARMSEC} thumb 0x40c8 0x8f3a1 # usat16	r0, #8, r1
${ARMSEC} thumb 0x40ca 0x8f3a1 # usat16	r0, #8, r1
${ARMSEC} thumb 0x40c8 0xf343fae3 # usax	r3, r3, r3
${ARMSEC} thumb 0x40ca 0xf343fae3 # usax	r3, r3, r3
${ARMSEC} thumb 0x40c8 0xf340fae0 # usax	r3, r0, r0
${ARMSEC} thumb 0x40ca 0xf340fae0 # usax	r3, r0, r0
${ARMSEC} thumb 0x40c8 0xf242fae0 # usax	r2, r0, r2
${ARMSEC} thumb 0x40ca 0xf242fae0 # usax	r2, r0, r2
${ARMSEC} thumb 0x40c8 0xf240fae2 # usax	r2, r2, r0
${ARMSEC} thumb 0x40ca 0xf240fae2 # usax	r2, r2, r0
${ARMSEC} thumb 0x40c8 0xf243fae0 # usax	r2, r0, r3
${ARMSEC} thumb 0x40ca 0xf243fae0 # usax	r2, r0, r3
${ARMSEC} thumb 0x40c8 0xf242fad2 # usub16	r2, r2, r2
${ARMSEC} thumb 0x40ca 0xf242fad2 # usub16	r2, r2, r2
${ARMSEC} thumb 0x40c8 0xf341fad1 # usub16	r3, r1, r1
${ARMSEC} thumb 0x40ca 0xf341fad1 # usub16	r3, r1, r1
${ARMSEC} thumb 0x40c8 0xf242fad1 # usub16	r2, r1, r2
${ARMSEC} thumb 0x40ca 0xf242fad1 # usub16	r2, r1, r2
${ARMSEC} thumb 0x40c8 0xf341fad3 # usub16	r3, r3, r1
${ARMSEC} thumb 0x40ca 0xf341fad3 # usub16	r3, r3, r1
${ARMSEC} thumb 0x40c8 0xf243fad0 # usub16	r2, r0, r3
${ARMSEC} thumb 0x40ca 0xf243fad0 # usub16	r2, r0, r3
${ARMSEC} thumb 0x40c8 0xf141fac1 # usub8	r1, r1, r1
${ARMSEC} thumb 0x40ca 0xf141fac1 # usub8	r1, r1, r1
${ARMSEC} thumb 0x40c8 0xf241fac1 # usub8	r2, r1, r1
${ARMSEC} thumb 0x40ca 0xf241fac1 # usub8	r2, r1, r1
${ARMSEC} thumb 0x40c8 0xf040fac3 # usub8	r0, r3, r0
${ARMSEC} thumb 0x40ca 0xf040fac3 # usub8	r0, r3, r0
${ARMSEC} thumb 0x40c8 0xf140fac1 # usub8	r1, r1, r0
${ARMSEC} thumb 0x40ca 0xf140fac1 # usub8	r1, r1, r0
${ARMSEC} thumb 0x40c8 0xf041fac3 # usub8	r0, r3, r1
${ARMSEC} thumb 0x40ca 0xf041fac3 # usub8	r0, r3, r1
${ARMSEC} thumb 0x40c8 0xf3a3fa53 # uxtab	r3, r3, r3, ror #16
${ARMSEC} thumb 0x40ca 0xf3a3fa53 # uxtab	r3, r3, r3, ror #16
${ARMSEC} thumb 0x40c8 0xf3b2fa52 # uxtab	r3, r2, r2, ror #24
${ARMSEC} thumb 0x40ca 0xf3b2fa52 # uxtab	r3, r2, r2, ror #24
${ARMSEC} thumb 0x40c8 0xf080fa53 # uxtab	r0, r3, r0
${ARMSEC} thumb 0x40ca 0xf080fa53 # uxtab	r0, r3, r0
${ARMSEC} thumb 0x40c8 0xf0b2fa50 # uxtab	r0, r0, r2, ror #24
${ARMSEC} thumb 0x40ca 0xf0b2fa50 # uxtab	r0, r0, r2, ror #24
${ARMSEC} thumb 0x40c8 0xf3b1fa50 # uxtab	r3, r0, r1, ror #24
${ARMSEC} thumb 0x40ca 0xf3b1fa50 # uxtab	r3, r0, r1, ror #24
${ARMSEC} thumb 0x40c8 0xf2b2fa32 # uxtab16	r2, r2, r2, ror #24
${ARMSEC} thumb 0x40ca 0xf2b2fa32 # uxtab16	r2, r2, r2, ror #24
${ARMSEC} thumb 0x40c8 0xf2b1fa31 # uxtab16	r2, r1, r1, ror #24
${ARMSEC} thumb 0x40ca 0xf2b1fa31 # uxtab16	r2, r1, r1, ror #24
${ARMSEC} thumb 0x40c8 0xf1b1fa30 # uxtab16	r1, r0, r1, ror #24
${ARMSEC} thumb 0x40ca 0xf1b1fa30 # uxtab16	r1, r0, r1, ror #24
${ARMSEC} thumb 0x40c8 0xf290fa32 # uxtab16	r2, r2, r0, ror #8
${ARMSEC} thumb 0x40ca 0xf290fa32 # uxtab16	r2, r2, r0, ror #8
${ARMSEC} thumb 0x40c8 0xf1b3fa30 # uxtab16	r1, r0, r3, ror #24
${ARMSEC} thumb 0x40ca 0xf1b3fa30 # uxtab16	r1, r0, r3, ror #24
${ARMSEC} thumb 0x40c8 0xf2b2fa12 # uxtah	r2, r2, r2, ror #24
${ARMSEC} thumb 0x40ca 0xf2b2fa12 # uxtah	r2, r2, r2, ror #24
${ARMSEC} thumb 0x40c8 0xf190fa10 # uxtah	r1, r0, r0, ror #8
${ARMSEC} thumb 0x40ca 0xf190fa10 # uxtah	r1, r0, r0, ror #8
${ARMSEC} thumb 0x40c8 0xf080fa13 # uxtah	r0, r3, r0
${ARMSEC} thumb 0x40ca 0xf080fa13 # uxtah	r0, r3, r0
${ARMSEC} thumb 0x40c8 0xf291fa12 # uxtah	r2, r2, r1, ror #8
${ARMSEC} thumb 0x40ca 0xf291fa12 # uxtah	r2, r2, r1, ror #8
${ARMSEC} thumb 0x40c8 0xf1a2fa13 # uxtah	r1, r3, r2, ror #16
${ARMSEC} thumb 0x40ca 0xf1a2fa13 # uxtah	r1, r3, r2, ror #16
${ARMSEC} thumb 0x40c8 0xb2db # uxtb	r3, r3
${ARMSEC} thumb 0x40ca 0xb2db # uxtb	r3, r3
${ARMSEC} thumb 0x40c8 0xb2cb # uxtb	r3, r1
${ARMSEC} thumb 0x40ca 0xb2cb # uxtb	r3, r1
${ARMSEC} thumb 0x40c8 0xf191fa3f # uxtb16	r1, r1, ror #8
${ARMSEC} thumb 0x40ca 0xf191fa3f # uxtb16	r1, r1, ror #8
${ARMSEC} thumb 0x40c8 0xf1a3fa3f # uxtb16	r1, r3, ror #16
${ARMSEC} thumb 0x40ca 0xf1a3fa3f # uxtb16	r1, r3, ror #16
${ARMSEC} thumb 0x40c8 0xf282fa5f # uxtb.w	r2, r2
${ARMSEC} thumb 0x40ca 0xf282fa5f # uxtb.w	r2, r2
${ARMSEC} thumb 0x40c8 0xf081fa5f # uxtb.w	r0, r1
${ARMSEC} thumb 0x40ca 0xf081fa5f # uxtb.w	r0, r1
${ARMSEC} thumb 0x40c8 0xb29b # uxth	r3, r3
${ARMSEC} thumb 0x40ca 0xb29b # uxth	r3, r3
${ARMSEC} thumb 0x40c8 0xb299 # uxth	r1, r3
${ARMSEC} thumb 0x40ca 0xb299 # uxth	r1, r3
${ARMSEC} thumb 0x40c8 0xf2a2fa1f # uxth.w	r2, r2, ror #16
${ARMSEC} thumb 0x40ca 0xf2a2fa1f # uxth.w	r2, r2, ror #16
${ARMSEC} thumb 0x40c8 0xf3b1fa1f # uxth.w	r3, r1, ror #24
${ARMSEC} thumb 0x40ca 0xf3b1fa1f # uxth.w	r3, r1, ror #24
