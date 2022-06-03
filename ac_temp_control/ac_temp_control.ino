#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Mitsubishi.h>

//wIFI
const char* SSID = "Olimpo4"; // Nome da rede WIFI a ser conectada
const char* PASSWORD = "472120469659"; //Senha da rede WIFI a ser conectada
WiFiClient wifiClient; //Classe WIFI

//MQTT Server

const char* BROKER_MQTT = "broker.hivemq.com"; //Broker MQTT a ser conectado
int BROKER_PORT = 1883; //Porta do Broker a ser conectada

#define ID_MQTT "nodeMCU" //ID do client
PubSubClient MQTT(wifiClient); // Classe MQTT



// Temperature sensor
const int tempPin = A0;

// AC control variables
const uint16_t kIrLed = 14;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRMitsubishiAC ac(kIrLed); //GPIO to control AC
IRsend irsend(kIrLed); //GPIO to control TV
int acTemp = 20;
int desiredTemp = 20;
boolean autoMode = false;

// Timers auxiliar variables
long now = millis();
long lastUpdate = 0;
long acUpdate = 0;

//Definição de funções a serem chamadas
void mantemConexoes();
void conectaWifi();
void conectaMQTT();
void enviaPacote();
void recebePacote(char* topic, byte* payload, unsigned int length);

/**
 * Função responsável pelo setup do nodeMCU ao inicalizar
 */
void setup() {

  pinMode(16, OUTPUT);
  pinMode(2, OUTPUT);

  //inicia o serviço de ligar a tv
  irsend.begin();

  //turn off builtin leds
  digitalWrite(16, HIGH);
  digitalWrite(2, HIGH);


  Serial.begin(115200);

  conectaWifi(); //Chamada de função para conexão WIfi
  MQTT.setServer(BROKER_MQTT, BROKER_PORT); //Chamada de fun~~ao para conexão MQTT
  MQTT.setCallback(recebePacote); //Definição de função que seta outra função quando um pacote é recebido

    //AC default values
  ac.on();
  ac.setFan(1);
  ac.setMode(kMitsubishiAcCool);
  ac.setTemp(acTemp);
  ac.setVane(kMitsubishiAcVaneAuto);
}
/*
 * Funçaõ responsável pelo loop padrão do nodemccu
 */
void loop() {
    mantemConexoes(); //Chamada de função que mantem conexões ativas
    MQTT.loop(); //Função MQTT que fica em loop para receber e enviar pacotes

    float celsius;

    now = millis();
  // Publishes new temperature every 10 seconds
  if (now - lastUpdate > 1000) {
    lastUpdate = now;
    
    //read temperature
    int analogValue = analogRead(tempPin);
    float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
    celsius = millivolts/10;
    
    char celsiusChar[8]; // Buffer big enough for 7-character float
    dtostrf(celsius, 6, 2, celsiusChar); 

    // Publishes Temperature values
    MQTT.publish("testtopic/info/temp", celsiusChar);
  
    String acStatus = "<table><tr><td>Estado</td><td>";
    if(ac.getPower()){
      acStatus += "Ligado";
      acStatus += "</td></tr><tr><td>Temperatura</td><td>";
      acStatus += ac.getTemp();
      acStatus += "</td></tr></table>";
    }else{
      acStatus += "Desligado</td></tr></table>";
    }
    
    //publish ac current details
    MQTT.publish("testtopic/info/ac", acStatus.c_str());
  }

  if(now - acUpdate > 300000){

    acUpdate = now;
    int ambientTemp = (int) celsius;
    
    if (desiredTemp != ambientTemp && autoMode){
      
      if(desiredTemp < ambientTemp && acTemp > 16){
        ac.on();
        acTemp -= 1;
        ac.setTemp(acTemp);
        ac.send();
        
      } else if (desiredTemp < ambientTemp && acTemp == 16){
        ac.off();
        ac.send();
        
      } else if (desiredTemp > ambientTemp && acTemp < 31){
        ac.on();
        acTemp += 1;
        ac.setTemp(acTemp);
        ac.send();
        
      } else if (desiredTemp > ambientTemp && acTemp == 31){
        ac.off();
        ac.send();
      }
      
    }
    
  }
  
}
/**
 * Função responsável por manter as conexões de MQTT e WIfi ativas
 */
