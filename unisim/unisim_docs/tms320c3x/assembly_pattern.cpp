	ldiu %subst, bk  ; /*(1)*/ load block size (should be at most 8)
	and %0 - 1, %src_int_reg ; /*(2)*/ crop random value between 0 and bk - 1
	ldiu %2, %clobber ir0; /*(3)*/ load ir0 with this random value
	ldiu %src_float_buf[16], %dst_aux_reg ; /*(4)*/ load a pointer to a buffer of 16 words
	addi %subst, %6 ; /*(5)*/
	andn %7, %6  ; /*(6)*/ align circular buffer start on block size
	ldiu %st_in, %clobber st ; /*(7)*/
	%subst *%6++(ir0)%%, %src_dst_float_reg; <-- instruction under test /*(8)*/
	ldiu st, %st_out ; /*(9)*/

