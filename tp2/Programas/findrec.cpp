#include <iostream>
#include <fstream>
#include "../hash/hash.hpp"
#include "../definicoes.hpp"


int main(int argc, char** argv){
    if(argc < 2){
        cerr << "Poucos argumentos passados na chamada do programa!" << endl;
    }

    ifstream hashFile(CAMINHO_HASH);

    Hash hash();
    
    return 0;
}