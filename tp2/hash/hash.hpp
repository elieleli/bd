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
    

    explicit Hash(fstream &arquivo) : arquivo(arquivo) {
        Bloco bloco;
        for (int i = 0; i < 10; i++) { //12 blocos por bucket e 20000 buckets no vetor
            arquivo.write(reinterpret_cast<char*>(&bloco), TAM_BLOCO);
            if(!arquivo){cerr << "ERRO AO ESCREVER O BLOCO" << endl; break;}
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
        return hash % 10;
    }


    bool insertItem(Registro &registro) {
        cout << "ENTREI NA FUNCAO" << endl;
        int key = hashFunction(registro.id);               // Calcula a chave
        cout << "CALCULEI A CHAVE HASH" << endl;
        int pos = key * (12 * TAM_BLOCO);                  // Calcula o offset inicial no arquivo
        cout << "ACHEI A POSICAO" << endl;
        Bloco bloco;
        cout << "INSTANCIA DO BLOCO" << endl;
        int cont_bloco = 0;
        cout << "CONTADOR DOS BLOCOS" << endl;

        this->arquivo.clear();

        while (cont_bloco < 12) {
            // Posiciona o ponteiro de leitura no bloco atual
            this->arquivo.seekg(pos, ios::beg);
            cout << "POSICIONANDO O PONTEIRO NO COMEÇO" << endl;
            if(!this->arquivo){
                cout << "DEU ERRADO" << endl;
                return false;
            }
            cout << "DEU CERTO" << endl;
            // Se não puder ler o bloco, retornar false
            this->arquivo.read(reinterpret_cast<char*>(&bloco), TAM_BLOCO);
            cout << "LENDO O BLOCO PRE ALOCADO" << endl;
            if (!this->arquivo) {
                cout << "DEU ERRADO" << endl;
                return false;
            }
            cout << "BLOCO LIDO" << endl;
            // Tenta inserir o registro no bloco atual
            if (bloco.inserir_registro(registro)) {
                // Posiciona o ponteiro de escrita no bloco atual para sobrescrever
                cout << "VOLTANDO O PONTEIRO PARA O BLOCO ATUAL" << endl;
                this->arquivo.seekp(pos, ios::beg);
                if(!this->arquivo){cout << "DEU ERRADO" << endl; return false;}
                cout << "ESCREVENDO O BLOCO DE VOLTA" << endl;
                this->arquivo.write(reinterpret_cast<char*>(&bloco), TAM_BLOCO);
                if(!this->arquivo){cout << "DEU ERRADO" << endl; return false;}
                // Verifica se a escrita foi bem-sucedida
                // if (!this->arquivo) {
                //     return false;
                // }
                
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
            this->arquivo.seekg(pos, ios::beg);
            
            // Se não puder ler o bloco, retornar nullopt
            this->arquivo.read(reinterpret_cast<char*>(&bloco), TAM_BLOCO);
            if (!this->arquivo) {
                return nullopt; // Falha ao ler o bloco
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

        return nullopt; // Registro não encontrado
    }


};

