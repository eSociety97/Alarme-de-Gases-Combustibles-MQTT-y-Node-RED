/**********Librerias**************************
 * Incluimos la libreria para controlar Wifi *
 * Incluimos la libreria para usar MQTT      *
 *********************************************/
#include<ESP8266WiFi.h>
#include<PubSubClient.h>

//Definimos el pin que usaremos para conectar el sensor MQ9 
#define sensor D4
//Definimos el pin que usaremos para conectar el buzzer
#define buzzer D1

/**********Datos de Wifi**********************
 * Aqui escribiremos el nombre que tiene el  *
 * Wifi y tambien la contraseña              *
 *********************************************/
const char* ssid = "AndroidAP";
const char* password = "dxdp7742";

/**********Datos de Clound MQTT **************
 * Aqui escribiremos los datos de nuestra    *
 * instancia en Clound MQTT                  *
 *********************************************/
char mqtt_servidor[50] = "postman.cloudmqtt.com";
int mqtt_puerto = 18283 ;
String mqtt_usuario = "Sensor";
const char* mqtt_clave = "conusero";
char conexion_MQTT[50];


unsigned long timerEnvioDatos = 0;

/************* Objetos ***********************
 * Creamos un objeto WifiCliente             *
 * Creamos un obejto PubSubClinte            *
 *********************************************/
WiFiClient espCliente; //Este Cliente se encargara de la comunicacion con el Wifi 
PubSubClient client(espCliente); //Este cliente utiliza le cliente anterior para poder crear la conexion MQTT

void setup(){
  pinMode(buzzer,OUTPUT); //Declaramos nuestro pin como salida 
  pinMode(sensor,INPUT); //Declaramos nuestro pin como entrada
  Serial.begin(115200); //Iniciamos la comunicacion Serial a 115 Baudios (Bits/seg)
  ConectarWifi();  //Nos conectamos al Wifi 
  client.setServer(mqtt_servidor,mqtt_puerto); //Nos conectamos a CloudMQTT 
  }
  
void loop(){
  //Verificamos si el cliente no esta conectado
  if(!client.connected()){
    //Si no esta conectado entonces volvemos a intentar conectarnos.
    reconectar();
    }
    //Si esta conectado entonces 
    else{
       if (millis() - timerEnvioDatos > 300) { 
       timerEnvioDatos=millis();

  int valor = digitalRead(sensor);//Leeremos una entrada Digital

  if(valor == HIGH){ //Si valor es HIGH o esta activo
  //Entonces publicaremos un mensaje que nos indique que no hay gases Combustibles
   tone(buzzer,0,0);
  client.publish("alerta", "Sin gases Combustibles");
  client.publish("/gases","0");
 
  }
  //Si valor es LOW o esta bajo
  else{    
    tone(buzzer,700,700);
  //Entonces publicaremos un mensaje que nos Alerte que hay gases Combustibles
  client.publish("alerta", "Alerta de Gases Combustibles");
  client.publish("/gases","100");
  
   }
  }
 }
}
//Funcion que conecta con el Servidor CloudMQTT
void reconectar(){

  uint8_t retries = 5;

  while(!client.connected()){
    Serial.print("Intentando Conexion MQTT...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff),HEX);
    mqtt_usuario.toCharArray(conexion_MQTT,50);
      if(client.connect("" ,conexion_MQTT,mqtt_clave)){
          Serial.println("Conectado");
        }else{
          Serial.print("Fallo, rc =");
          Serial.print(client.state());
          Serial.println("Intenta nuevamente en 5 Segundos");
          delay(5000);
       }      
         retries--;
     if(retries == 0){
      while(1);
      }
    }
  }
//Funcion Conectar Wifi
void ConectarWifi(){

  WiFi.begin(ssid,password);
  Serial.print("Conectado a:");
  Serial.println(ssid);

  while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print("...");
        }
              
    Serial.println("");
    Serial.println("WiFi Conectado");
  }
