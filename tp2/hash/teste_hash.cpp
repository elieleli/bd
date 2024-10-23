#include "hash.hpp"
#include <iostream>
#include <fstream>
#include "../bloco/bloco.hpp"
#include "../definicoes.hpp"
using namespace std;

int main(int argc, char **argv){
    
    fstream arquivo_teste("../Arquivos Gerados/teste.bin", ios::binary | ios::out);

    Bloco bloco;

    arquivo_teste.write(reinterpret_cast<char*>(&bloco), TAM_BLOCO);

    // Hash hash(arquivo_teste);
    
    // arquivo_teste.close();
    
    return 0;
} 