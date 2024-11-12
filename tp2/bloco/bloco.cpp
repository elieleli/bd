#include <iostream>
#include "../definicoes.hpp"
#include "bloco.hpp"

using namespace std;

int main() {
    // Criando alguns registros
    Registro r1(1, "Titulo 1", 2020, "Autor 1", 5, "2023-01-01", "Snippet 1");
    Registro r2(2, "Titulo 2", 2021, "Autor 2", 10, "2023-01-02", "Snippet 2");

    // Criando um bloco
    Bloco bloco;

    // Testando a inserção de registros
    if (bloco.inserir_registro(r1)) {
        cout << "Registro 1 inserido com sucesso!" << endl;
    } else {
        cout << "Falha ao inserir Registro 1." << endl;
    }

    if (bloco.inserir_registro(r2)) {
        cout << "Registro 2 inserido com sucesso!" << endl;
    } else {
        cout << "Falha ao inserir Registro 2." << endl;
    }

    // Tentando inserir mais um registro (deve falhar porque o limite é 2)
    Registro r3(3, "Titulo 3", 2022, "Autor 3", 15, "2023-01-03", "Snippet 3");
    if (bloco.inserir_registro(r3)) {
        cout << "Registro 3 inserido com sucesso!" << endl;
    } else {
        cout << "Falha ao inserir Registro 3, bloco cheio!" << endl;
    }

    // Testando a busca de registros
    Registro* reg = bloco.buscar_registro(1);
    if (reg) {
        cout << "Registro 1 encontrado:" << endl;
    } else {
        cout << "Registro 1 não encontrado." << endl;
    }

    reg = bloco.buscar_registro(2);
    if (reg) {
        cout << "Registro 2 encontrado:" << endl;
    } else {
        cout << "Registro 2 não encontrado." << endl;
    }

    // Tentando buscar um registro inexistente
    reg = bloco.buscar_registro(3);
    if (reg) {
        cout << "Registro 3 encontrado:" << endl;
        reg->print();
    } else {
        cout << "Registro 3 não encontrado." << endl;
    }

    // Imprimindo todos os registros do bloco
    cout << "\nTodos os registros no bloco:" << endl;
    bloco.printBlco();

    return 0;
}
