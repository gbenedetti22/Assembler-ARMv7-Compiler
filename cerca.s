.global cerca
.type cerca,%function

cerca:@r0=string r1=lenght r2=carattere da cercare
mov r4,#1
mov r6,#0
while:
cmp r1,#0
moveq r0,r4
moveq pc,lr
ldrb r3,[r0,r6]
cmp r3,r2
moveq r4,#0
moveq r1,#1
sub r1,r1,#1
add r6,r6,#1
b while
