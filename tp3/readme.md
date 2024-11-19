# Função `reservaAssentoVersaoA`

## Descrição
Realiza a reserva de um assento em um sistema de banco de dados como uma única transação. O processo consiste em três etapas principais:

1. Recuperar a lista de assentos disponíveis.
2. Simular a escolha do cliente por um assento aleatório.
3. Atualizar a tabela para marcar o assento escolhido como reservado.

Se ocorrer um conflito (ex.: o assento escolhido já tiver sido reservado por outro cliente), a operação será repetida até que a reserva seja bem-sucedida ou não existam mais assentos disponíveis.

---

## Parâmetros
- **`con` (connection):**  
  Um objeto de conexão ao banco de dados (ex.: criado pelo `psycopg2.connect`), usado para executar operações SQL e gerenciar transações.

---

## Retorno
- **`int`:**  
  O número de tentativas realizadas até que a reserva fosse concluída ou determinada como impossível (ex.: sem assentos disponíveis).  
  Retorna `None` em caso de erro durante o processo.

---

## Exceções Tratadas
- **`psycopg2.DatabaseError`:**  
  Qualquer erro relacionado ao banco de dados (ex.: problemas de conexão, conflitos de transação).  
  A transação é revertida (`rollback`) antes de o erro ser registrado.

- **`Exception`:**  
  Outros erros inesperados durante o processo.  
  A transação também é revertida (`rollback`), e o erro é registrado.

---

## Etapas do Processo
1. **Recuperação de Assentos Disponíveis:**
   - A função utiliza outra função chamada `qtdAssentos(con)` para obter a lista de assentos disponíveis no banco de dados.
   - Se não houver assentos disponíveis, a operação é encerrada.

2. **Escolha do Assento:**
   - Simula o comportamento do cliente com um atraso de 1 segundo (`time.sleep(1)`) e seleciona um assento aleatório utilizando a função `escolheAleatorio`.

3. **Reserva do Assento:**
   - Tenta atualizar a tabela para marcar o assento como reservado usando um comando `UPDATE`.
   - Caso a atualização não seja bem-sucedida (`cur.rowcount == 0`), reinicia o processo para permitir que o cliente escolha outro assento.

---

## Fluxo de Controle
- A função entra em um loop para tentar realizar a reserva. O loop só é encerrado em três situações:
  1. Um assento é reservado com sucesso.
  2. Não há mais assentos disponíveis.
  3. Um erro ocorre, resultando na interrupção do processo.

- Se a reserva for concluída com sucesso, a transação é confirmada (`commit`).  
- Caso contrário, a transação é revertida (`rollback`) antes de encerrar.

---

## Dependências
- **`logging`:** Utilizado para registrar informações sobre o progresso e erros.
- **`time`:** Utilizado para simular o tempo de escolha do cliente.
- **`psycopg2`:** Biblioteca para interagir com o banco de dados PostgreSQL.
- **Funções auxiliares externas:**
  - `qtdAssentos(con)`: Retorna os assentos disponíveis no banco de dados.
  - `escolheAleatorio(lista)`: Escolhe aleatoriamente um item de uma lista fornecida.

---

## Exemplo de Uso
```python
import psycopg2
from psycopg2 import sql

# Conexão com o banco de dados
con = psycopg2.connect(
    dbname="meubanco",
    user="usuario",
    password="senha",
    host="localhost",
    port=5432
)

# Reserva de um assento
tentativas = reservaAssentoVersaoA(con)

if tentativas is not None:
    print(f"Reserva realizada com sucesso após {tentativas} tentativa(s).")
else:
    print("A reserva falhou.")

```

# Função `reservaAssentoVersaoB`

A função `reservaAssentoVersaoB` realiza a reserva de um assento em um voo simulando duas transações de banco de dados:

1. **Recuperação de Assentos Disponíveis** (Transação 1)
2. **Seleção e Reserva do Assento** (Transação 2)

### Descrição

A função tenta reservar um assento em um loop até que a reserva seja bem-sucedida. Durante o processo, ela:
1. **Recupera os assentos disponíveis** no banco de dados.
2. O **cliente escolhe um assento aleatoriamente**, o que é simulado com um atraso de 1 segundo.
3. **Reserva o assento** no banco de dados, marcando-o como não disponível (reservado).
4. Se o assento já estiver reservado, tenta novamente.

A função utiliza transações do PostgreSQL para garantir que as operações de leitura e escrita no banco de dados sejam consistentes.

### Parâmetros

- **`con`** (tipo: `connection`): A conexão ativa com o banco de dados PostgreSQL. A função assume que esta conexão já está configurada e disponível para uso.

### Retorno

- Retorna um **inteiro** que representa o número de tentativas feitas para realizar a reserva do assento. Se não houver assentos disponíveis, a função retorna `None` e o processo é interrompido.

### Passos Detalhados

1. **Recuperação de Assentos Disponíveis**:
   - A função chama `qtdAssentos(con)` para obter os assentos disponíveis.
   - Se não houver assentos disponíveis, o processo é interrompido, e uma mensagem é registrada.

2. **Escolha Aleatória do Assento**:
   - O cliente é simulado escolhendo um assento aleatoriamente entre os disponíveis. Esse processo é simulado por um atraso de 1 segundo usando `time.sleep(1)`.

3. **Reserva do Assento**:
   - A função tenta reservar o assento escolhido executando uma transação SQL com um comando `UPDATE` para marcar o assento como reservado (`disp = FALSE`).
   - Se o assento já estiver reservado, a função tenta novamente até que a reserva seja bem-sucedida.

4. **Controle de Erros**:
   - Se ocorrer um erro relacionado ao banco de dados (`psycopg2.DatabaseError`), a função faz um `rollback` da transação e registra o erro.
   - Se ocorrer qualquer outro erro inesperado, a função também faz um `rollback` e registra a exceção.

5. **Finalização**:
   - Ao final do processo, o número de tentativas realizadas é retornado. Se o assento for reservado com sucesso, o loop é interrompido.

### Exemplo de Uso

```python
import psycopg2
import logging
import time

# Exemplo de conexão com o banco de dados
con = psycopg2.connect(dbname="moviesdb", user="postgres", password="senha", host="localhost", port="5432")

# Chamada da função para realizar a reserva de um assento
tentativas = reservaAssentoVersaoB(con)

print(f"Tentativas realizadas: {tentativas}")


