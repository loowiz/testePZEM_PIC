#ifndef	SERIAL_H_
#define SERIAL_H_

// ============================================================================================ //
//					BIBLIOTECA PARA COMUNICA��O SERIAL RS-232 PELA USART						//
//																								//
//	ESCOLA SENAI "A. JACOB LAFER																//
//	CURSO T�CNICO DE ELETROELETR�NICA															//
//	DISCIPLINA: SELDI																			//
//	DATA: 20/06/2018																			//
//	REVIS�O: B																					//
// ============================================================================================	//

// ============================================================================================	//
//										DEFINI��ES DE HARDWARE									//
// ============================================================================================	//
#define	Serial_RX		PORTCbits.RC7
#define Serial_TX		PORTCbits.RC6

// ============================================================================================	//
//									SUB-ROTINA DE INICIALIZA��O									//
// ============================================================================================	//
void Inicializa_Serial(){
	TRISCbits.TRISC7 = 1;			// Pino de recep��o deve ser setado (p.237 Datasheet)
	TRISCbits.TRISC6 = 1;			// Pino de transmiss�o deve ser setado (p.237 Datasheet)
	
	TXSTA = 0b00100000;				// Habilita transmiss�o em modo ass�ncrono (p.238 Datasheet)
	RCSTA = 0b10010000;				// Habilita recap��o (p.239 Datasheet)
	BAUDCON = 0b01000000;			// Controle de Baudrate(p.240 Datasheet)
	//BAUDCON.BRG16 = 0;			// Gerador de BaudRate em 8-bit
	//SPBRGH	= 0x00;				// Parte alta do registrador do gerador de BaudRate (Desconsiderado)
	SPBRG = 31;						// Comunica��o em 9600bps - Valores Tabelados (p. 241 Datasheet)
	
	/*======================================================|
	|	C�lculo do BaudRate									|
	|=======================================================|
	| Ref. p. 242 do datasheet								|
	|														|
	| [SPBRGH:SPBRG] = (Fosc / (64*BaudRate)) - 1			|
	|														|
	| Para 20MHz e BaudRate de 9600:						|
	|														|
	| [SPBRGH:SPBRG] = (20000000/(64*9600)) - 1 = 31.55		|
	| [SPBRGH:SPBRG] = 31									|
	|														|
	| Com este valor, teremos um BaudRate de:				|
	|														|
	| BaudRate = 20000000 / (64*(31+1)) =~ 9765				|
	| Erro de 1.72%											|
	|======================================================*/	
	
}	
// ============================================================================================	//


// ============================================================================================	//
//									SUB-ROTINA DE TRANSMISS�O									//
// ============================================================================================	//
void Escreve_Serial(char Caractere){
	TXREG = Caractere;				// Carrega o registrador de transmiss�o com o caractere
	while(!TXSTAbits.TRMT);			// Aguarda o fim da transmiss�o (quando TRMT = 1)	
}	

// ============================================================================================	//


// ============================================================================================	//
//									SUB-ROTINA DE RECEP��O										//
// ============================================================================================	//
char Verifica_Serial(void) {
	return(PIR1bits.RCIF);			// RCIF = 1 quando buffer de recep��o estiver cheio (p.102 Datasheet)
}	
// ============================================================================================	//


// ============================================================================================	//
//									SUB-ROTINA DE RECEP��O										//
// ============================================================================================	//
char Ler_Serial(void) {
	return(RCREG);			// RCIF � zerado quando o buffer de recep��o for lido
}	
// ============================================================================================	//

#endif