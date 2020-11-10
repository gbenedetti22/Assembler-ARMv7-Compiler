#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <array>
#include <vector>
#include <thread>
#include <pthread.h>

using namespace std;

void getFilesInCurrentDir(const char* cmd, vector<string>* result);
void _start_(string file);
void compile_execute(string file, bool flag);
void debug(string file, bool flag);
void _start_server_(string file);
void _start_client_(string file);
int check_command();
void print_menu();

int main(int argc, char const *argv[])
{
    system("clear");
    if(check_command()>0){
        perror("Il compilatore non può funzionare senza i seguenti pacchetti\n");
        cout << "Eseguire i seguenti comandi per installarli: \n\n";
        cout << "sudo apt install gcc-arm-linux-gnueabihf\n";
        cout << "sudo apt install qemu\n";
        cout << "sudo apt install gdb-multiarch\n";
        return 0;
    }
    
    system("clear");
    vector<string> file_list;
    string file_choosed;
    
    getFilesInCurrentDir("ls -d *.s", &file_list);

    if(file_list.size() == 0){
        perror("Non è stato trova nessun file assembly nella directory corrente\n");
        perror("File *.s is missing");
        return 0;
    }else if(file_list.size() == 1){
        file_choosed=file_list.at(0);
    }else{
        cout << "Inserire l indice del file assembly che si vuole compilare ed eseguire\n\n";
        int index=0;
        for(int i=0; i < file_list.size(); i++){
            cout << i << ' ' << file_list.at(i) << "\n";
        }
            cin >> index;
            if(index > file_list.size()){
                return 0;
            }else {
                file_choosed=file_list.at(index);
            }
    }
    file_choosed.erase(remove(file_choosed.begin(), file_choosed.end(), '\n'), file_choosed.end());
    _start_(file_choosed);
    
    system("clear");
    return 0;
}

void getFilesInCurrentDir(const char* cmd, vector<string>* result) {
    array<char, 128> buffer;
    int index = 0;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("error exec");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result->insert(result->end(), buffer.data());
        index++;
    }
}
void _start_(string file){
    system("clear");
    int index;
    bool executed=false;

    print_menu();

    while(!executed){
        cin >> index;
                
        switch(index){
            case 0:
                compile_execute(file, false);
                break;
            case 1:
                compile_execute(file, true);
                break;
            case 2:
                debug(file, false);
                break;
            case 3:
                debug(file, true);
                break;
            case 4:
                system("clear");
                cout << "break\t\t per settare un Breakpoint (e.g. break main)\n";
                cout << "c\t\t per continuare l esecuzione\n";
                cout << "tui reg global\t per vedere i registri e il codice passo passo con una UI\n";
                cout << "n\t\t per continuare passo passo dopo il breakpoint\n\n";
                cout << "TIPS: appena aperto il debugger setta un breakpoint, premi c, scrivi tui reg global e vai avanti con n\n\n\n";

                cout << "0 Compila ed esegui\n\n";
                cout << "1 Debug\n\n";
                cout << "2 Istruzioni Debug\n\n";
                cout << "3 Exit\n\n";
                break;
            default: executed=true;
        }
    }
}
int check_command(){
    int i = system("dpkg -s qemu-user");
    int j = system("dpkg -s gcc-arm-linux-gnueabihf");
    int k = system("dpkg -s gdb-multiarch");
    
    system("clear");
    cout << "dpkg -s qemu-user: " << i << "\n\n";
    cout << "dpkg -s gcc-arm-linux-gnueabihf: " << j << "\n\n";
    cout << "dpkg -s gdb-multiarch: " << k << "\n\n\n\n";

    return i + j + k;
}

void compile_execute(string file, bool flag){
    string cmd="r";
    while (cmd == "r")
    {
        system("clear");

        cmd = "arm-linux-gnueabihf-gcc " + file + " -o " + file.substr(0, file.size() - 2) + " -static";
        if(flag){
            cmd = "arm-linux-gnueabihf-gcc " + file + " -o " + file.substr(0, file.size() - 2) + " -static -mfpu=neon";
        }

        system(cmd.c_str());

        cmd = "qemu-arm ./" + file.substr(0, file.size() - 2);
        system(cmd.c_str());

        cout << "\n\npremi r per ricompilare...";
        cout << "\n\npremi s per tornare al menù...\n";
        cin >> cmd;
    }
    system("clear");
    print_menu();
}
void debug(string file, bool flag){
    system("clear");
    string cmd="";
    if(flag){
        cmd = "arm-linux-gnueabihf-gcc "+ file +" -o "+ file.substr(0, file.size()-2) +" -static -mfpu=neon -ggdb3";
    }else
    {
        cmd = "arm-linux-gnueabihf-gcc "+ file +" -o "+ file.substr(0, file.size()-2) +" -static -ggdb3";
    }
    
    system(cmd.c_str());

    thread server(_start_server_, file);
    thread client(_start_client_, file);

    client.join();
    server.join();

    system("clear");
    print_menu();
}
void print_menu(){
    cout << "0 Compila ed esegui\n\n";
    cout << "1 Compila ed esegui (vmov, vadd ecc..)\n\n";
    cout << "========================================================\n\n";
    cout << "2 Debug\n\n";
    cout << "3 Debug (vmov, vadd ecc..)\n\n";
    cout << "========================================================\n\n";
    cout << "4 Istruzioni Debug\n\n";
    cout << "5 Exit\n\n";
}
void _start_server_(string file){
    string cmd= "qemu-arm -g 12345 ./"+file.substr(0, file.size()-2);
    system(cmd.c_str());
}
void _start_client_(string file){
    string cmd="gdb-multiarch -q --nh -ex 'set architecture arm' -ex 'file "+ file.substr(0, file.size()-2) +"' -ex 'target remote localhost:12345'";
    system(cmd.c_str());
}