#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define lenght_of(x)  (sizeof(x) / sizeof((x)[0]))

extern int cerca(char*,int,char);

//gdb-multiarch -q --nh -ex 'set architecture arm' -ex 'file a' -ex 'target remote localhost:12345'
int main(int argc, char const *argv[])
{
    char v='b';
    char* input="ciao bello";
    int lenght=strlen(input);
    
    int a=cerca(input,lenght,v);

    if(a==0){
        printf("Valore trovato\n");
    }else
    {
        printf("Valore non trovato\n");
    }

    return 0;
}