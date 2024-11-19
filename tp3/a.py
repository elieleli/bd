import psycopg2
from psycopg2 import sql
from psycopg2.extensions import connection, ISOLATION_LEVEL_READ_COMMITTED, ISOLATION_LEVEL_SERIALIZABLE
import logging
import random
from typing import List
import time
import threading
import matplotlib.pyplot as plt
import statistics


# Configuração de logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# Configuração do banco de dados
config = {
    'host': 'localhost',
    'database': 'aviaotp3',
    'user': 'postgres',
    'password': 'test123'
}

def set_isolation_level(con, level: str):
    """
    Configura o nível de isolamento da transação.
    """
    if level == "read committed":
        con.set_isolation_level(ISOLATION_LEVEL_READ_COMMITTED)
        logging.info("Nível de isolamento configurado para 'read committed'.")
    elif level == "serializable":
        con.set_isolation_level(ISOLATION_LEVEL_SERIALIZABLE)
        logging.info("Nível de isolamento configurado para 'serializable'.")
    else:
        raise ValueError(f"Nível de isolamento '{level}' não é válido.")


def check_and_create_db(config):
    """
    Verifica se o banco de dados existe. Se não, cria o banco de dados.
    """
    # Conectar ao banco 'postgres', que é padrão
    con = psycopg2.connect(
        host=config['host'],
        user=config['user'],
        password=config['password'],
        dbname='postgres'
    )
    con.autocommit = True
    cur = con.cursor()

    try:
        # Verificar se o banco de dados já existe
        cur.execute(sql.SQL("SELECT 1 FROM pg_database WHERE datname = %s"), [config['database']])
        if not cur.fetchone():
            # Se não existir, criar o banco de dados
            cur.execute(sql.SQL("CREATE DATABASE {}").format(sql.Identifier(config['database'])))
            logging.info(f"Banco de dados '{config['database']}' criado com sucesso.")
        else:
            logging.info(f"O banco de dados '{config['database']}' já existe.")
    finally:
        cur.close()
        con.close()

def connect_to_db(config) -> connection:
    """
    Conecta ao banco de dados especificado.
    """
    return psycopg2.connect(
        host=config['host'],
        user=config['user'],
        password=config['password'],
        dbname=config['database']
    )

def criaTabela(con: connection) -> None:
    """
    Cria a tabela 'Assento' no banco de dados, se ela não existir.
    """
    comando = """
        CREATE TABLE IF NOT EXISTS Assento (
            num_voo INTEGER NOT NULL,
            disp BOOLEAN NOT NULL,
            PRIMARY KEY (num_voo)
        );
    """

    try:
        with con.cursor() as cur:
            cur.execute(comando)
            logging.info("Tabela criada com sucesso.")
        con.commit()
    except psycopg2.DatabaseError as db_error:
        con.rollback()
        logging.error(f"Erro ao criar tabela no banco de dados: {db_error}")
    except Exception as error:
        con.rollback()
        logging.error(f"Erro inesperado: {error}")


def populaTabela(con: connection) -> None:
    """
    Popula a tabela 'Assento' com 200 tuplas que representam Assentos disponíveis.
    Cada tupla contém:
    - num_voo: Número do Assento (de 1 a 200).
    - disp: Booleano indicando se o Assento está disponível (True inicialmente).
    """
    comando = """
        INSERT INTO Assento (num_voo, disp)
        VALUES (%s, %s)
        ON CONFLICT (num_voo) DO NOTHING;
    """

    try:
        with con.cursor() as cur:
            # Gerar 200 tuplas para inserção
            assentos = [(num_voo, True) for num_voo in range(1, 201)]
            
            # Inserir os dados
            cur.executemany(comando, assentos)
            con.commit()
            logging.info("Tabela 'assento' populada com sucesso.")
    except psycopg2.DatabaseError as db_error:
        con.rollback()
        logging.error(f"Erro ao popular tabela no banco de dados: {db_error}")
    except Exception as error:
        con.rollback()
        logging.error(f"Erro inesperado: {error}")



def qtdAssentos(con: connection) -> list:
    """
    Retorna uma lista com os números dos assentos livres na tabela 'assento',
    ou seja, as tuplas em que o valor de 'disp' é True.
    """
    comando = "SELECT num_voo FROM assento WHERE disp = TRUE;"
    
    try:
        with con.cursor() as cur:
            cur.execute(comando)
            assentos_disponiveis = cur.fetchall()  # Retorna uma lista de tuplas
            # Extraímos apenas os números dos assentos
            assentos_disponiveis = [assento[0] for assento in assentos_disponiveis]
            #logging.info(f"Assentos disponíveis: {assentos_disponiveis}")
            return assentos_disponiveis
    except psycopg2.DatabaseError as db_error:
        logging.error(f"Erro ao consultar os assentos disponíveis: {db_error}")
        raise
    except Exception as error:
        logging.error(f"Erro inesperado: {error}")
        raise

