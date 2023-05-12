#include <LiquidCrystal.h>
#include "DHT.h"
#include <Ultrasonic.h>

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

//Define os pinos que serão utilizados para ligação ao display
#define DHTPIN 2     // Pino digital conectado ao sensor DHT11
#define DHTTYPE DHT11   // Tipo de sensor: DHT 11

DHT dht(DHTPIN, DHTTYPE);

int portaLDR = A5;

Ultrasonic ultrassom(8,7); //Inicializa o objeto para manipulação do sensor ultrassônico
long distance;                             

byte mac[]    = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED};


/** MQTT TE Server **/
String server = "m14.cloudmqtt.com";
int port = 12891;
String username = "vqlvmcfj";
String password = "Vzw6NIX4voxY";

String clientName = "ErickCassiLucasC"; //TODO modificar aqui! Cada cliente MQTT deve ter um nome único!

// Aqui são tratadas as mensagens que são recebidas do servidor MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message = "";
  
  for (int i=0;i<length;i++) {
    message += (char)payload[i];
  }

  Serial.println(message);
}

// Criação e inicialização dos objetos para a criação de um cliente MQTT
EthernetClient ethClient;
PubSubClient client(ethClient);

// Função para criação da conexão com o MQTT Broker
void connectToMQTTBroker() {

  // Tentativa (de forma repetida) de conexão
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect(clientName.c_str(), username.c_str(), password.c_str())) { 
      Serial.println("connected");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("trying again in 5 seconds");
      // Aguarda 5 segundos antes de tentar novamente a conexão
      delay(5000);
    }
  }
  
}

//Aqui acontecem as inscrições em tópicos
void subscribeToTopics() {
  client.subscribe("te/arduinoXX"); //TODO: modificar sufixo do nome do tópico
}

void setup()
{
 Serial.begin(9600);
 Ethernet.begin(mac);

 dht.begin();

   // Setando informações do Broker MQTT e da função de callback
 client.setServer(server.c_str(), port);
 client.setCallback(callback);

 connectToMQTTBroker();
 subscribeToTopics();

}

void loop()
{
  if(!client.connected()) {
    connectToMQTTBroker();
    subscribeToTopics();
  }
  
  float t = dht.readTemperature();
  String conT(t);
  Serial.println(conT.c_str());
  client.publish("te/temperatura",conT.c_str()); // TODO: modificar sufixo do nome do tópico
  
  float h = dht.readHumidity();
  String conH(h);
  Serial.println(conH.c_str());
  client.publish("te/umidade",conH.c_str()); // TODO: modificar sufixo do nome do tópico
  
  int luminosity = analogRead(portaLDR);
  String conL(luminosity);
  Serial.println(conL.c_str());
  client.publish("te/luminosidade",conL.c_str()); // TODO: modificar sufixo do nome do tópico
  
  
  distance = ultrassom.Ranging(CM);
  String conD(distance);
  Serial.println(conD.c_str());
  client.publish("te/distancia",conD.c_str()); // TODO: modificar sufixo do nome do tópico

  client.loop();

  delay(1000);
  
}
