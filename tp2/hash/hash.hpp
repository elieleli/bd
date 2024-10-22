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
        return hash % 240000;
    }

    bool insere_hash(Registro registro){
        int key = hashFunction(registro.id);
        int pos = key * (12*TAM_BLOCO);

    }

    bool insertItem(Registro &registro) {
        int key = hashFunction(registro.id);               // Calcula a chave
        int pos = key * (12 * TAM_BLOCO);                  // Calcula o offset inicial no arquivo
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
        int pos = key * (12 * TAM_BLOCO);                   // Calcula o offset inicial no arquivo
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

