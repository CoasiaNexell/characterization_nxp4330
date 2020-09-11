.syntax unified
    .text
    .arch armv7-a
    .fpu neon
    .arm

    .global main
    .global sysconf
    .global fork

/* optimal value for LOOP_UNROLL_FACTOR seems to be BTB size dependent */
#define LOOP_UNROLL_FACTOR   110
/* 64 seems to be a good choice */
#define STEP                 64

.func main
main:

#ifdef __linux__
        mov         r0, 84 /* _SC_NPROCESSORS_ONLN */
        blx         sysconf
        mov         r4, r0
        cmp         r4, #2
        blt         1f
        blx         fork /* have at least 2 cores */
        cmp         r4, #4
        blt         1f
        blx         fork /* have at least 4 cores */
1:
#endif

        ldr         lr, =(STEP * 4 + 15)
        subs        lr, sp, lr
        bic         lr, lr, #15
        mov         ip, #STEP
        mov         r0, #0
        mov         r1, #0
        mov         r2, #0
        mov         r3, #0
        ldr         r4, =0xFFFFFFFF
        b           0f
    .ltorg
0:
    .rept LOOP_UNROLL_FACTOR
        vld2.8      {q0}, [lr, :128], ip
        it          ne
        smlalne     r0, r1, lr, r4
        bne         1f
1:
        vld2.8      {q1}, [lr, :128], ip
        it          ne
        smlalne     r2, r3, lr, r4
        bne         1f
1:
        vld2.8      {q2}, [lr, :128], ip
        vld2.8      {q3}, [lr, :128], ip
        it          ne
        subsne      lr, lr, #(STEP * 4)
    .endr
        bne         0b
.endfunc
