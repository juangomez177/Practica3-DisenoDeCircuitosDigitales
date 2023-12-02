// C++ code
#include <avr/sleep.h>

// Asignación de los pines
// Pines de SALIDA
int rotacionIzq = 2;
int rotacionDer = 3;
int ledRojo = 4;
int ledVerde = 5;

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
int derecha;   // PARA ALMACENAR EL VALOR SI ES DERECHA
// int ninguno;//PARA ALMACENAR EL VALOR SI ES NINGUNO

// Variables para el temporizador
int i = 1;
int segundosTotal = 0; // Para contar los segundos. Si llega a 10 segundos, se reinicia
bool bloqueo = false;  // Para bloquear la lectura de los sensores cuando hay una rotación en curso

// Subrutina de interipción que se ejecutará a 1Hz ó 1 segundo
ISR(TIMER1_COMPA_vect)
{

    // Por una parte, los segundos se empiezan a contar hasta que haya una rotación.
    // Por otra parte. Si no hay rotación, no se cuentan los segundos y no se bloquea las entradas de los sensores
    if (izquierda == 1 | derecha == 1)
    {
        segundosTotal = segundosTotal + 1;
    }
    // Una vez completado el ciclo de 10 segundo, se reinicia el segundero y se habilita de nuevo la lectura de los sensores
    if (segundosTotal == 10)
    {
        segundosTotal = 0;
        bloqueo = false;
    }
}

void setup()
{
    Serial.begin(9600);

    // Pines de SALIDA
    pinMode(rotacionIzq, OUTPUT);
    pinMode(rotacionDer, OUTPUT);
    pinMode(ledRojo, OUTPUT);
    pinMode(ledVerde, OUTPUT);
    // pinMode(alarma, OUTPUT);

    // Pines de ENTRADA
    pinMode(sensorA, INPUT);
    pinMode(sensorB, INPUT);
    pinMode(sensorC, INPUT);
    pinMode(sensorD, INPUT);

    // Configuración de la interrupcion
    cli(); // Detiene todas las interrupciones

    TCCR1A = 0; // Registro de configuración A
    TCCR1B = 0; // Registro de configuración B

    TCNT1 = 0;     // Registro que lleva la cuenta=0
    OCR1A = 15624; // Registro de comparación 15624=(16*10^6)/(1*1024) - 1

    TCCR1B |= (1 << WGM12);              // Modo de operación como CMT
    TCCR1B |= (1 << CS12) | (1 << CS10); // preescaler 1024

    TIMSK1 |= (1 << OCIE1A); // Habilita interrupciones por Timer
    sei();                   // Activa todas las interrupciones
}

void loop()
{
    cli();
    // La lectura de los sensores será bloqueada entre intervalos de 10 segundos y si hay una rotación en curso
    if (bloqueo == false)
    {
        a = digitalRead(sensorA);
        b = digitalRead(sensorB);
        c = digitalRead(sensorC);
        d = digitalRead(sensorD);

        // Mapeado de los pines de salida de acuerdo a la expresión booleana de los valores de entrada de los sensores
        // ninguno = (a == 1 & c == 1 & d == 1) || (a == 0 & b == 0 & c == 0 & d == 0) || (a == 0 & b == 1 & c == 1 & d == 0);
        izquierda = (a == 0 & b == 0 & d == 1) || (a == 1 & c == 0 & d == 1) || (a == 0 & b == 1 & c == 0 & d == 0) || (a == 1 & b == 1 & c == 1 & d == 0);
        derecha = (b == 0 & c == 1 & d == 0) || (a == 0 & b == 1 & d == 1) || (a == 1 & c == 0 & d == 0);
    }

    // Asignación correspondiente al tipo de rotación
    // Izquierda
    if (izquierda == 1)
    {
        bloqueo = true;

        // Tiempo de encendido del led y motor de 5 segundos
        if (segundosTotal < 5)
        {
            digitalWrite(rotacionIzq, izquierda);
            digitalWrite(ledRojo, izquierda);

            // Tiempo de apagado del led y motor de 5 segundos, hasta que se complete el ciclo de 10 segundos
        }
        else if (segundosTotal >= 5)
        {
            digitalWrite(rotacionIzq, LOW);
            digitalWrite(ledRojo, LOW);
        }

        // Derecha
    }
    else if (derecha == 1)
    {
        bloqueo = true;

        // Tiempo de encendido del led y motor de 4 segundos
        if (segundosTotal < 4)
        {
            digitalWrite(rotacionDer, derecha);
            digitalWrite(ledVerde, derecha);

            // Tiempo de apagado del led y motor de 6 segundos, hasta que se complete el ciclo de 10 segundos
        }
        else if (segundosTotal >= 4)
        {
            digitalWrite(rotacionDer, LOW);
            digitalWrite(ledVerde, LOW);
        }
    }
    sei();
    sleep_enable();
}
