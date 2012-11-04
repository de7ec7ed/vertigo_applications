
#include <defines.h>

callsign: .word CALLSIGN

add_module: .word 0//LDR_ADD_MODULE

.globl ldr_call_add_module
ldr_call_add_module:
	push {r1 - r5}
	push {r0 - r3}
	ldr r0, callsign
	ldr r1, add_module
	pop {r2 - r5}
	mcr	p0, 0, r0, c0, c0, 0
	pop {r1 - r5}
	mov pc, lr