def escolheAleatorio(assentos_disponiveis: List[int]) -> int:
    """
    Escolhe aleatoriamente um assento disponível da lista fornecida.
    """
    if not assentos_disponiveis:
        raise ValueError("A lista de assentos disponíveis está vazia.")
    
    # Escolher um assento aleatório da lista
    assento_escolhido = random.choice(assentos_disponiveis)
    
    return assento_escolhido

import time

def reservaAssentoVersaoA(con: connection) -> int:
    """
    Realiza a reserva de um assento como uma única transação:
    1. Recupera os assentos disponíveis.
    2. O cliente escolhe o assento aleatoriamente (simulado com 1 segundo de espera).
    3. Atualiza a tabela para marcar o assento como reservado.
    """
    tentativas = 0
    try:
        while True:
            tentativas += 1
            with con.cursor() as cur:
                # Passo 1: Recupera os assentos disponíveis
                assentos_disponiveis = qtdAssentos(con)
                
                if not assentos_disponiveis:
                    logging.info("Não há assentos disponíveis.")
                    return tentativas
                
                # Passo 2: Cliente escolhe um assento aleatório (simulação de escolha)
                logging.info("Cliente está escolhendo um assento...")
                time.sleep(1)  # Simulando o tempo de escolha do cliente (1 segundo)
                assento_escolhido = escolheAleatorio(assentos_disponiveis)
                logging.info(f"Assento escolhido: {assento_escolhido}")
                
                # Passo 3: Atualiza a tabela para marcar o assento como reservado
                comando_update = """
                    UPDATE Assento
                    SET disp = FALSE
                    WHERE num_voo = %s AND disp = TRUE;
                """
                cur.execute(comando_update, (assento_escolhido,))
                
                # Verifica se o update foi bem-sucedido
                if cur.rowcount == 0:
                    logging.info(f"Assento {assento_escolhido} já foi reservado, tentando novamente...")
                else:
                    con.commit()
                    logging.info(f"Assento {assento_escolhido} reservado com sucesso.")
                    break  # Sai do loop após reservar com sucesso

    except psycopg2.DatabaseError as db_error:
        con.rollback()
        logging.error(f"Erro ao realizar a reserva: {db_error}")
    except Exception as error:
        con.rollback()
        logging.error(f"Erro inesperado durante o processo de reserva: {error}")
    finally:
        return tentativas

def reservaAssentoVersaoB(con: connection) -> int:
    """
    Realiza a reserva de um assento em duas transações:
    1. Recupera os assentos disponíveis (transação 1).
    2. O cliente escolhe o assento aleatoriamente (simulado com 1 segundo de espera).
    3. Atualiza a tabela para marcar o assento como reservado (transação 2).
    """
    tentativas = 0
    try:
        while True:  # Loop até encontrar um assento disponível e reservar
            tentativas += 1
            with con.cursor() as cur:
                # Passo 1: Recupera os assentos disponíveis (transação 1)
                assentos_disponiveis = qtdAssentos(con)
                
                if not assentos_disponiveis:
                    logging.info("Não há assentos disponíveis.")
                    return
                
                # Passo 2: Cliente escolhe um assento aleatório (simulação de escolha)
                logging.info("Cliente está escolhendo um assento...")
                time.sleep(1)  # Simulando o tempo de escolha do cliente (1 segundo)
                assento_escolhido = escolheAleatorio(assentos_disponiveis)
                logging.info(f"Assento escolhido: {assento_escolhido}")
            
            # Passo 3: Tenta reservar o assento escolhido (transação 2)
            with con.cursor() as cur:
                comando_update = """
                    UPDATE Assento
                    SET disp = FALSE
                    WHERE num_voo = %s AND disp = TRUE;
                """
                cur.execute(comando_update, (assento_escolhido,))
                if cur.rowcount == 0:  # Nenhuma linha foi afetada, assento já reservado
                    logging.info(f"Assento {assento_escolhido} já foi reservado, tentando novamente...")
                else:
                    con.commit()
                    logging.info(f"Assento {assento_escolhido} reservado com sucesso.")
                    break  # Sai do loop quando a reserva for bem-sucedida

    except psycopg2.DatabaseError as db_error:
        con.rollback()
        logging.error(f"Erro ao realizar a reserva: {db_error}")
    except Exception as error:
        con.rollback()
        logging.error(f"Erro inesperado durante o processo de reserva: {error}")
    finally:
        return tentativas


