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

void multiple_file();
void one_file_assembly();
void getFilesInCurrentDir(const char *cmd, vector<string> *result);
void _start_(vector<string> file_list);
void compile_execute(vector<string> file_list, bool flag);
void debug(vector<string> file_list, bool flag);
void _start_server_(string file);
void _start_client_(string file);
int check_command();
void print_menu();

//compile with: g++ -pthread -o assembly_compiler compiler.cpp
int main(int argc, char const *argv[])
{
    system("clear");
    //controllo che i comandi siano installati
    if (check_command() > 0)
    {
        perror("Il compilatore non può funzionare senza i seguenti pacchetti\n");
        cout << "Eseguire i seguenti comandi per installarli: \n\n";
        cout << "sudo apt install gcc-arm-linux-gnueabihf\n";
        cout << "sudo apt install qemu-user\n";
        cout << "sudo apt install gdb-multiarch\n";
        return 0;
    }
    system("clear");
    int scelta = 0;
    cout << "0 Compila ed esegui un unico file Assembly\n\n";
    cout << "1 Compila ed esegui file C e Assembly\n\n";
    cin >> scelta;
    if (scelta == 0)
    {
        one_file_assembly();
    }
    else
    {
        multiple_file();
    }

    system("clear");
    return 0;
}

void multiple_file()
{
    system("clear");
    vector<string> file_list;
    vector<string> files_choosed;

    getFilesInCurrentDir("ls -d *.s *.c", &file_list);

    if (file_list.size() == 0)
    {
        perror("Non è stato trova nessun file assembly nella directory corrente\n");
        perror("File *.s or *.c is missing");
        return;
    }
    else if (file_list.size() == 1)
    {
        files_choosed.insert(files_choosed.end(), file_list.at(0));
    }
    else
    {
        cout << "Inserire l indice dei file C e assembly che si vuole compilare ed eseguire\n\n";
        int index = 0;
        for (int i = 0; i < file_list.size(); i++)
        {
            cout << i << ' ' << file_list.at(i) << "\n";
        }
        cout << "==============================================\n\n";
        cout << "-1 per confermare\n\n";
        while (index >= 0)
        {
            cin >> index;
            if (index >= 0 && index < file_list.size())
                files_choosed.insert(files_choosed.end(), file_list.at(index));
        }

        if (files_choosed.size() == 0)
            return;
    }
    _start_(files_choosed);
}

void one_file_assembly()
{
    system("clear");
    vector<string> file_list;
    vector<string> file_choosed;

    getFilesInCurrentDir("ls -d *.s", &file_list); //metto tutti i file .s della cartella corrente, nel vettore file_list

    if (file_list.size() == 0)
    {
        perror("Non è stato trova nessun file assembly nella directory corrente\n");
        perror("File *.s is missing");
        return;
    }
    else if (file_list.size() == 1)
    {
        file_choosed.insert(file_choosed.end(), file_list.at(0));
    }
    else
    {
        cout << "Inserire l indice del file assembly che si vuole compilare ed eseguire\n\n";
        int index = 0;
        for (int i = 0; i < file_list.size(); i++)
        {
            cout << i << ' ' << file_list.at(i) << "\n";
        }
        cin >> index;
        if (index > file_list.size())
        {
            return;
        }
        else
        {
            file_choosed.insert(file_choosed.end(), file_list.at(index));
        }
    }
    _start_(file_choosed);

    system("clear");
    return;
}

void getFilesInCurrentDir(const char *cmd, vector<string> *result)
{
    array<char, 128> buffer;
    int index = 0;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw runtime_error("error exec");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result->insert(result->end(), buffer.data());
        index++;
    }
}
void _start_(vector<string> file_list)
{
    system("clear");
    int index;
    bool executed = false;

    print_menu();

    while (!executed)
    {
        cin >> index;

        switch (index)
        {
        case 0:
            compile_execute(file_list, false);
            break;
        case 1:
            compile_execute(file_list, true);
            break;
        case 2:
            debug(file_list, false);
            break;
        case 3:
            debug(file_list, true);
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
        default:
            executed = true;
        }
    }
}
int check_command()
{
    int i = system("dpkg -s qemu-user");
    int j = system("dpkg -s gcc-arm-linux-gnueabihf");
    int k = system("dpkg -s gdb-multiarch");

    system("clear");
    cout << "dpkg -s qemu-user: " << i << "\n\n";
    cout << "dpkg -s gcc-arm-linux-gnueabihf: " << j << "\n\n";
    cout << "dpkg -s gdb-multiarch: " << k << "\n\n\n\n";

    return i + j + k;
}

