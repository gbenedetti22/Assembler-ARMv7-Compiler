E' necessario installare/avere i seguenti componenti:<br/>
sudo apt install gcc-arm-linux-gnueabihf<br/>
sudo apt install qemu-user<br/>
sudo apt install gdb-multiarch<br/>
<br/>
Per compilare, usare:<br/>
g++ -pthread -o assembly_compiler compile.cpp
