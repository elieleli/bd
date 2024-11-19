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
```


# Função `reservaAssentoThread`

## Descrição
Gerencia a reserva de um assento em um sistema de banco de dados dentro do contexto de uma thread. A função permite alternar entre duas versões de lógica de reserva (`A` ou `B`) e utiliza tratamento robusto de erros para assegurar a consistência do banco de dados em caso de falhas.

O objetivo é registrar uma tentativa de reserva e retornar o número de tentativas realizadas, com suporte para execução concorrente.

---

## Parâmetros
- **`con` (connection):**  
  Objeto de conexão com o banco de dados (ex.: criado pelo `psycopg2.connect`), utilizado para executar operações SQL e gerenciar transações.

- **`thread_id` (int):**  
  Identificador único para a thread que está executando a função. Esse ID é utilizado para personalizar mensagens de log, facilitando o rastreamento em ambientes multithread.

- **`versao` (str):**  
  Especifica qual versão da lógica de reserva será usada. Os valores aceitos são:
  - `"A"`: Utiliza a função `reservaAssentoVersaoA`.
  - `"B"`: Utiliza a função `reservaAssentoVersaoB`.  
  Outros valores são considerados inválidos e resultam no encerramento imediato do processo.

---

## Retorno
- **`int`:**  
  O número de tentativas realizadas até a conclusão da reserva ou interrupção por erro.  
  - Retorna `0` se a versão fornecida for inválida ou se nenhum processo de reserva for iniciado devido a erros.

---

## Exceções Tratadas
- **`psycopg2.DatabaseError`:**  
  Captura erros relacionados ao banco de dados, como conflitos de transação ou problemas de conectividade.  
  - A transação é revertida (`rollback`) antes do erro ser registrado no log.

- **`Exception`:**  
  Captura outros erros inesperados.  
  - A transação também é revertida (`rollback`), e o erro é registrado no log para facilitar a depuração.

---

## Etapas do Processo
1. **Validação da Versão:**
   - A função verifica se o parâmetro `versao` é válido. Se não for, encerra o processo e registra uma mensagem de erro no log.

2. **Execução da Lógica de Reserva:**
   - De acordo com o valor de `versao`, chama a função apropriada (`reservaAssentoVersaoA` ou `reservaAssentoVersaoB`).
   - As funções chamadas são responsáveis por gerenciar a lógica específica de reserva e devem retornar o número de tentativas realizadas.

3. **Tratamento de Erros:**
   - Em caso de erro, realiza um rollback da transação e registra o incidente no log, diferenciando entre erros de banco de dados e outros erros.

4. **Registro de Sucesso:**
   - Se a reserva for bem-sucedida, registra no log o número de tentativas realizadas.

---

## Fluxo de Controle
1. A função inicia com a validação da versão fornecida.
2. Dependendo da versão, chama a função de reserva correspondente.
3. Em caso de erro, executa o rollback e retorna `0` ou o número de tentativas realizadas antes da falha.
4. Se a reserva for concluída, registra o sucesso no log e retorna o número de tentativas.

---

## Dependências
- **`logging`:**  
  Utilizado para registrar mensagens de progresso, sucesso e erros, incluindo detalhes da thread que executa a função.

- **`psycopg2`:**  
  Biblioteca usada para interagir com o banco de dados PostgreSQL.

- **Funções auxiliares externas:**
  - `reservaAssentoVersaoA(con)`: Implementação específica da lógica de reserva na versão A.
  - `reservaAssentoVersaoB(con)`: Implementação específica da lógica de reserva na versão B.

---

## Exemplo de Uso
```python
import psycopg2
import logging

# Configuração do log
logging.basicConfig(level=logging.INFO)

# Conexão com o banco de dados
con = psycopg2.connect(
    dbname="meubanco",
    user="usuario",
    password="senha",
    host="localhost",
    port=5432
)

# Reserva em uma thread específica
thread_id = 1
versao = "A"
tentativas = reservaAssentoThread(con, thread_id, versao)

if tentativas > 0:
    print(f"Reserva concluída com sucesso após {tentativas} tentativa(s).")
else:
    print("A reserva não foi realizada.")
