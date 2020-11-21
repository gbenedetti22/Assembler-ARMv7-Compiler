.data
voti: .word 30,28,27,24,28,18,-1
result: .string "La media dei voti è: %d \n"
.text
.global main

main:   ldr r0,=voti
        ldr r1,[r0]
        mov r2,#0
        mov r3,#0

for:    ldr r1,[r0],#4
        cmp r1,#0
        addgt r2,r2,r1
        addgt r3,r3,#1
        blt divisione
        b for

divisione:  
        cmp r2,r3
        blt	fine
        sub	r2,r2,r3
        add	r4,r4,#1
        b	divisione

fine:   ldr r0,=result
        mov r1,r4
        bl printf

        mov r7,#1
        svc 0
