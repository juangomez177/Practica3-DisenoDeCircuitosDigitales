// C++ code
#include <avr/sleep.h>

// Asignación de los pines
// Pines de SALIDA
int rotacionIzq = 2;
int rotacionDer = 3;
int ledRojo = 4;
int ledVerde = 5;
int alarma = 6;
int ledAmarillo = 7;
int ledAzul = 12;

// Pines de ENTRADA
int sensorA = 8;
int sensorB = 9;
int sensorC = 10;
int sensorD = 11;

int a = 0; // PARA ALMACENAR EL VALOR DEL SENSOR A
int b = 0; // PARA ALMACENAR EL VALOR DEL SENSOR B
int c = 0; // PARA ALMACENAR EL VALOR DEL SENSOR C
int d = 0; // PARA ALMACENAR EL VALOR DEL SENSOR D

int izquierda; // PARA ALMACENAR EL VALOR SI ES IZQUIERDA
int derecha; // PARA ALMACENAR EL VALOR SI ES DERECHA
//int ninguno;//PARA ALMACENAR EL VALOR SI ES NINGUNO

// Variables para el temporizador
int segundosTotal = 0; // Para contar los segundos. Si llega a 10 segundos, se reinicia
bool bloqueoSensor = false; // Para bloquear la lectura de los sensores cuando hay una rotación en curso
bool flag = true; //Para iterar los segundos

// Variables para activar la alarma basada en estados
int giro; // 0=Izquierda, 1=Derecha
int secuenciaEsperada[] = {0, 1, 0, 0, 1, 0}; // Secuencia Izq, Der, Izq, Izq, Der, Izq
int contador = 0; // Contador de la secuencia. Si el contador llega a 6, la secuencia es correcta
bool banderaGiro = false; //Determina si se ha realizado un giro por primera vez para un estado de rotación

//Numeración del los estados posibles
enum fsm_estados { 
  izquierdaE, 
  derechaE, 
  alarmaE,
  noGiro
};
enum fsm_estados estado;

//Para visualizar el contador con el conteo de la secuencia
const int d0 = A0; // Salida binaria de 4 dígitos para represental de 0 a 9, en nuestro caso sólo será 0 a 6
const int d1 = A1;	
const int d2 = A2;	
const int d3 = A3;	

// SUBRUTINA DE INTERRUPCIÓN, que se ejecutará a 1Hz ó 1 segundo
ISR(TIMER1_COMPA_vect) {
  	//Serial.println("Ejecutando SUBRUTINA");
  	flag=false; 
  
  	// Los segundos se empiezan a contar si hay una condición de giro
  	if (izquierda==1 | derecha==1 ) {
   		segundosTotal = segundosTotal + 1;
  	}
  	
  	// Una vez completado el ciclo de 10 segundo, se reinicia el segundero y se habilita de nuevo la lectura de los sensores
  	if (segundosTotal == 10) {
    	segundosTotal = 0;
    	bloqueoSensor = false;
  	}
  	//Serial.println("");	
}

void setup() {
  
  	Serial.begin(9600);

  	// Pines de SALIDA
  	pinMode(rotacionIzq, OUTPUT);
  	pinMode(rotacionDer, OUTPUT);
  	pinMode(ledRojo, OUTPUT);
  	pinMode(ledVerde, OUTPUT);
  	pinMode(alarma, OUTPUT);

  	pinMode(d0, OUTPUT);
  	pinMode(d1, OUTPUT);
  	pinMode(d2, OUTPUT);
  	pinMode(d3, OUTPUT);


  	// Pines de ENTRADA
  	pinMode(sensorA, INPUT);
  	pinMode(sensorB, INPUT);
  	pinMode(sensorC, INPUT);
  	pinMode(sensorD, INPUT);
	
  	//Inicizalización del estado
  	estado=noGiro;
  
    // Configuración de la interrupcion
    cli(); // Detiene todas las interrupciones
    TCCR1A = 0; // Registro de configuración A
    TCCR1B = 0; // Registro de configuración B
    TCNT1 = 0; // Registro que lleva la cuenta=0
    OCR1A = 15624; // Registro de comparación: 15624=(16*10^6)/(1*1024) - 1

    TCCR1B |= (1 << WGM12); // Modo de operación como CMT
    TCCR1B |= (1 << CS12) | (1 << CS10); // preescaler 1024
    TIMSK1 |= (1 << OCIE1A); // Habilita interrupciones por Timer
    sei(); // Activa todas las interrupciones
}

