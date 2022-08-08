/*=======================================================
Nome do projeto:	Teste de comunicação PZEM-004T
Versão:				1.0
Últiva revisão:		07/08/2022
Desenvolvimento:	Luiz Ricardo Bitencourt

Descritivo Funcional:
Utilização da comunicação Serial com o módulo USART do PIC para coleta
de dados do PZEM-004T.

Inputs:
- 

Outputs:
- Port D: Comunicação com o Display (Data Pins)
- E0: Pino RS do Display
- E1: Pino E (Enable) do Display
- E2: Pino RW do Display

Serial:
- RC6: Pino para recepção serial
- RC7: Pino para transmissão serial

=========================================================*/
#include<xc.h>                          // Biblioteca padrão para o PIC18F4550
#include "lcd_4vias.h"                  // Biblioteca com funções do LCD
#include "serial.h"						// Biblioteca com funções da USART


// ==========================================================
// DEFINIÇÕES
// ==========================================================
#define ADDRESS     0x00        // Endereço do módulo (0x00 para broadcast)

#define READHOLDING 0x03
#define READINPUT   0x04
#define WRITESINGLE 0x06
#define CALIBRATE   0x41
#define RESETENERGY 0x42

#define TENSAO      0x0000
#define CORRENTEL   0x0001
#define CORRENTEH   0x0002
#define POTENCIAL   0x0003
#define POTENCIAH   0x0004
#define ENERGIAL    0x0005
#define ENERGIAH    0x0006
#define FREQUENCIA  0x0007
#define FP          0x0008
#define ALARM       0x0009

#define Botao_01    PORTBbits.RB1
#define Botao_02    PORTBbits.RB0
#define Botao_03    PORTBbits.RC2
#define Botao_04    PORTBbits.RC4
#define Tris_Botao_01   TRISBbits.TRISB1
#define Tris_Botao_02   TRISBbits.TRISB0
#define Tris_Botao_03   TRISCbits.TRISC2
// ==========================================================


// ==========================================================
// Variáveis e constantes globais
// ==========================================================
unsigned char finalCRC[2];
unsigned char recebido[25];
// ==========================================================


// ==========================================================
// CONFIGURAÇÕES DO MICROCONTROLADOR
// ==========================================================
#pragma config  FOSC    = HS
#pragma config  PLLDIV  = 1
#pragma config  CPUDIV  = OSC1_PLL2
#pragma config  USBDIV  = 1
#pragma config  BORV    = 0
#pragma config  WDTPS   = 1
#pragma config  MCLRE   = ON
#pragma config	PWRT=ON, LVP=OFF, IESO=OFF, WDT=OFF, PBADEN=OFF, BOR=OFF
#pragma config  CCP2MX=ON, LPT1OSC=OFF, ICPRT=OFF, XINST=OFF, VREGEN=OFF
#pragma config  CP0=OFF, CP1=OFF, CP2=OFF, CP3=OFF, CPB=OFF, CPD=OFF, WRTD = OFF
#pragma config  WRT0=OFF, WRT1=OFF, WRT2=OFF, WRT3=OFF, WRTC = OFF, WRTB = OFF
#pragma config  EBTR0=OFF, EBTR1=OFF, EBTR2=OFF, EBTR3=OFF, EBTRB=OFF
// ==========================================================


//======================================================
// Cálculo do CRC
//======================================================
void CRCcalc(unsigned char *toCalc){
    char i, j;
    long checksum = 0xFFFF;
    
    for(i = 0; i < 2; i++){
        checksum ^= *toCalc;
        toCalc++;
        for(j = 0; j < 8; j++) {
            if ((checksum & 0x01) == 0){
                checksum >>= 1;
            }
            else {
                checksum = (checksum >> 1) ^ 0xA001;
            }
        }
    }
    finalCRC[0] = checksum;
    finalCRC[1] = checksum >> 8;
}
//======================================================


//======================================================
// Coleta de dados do PZEM
// https://innovatorsguru.com/wp-content/uploads/2019/06/PZEM-004T-V3.0-Datasheet-User-Manual.pdf
//======================================================
void leituraPZEM(){
    char i = 0;
    unsigned char mensagem[6] = { ADDRESS, READINPUT, 0x00, 0x00, 0x00, 0x0A };
    
    CRCcalc(mensagem);
    
    Escreve_Serial(ADDRESS);
    Escreve_Serial(READINPUT);
    Escreve_Serial(0x00);
    Escreve_Serial(0x00);
    Escreve_Serial(0x00);
    Escreve_Serial(0x0A);
    Escreve_Serial(finalCRC[0]);
    Escreve_Serial(finalCRC[1]);
    
    while(i < 25){
        if(Verifica_Serial() == 1){
            recebido[i] = Ler_Serial();
            i++;
        }
    }
}
//======================================================


