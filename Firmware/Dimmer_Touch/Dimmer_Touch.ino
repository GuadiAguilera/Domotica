#include <ESP8266WiFi.h>

//const char* ssid = "WiFi Aguilera";//escribe tu SSID
//const char* password = "0142035152";//escribe tu password
//const char* ssid = "LabElectro-2";//escribe tu SSID
//const char* password = "njLHwHh43";//escribe tu password
const char *ssid = "Maestros";
const char *password = "docentes_2018";

int Lampara = 16; // Lampara rele...pin DO
const byte zc_pin = 5; //Entrada ZC... pin D1
const byte triac_gate = 4; //Salida PWM... pin D2
int TouchSensor = 14; //sensor...pin D5
int Valor= 0;
int EstadoLuz = 0;
bool ZC = 0;
uint16_t alpha;
WiFiServer server(80);
void ICACHE_RAM_ATTR ZC_detect ();

void setup() {
  Serial.begin(115200); //Inicializamos la comunicacion con el modulo
  delay(10);
  pinMode(Lampara, OUTPUT); //Definimos el pin GPI02 como salida
  digitalWrite(Lampara, LOW); //Iniciamos con el led Apagado
  pinMode(TouchSensor, INPUT);
  pinMode(zc_pin,INPUT_PULLUP); 
  pinMode(triac_gate, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(zc_pin),ZC_detect,FALLING);
  //attachInterrupt(0, ZC_detect, CHANGE); 
  digitalWrite(triac_gate, LOW);// Enable external interrupt (INT0)

  // Conectamos a el WiFi
  Serial.println();
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");

  // Iniciar servidor
  server.begin();
  Serial.println("Servidor iniciado");

  // Imprimimos el IP que nos asigna el moduoo para conectarnos
  Serial.print("Usa esta URL para conectarte: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}
void ZC_detect() {
  ZC = 1;
}

void loop() {
  // Checamos si un cliente se ha conectado
  WiFiClient client = server.available();
  if (client) {
    //return;
    // Espere hasta que el cliente envíe algunos datos
    Serial.println("nuevo cliente");
    while (!client.available()) {
      delay(1);
    }
    // Leemos la primer linea de la peticion
    String request = client.readStringUntil('\r');
    Serial.println(request);
    if (request.indexOf("/LED=ON") != -1) {
      digitalWrite(Lampara, HIGH);
      EstadoLuz = HIGH;
    }
    if (request.indexOf("/LED=OFF") != -1) {
      digitalWrite(Lampara, LOW);
      EstadoLuz = LOW;
    }
    if (request.indexOf("/BAJO") != -1) {
      //digitalWrite(triac_gate, HIGH);
      Valor = 255;
      Serial.println("BAJO");
    }
    if (request.indexOf("/MEDIO") != -1) {
      //digitalWrite(triac_gate, HIGH);
      Valor = 500;
      Serial.println("MEDIO");
    }
    if (request.indexOf("/ALTO") != -1) {
      //digitalWrite(triac_gate, HIGH);
      Valor = 880;
      Serial.println("ALTO");
    }
    if (request.indexOf("/ENCENDIDO") != -1) {
      //digitalWrite(triac_gate, HIGH);
      Valor = 1000;
      Serial.println("ENCENDIDO");
    }
    if (request.indexOf("/APAGADO") != -1) {
      //digitalWrite(triac_gate, HIGH);
      Valor = 73;
      Serial.println("APAGADO");
     }

    // Regresamos una respuesta
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");

    client.print("La lampara esta: ");

    if (EstadoLuz == HIGH) {
      client.print("Encendido");
    } else if (EstadoLuz == LOW) {
      client.print("Apagado");
    }
    client.println("<br><br>");
    client.println("<a href=\"ON\"><button>Encender</button></a>&nbsp;<a href=\"OFF\"><button>Apagar</button></a></p>");
    client.println("</html>");
    client.println("<br><br>");

    client.print("El dimmer esta: ");

    if(Valor == 73){
        client.print("Apagado");
    }else if (Valor == 255) {
      client.print("Encendido en un 25%");
    } else if(Valor == 500){
      client.print("Encendido en un 50%");
    }else if(Valor == 880){
      client.print("Encendido en un 75%");
    }else if(Valor == 1000){
      client.print("Encendido en un 100%");
    }
    client.println("<br><br>");
    client.println("<a href=\"APAGADO\"><button>Apagado</button></a>&nbsp;<a href=\"BAJO\"><button>Bajo</button></a>&nbsp;<a href=\"MEDIO\"><button>Medio</button></a>&nbsp;<a href=\"ALTO\"><button>Alto</button></a>&nbsp;<a href=\"ENCENDIDO\"><button>Encencido</button></a></p>");
    client.println("</html>");
    //<input type='button' name='boton' value='Boton' style='width:70px; height:25px'>

    delay(1);
    Serial.println("Cliente desconectado");
    Serial.println("");
    client.flush();

  }

  // Checamos si coincide la solicitud

  if (digitalRead(TouchSensor) == HIGH) //lectura de señal del sensor
  {
    delay(150);
    if (digitalRead(TouchSensor) == HIGH) //lectura de señal del sensor
      {   
        if (EstadoLuz == 1)
        {
          EstadoLuz = 0;
          digitalWrite(Lampara, LOW);   // si la lectura esta en alto el led se enciende
          Serial.println("OFF");
        }//ESTADO LUZ
        else
        {
          EstadoLuz = 1;
          digitalWrite(Lampara, HIGH);    // si no existe lectura el ed se apaga o no enciende
          Serial.println("ON");

        } //ELSE
      }
  } // if sensor

  if(ZC){
    if(alpha < 9500) {
      delayMicroseconds(alpha);
      digitalWrite(triac_gate, HIGH);
      delayMicroseconds(200);
      digitalWrite(triac_gate, LOW);
    }
    ZC = 0;
 
    alpha = ( 1023 - Valor ) * 10;
    if(alpha > 9500)
      alpha = 9500;
  }

  //delay(50); // tiempo de lectura
}

