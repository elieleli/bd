/* "
#include <iostream>


int main() {
  // Exemplo de uso
  Registro linha(1, "Titulo de Exemplo", 2024, "Autor Exemplo", 100, "2024-10-15", "Este é o snippet de exemplo.");
  linha.print();

  std::cout << sizeof(Registro) << endl;

  return 0;
}
 */

#include <fstream>
#include "registro.hpp"

using namespace std;

int main(){
    ofstream arquivo("teste.bin", ios::binary);
    Registro *linha = new Registro(1, "Titulo de Exemplo", 2024, "Autor Exemplo", 100, "2024-10-15", "Este é o snippet de exemplo.");

    arquivo << linha;

    arquivo.close();

    return 0;
}