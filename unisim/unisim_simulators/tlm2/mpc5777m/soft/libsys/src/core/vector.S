#ifdef __ghs__
    .vle
#endif

.globl VTABLE
.globl IVOR0_Vector
.globl IVOR1_Vector
.globl IVOR2_Vector
.globl IVOR3_Vector
.globl IVOR4_Vector
.globl IVOR5_Vector
.globl IVOR6_Vector
.globl IVOR7_Vector
.globl IVOR8_Vector
.globl IVOR9_Vector
.globl IVOR10_Vector
.globl IVOR11_Vector
.globl IVOR12_Vector
.globl IVOR13_Vector
.globl IVOR14_Vector
.globl IVOR15_Vector

.extern     IVOR4_Handler
.section    .core_exceptions_table, "axv"

.align 4
VTABLE:
IVOR0_Vector:
e_b   IVOR0_Vector

.align 4
IVOR1_Vector:
e_b   IVOR1_Vector

.align 4
IVOR2_Vector:
e_b   IVOR2_Vector

.align 4
IVOR3_Vector:
e_b   IVOR3_Vector

.align 4
IVOR4_Vector:
e_b   IVOR4_Handler

.align 4
IVOR5_Vector:
e_b   IVOR5_Vector

.align 4
IVOR6_Vector:
e_b   IVOR6_Vector

.align 4
IVOR7_Vector:
e_b   IVOR7_Vector

.align 4
IVOR8_Vector:
se_illegal  /* for semihosting support  */
se_rfi

.align 4
IVOR9_Vector:
e_b   IVOR9_Vector

.align 4
IVOR10_Vector:
e_b   IVOR10_Vector

.align 4
IVOR11_Vector:
e_b   IVOR11_Vector

.align 4
IVOR12_Vector:
e_b   IVOR12_Vector

.align 4
IVOR13_Vector:
e_b   IVOR13_Vector

.align 4
IVOR14_Vector:
e_b   IVOR14_Vector

.align 4
IVOR15_Vector:
e_b   IVOR15_Vector
