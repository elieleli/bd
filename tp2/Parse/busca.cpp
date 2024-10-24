// busca.cpp
#include <iostream>
#include <fstream>
#include "../hash/hash.hpp"
using namespace std;

int main() {
    // Abrindo o arquivo binário para operações de busca
    fstream file("hash_test.dat", ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Erro ao reabrir o arquivo para leitura." << endl;
        return 1;
    }

    // Instanciando a tabela hash novamente para buscar os registros
    Hash tabela_hash1(file);

    // Buscando o Registro 1
    auto resultado1 = tabela_hash1.searchItem(1);
    if (resultado1) {
        cout << "Registro 1 encontrado: " << resultado1->titulo << endl;
    } else {
        cout << "Registro 1 não encontrado." << endl;
    }

    // Buscando o Registro 2
    auto resultado2 = tabela_hash1.searchItem(2);
    if (resultado2) {
        cout << "Registro 2 encontrado: " << resultado2->titulo << endl;
    } else {
        cout << "Registro 2 não encontrado." << endl;
    }

    // Fechando o arquivo
    file.close();
    return 0;
}
