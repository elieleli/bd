#include <iostream>
#include <fstream>
#include <cstring>
#include "../Registro/registro.hpp"

using namespace std;

class Person{
    public:
    string nome;
    int idade;

    Person(string n, int i) : nome(n), idade(i){}
};

int main(){
    
    fstream arquivo("teste.bin", ios::binary | ios::out);
    
    Registro *r1 = new Registro(15, "titulo15", 2001, "autores1", 103, "00/00/0001", "hahahahahahahahaha");
    
    arquivo.write(reinterpret_cast<const char*>(r1), sizeof(Registro));
    arquivo.close();
    delete r1;
    Registro *r2 = new Registro(0, "", 0, "", 0, "", "");

    ifstream inFile("teste.bin", ios::binary | ios::in);
    inFile.read(reinterpret_cast<char*>(r2), sizeof(Registro));
    inFile.close(); 

    r2->print();
    delete r2;

    return 0;
}