def reservaAssentoThread(con: connection, thread_id: int, versao: str) -> int:
    """
    Realiza a reserva de um assento para um agente de viagem em uma thread.
    Dependendo da versão especificada, utiliza a versão A ou B da função de reserva.
    Retorna o número de tentativas realizadas.
    """
    tentativas = 0
    try:
        if versao == "A":
            tentativas = reservaAssentoVersaoA(con)
        elif versao == "B":
            tentativas = reservaAssentoVersaoA(con)
        else:
            logging.error(f"[Thread {thread_id}] Versão inválida para a reserva: {versao}")
            return tentativas  # Retorna 0 se versão inválida
        logging.info(f"[Thread {thread_id}] Reserva concluída com sucesso após {tentativas} tentativa(s).")
    except psycopg2.DatabaseError as db_error:
        con.rollback()
        logging.error(f"[Thread {thread_id}] Erro ao realizar a reserva: {db_error}")
    except Exception as error:
        con.rollback()
        logging.error(f"[Thread {thread_id}] Erro inesperado durante o processo de reserva: {error}")
    return tentativas



def experimento(k: int, isolamento: str, versao: str) -> tuple:
    """
    Realiza um experimento com k agentes de viagem, um nível de isolamento especificado
    e a versão da função de reserva a ser utilizada (A ou B).
    Retorna o tempo de execução, o maior valor de tentativas, o menor valor de tentativas,
    e a média de tentativas.
    """
    try:
        start_time = time.time()  # Marca o início do tempo de execução

        # Conectar ao banco de dados
        con = connect_to_db(config)
        logging.info(f"Conexão estabelecida com o banco de dados para o experimento com {k} agentes, "
                     f"nível de isolamento '{isolamento}' e versão '{versao}'.")

        # Configura o nível de isolamento
        set_isolation_level(con, isolamento)
        
        # Estrutura para armazenar os retornos
        resultados = []
        resultados_lock = threading.Lock()  # Lock para proteger o acesso à lista de resultados

        # Função wrapper para chamar a reserva e armazenar os retornos
        def thread_func(con, thread_id, versao):
            tentativas = reservaAssentoThread(con, thread_id, versao)
            with resultados_lock:
                resultados.append(tentativas)  # Apenas o número de tentativas é armazenado

        # Cria as threads para os agentes de viagem
        threads = []
        for i in range(k):
            thread = threading.Thread(target=thread_func, args=(con, i + 1, versao))
            threads.append(thread)
            thread.start()
        
        # Espera todas as threads terminarem
        for thread in threads:
            thread.join()

        end_time = time.time()  # Marca o fim do tempo de execução
        total_time = end_time - start_time  # Calcula o tempo total de execução

        # Calcula as estatísticas
        if resultados:
            max_tentativas = max(resultados)
            min_tentativas = min(resultados)
            media_tentativas = statistics.mean(resultados)
        else:
            max_tentativas = min_tentativas = media_tentativas = 0

        logging.info(f"Resultados das threads: {resultados}")
        logging.info(f"Experimento com {k} agentes, isolamento '{isolamento}' e versão '{versao}' concluído "
                     f"em {total_time:.2f} segundos.")
        
        return total_time, max_tentativas, min_tentativas, media_tentativas

    except Exception as e:
        logging.error(f"Erro durante o experimento: {e}")
        return 0, 0, 0, 0
    finally:
        # Fechar a conexão
        try:
            con.close()
            logging.info("Conexão com o banco de dados fechada.")
        except Exception as e:
            logging.error(f"Erro ao fechar a conexão: {e}")


def executar_experimentos():
    """
    Executa os experimentos para diferentes valores de k, níveis de isolamento e versões da reserva.
    Armazena os tempos de execução e as estatísticas dos resultados.
    """
    # Estrutura para armazenar resultados
    resultados = {
        "read committed": {"A": [], "B": []},
        "serializable": {"A": [], "B": []}
    }

    for k in [1, 2, 4, 6, 8, 10]:
        for isolamento in ["read committed", "serializable"]:
            for versao in ["A", "B"]:
                # Executa o experimento e captura os valores retornados
                tempo, max_tentativas, min_tentativas, media_tentativas = experimento(k, isolamento, versao)
                
                # Armazena os resultados em um dicionário
                resultados[isolamento][versao].append({
                    "k": k,
                    "tempo": tempo,
                    "max_tentativas": max_tentativas,
                    "min_tentativas": min_tentativas,
                    "media_tentativas": media_tentativas
                })

    # Gerar gráficos com base nos resultados armazenados
    gerar_graficos(resultados)


