#include <iostream>
#include <fstream>
#include "../hash/hash.hpp"
#include "../definicoes.hpp"


int main(int argc, char** argv){
    if(argc < 2){
        cerr << "Poucos argumentos passados na chamada do programa!" << endl;
        return 1;
    }

    fstream hashFile(CAMINHO_HASH, ios::binary | ios::in);

    Hash hash(hashFile);
    int blocos_lidos;

    std::optional<Registro> registro = hash.searchItem(stoi(argv[1]), blocos_lidos);

    // registro->print();
    cout << "Numero de blocos lidos: " << blocos_lidos << endl;
    return 0;
}