void loop() {
   
    cli();
   	//Serial.println("-----------Nueva TITERACION-------------");
   
    // La lectura de los sensores será bloqueada entre intervalos de 10 segundos y si hay una rotación en curso
    if (bloqueoSensor == false){
 		
    	a = digitalRead(sensorA);
     	b = digitalRead(sensorB);
      	c = digitalRead(sensorC);
      	d = digitalRead(sensorD);

      	// Mapeado de los pines de salida de acuerdo a la expresión booleana de los valores de entrada de los sensores 
      	// ninguno = (a == 1 & c == 1 & d == 1)|(a == 0 & b == 0 & c == 0 & d == 0) |(a == 0 & b == 1 & c == 1 & d == 0);
      	izquierda = (a == 0 & b == 0 & d == 1) | (a == 1 & c == 0 & d == 1) |(a == 0 & b == 1 & c == 0 & d == 0) |(a == 1 & b == 1 & c == 1 & d == 0);
      	derecha = (b == 0 & c == 1 & d == 0) | (a == 0 & b == 1 & d == 1) | (a == 1 & c == 0 & d == 0);
		
      	// Asignación del estado de acuerdo a las entradas de los sensores
   		if (izquierda == 1){
        	estado = izquierdaE;
        	banderaGiro = false;
        	bloqueoSensor = true;

      	}else if (derecha == 1){
        	estado = derechaE;
        	banderaGiro = false;
        	bloqueoSensor = true;

      	}else{
        	estado = noGiro;
      	}
   	}
	
  	// Casos correspondientes al tipo de rotación
	switch (estado){
		
      	// Estado Inicial o De No giro
 		case noGiro:
			Serial.println("Estado es de NO GIRO");
			
      		contador = 0;
			visualizarContador();
      
      		Serial.println("");
			break;
			
		// Estado Izquierda
      	case izquierdaE:
      		Serial.println("Estado es IZQUIERDA");
      		
			giro = 0;

      		// Verifica si es la primera vez que se está realizando una rotación desde el último estado. 
      		// Ya que como el loop se ejecuta varias veces incluso para un mismo estado, el condator poría crecer indiscriminadamente
      		if (banderaGiro == false){
              	
              	// Compara la secuencia esperada con el indice del contador y el giro actual (Izquierda), de lo contario, el contador se reinicia
				if (secuenciaEsperada[contador] == giro){
					contador++;
					visualizarContador();
       					
                  	// La activación de la alarma ocurre en el estado Izquierda, al ser el último de la secuencia
                  	if(contador==6){
                   		estado=alarmaE;
                      	
                      	// Es necesario desactivar todas las asociaciones al estado de la izquierda y derecha
                      	segundosTotal=0;
                   		digitalWrite(rotacionIzq, LOW);
						digitalWrite(ledRojo, LOW);
						izquierda=0;
      					derecha=0;
                      
                      	Serial.println("");
                      	break;
                  	}
                  
				}else{
					contador = 0;
					visualizarContador();      
				}
              
				banderaGiro = true; // Garantizamos que el bloque IF se ejecute sólo 1 vez
			}

			// Tiempo de encendido del led y motor de 5 segundos
			if (segundosTotal < 5){
				digitalWrite(rotacionIzq, izquierda);
				digitalWrite(ledRojo, izquierda);

			// Tiempo de apagado del led y motor de 5 segundos, hasta que se complete el ciclo de 10 segundos
			}else if (segundosTotal >= 5){
				digitalWrite(rotacionIzq, LOW);
				digitalWrite(ledRojo, LOW);
            }
      
      		Serial.println("");
			break;
		
      	// Estado Derecha
		case derechaE:
      		Serial.println("Estado es DERECHA");
   
			giro = 1;
      
      		// Verifica si es la primera vez que se está realizando una rotación desde el último estado. 
      		// Ya que como el loop se ejecuta varias veces incluso para un mismo estado, el condator poría crecer indiscriminadamente
      		if (banderaGiro == false){
              	
              	// Compara la secuencia esperada con el indice del contador y el giro actual (Derecha), de lo contario, el contador se reinicia
				if (secuenciaEsperada[contador] == giro){
					contador++;
					visualizarContador();
                  
                }else{
					contador = 0;
					visualizarContador();              	
				}
				
              	banderaGiro = true; // Garantizamos que el bloque IF se ejecute sólo 1 vez
			}

			// Tiempo de encendido del led y motor de 4 segundos
			if (segundosTotal < 4){
				digitalWrite(rotacionDer, derecha);
				digitalWrite(ledVerde, derecha);

			// Tiempo de apagado del led y motor de 6 segundos, hasta que se complete el ciclo de 10 segundos
			}else if (segundosTotal >= 4){
				digitalWrite(rotacionDer, LOW);
				digitalWrite(ledVerde, LOW);
			}

      		Serial.println("");
			break;

      	// Estado de alarma
		case alarmaE:
     		Serial.println("Estado es ALARMA");
		 	bloqueoSensor=true;//Bloqueo de la lectura de sensores
      
      		// Encendido de la alarma intermitentemente por 10 segundos
      		sei();
      		digitalWrite(ledAmarillo, HIGH);
      		for (int i = 0; i < 10; i++) {
          		tone(alarma, 2500);
              	digitalWrite(ledAzul,HIGH);
          		delay(500);
          		noTone(alarma);
              	digitalWrite(ledAzul,LOW);
          		delay(500);
			}
      		digitalWrite(ledAmarillo, LOW);
        	cli();
      		
      		// Reinicio del contador y desbloqueo de sensores
      		contador = 0;
      		visualizarContador();
       		bloqueoSensor=false;
      
      		Serial.println("");
			break;
	}
	//Serial.println("-------------------FIN LOOP---------------");
  	//Serial.println("");
  	sei();
  
  	//delay(1000);
	//sleep_enable();
  	flag=true;
  	infiniteLoop();
}

//Médoto para visualizar el contador de la secuencia
void visualizarContador() {
  
  // Mapeado del valor del contador en los pines de salida
  digitalWrite(d0, contador & 0x01); // Bit 0
  digitalWrite(d1, (contador >> 1) & 0x01); // Bit 1
  digitalWrite(d2, (contador >> 2) & 0x01); // Bit 2
  digitalWrite(d3, (contador >> 3) & 0x01); // Bit 3
}

//Médoto para visualizar los segundos en el led azul
void infiniteLoop(){
  while(flag){
    digitalWrite(ledAzul,HIGH);
    delay(500);
    digitalWrite(ledAzul,LOW);
    delay(500);
  }
}