def gerar_graficos(resultados):
    """
    Gera gráficos para os tempos de execução, máximo, mínimo e média de tentativas
    para cada par de versões de reserva e nível de isolamento.
    """
    for isolamento in resultados:
        # Inicializa listas para armazenar os dados por versão
        tempos_por_versao = {"A": [], "B": []}
        max_tentativas_por_versao = {"A": [], "B": []}
        min_tentativas_por_versao = {"A": [], "B": []}
        media_tentativas_por_versao = {"A": [], "B": []}
        
        for versao in resultados[isolamento]:
            for experimento in resultados[isolamento][versao]:
                # Extração dos dados relevantes
                tempos_por_versao[versao].append(experimento["tempo"])
                max_tentativas_por_versao[versao].append(experimento["max_tentativas"])
                min_tentativas_por_versao[versao].append(experimento["min_tentativas"])
                media_tentativas_por_versao[versao].append(experimento["media_tentativas"])
        
        # k: Número de agentes (fixado nos valores do experimento)
        k_values = [1, 2, 4, 6, 8, 10]
        
        # Gráfico 1: Tempo de execução
        plt.figure(figsize=(8, 6))
        for versao in tempos_por_versao:
            plt.plot(k_values, tempos_por_versao[versao], marker='o', label=f"Versão {versao}")
        plt.title(f"Tempo de Execução vs. Número de Agentes ({isolamento})")
        plt.xlabel("Número de Agentes (k)")
        plt.ylabel("Tempo de Execução (segundos)")
        plt.xticks(k_values)
        plt.grid(True)
        plt.legend()
        plt.tight_layout()
        plt.show()

        # Gráfico 2: Máximo de tentativas
        plt.figure(figsize=(8, 6))
        for versao in max_tentativas_por_versao:
            plt.plot(k_values, max_tentativas_por_versao[versao], marker='o', label=f"Versão {versao}")
        plt.title(f"Máximo de Tentativas vs. Número de Agentes ({isolamento})")
        plt.xlabel("Número de Agentes (k)")
        plt.ylabel("Máximo de Tentativas")
        plt.xticks(k_values)
        plt.grid(True)
        plt.legend()
        plt.tight_layout()
        plt.show()

        # Gráfico 3: Mínimo de tentativas
        plt.figure(figsize=(8, 6))
        for versao in min_tentativas_por_versao:
            plt.plot(k_values, min_tentativas_por_versao[versao], marker='o', label=f"Versão {versao}")
        plt.title(f"Mínimo de Tentativas vs. Número de Agentes ({isolamento})")
        plt.xlabel("Número de Agentes (k)")
        plt.ylabel("Mínimo de Tentativas")
        plt.xticks(k_values)
        plt.grid(True)
        plt.legend()
        plt.tight_layout()
        plt.show()

        # Gráfico 4: Média de tentativas
        plt.figure(figsize=(8, 6))
        for versao in media_tentativas_por_versao:
            plt.plot(k_values, media_tentativas_por_versao[versao], marker='o', label=f"Versão {versao}")
        plt.title(f"Média de Tentativas vs. Número de Agentes ({isolamento})")
        plt.xlabel("Número de Agentes (k)")
        plt.ylabel("Média de Tentativas")
        plt.xticks(k_values)
        plt.grid(True)
        plt.legend()
        plt.tight_layout()
        plt.show()




def main():
    con = None  # Inicializar a variável `con` para garantir que ela seja definida

    try:
        # Verificar e criar o banco de dados, se necessário
        check_and_create_db(config)
        
        # Conectar ao banco de dados
        con = connect_to_db(config)
        logging.info("Conexão estabelecida com o banco de dados.")

        # Passo 1: Criar a tabela 'assento' e popular os dados
        criaTabela(con)
        populaTabela(con)
        logging.info("Iniciando os experimentos...")
        
        # Função que executa os experimentos e gera os gráficos
        executar_experimentos()  

    except Exception as e:
        logging.error(f"Erro durante a execução do programa: {e}")
    
    finally:
        # Fechar a conexão se ela foi estabelecida corretamente
        if con:
            try:
                con.close()
                logging.info("Conexão com o banco de dados fechada.")
            except Exception as e:
                logging.error(f"Erro ao fechar a conexão: {e}")





# Chamar a função principal
if __name__ == "__main__":
    main()
