#include <fstream>
#include "../definicoes.hpp"
#include "../bloco/bloco.hpp"
#include "../Registro/registro.hpp"



using namespace std;

int main(){


    Bloco bloco;
    
    fstream inFile(CAMINHO_HASH, ios::binary | ios::in | ios::out);

    inFile.read(reinterpret_cast<char*>(&bloco), TAM_BLOCO);

    Registro r1(1, "titulo1", 2021, "macacos me mordam", 15, "14:45 05/05/2005", "hahahahahahahahahahahaha");
    Registro r2(2, "titulo2", 2022, "mordam me macacos", 17, "15:47 27/12/2004", "kakakakakakakakakakakaka");


    if(bloco.inserir_registro(r1) == false || bloco.inserir_registro(r2) == false){

        cout << "DEU ERRO CUMPADI" << endl;
    }

    inFile.write(reinterpret_cast<char*>(&bloco), TAM_BLOCO);

    inFile.close();
    return 0;
}