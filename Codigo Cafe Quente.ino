#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#define DEBUG
#define L1 4// pino D2 no NodeMCU de saida para acionamento da cafeteira
#define L2 13// pino D7 no NodeMCU de saida para acionamento da bomba

OneWire  ds(2);  // pino D4 no NodeMCU de saida para acionamento do sensor de temperatura

const char* ssid = "eduteste"; 
const char* password =  "12345678";
const char* mqttServer = "tailor.cloudmqtt.com";
const int mqttPort = 16495; 
const char* mqttUser = "tpzldwef"; 
const char* mqttPassword = "6cAMJxXgordW";
const char* mqttTopicSub ="cafeteira/L1";

WiFiClient espClient;
PubSubClient client(espClient);


void setup() { 
   
  Serial.begin(115200);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) 
  {   
     delay(500);
     #ifdef DEBUG
    Serial.println("Conectando a WiFi..");
    #endif
  }
  #ifdef DEBUG
  Serial.println("Conectado na rede wifi!");
  #endif 
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.println("Conectando ao servidor MQTT...");
    #endif
    
    if (client.connect("ESP8266Client", mqttUser, mqttPassword ))
    {
      #ifdef DEBUG
      Serial.println("Conectado ao servidor MQTT!");
      #endif
 
    } else {
      #ifdef DEBUG 
 
      Serial.print("Falha ao conectar ");
      Serial.print(client.state());
      #endif
      delay(2000);
 
    }
  }
  //subscreve no tópico
  client.publish("Status ","Reiniciado!");
  client.publish("Placa","Em funcionamento!"); 
  client.subscribe(mqttTopicSub);
  client.subscribe("Temperatura");
  delay(10000);
}

void callback(char* topic,byte* payload, unsigned int length) {

  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);

  #ifdef DEBUG
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
  #endif

  //aciona saída conforme msg recebida 
  
  //o módulo RELE usado tem acionamento invertido. Se necessário ajuste para o seu modulo
  if (strMSG == "cafeteira off"){
    digitalWrite(L1, HIGH);
  }else if (strMSG == "cafeteira on"){   
     digitalWrite(L1, LOW);  
  }else if (strMSG == "bomba off"){
      digitalWrite(L2, HIGH);
  }else if (strMSG == "bomba on"){
      digitalWrite(L2, LOW);
  }
}

 void tempSensor(){
 
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }
  

  if (OneWire::crc8(addr, 7) != addr[7]) {
      return;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();

  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;

  String msg;
  char temp[58];
  msg = celsius;
  msg = msg + "°C";
  msg.toCharArray(temp,58);
  client.publish("Temperatura", temp);

  
  
  }

void reconect() {
  //Enquanto estiver desconectado
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.println("Tentando conectar ao servidor MQTT...");
    #endif
     
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP8266Client", mqttUser, mqttPassword) :
                     client.connect("ESP8266Client");

    if(conectado) {
      #ifdef DEBUG
      Serial.println("Conectado!");
      #endif
      //subscreve no tópico
      client.subscribe(mqttTopicSub, 1); //nivel de qualidade: QoS 1
    } else {
      #ifdef DEBUG
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 5s...");
      #endif
      //Aguarda 5 segundos 
      delay(5000);
    }
  }
}
 
void loop() {
  if (!client.connected()) {
    reconect();
  }
  client.loop();
  tempSensor();
}