void compile_execute(vector<string> file_list, bool flag)
{
    string cmd = "r";
    if (file_list.size() == 1)
    {
        while (cmd == "r") //costruisce il comando per un unico file
        {
            system("clear");
            string file = file_list.at(0);
            file.erase(remove(file.begin(), file.end(), '\n'), file.end());

            cmd = "arm-linux-gnueabihf-gcc " + file + " -o " + file.substr(0, file.size() - 2) + " -static";
            if (flag)
            {
                cmd = "arm-linux-gnueabihf-gcc " + file + " -o " + file.substr(0, file.size() - 2) + " -static -mfpu=neon";
            }

            system(cmd.c_str());

            cmd = "qemu-arm ./" + file.substr(0, file.size() - 2);
            system(cmd.c_str());

            cout << "\n\npremi r per ricompilare...";
            cout << "\n\npremi s per tornare al menù...\n";
            cin >> cmd;
        }
    }
    else
    {
        string files = "";
        string output_file_name = file_list.at(0); //prendo il primo file come nome di output
        output_file_name.erase(remove(output_file_name.begin(), output_file_name.end(), '\n'), output_file_name.end());

        output_file_name = output_file_name.substr(0, output_file_name.size() - 2); //rimuovo l estensione

        for (int i = 0; i < file_list.size(); i++)
        {
            files.append(" " + file_list.at(i));
        }
        files.erase(remove(files.begin(), files.end(), '\n'), files.end()); //rimuovo lo \n finale

        while (cmd == "r")
        {
            system("clear");
            cmd = "arm-linux-gnueabihf-gcc";
            cmd.append(files + " -o " + output_file_name + " -static");
            if (flag)
                cmd.append("-mfpu=neon");

            system(cmd.c_str()); //compila

            cmd = "qemu-arm ./" + output_file_name;
            system(cmd.c_str()); //esegui

            cout << "\n\npremi r per ricompilare...";
            cout << "\n\npremi s per tornare al menù...\n";
            cin >> cmd;
        }
    }

    system("clear");
    print_menu();
}
void debug(vector<string> file_list, bool flag)
{
    if (file_list.size() == 1)
    {
        system("clear");
        string cmd = "";
        string file = file_list.at(0);
        file.erase(remove(file.begin(), file.end(), '\n'), file.end());
        if (flag)
        {
            cmd = "arm-linux-gnueabihf-gcc " + file + " -o " + file.substr(0, file.size() - 2) + " -static -mfpu=neon -ggdb3";
        }
        else
        {
            cmd = "arm-linux-gnueabihf-gcc " + file + " -o " + file.substr(0, file.size() - 2) + " -static -ggdb3";
        }

        system(cmd.c_str());

        thread server(_start_server_, file);
        thread client(_start_client_, file);

        client.join();
        server.join();
    }
    else
    {
        string files = "";
        string output_file_name = file_list.at(0); //prendo il primo file come nome di output
        output_file_name.erase(remove(output_file_name.begin(), output_file_name.end(), '\n'), output_file_name.end());

        output_file_name = output_file_name.substr(0, output_file_name.size() - 2); //rimuovo l estensione

        for (int i = 0; i < file_list.size(); i++)
        {
            files.append(" " + file_list.at(i));
        }
        files.erase(remove(files.begin(), files.end(), '\n'), files.end()); //rimuovo lo \n finale

        system("clear");
        string cmd = "arm-linux-gnueabihf-gcc";
        cmd.append(files + " -o " + output_file_name + " -static -ggdb3");
        if (flag)
            cmd.append("-mfpu=neon");

        system(cmd.c_str()); //compila

        thread server(_start_server_, output_file_name.append(".c"));
        thread client(_start_client_, output_file_name);

        client.join();
        server.join();
    }

    system("clear");
    print_menu();
}
void print_menu()
{
    cout << "0 Compila ed esegui\n\n";
    cout << "1 Compila ed esegui (vmov, vadd ecc..)\n\n";
    cout << "========================================================\n\n";
    cout << "2 Debug\n\n";
    cout << "3 Debug (vmov, vadd ecc..)\n\n";
    cout << "========================================================\n\n";
    cout << "4 Istruzioni Debug\n\n";
    cout << "5 Exit\n\n";
}
void _start_server_(string file)
{
    string cmd = "qemu-arm -g 12345 ./" + file.substr(0, file.size() - 2);
    system(cmd.c_str());
}
void _start_client_(string file)
{
    string cmd = "gdb-multiarch -q --nh -ex 'set architecture arm' -ex 'file " + file.substr(0, file.size() - 2) + "' -ex 'target remote localhost:12345'";
    system(cmd.c_str());
}