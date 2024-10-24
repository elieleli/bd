#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include "../Registro/registro.hpp"
#include "../hash/hash.hpp"

using namespace std;

// Função para remover aspas em um stream de entrada
void remover_aspas(istream& stream, string& resultado) {
    resultado.clear();
    char ch;

    while (stream.get(ch)) {
        if (ch == '"' && stream.peek() == ';') {
            stream.get(); // Consome o próximo caractere (;) e sai do loop
            break;
        }
        resultado += ch;
    }
}

// Função para normalizar uma string (remove caracteres inválidos, múltiplos espaços)
string formatar_str(string str) {
    // Remove caracteres não ASCII, novas linhas e aspas
    str.erase(remove_if(str.begin(), str.end(), [](unsigned char c) {
        return c > 127 || c == '\n' || c == '\"';
    }), str.end());

    // Remove múltiplos espaços
    auto novo_final = unique(str.begin(), str.end(), [](char esquerda, char direita) {
        return isspace(static_cast<unsigned char>(esquerda)) && isspace(static_cast<unsigned char>(direita));
    });
    str.erase(novo_final, str.end());

    // Remove espaços no início e no fim
    str.erase(0, str.find_first_not_of(' '));  // Remove espaços do início
    str.erase(str.find_last_not_of(' ') + 1);  // Remove espaços do fim

    return str;
}

void processa_arqv(ifstream &arquivo, Hash &hash)
{
    int id_str = 0 ;

    string linha;
    while (getline(arquivo, linha))
    {
        id_str++;
        stringstream linha_processada(linha);
        string dado;
    
        int id, ano, citacoes;
        string titulo, autores, atualizacao, snippet;

        try
        {
            // atualiza o campo id apos a remocao das aspas
            getline(linha_processada, dado, '"');
            remover_aspas(linha_processada, dado);
            id = stoi(dado);

            // atualiza o campo titulo apos a remocao das aspas
            getline(linha_processada, dado, '"');
            remover_aspas(linha_processada, dado);
            titulo = dado;

            // atualiza o campo ano apos a remocao das aspas
            getline(linha_processada, dado, '"');
            remover_aspas(linha_processada, dado);
            ano = stoi(dado);

            // atualiza o campo autores após a remocao das aspas
            getline(linha_processada, dado, '"');
            remover_aspas(linha_processada, dado);
            autores = dado;

            // atualiza o campo citacoes apos a remocao das aspas
            getline(linha_processada, dado, '"');
            remover_aspas(linha_processada, dado);
            citacoes = stoi(dado);

            // atualiza o campo atualizacao apos a remocao das aspas
            getline(linha_processada, dado, '"');
            remover_aspas(linha_processada, dado);
            atualizacao = dado;

            // atualiza o snippet apos a remocao das aspas
            getline(linha_processada, dado, '"');
            remover_aspas(linha_processada, dado);
            snippet = dado;

            // normaliza todas as strings
            titulo = formatar_str(titulo);
            autores = formatar_str(autores);
            atualizacao = formatar_str(atualizacao);
            snippet = formatar_str(snippet);

            
            try{
            Registro *registro = new Registro(id, titulo, ano,autores,citacoes,atualizacao,snippet);

            hash.insertItem(registro);
            } catch(const invalid_argument& e){
                cerr<< "ERRO AO CRIAR REGISTRO NA LINHA: " << id<< endl;
            }

        } catch (const invalid_argument& e) {
            //cout << "Erro de conversão na linha: " << id_str << endl;
            continue; // Pula para a próxima linha em caso de erro
        } catch (const out_of_range& e) {
            //cout << "Valor fora do intervalo na linha: " << id_str << endl;
            continue;
        }
    }
    arquivo.close();

    return;
}


int main() {
    // ifstream arquivo("sample_parse.csv");
    fstream arquivo_hash(CAMINHO_HASH, ios::binary | ios::out);
    Hash hash(arquivo_hash);

    // processa_arqv(arquivo, hash);

    return 0;
}
