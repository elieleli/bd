#include <iostream>
#include <cstring>
#include <optional>
#include "hash.hpp"
#include <bits/stdc++.h>

int main() {
    // Abrindo o arquivo binário para leitura/escrita
    fstream arquivo("hash_test.dat", ios::in | ios::out | ios::binary);

    // Se o arquivo não existir, criar um novo
    if (!arquivo) {
        cout << "Arquivo não encontrado. Criando um novo arquivo..." << endl;
        arquivo.open("hash_test.dat", ios::out | ios::binary);  // Cria o arquivo
        if (!arquivo) {
            cerr << "Erro ao criar o arquivo de teste." << endl;
            return 1;
        }
        arquivo.close();
        // Reabrir o arquivo em modo de leitura/escrita
        arquivo.open("hash_test.dat", ios::in | ios::out | ios::binary);
        if (!arquivo) {
            cerr << "Erro ao reabrir o arquivo criado." << endl;
            return 1;
        }
    }

    // Instanciando a tabela hash
    Hash tabela_hash(arquivo);

    // Criando registros de exemplo
    Registro reg1(1, "Primeiro Artigo", 2022, "Autor1", 10, "10/10/2023", "Snippet do artigo 1");
    Registro reg2(2, "Segundo Artigo", 2023, "Autor2", 20, "11/10/2023", "Snippet do artigo 2");

    // Inserindo registros na tabela hash
    if (tabela_hash.insertItem(reg1)) {
        cout << "Registro 1 inserido com sucesso!" << endl;
    } else {
        cout << "Falha ao inserir Registro 1." << endl;
    }

    if (tabela_hash.insertItem(reg2)) {
        cout << "Registro 2 inserido com sucesso!" << endl;
    } else {
        cout << "Falha ao inserir Registro 2." << endl;
    }

    // Fechando o arquivo após a inserção
    arquivo.close();

    // Reabrindo o arquivo para operações de busca
    arquivo.open("hash_test.dat", ios::in | ios::out | ios::binary);
    if (!arquivo) {
        cerr << "Erro ao reabrir o arquivo para leitura." << endl;
        return 1;
    }

    // Instanciando a tabela hash novamente para buscar os registros
    Hash tabela_hash1(arquivo);

    // Buscando os registros inseridos
    auto resultado1 = tabela_hash1.searchItem(1);
    if (resultado1) {
        cout << "Registro 1 encontrado: " << resultado1->titulo << endl;
    } else {
        cout << "Registro 1 não encontrado." << endl;
    }

    auto resultado2 = tabela_hash1.searchItem(2);
    if (resultado2) {
        cout << "Registro 2 encontrado: " << resultado2->titulo << endl;
    } else {
        cout << "Registro 2 não encontrado." << endl;
    }

    // Fechando o arquivo no final do programa
    arquivo.close();
    return 0;
}
