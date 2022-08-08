#ifndef	SERIAL_H_
#define SERIAL_H_

// ============================================================================================ //
//					BIBLIOTECA PARA COMUNICAÇÃO SERIAL RS-232 PELA USART						//
//																								//
//	ESCOLA SENAI "A. JACOB LAFER																//
//	CURSO TÉCNICO DE ELETROELETRÔNICA															//
//	DISCIPLINA: SELDI																			//
//	DATA: 20/06/2018																			//
//	REVISÃO: B																					//
// ============================================================================================	//

// ============================================================================================	//
//										DEFINIÇÕES DE HARDWARE									//
// ============================================================================================	//
#define	Serial_RX		PORTCbits.RC7
#define Serial_TX		PORTCbits.RC6

// ============================================================================================	//
//									SUB-ROTINA DE INICIALIZAÇÃO									//
// ============================================================================================	//
void Inicializa_Serial(){
	TRISCbits.TRISC7 = 1;			// Pino de recepção deve ser setado (p.237 Datasheet)
	TRISCbits.TRISC6 = 1;			// Pino de transmissão deve ser setado (p.237 Datasheet)
	
	TXSTA = 0b00100000;				// Habilita transmissão em modo assíncrono (p.238 Datasheet)
	RCSTA = 0b10010000;				// Habilita recapção (p.239 Datasheet)
	BAUDCON = 0b01000000;			// Controle de Baudrate(p.240 Datasheet)
	//BAUDCON.BRG16 = 0;			// Gerador de BaudRate em 8-bit
	//SPBRGH	= 0x00;				// Parte alta do registrador do gerador de BaudRate (Desconsiderado)
	SPBRG = 31;						// Comunicação em 9600bps - Valores Tabelados (p. 241 Datasheet)
	
	/*======================================================|
	|	Cálculo do BaudRate									|
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
//									SUB-ROTINA DE TRANSMISSÃO									//
// ============================================================================================	//
void Escreve_Serial(char Caractere){
	TXREG = Caractere;				// Carrega o registrador de transmissão com o caractere
	while(!TXSTAbits.TRMT);			// Aguarda o fim da transmissão (quando TRMT = 1)	
}	

// ============================================================================================	//


// ============================================================================================	//
//									SUB-ROTINA DE RECEPÇÃO										//
// ============================================================================================	//
char Verifica_Serial(void) {
	return(PIR1bits.RCIF);			// RCIF = 1 quando buffer de recepção estiver cheio (p.102 Datasheet)
}	
// ============================================================================================	//


// ============================================================================================	//
//									SUB-ROTINA DE RECEPÇÃO										//
// ============================================================================================	//
char Ler_Serial(void) {
	return(RCREG);			// RCIF é zerado quando o buffer de recepção for lido
}	
// ============================================================================================	//

#endif