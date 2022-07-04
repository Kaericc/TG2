import os
import serial
import pandas as pd
from google.cloud import storage


arduino_port = "COM5"  # Definição da porta serial relacionada ao ESP32
baud = 9600  # Band rate utilizado

# Função para criação de nomes únicos que serão utilizados nos arquivos .csv

def acha_proximo_nome(nome_arquivo):
    if not os.path.exists(nome_arquivo):
        return nome_arquivo
    base, extensao = os.path.splitext(nome_arquivo)
    numero = 1
    while True:
        nome_arquivo = '{}_{:03d}{}'.format(base, numero, extensao)
        if not os.path.exists(nome_arquivo):
            return nome_arquivo
        numero += 1


# Conexão e configuração com as portas do arduino
ser = serial.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)

# Criação do .csv único
FileName = acha_proximo_nome('analog-data.csv')

# Abertura do .csv
file = open(FileName, 'w', newline='')
print("Created file")

# Número de linhas que serão coletadas
samples = 6000
print_labels = True

# Linha em que será iniciado o salvamento dos dados no arquivo .csv aberto
line = 0

# Impressão do cabeçalho do arquivo e posteriormente do salvamento dos dados enviados pela porta serial
while line <= samples:
    if print_labels:
        if line == 0:
            print("Printing Column Headers")
        else:
            print("Line " + str(line) + ": writing...")
    getData = str(ser.readline())
    data = getData[0:][:-2]
    print(data)

    file = open(FileName, "a")
    file.write(data + "\n")  # write data with a newline
    line = line + 1

# Finalização da coleta de dados e fechamento do arquivo gerado
print("Data collection complete!")
file.close()

#####################################################################################

# Algoritmo responsável pelo envio do arquivo de coleta realizado

# Acesso ao bucket criado dentro do Google Cloud Storage
os.environ['GOOGLE_APPLICATION_CREDENTIALS'] = 'database-vibracao-1b8b2a47d686.json'

storage_client = storage.Client()

"""
Caso seja necessário, existe a possibilidade de criação de um novo bucket diretamente pelo código
"""

# Informações básicas do bucket utilizado

bucket_name = 'vibration_data_bucket'
bucket = storage_client.bucket(bucket_name)
bucket.location = 'US'
#bucket = storage_client.create_bucket(bucket)


# vars(bucket)


# Acessando o bucket especifico

my_bucket = storage_client.get_bucket('vibration_data_bucket')


# Realizando o upload dos arquivos indicados no caminho selecionado


def upload_to_bucket(blob_name, file_path, bucket_name):
    try:
        bucket = storage_client.get_bucket(bucket_name)
        blob = bucket.blob(blob_name)
        blob.upload_from_filename(file_path)
        return True
    except Exception as e:
        print(e)
        return False


file_path = r'C:\Curso'

    upload_to_bucket(FileName, os.path.join(file_path, FileName), 'vibration_data_bucket')
