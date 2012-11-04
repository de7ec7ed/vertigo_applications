
#include <defines.h>

callsign: .word CALLSIGN

remove_module: .word 1//LDR_REMOVE_MODULE

.globl _ldr_call_remove_module
_ldr_call_remove_module:
	push {r1 - r5}
	push {r0 - r3}
	ldr r0, callsign
	ldr r1, remove_module
	pop {r2 - r5}
	mcr	p0, 0, r0, c0, c0, 0
	pop {r1 - r5}
	mov pc, lr
