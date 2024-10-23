#ifndef BLOCO_HPP
#define BLOCO_HPP

#include <iostream>
#include <cstring>
#include "../Registro/registro.hpp"
#include "../definicoes.hpp"

using namespace std;


class Bloco{
  public:
    int registros;
    char dados[TAM_BLOCO];
    
    Bloco(){
      this->registros = 0;
      for(int i=0; i<TAM_BLOCO; i++){
        this->dados[i]=0;
      }
    }

    bool inserir_registro(Registro *registro){
      if(this->registros == 2){
        return false;
      }
      if(this->registros == 0){
        memcpy(&(this->dados[0]), registro, TAM_REG);
      }
      else{
        memcpy(&(this->dados[TAM_REG]), registro, TAM_REG);
      }

      this->registros++;
      return true;
    }

    Registro* buscar_registro(int id){
      Registro* registro = NULL;
      int tam = TAM_REG * this->registros;
      int i = 0;
      while (i<tam){
        registro = reinterpret_cast<Registro*>(&this->dados[i]);
        if(registro->id == id){ 
          return registro;
        }
        else{i += TAM_REG;}
      }
      registro = NULL;
      return registro;
    }

    void printBlco(){
      Registro* registro = NULL;
      int tam = TAM_REG * this->registros;
      int i = 0;
      while (i<tam){
        registro = reinterpret_cast<Registro*>(&this->dados[i]);
        registro->print();
        i+=TAM_REG;
      }
    }
};
#endif