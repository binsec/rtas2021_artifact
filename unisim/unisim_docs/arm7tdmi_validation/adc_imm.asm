eor r9, r9, r9; 
mov r10, %u4;
mov r10, r10, ror #4;
msr cpsr_f, r10;
adc%(,eq,ne,cs,cc,mi,pl,vs,vc,hi,ls,ge,lt,gt,le,al)%(,s) r9, %r, %s8,4,2;
mrs r10, cpsr;
mov r10, r10, ror #28;
and %R, r10, #15;
mov %R, r9
