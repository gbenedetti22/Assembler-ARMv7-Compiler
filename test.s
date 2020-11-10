.data
.text
fmt:    .string "Elevo %d alla %d \n"
result: .string "Risultato = %d \n"
.global main

main:   ldr r0,=fmt
        mov r1,#2
        mov r2,#3
        bl printf

        mov	r0,#2
		mov	r1,#3
				
		mov	r7,r0

potenza:
        sub	r1,r1,#1
		cmp	r1,#0
		beq	stampa
		b	moltiplico

moltiplico:
        mov	r2,r0
		mov	r3,r7
		mov	r4,r2

while:
        cmp	r3,#0
		beq	break
		add	r2,r2,r4
		sub	r3,r3,#1
		b	while

break:  sub	r2,r2,r4
		mov	r0,r2
        b potenza

stampa: mov r1,r0
        ldr r0,=result
        bl printf

        mov r7,#1
        svc 0
