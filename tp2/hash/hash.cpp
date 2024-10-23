#include <iostream>
#include <cstring>
#include <optional>
#include "../definicoes.hpp"
#include "../bloco/bloco.hpp"
#include <bits/stdc++.h>

using namespace std;

class Hash{


    public:
    fstream &arquivo;
    

    Hash(fstream &arquivo) : arquivo(arquivo) {
        Bloco bloco;
        for (int i = 0; i < 240000; i++) { //12 blocos por bucket e 20000 buckets no vetor
            arquivo.write(reinterpret_cast<char*>(&bloco), TAM_BLOCO);
        }
    }

    // Função de hash que recebe um inteiro e retorna uma chave hash única
    int hashFunction(int key) {
        // Combinação de operações bit a bit com módulo para gerar a chave hash
        unsigned int hash = key;
        
        // Exemplo de manipulação simples com operações XOR e bit-shift
        hash = (hash ^ (hash >> 16)) * 0x45d9f3b;
        hash = (hash ^ (hash >> 16)) * 0x45d9f3b;
        hash = (hash ^ (hash >> 16));
        
        // Garante que o valor da hash seja restrito ao tamanho da tabela hash
        return (hash * (12 * TAM_BLOCO)) % 240000;
    }  

    bool insertItem(Registro &registro) {
        int pos = hashFunction(registro.id);               // Calcula a chave
        Bloco bloco;
        int cont_bloco = 0;

        while (cont_bloco < 12) {
            // Posiciona o ponteiro de leitura no bloco atual
            this->arquivo.seekg(pos, std::ios::beg);
            
            // Se não puder ler o bloco, retornar false
            this->arquivo.read(reinterpret_cast<char*>(&bloco), TAM_BLOCO);
            if (!this->arquivo) {
                return false;
            }

            // Tenta inserir o registro no bloco atual
            if (bloco.inserir_registro(registro)) {
                // Posiciona o ponteiro de escrita no bloco atual para sobrescrever
                this->arquivo.seekp(pos, std::ios::beg);
                this->arquivo.write(reinterpret_cast<char*>(&bloco), TAM_BLOCO);

                // Verifica se a escrita foi bem-sucedida
                if (!this->arquivo) {
                    return false;
                }
                
                return true; // Inserção bem-sucedida
            } else {
                // Passa para o próximo bloco no bucket
                pos += TAM_BLOCO;
                cont_bloco++;
            }
        }

        return false; // Se não conseguiu inserir em nenhum dos 12 blocos do bucket
    }

    optional<Registro> searchItem(int id) {
        int key = hashFunction(id);                         // Calcula a chave
        int pos = (key * (12 * TAM_BLOCO))%240000;                   // Calcula o offset inicial no arquivo
        Bloco bloco;
        int cont_bloco = 0;

        while (cont_bloco < 12) {
            // Posiciona o ponteiro de leitura no bloco atual
            this->arquivo.seekg(pos, std::ios::beg);
            
            // Se não puder ler o bloco, retornar std::nullopt
            this->arquivo.read(reinterpret_cast<char*>(&bloco), TAM_BLOCO);
            if (!this->arquivo) {
                return std::nullopt; // Falha ao ler o bloco
            }

            // Tenta encontrar o registro no bloco atual
            Registro* registro_encontrado = bloco.buscar_registro(id);
            if (registro_encontrado) {
                return *registro_encontrado; // Retorna o registro encontrado
            } else {
                // Passa para o próximo bloco no bucket
                pos += TAM_BLOCO;
                cont_bloco++;
            }
        }

        return std::nullopt; // Registro não encontrado
    }


};


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

    //Inserindo registros na tabela hash
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

    // Buscando os registros inseridos
    auto resultado1 = tabela_hash.searchItem(1);
    if (resultado1) {
        cout << "Registro 1 encontrado: " << resultado1->titulo << endl;
    } else {
        cout << "Registro 1 não encontrado." << endl;
    }

    auto resultado2 = tabela_hash.searchItem(2);
    if (resultado2) {
        cout << "Registro 2 encontrado: " << resultado2->titulo << endl;
    } else {
        cout << "Registro 2 não encontrado." << endl;
    }

    // Fechando o arquivo
    arquivo.close();
    return 0;
}
