// C++ code

//Asignación de los pines
//Pines de SALIDA
int rotacionIzq=2;
int rotacionDer=3;
int ledRojo=4;
int ledVerde=5;

//Pines de ENTRADA
int sensorA=8;
int sensorB=9;
int sensorC=10;
int sensorD=11;

int a=0; //PARA ALMACENAR EL VALOR DEL SENSOR A
int b=0; //PARA ALMACENAR EL VALOR DEL SENSOR B
int c=0; //PARA ALMACENAR EL VALOR DEL SENSOR C
int d=0; //PARA ALMACENAR EL VALOR DEL SENSOR D

int izquierda;//PARA ALMACENAR EL VALOR SI ES IZQUIERDA
int derecha;//PARA ALMACENAR EL VALOR SI ES DERECHA
int ninguno;//PARA ALMACENAR EL VALOR SI ES NINGUNO

void setup(){
  	//Pines de SALIDA
    pinMode(rotacionIzq, OUTPUT);
    pinMode(rotacionDer, OUTPUT);
    pinMode(ledRojo, OUTPUT);
    pinMode(ledVerde, OUTPUT);
  	//pinMode(alarma, OUTPUT);
  	
  	//Pines de ENTRADA
  	pinMode(sensorA, INPUT); 
  	pinMode(sensorB, INPUT); 
  	pinMode(sensorC, INPUT); 
  	pinMode(sensorD, INPUT); 
}

void loop() {
    	a = digitalRead(sensorA);
    	b = digitalRead(sensorB);
    	c = digitalRead(sensorC);
    	d = digitalRead(sensorD);
  		//Mapeado de los pindes de salida de acuerdo a la expresión booleana de los valoesde entrada
    	//ninguno = (a == 1 & c == 1 & d == 1) || (a == 0 & b == 0 & c == 0 & d == 0) || (a == 0 & b == 1 & c == 1 & d == 0);
    	izquierda = (a == 0 & b == 0 & d == 1) || (a == 1 & c == 0 & d == 1) || (a == 0 & b == 1 & c == 0 & d == 0) || (a == 1 & b == 1 & c == 1 & d == 0);
    	derecha = (b == 0 & c == 1 & d == 0) || (a == 0 & b == 1 & d == 1) || (a == 1 & c == 0 & d == 0);
	
  	//Asignación correspondiente al tipo de rotación
    	digitalWrite(rotacionIzq, izquierda);
    	digitalWrite(ledRojo, izquierda);
    	digitalWrite(rotacionDer, derecha);
    	digitalWrite(ledVerde, derecha);
} 