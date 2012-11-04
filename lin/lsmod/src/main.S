
#include <defines.h>

callsign: .word CALLSIGN

copy_module_header: .word 2//LDR_COPY_MODULE_HEADER

.globl ldr_call_copy_module_header
ldr_call_copy_module_header:
	push {r1 - r4}
	push {r0 - r2}
	ldr r0, callsign
	ldr r1, copy_module_header
	pop {r2 - r4}
	mcr	p0, 0, r0, c0, c0, 0
	pop {r1 - r4}
	mov pc, lr
