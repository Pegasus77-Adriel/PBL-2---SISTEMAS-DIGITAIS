// UNIVERSIDADE ESTADUAL DE FEIRA DE SANTANA
// PROBLEMA 2- TEC499- MI SISTEMAS DIGITAIS
// CÓDIGO DE LEITURA DE SENSOR COM COMUNICAÇÃPO UART ENTRE RASBERRY PI ZERO E A NODEMCU
// SEMESTRE 2022.2
// AUTOR - ADRIEL SANTANA OLIVEIRA

// bibliotecas utilizadas
#include <stdio.h>
#include <unistd.h>			
#include <fcntl.h>			
#include <termios.h>		
#include<string.h>
#include<time.h>

// definação de comandos
#define SITUACAO 0x03
#define ANALOGICO 0x04
#define DIGITAL 0x05
#define LED 0x06
#define TRUE 1

#define GET_NODEMCU_SITUACAO 0x03
#define GET_NODEMCU_ANALOGICO_INPUT 0x04
#define GET_NODEMCU_DIGITAL_INPUT 0x05
#define SET_ON_LED_NODEMCU 0x06
#define SET_OFF_LED_NODEMCU 0x07

// Declaração das funções da biblioteca para o display
extern void map();
extern void lcdInit();
extern void clear();
extern void lcdWrite(int a);
void delay(int number_of_seconds);
void write_str(char c[]);

// Declaração de funçõe implementadas e UART
int lerMenuInicial();
int lerMenuInicial();
unsigned char getAnalogicRequestCode();
int getSensor();
void write_bytes(int uart_filestream, unsigned char comando, unsigned char endereco);
void read_bytes(int uart_filestream);


int main(){
	
    //Abertura do arquivo da UART
	int uart_filestream = -1;
	uart_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);	//Abre em modo escrita/leitura bloqueado
	
	if (uart_filestream == -1){
		printf("\nErro: nao eh posssivel abrir o arquivo da UART.\n");
	}
	
	//Configuracao UART
	struct termios options;
	tcgetattr(uart_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart_filestream, TCIFLUSH);
	tcsetattr(uart_filestream, TCSANOW, &options);
	
	//Inicializando o Display
	map();
	lcdInit();
	write_str("Tudo Pronto");
	
	int value = 0;
	while(value != 6){            
		value = lerMenuInicial();                			// Loop Geral
		while (value < 1 || value > 6){
			value = lerMenuInicial();
		}
        
         switch(value){								// Analisando o comando escolhido				
            case 1:{
                write_bytes(uart_filestream, GET_NODEMCU_SITUACAO, 0);	      	// Enviando o comando para ESP e lendo o retorno 
                read_bytes(uart_filestream);
                break;
            }
            case 2:{
                write_bytes(uart_filestream, GET_NODEMCU_ANALOGICO_INPUT, 0);
                read_bytes(uart_filestream);
                break;
            }
            case 3:{
            	unsigned char sensor =  getAnalogicRequestCode();
                write_bytes(uart_filestream, GET_NODEMCU_DIGITAL_INPUT, sensor);
                read_bytes(uart_filestream);
                break;
            }
            case 4:{
                write_bytes(uart_filestream, SET_ON_LED_NODEMCU, 0);
                read_bytes(uart_filestream);
                break;
            }
            case 5:{
                write_bytes(uart_filestream, SET_OFF_LED_NODEMCU, 0);
                read_bytes(uart_filestream);
                break;
            }

            default: {
                printf("Operação finalizada!\n");
                break;
            }
        }
        if(value == 6)								// Opção de encerrar o programa
        	break;
        
	}
	
    	close(uart_filestream);
	
		
	return 0;	
}

// Função responsável pela escrita UART
void write_bytes(int uart_filestream, unsigned char comando, unsigned char endereco){
	//----- TX BYTES -----
	unsigned char tx_buffer[10];
	unsigned char *p_tx_buffer;
	
	p_tx_buffer = &tx_buffer[0];
	*p_tx_buffer++ = comando;
	*p_tx_buffer++ = endereco;

	if (uart_filestream != -1)
	{
		int count = write(uart_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));  // Envia os dados através da UART e ver se exis
		sleep(1);									   // analisa para checkar se não houve falha no envio
		if (count < 0)
		{
			printf("UART TX error\n");
		}
	}

}

