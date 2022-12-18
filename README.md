# Comunicação UART entre dispositivos e leitura de sensores

Esse projeto implementa uma comunicação serial entre uma Raspberry Pi Zero W e um NodeMCU, sendo capaz de controlar o acionamento de um conjunto de sensores conectados ao NodeMCU, assim como monitorar o seu funcionamento.

## Desenvolvedor
- [Adriel Santana Oliveira](https://github.com/Pegasus77-Adriel)

## Descrição do problema
Foi implementado o protocolo de comunicação UART entre um Single Board Computer (SBC) Raspberry Pi Zero W e a plataforma de IoT NodeMCU. O sistema é comandado pelo SBC, sendo capaz de controlar o acionamento de um conjunto de sensores analógico/digitais conectados ao NodeMCU, assim como monitorar o seu funcionamento.

## Solução
### Recursos utilizados
- Raspberry Pi Zero W
- Display LCD Hitachi HD44780U
- Botões
- GPIO Extension Board
- ESP8266 NodeMCU ESP-12

### Comunicação entre dispositivos
O SBC é responsável por iniciar as comunicações, enviando solicitações para o NodeMCU, requerindo os dados dos sensores analógicos e digitais. O NodeMCU deve retornar mensagens indicando os estados dos sensores e até mesmo da placa. A tabela abaixo indica as mensagens utilizadas para comunicação entre os dois dispositivos.

![tabelas_de_comandos](https://user-images.githubusercontent.com/61716547/208269996-ccb43735-9d31-4415-bd4d-2bca67e3266f.png)

Cada mensagem descrita na tabela acima representa uma situação diferente que pode ocorrer na interação entre os dispositivos, sendo que ambos devem identificar as suas mensagens de requisição e as mensagens de resposta do outro dispositivo.

O diagrama a seguir descreve de forma simplificada os estados pelos quais os dispositivos envolvidos nesse sistema devem transitar para que seja feita a comunicação e as informações sejam apresentadas ao usuário.

### Diagrama de funcionamento
![Fluxograma(2)](https://user-images.githubusercontent.com/61716547/208270533-7a519b99-2f13-4f91-9a75-e879bec440d2.png)

A Raspberry deve iniciar a comunicação através da requisição dos dados dos sensores, essa requisição é enviada para o NodeMCU, que faz a leitura dos sensores e envia os dados para Raspberry Pi de acordo com a disponibilidade dos sensores. Os dados enviados podemindicar inclusive que houve algum problema na leitura.

Quando a resposta chega ao Raspberry Pi, alguns caracteres são enviados ao display, exibindo entre outras coisas, os dados que foram recebidos dos sensores. Então os componentes do sistema voltam para o modo de espera, aguardando uma próxima solicitação do usuário.

### Comunicação UART
O protocolo de comunicação utilizado é o UART (Universal Asynchronous Receiver/Transmitter ou Receptor/Transmissor Assíncrono Universal), que define um conjunto de regras para a troca de dados seriais entre dois dispositivos. A comunicação utiliza dois fios conectados entre o transmissor e o receptor para transmitir e receber informações entre ambas as direções.

Uma vantagem do UART é que ele é assíncrono, de forma que o transmissor e o receptor não precisam compartilhar um sinal de clock comum, com isso, ambas as extremidades devem transmitir ao mesmo tempo e em velocidade predefinida para poder ter a mesma temporização de bits, essa taxa é denominada taxa de baud. A taxa de baud utilizada no projeto foi 9600. Além de ter a mesma taxa de bauds, ambos os lados de uma conexão UART também têm que usar a mesma estrutura de frames e parâmetros.

A estrutura de frames da UART consiste em um bit inicial, um bit final, um bit de paridade e bits de dados. O bit inicial define o início da comunicação através da mudança de estado do sinal lógico, ele é seguido dos bits de dados, que nessa solução são 8 bits (1 byte), seguido do bit final, que determina a finalização da comunicação. O bit de paridade é enviando antes do bit final, servindo para determinar se existem erros nos dados transmitidos, porém não foi utilizado nessa solução.

Os pinos utilizados na conexão da UART na Raspberry Pi Zero W são os GPIO 15 (TXD) para o transmissor e GPIO16 (RXD) para o receptor, enquanto os pinos na NodeMCU são os GPIO 1 (TXD 0) para o transmissor e GPIO 3 (RXD 0) para o receptor. Para que a comunicação seja possível, o transmissor de uma placa é ligado no receptor de outra e vice-versa.

### Controlando a Raspberry
O arquivo [main.c](https://github.com/Pegasus77-Adriel/PBL-2---SISTEMAS-DIGITAIS/tree/main/main.c) é responsável por controlar a comunicação UART no Raspberry Pi Zero W, sendo responsável por iniciar a comunicação,  possuindo também métodos para enviar e receber dados via UART. Também é responsável por apresentar os dados para o usuário, assim como interfaciar a comunicação, exibindo os valores solicitados no terminal e no display LCD.

Esse arquivo utiliza a biblioteca Termios (ver Materiais de Referência) que define a interface de E/S da placa, trabalhando com arquivos do sistema operacional para efetuar a comunicação UART pelos pinos Tx e Rx. A seguir estão descritas as funções presentes no arquivo:
- *wryte_bytes()*: transmite dados via comunicação UART;
- *read_bytes()*: recebe dados via comunicação UART;
- *lerMenuInicial()*: menu de opções para transmissão de dados da uart;
- *getSensor()*: menu de opções para escolha do sensor que vai ser lido;
- *getAnalogicRequestCode()*:
- *delay()*: gera um atraso na execução do código;
- *write_str()*: escreve caracteres no display LCD.

Os arquivos [map.s](https://github.com/Pegasus77-Adriel/PBL-2---SISTEMAS-DIGITAIS/tree/main/main.c), [lcdInit.s](https://github.com/Pegasus77-Adriel/PBL-2---SISTEMAS-DIGITAIS/tree/main/lcdInit.s), [lcdWrite.s](https://github.com/Pegasus77-Adriel/PBL-2---SISTEMAS-DIGITAIS/tree/main/lcdWrite.s) e [clear.s](https://github.com/Pegasus77-Adriel/PBL-2---SISTEMAS-DIGITAIS/tree/main/clear.s) são escritos em Assembly ARMv6 e são responsáveis por fazer o mapeamento dos pinos do display e controlar as funções básicas do mesmo, como escrever e limpar o display.

### Controlando a NodeMCU
O arquivo [uart_node.ino](https://github.com/Pegasus77-Adriel/PBL-2---SISTEMAS-DIGITAIS/tree/main/uart_node.ino) é responsável por controlar a comunicação UART no NodeMCU, recebendo as solicitações da Raspberry e encaminhando as mensagens de resposta de acordo com a situação dos sensores analógico e digitais. O código presente nesse arquivo pode ser executado na plataforma Arduino IDE.

Nesse arquivo são utilizadas as bibliotecas ESP8266Wifi, ESP8266mDNS, WifiUdp e ArduinoOTA (ver Materiais de Referência) para efetuar a conexão sem fio com a NodeMCU, definindo as configurações iniciais da placa e possibilitando o envio de códigos por conexão sem fio. A configuração dos pinos é efetuada com linguagem Arduino e a lógica de comunicação da UART é feita em linguagem C. A lógica de comunicação da UART define mensagens de respostas para cada requisição que é feita para a placa.

A seguir estão descritas algumas funções da linguagem Arduino utilizadas no arquivo [uart_node.ino](https://github.com/Pegasus77-Adriel/PBL-2---SISTEMAS-DIGITAIS/tree/main/uart_node.ino):
- *pinMode()*: configura o pino especificado para funcionar como uma entrada ou saída;
- *Serial.read()*: lê os dados seriais de entrada;
- *Serial.write()*: escreve dados binários na porta serial. Esses dados são enviados como um byte ou séries de bytes;
- *Serial.print()*: imprime dados na porta serial como texto ASCII, sendo que  os valores de retorno dessa função podem assumir várias formas a depender do tipo de dado;
- *digitalWrite()*: aciona um valor HIGH ou LOW em um pino digital.

## Testes
Os casos de testes abaixo foram testados manualmente após a finalização do produto:

### Solicitar Estado da NodeMCU
O usuário irá selecionar a opção pelo terminal, e aguardar um resultado:
- O sistema irá informar que a NodeMCU está operando normalmente
- O sistema irá informar que a NodeMCU não está operando, situação ocorre quando o Arduino não finalizou a configuração do ESP8266

### Solicitar Valor do Sensor Analógico
O usuário irá selecionar a opção pelo terminal, e aguardar um resultado:
- O sistema irá informar o valor capturado pelo único sensor analógico disponível

### Solicitar Valor de um Sensor Digital
O usuário irá selecionar a opção pelo terminal, em seguida escolher um sensor de 1 a 8, que correspondem aos pinos D0 a D7. Em seguida aguarda o resultado:
- O sistema irá informar o valor capturado pelo sensor digital selecionado

### Controle do LED
O usuário pode controlar o led embutido no NodeMCU:
- O usuário solicita pelo terminal que o LED seja acesso, o sistema acenderá o LED, caso este esteja apagado.
- O usuário solicita pelo terminal que o LED seja apagado, o sistema apagará o LED, caso este esteja acesso. 

## Materiais de referência
[Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#raspberry-pi-zero-w)

[ESP8266 Arduino Core Documentation](https://readthedocs.org/projects/arduino-esp8266/downloads/pdf/latest/)

[SoftwareSerial Library](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&cad=rja&uact=8&ved=2ahUKEwjSluW4ypz7AhU_kZUCHbP9C5kQFnoECAoQAQ&url=https%3A%2F%2Fwww.arduino.cc%2Fen%2FReference%2FsoftwareSerial&usg=AOvVaw2kUbQNvvMDReS_1LIPB82g)

[HardwareSerial Library](https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/HardwareSerial.h)

[Termios Library](https://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html)