```
# Função `experimento`

## Descrição
Executa um experimento concorrente com múltiplos agentes simulando reservas em um sistema de banco de dados.  
O experimento mede o desempenho e os resultados de uma função de reserva sob diferentes condições de concorrência e isolamento.

O processo consiste em:

1. Configurar o nível de isolamento especificado.
2. Criar e executar `k` threads, onde cada thread representa um agente de viagem tentando realizar uma reserva.
3. Coletar estatísticas sobre as tentativas realizadas por cada agente.
4. Retornar o tempo de execução total e estatísticas sobre as tentativas.

---

## Parâmetros
- **`k` (int):**  
  O número de agentes (threads) que participarão do experimento.

- **`isolamento` (str):**  
  O nível de isolamento a ser configurado para as transações. Exemplos: `'READ COMMITTED'`, `'SERIALIZABLE'`.

- **`versao` (str):**  
  A versão da função de reserva a ser utilizada. Exemplos: `'A'` ou `'B'`.

---

## Retorno
- **`tuple`:**  
  Retorna uma tupla com quatro elementos:
  1. **`tempo_total` (float):** Tempo total de execução do experimento (em segundos).
  2. **`max_tentativas` (int):** O maior número de tentativas realizadas por um agente.
  3. **`min_tentativas` (int):** O menor número de tentativas realizadas por um agente.
  4. **`media_tentativas` (float):** A média de tentativas realizadas pelos agentes.

  Retorna `(0, 0, 0, 0)` em caso de erro durante a execução.

---

## Exceções Tratadas
- **`Exception`:**  
  Captura erros genéricos durante a execução do experimento, registrando o problema em logs e retornando valores padrão.

- **Erros ao Fechar a Conexão:**  
  Quaisquer problemas ao fechar a conexão com o banco de dados também são capturados e registrados, mas não interrompem o fluxo principal.

---

## Etapas do Processo
1. **Início do Experimento:**
   - Registra o início do tempo de execução.
   - Conecta ao banco de dados e configura o nível de isolamento fornecido.

2. **Criação e Execução de Threads:**
   - Cria `k` threads, onde cada thread executa a função de reserva `reservaAssentoThread` com um identificador único e a versão fornecida.
   - Utiliza um `threading.Lock` para proteger o acesso à lista compartilhada de resultados.

3. **Coleta de Resultados:**
   - Após a execução de todas as threads, calcula as estatísticas:
     - Maior número de tentativas.
     - Menor número de tentativas.
     - Média das tentativas.

4. **Finalização:**
   - Fecha a conexão com o banco de dados, garantindo que todos os recursos sejam liberados.

---

## Dependências
- **`time`:** Para medir o tempo total de execução.  
- **`threading`:** Para criar e gerenciar threads concorrentes.  
- **`logging`:** Para registrar informações do progresso e eventuais erros.  
- **`statistics`:** Para calcular a média das tentativas.  
- **Funções auxiliares externas:**
  - `connect_to_db(config)`: Estabelece a conexão com o banco de dados.
  - `set_isolation_level(con, isolamento)`: Configura o nível de isolamento para a conexão.
  - `reservaAssentoThread(con, thread_id, versao)`: Realiza a reserva de assento para cada agente.

---

## Fluxo de Controle
- Cada thread tenta realizar reservas utilizando a função de reserva fornecida.
- Resultados são armazenados em uma lista compartilhada com sincronização via `Lock`.
- Após o término das threads, as estatísticas são calculadas e retornadas.

---

## Exemplo de Uso
```python
# Configurações do experimento
k = 10  # Número de agentes
isolamento = 'SERIALIZABLE'  # Nível de isolamento
versao = 'A'  # Versão da função de reserva

# Executa o experimento
resultado = experimento(k, isolamento, versao)

# Exibe os resultados
print(f"Tempo total: {resultado[0]:.2f} segundos")
print(f"Máximo de tentativas: {resultado[1]}")
print(f"Mínimo de tentativas: {resultado[2]}")
print(f"Média de tentativas: {resultado[3]:.2f}")
```

# Função `executar_experimentos`

## Descrição
Executa uma série de experimentos para diferentes combinações de parâmetros, incluindo valores de `k`, níveis de isolamento de transação e versões da função de reserva. A função armazena os tempos de execução e as estatísticas associadas aos experimentos, facilitando a análise de desempenho de diferentes configurações.

A função realiza experimentos com as seguintes combinações de parâmetros:
- **`k`**: número de threads (1, 2, 4, 6, 8, 10).
- **Isolamento de transação**: dois níveis - `"read committed"` e `"serializable"`.
- **Versão da função de reserva**: versões `"A"` e `"B"`.

Após a execução de todos os experimentos, os resultados são armazenados em um dicionário e, em seguida, gráficos são gerados com base nos dados coletados.

---

## Parâmetros
Nenhum parâmetro é necessário, pois os valores de `k`, níveis de isolamento e versões são pré-definidos dentro da função.

---

## Retorno
- **`None`**: A função não retorna valor. O objetivo principal é realizar os experimentos e gerar gráficos a partir dos resultados coletados.

---

## Exceções Tratadas
A função não possui tratamento explícito de exceções, mas erros podem ocorrer caso algum experimento falhe. Caso falhe, os experimentos seguintes podem ser afetados.

---

## Etapas do Processo
1. **Estruturação dos Resultados:**
   - A função define um dicionário `resultados` para armazenar os tempos e estatísticas de cada experimento. A chave principal é o nível de isolamento, e as subchaves são as versões da função de reserva.

2. **Execução dos Experimentos:**
   - A função percorre as possíveis combinações de valores de `k`, níveis de isolamento e versões, e executa o experimento chamando a função `experimento(k, isolamento, versao)`.
   - Os resultados de cada experimento são armazenados em um formato estruturado, contendo as métricas de desempenho.

3. **Geração de Gráficos:**
   - Após a coleta de todos os dados, a função `gerar_graficos(resultados)` é chamada para criar visualizações baseadas nos resultados armazenados.

---

## Fluxo de Controle
- A função realiza três loops aninhados:
  1. Um loop para os valores de `k` (número de threads).
  2. Um loop para os níveis de isolamento de transação (`"read committed"` ou `"serializable"`).
  3. Um loop para as versões da função de reserva (`"A"` ou `"B"`).

- Para cada combinação de parâmetros, o experimento é executado e os resultados são armazenados.

- Após todos os experimentos, os gráficos são gerados com base nos resultados coletados.

---

## Dependências
- **`experimento(k, isolamento, versao)`**: Função responsável por executar o experimento e retornar as métricas de desempenho (tempo, máximo, mínimo e média de tentativas).
- **`gerar_graficos(resultados)`**: Função responsável por gerar gráficos a partir dos resultados dos experimentos.

---

## Exemplo de Uso
```python
# Executa os experimentos
executar_experimentos()

# Os gráficos gerados serão baseados nos resultados dos experimentos.
```