// Função responsável pela leitura UART
void read_bytes(int uart_filestream){
		//----- CHECK FOR ANY RX BYTES -----
	if (uart_filestream != -1)
	{
		// Read up to 255 characters from the port if they are there
		unsigned char rx_buffer[100];
		int rx_length = read(uart_filestream, (void*)rx_buffer, 100);		// Realiza a leitura dos dados recebidos
		if (rx_length < 0)							// Verifica se houve erro na leitura do sensor
		{
			
			printf("Erro na leitura\n");				      
		}
		else if (rx_length == 0)
		{
			//No data waiting
			printf("Nenhum dado disponível\n");
		}
		else
		{
			//Bytes received
			rx_buffer[rx_length] = '\0';
			clear();
			switch(rx_buffer[0]){
				case 0x00:
					printf("Status: ok!\n");
					write_str("Status: ok!");
					break;
				
				case 0x1F:
					printf("Status: Problema\n");
					write_str("Status: Problema");
					break;
				case 0x50:
					printf("Led aceso\n");
					write_str("Led aceso");
					break;
				case 0x51:
					printf("Led apagado\n");
					write_str("Led apagado");
					break;
				case 0x02:
					printf("LVL Sensor: %d\n", rx_buffer[1]);
					int dado = rx_buffer[1];
					write_str("Digital val:");
					rx_buffer[0] = ' ';
					char value [16];
					sprintf(value, "%d", dado);
					write_str(value);
					break;
				case 0x01:
					printf("Analog value:\n");
					rx_buffer[0] = ' ';
					printf("%s\n",rx_buffer);
					write_str("Analog val:");
					write_str(rx_buffer);
					break;
				default:
					write_str("Read error");
					break;
			}
		}
	}
}

// Função responsável por exibir o menu principal para o usuário
int lerMenuInicial(){
	int tx_menu = 0;

	// menu de opções para transmissão de dados da uart
	printf("Escolha uma opção:\n");
	printf("1 - Solicitar a situação atual do NodeMCU\n");
	printf("2 - Solicitar o valor da entrada analógica\n");
	printf("3 - Solicitar o valor de uma das entradas digitais\n");
	printf("4 - Acender o led da NodeMCU\n");
	printf("5 - Apaga o led da NodeMCU\n");
	printf("6 - Finaliza o programa\n");

	scanf("%i", &tx_menu);
	return tx_menu;
}

// Função responsável por exibir o menu de sensores para o usuário
int getSensor(){
	int sensor = 0;

	// menu de opções para escolha do sensor que vai ser lido
	printf("Escolha uma opção:\n");
	printf("1 - Sensor 1\n");
	printf("2 - Sensor 2\n");
	printf("3 - Sensor 3\n");
	printf("4 - Sensor 4\n");
	printf("5 - Sensor 5\n");
	printf("6 - Sensor 6\n");
	printf("7 - Sensor 7\n");
	printf("8 - Sensor 8\n");

	scanf("%i", &sensor);
	return sensor;
}

// Função responsável por analisar e enviar endereço corresponde ao sensor escolhido
unsigned char getAnalogicRequestCode(){
	int sensor = getSensor();
	while(sensor < 1 || sensor > 8) {
		printf("Sensor escolhido inválido!\n");
		sensor = getSensor();
	}
	
	switch(sensor){
		//D0
		case 1:{
		        return 0x05;
		}
		//D1
		case 2:{
		        return 0x10;
		}
		//d2
		case 3:{
		        return 0x04;
		}
		//d3
		case 4:{
		        return 0x00;
		}
		//d4
		case 5:{
		        return 0x02;
		}
		//d5
		case 6:{
		        return 0x0E;
		}
		//d6
		case 7:{
		        return 0x0C;
		}
		//d7
		case 8:{
		        return 0x0D;
		}
        }
}

// Delay para o display
void delay(int number_of_seconds){
    int mili = 1000*number_of_seconds;
    clock_t start_time = clock();
    while(clock() < start_time + mili);
}


// Escrita no display 
void write_str(char word[]){
    for (int i=0; i<strlen(word); i++){
        lcdWrite(word[i]);
    }
}


