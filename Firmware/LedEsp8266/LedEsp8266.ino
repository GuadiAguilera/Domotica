#include <ESP8266WiFi.h>

const char* ssid = "Estudiantes";//escribe tu SSID
const char* password = "educar_2018";//escribe tu password
//const char* ssid = "Guadi";//escribe tu SSID
//const char* password = "13072000Guadi";//escribe tu password
//const char* ssid = "LabElectro-2";//escribe tu SSID
//const char* password = "njLHwHh43";//escribe tu password

int TouchSensor = 5; //pin del sensor D1
int EstadoLuz = 1;
int Lampara = 16; // Usaremos el pin GPIO2 de el ESP8266/DO
WiFiServer server(80);

void setup() {
  Serial.begin(115200); //Inicializamos la comunicacion con el modulo
  delay(10);
  pinMode(Lampara, OUTPUT); //Definimos el pin GPI02 como salida
  digitalWrite(Lampara, LOW); //Iniciamos con el led Apagado
  pinMode(TouchSensor, INPUT);

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

    // Regresamos una respuesta
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");


    client.print("La luz esta: ");


    if (EstadoLuz == HIGH) {
      client.print("Encendido");
    } else if(EstadoLuz== LOW){
      client.print("Apagado");
    }
    client.println("<br><br>");
    client.println("<a href=\"LED=ON\"><button>Encender</button></a>&nbsp;<a href=\"LED=OFF\"><button>Apagar</button></a></p>");
    client.println("</html>");

      if (digitalRead(TouchSensor) == HIGH) //lectura de señal del sensor
  {
    delay(150);
    if (digitalRead(TouchSensor) == HIGH) //lectura de señal del sensor
      {   
        if (EstadoLuz == HIGH)
        {
          digitalWrite(Lampara, LOW);   // si la lectura esta en alto el led se enciende
          EstadoLuz = LOW;
          Serial.println("Led OFF");
          client.print("Apagado");
        }//estado luz
        else
        {
          digitalWrite(Lampara, HIGH);    // si no existe lectura el ed se apaga o no enciende
          EstadoLuz = HIGH;
          Serial.println("Led ON");
          client.print("Encendido");

        } //else
      }
  } // if sensor

    delay(1);
    Serial.println("Cliente desconectado");
    Serial.println("");
    client.flush();

  }

  // Checamos si coincide la solicitud

  //delay(50); // tiempo de lectura
}
