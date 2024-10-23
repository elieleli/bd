#include "hash.hpp"
#include <iostream>
#include <fstream>
#include "../bloco/bloco.hpp"
#include "../definicoes.hpp"
#include "../Registro/registro.hpp"
using namespace std;

int main(int argc, char **argv){
    
    fstream arquivo_teste("../Arquivos Gerados/teste.bin", ios::binary | ios::out);

    Registro r1(1, "titulo1", 2001, "autores1", 103, "00/00/0001", "hahahahahahahahaha");
    Registro r2(2, "titulo2", 2002, "autores2", 104, "00/00/0002", "kakakakakakakakaka");
    Registro r3(3, "titulo3", 2003, "autores3", 105, "00/00/0003", "jajajajajajajajaja");

    Hash hash(arquivo_teste);
    

    hash.insertItem(r1);

    arquivo_teste.close();

    return 0;
} 