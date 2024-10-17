#ifndef REGISTRO_HPP
#define REGISTRO_HPP
#include <iostream>
#include <cstring>

using namespace std;

    class Registro {
 public:
  int id;
  char titulo[300]; 
  int ano;
  char autores[150];
  int citacoes;
  char atualizacao[50]; 
  char snippet[1024];

  // Construtor com implementação dentro da classe
  Registro(int id, string titulo, int ano, string autores, int citacoes, string atualizacao, string snippet) {
    this->id = id;
    
    // Cópia segura usando strncpy
    strncpy(this->titulo, titulo.c_str(), sizeof(this->titulo) - 1);
    this->titulo[sizeof(this->titulo) - 1] = '\0';  // Garantir o terminador nulo

    this->ano = ano;

    strncpy(this->autores, autores.c_str(), sizeof(this->autores) - 1);
    this->autores[sizeof(this->autores) - 1] = '\0';  // Garantir o terminador nulo
    
    this->citacoes = citacoes;

    
    strncpy(this->atualizacao, atualizacao.c_str(), sizeof(this->atualizacao) - 1);
    this->atualizacao[sizeof(this->atualizacao) - 1] = '\0';  // Garantir o terminador nulo
    
    strncpy(this->snippet, snippet.c_str(), sizeof(this->snippet) - 1);
    this->snippet[sizeof(this->snippet) - 1] = '\0';  // Garantir o terminador nulo
  }

  void printId(){
    cout << "Id: " << this->id << endl;
  }

  // Método print com implementação dentro da classe
  void print() {
    cout << "-------------Registro:---------------" << endl;
    cout << "Id: " << this->id << endl;
    cout << "Titulo: " << this->titulo << endl;
    cout << "Ano: " << this->ano << endl;
    cout << "Autores: " << this->autores << endl;
    cout << "Citacoes: " << this->citacoes << endl;
    cout << "Atualização: " << this->atualizacao << endl;
    cout << "Snippet: " << this->snippet << endl;
    cout << "----------------------------------------" << endl;
  }
};
#endif