void mantemConexoes () {
    if(!MQTT.connected()) { //Verifica se a conexão MQTT esta ativa, se não estiver, chama a função que faz a conexão
      conectaMQTT();  
    }
    conectaWifi(); //Chamada da função que faz a conexão Wifi
}

/**
 * Função responsável por fazer a conexão Wifi
 */
void conectaWifi () {
  if(WiFi.status() == WL_CONNECTED) { //Verifica se o status é igual a conectado, se for, da um return na função e não executa o resto
    return;  
  }

  Serial.print("Conetando se na rede: ");
  Serial.print(SSID);

  WiFi.begin(SSID, PASSWORD); //Tenta a conexão Wifi
  while(WiFi.status() != WL_CONNECTED) {
      delay(10);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conetado com sucesso na rede: ");
  Serial.print(SSID);
  Serial.println("IP Obtido: ");
  Serial.print(WiFi.localIP());
}

/**
 *Função responsável por fazer a conexão com o broker MQTT
 */
void conectaMQTT() {
  while(!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT: ");
    Serial.print(BROKER_MQTT);
    if(MQTT.connect(ID_MQTT)) { //Verifica se esta conectado ao broker MQTT com aquele client
       MQTT.subscribe("testtopic/demo/led16");
      MQTT.subscribe("testtopic/demo/led2");
      MQTT.subscribe("testtopic/demo/tv");
      MQTT.subscribe("testtopic/ac/auto");
      MQTT.subscribe("testtopic/ac/temp");
      Serial.println("Conetado ao Broker com sucesso!");
    } else {
      Serial.println("Não foi possivel se conectar ao broker");
      Serial.println("Nova tentativa de conexão em 10 segundos");
      delay(10000);
    }
  }
}
/**
 * Funçao responsável por atribuir as ações quando um pacote é recebido
 */
void recebePacote(char* topic, byte* message, unsigned int length) 
{
     Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp = "";
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  Serial.println();

  //turn on led 16 (demo)
  if(String(topic) =="testtopic/demo/led16"){
      Serial.println("Changing nodemcu led 16 to: ");
      if(messageTemp == "true"){
        Serial.print("on");
        digitalWrite(16, LOW);
      }
      else if(messageTemp == "false"){
        Serial.print("off");
        digitalWrite(16, HIGH);
      }
  }

  //turn on led 2 (demo)
  if(String(topic) =="testtopic/demo/led2"){
      Serial.println("Changing nodemcu led 2 to: ");
      if(messageTemp == "true"){
        Serial.print("on");
        digitalWrite(2, LOW);
      }
      else if(messageTemp == "false"){
        Serial.print("off");
        digitalWrite(2, HIGH);
      }
  }

  //turn on/off sony tv (demo)
  if(String(topic) =="testtopic/demo/tv"){
      Serial.println("Turning on/off the tv");

     irsend.sendRC5(0x40BFA857, 32);
  }

  //switch ac control to automatic mode
  if(String(topic) =="testtopic/ac/auto"){
      Serial.println("Switching AC to ");
      if(messageTemp == "true"){
        Serial.print("automatic control");
        autoMode = true;
        desiredTemp = acTemp;
      }
      else if(messageTemp == "false"){
        Serial.print("manual control");
        autoMode = false;
        ac.on();
        ac.setTemp(desiredTemp);
        ac.send();
      }
  }

  
  //manually define the ac temperature
  if(String(topic) =="testtopic/ac/temp"){
    
    acTemp = messageTemp.toInt();
    if(!autoMode){
      Serial.println("Temperature updated to: ");
      Serial.print(acTemp);
      ac.on();
      ac.setTemp(acTemp);
      ac.send();
    }else{
      Serial.println("Desired temperature updated to: ");
      Serial.print(acTemp);
      desiredTemp = acTemp;
    }
  }
  
  Serial.println();
}