//======================================================
// Programa Principal
//======================================================
void main(void){
    // -------------------------------------------------
	// Declaração de variáveis locais
	// -------------------------------------------------
    char i = 0;
    unsigned char recebidoSemCRC[23];
    unsigned int tensao;
    unsigned long corrente;
    unsigned long potencia;
    unsigned long energia;
    unsigned int frequencia;
    unsigned int fp;
    unsigned int alarme;
    // -------------------------------------------------
	
    
    // -------------------------------------------------
	// Configurações iniciais
	// -------------------------------------------------
    Tris_Botao_01 = 1;      // Configura pino do botão como entrada
    Tris_Botao_02 = 1;      // Configura pino do botão como entrada
    Tris_Botao_03 = 1;      // Configura pino do botão como entrada
    UCON = 0x00;            //** PARA O PIC 18F4550: desativa opções de USB para utilizar a porta RC4 como entrada
    UCFGbits.UTRDIS = 1;    //** PARA O PIC 18F4550: desativa opções de USB para utilizar a porta RC4 como entrada
    TRISB = 0b00000011;     // Port B, saídas do RB2 até RB7
    
	IniciaLCD();            // Inicializa Display
	LimpaLCD();             // Limpa Display
	Inicializa_Serial();    // Inicializa módulo serial
                            // Padrão: 9600bps, 8bits, 1 stop bit, no parity
    // -------------------------------------------------
	

    // -------------------------------------------------
	// Recursos ativados uma única vez na inicialização
	// -------------------------------------------------	
	PosicionaLCD(1,1);
	StringLCD(" PZEM-004T-100A ");
	// -------------------------------------------------	

    
    // -------------------------------------------------	
    // Loop principal
    // -------------------------------------------------	
	while(1){
		if(Botao_01 == 1){
            tensao = 0;
            corrente = 0;
            potencia = 0;
            energia = 0;
            frequencia = 0;
            fp = 0;
            alarme = 0;
            
            leituraPZEM();
            
            for(i = 0; i < 23; i++){
                recebidoSemCRC[i] = recebido[i];
            }
            
            CRCcalc(recebidoSemCRC);
            
            if(finalCRC[0] == recebido[23] && finalCRC[1] == recebido[24]){
                tensao = recebido[9] << 8 | recebido[10];
                corrente = recebido[7] << 24 | recebido[8] << 16 | recebido[5] << 8 | recebido[6];
                potencia = recebido[11] << 24 | recebido[12] << 16 | recebido[9] << 8 | recebido[10];
                energia = recebido[15] << 24 | recebido[16] << 16 | recebido[13] << 8 | recebido[14];
                frequencia = recebido[17] << 8 | recebido[18];
                fp = recebido[19] << 8 | recebido[20];
                alarme = recebido[21] << 8 | recebido[22];
                
                PosicionaLCD(2,1);
                StringLCD("V = ");
                NumeroLCD(tensao);
                StringLCD(" V");
                for(i = 0; i < 5; i++) __delay_ms(100);
                
                PosicionaLCD(2,1);
                StringLCD("I = ");
                NumeroLCD(corrente);
                StringLCD(" V");
                for(i = 0; i < 5; i++) __delay_ms(100);
                
                PosicionaLCD(2,1);
                StringLCD("P = ");
                NumeroLCD(potencia);
                StringLCD(" W");
                for(i = 0; i < 5; i++) __delay_ms(100);
                
                PosicionaLCD(2,1);
                StringLCD("E = ");
                NumeroLCD(energia);
                StringLCD(" Wh");
                for(i = 0; i < 5; i++) __delay_ms(100);
                
                PosicionaLCD(2,1);
                StringLCD("f = ");
                NumeroLCD(frequencia);
                StringLCD(" Hz");
                for(i = 0; i < 5; i++) __delay_ms(100);
                
                PosicionaLCD(2,1);
                StringLCD("FP = ");
                NumeroLCD(fp);
                for(i = 0; i < 5; i++) __delay_ms(100);
                
                PosicionaLCD(2,1);
                StringLCD("Alarme: ");
                if(alarme == 0xffff) StringLCD("SIM");
                if(alarme == 0x0000) StringLCD("NAO");
                for(i = 0; i < 5; i++) __delay_ms(100);
            }
            else {
                StringLCD("Erro de CRC");
            }
		}	
	}
    // ------------------------------------------------------
}
