#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Servo.h>

Servo myservo;

// TODO: cada bancada deve ter um MAC address diferente
byte mac[]    = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xE7}; //TODO modificar aqui!!

/** MQTT TE Server **/
String server = "m14.cloudmqtt.com";
int port = 12891;
String username = "vqlvmcfj";
String password = "Vzw6NIX4voxY";

String clientName = "arduinoClientXXXewdfljsd"; //TODO modificar aqui! Cada cliente MQTT deve ter um nome único!

int ledPin = 9;

#include <LiquidCrystal.h>
//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Aqui são tratadas as mensagens que são recebidas do servidor MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message = "";
  
  for (int i=0;i<length;i++) {
    message += (char)payload[i];
  }

  String topicStr(topic);
  Serial.println(message);

  if(topicStr.equals("te/lampada")) {
    
    if(message.equals("1")) {
      Serial.println("lampada");
      digitalWrite(ledPin, HIGH);
    }
    else if(message.equals("0")) {
      digitalWrite(ledPin, LOW);
    }
    else {
      Serial.println("Command not recognized...");
    }
  }

  if(topicStr.equals("te/servo")) {
    
    myservo.write(message.toInt());
  }

  if(topicStr.equals("te/display")) {
    lcd.clear();
    lcd.setCursor(3, 0);    //Display LCD: Posiciona o cursor na coluna 3, linha 0
    lcd.print(message); //Display LCD: Escreve no display a string "INF - IFRS"
  }


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
  client.subscribe("te/lampada"); //TODO: modificar sufixo do nome do tópico
  client.subscribe("te/servo"); //TODO: modificar sufixo do nome do tópico
  client.subscribe("te/display"); //TODO: modificar sufixo do nome do tópico
}

void setup()
{
  Serial.begin(9600);

  // Inicializando o Ethernet Shield
  Ethernet.begin(mac);
  delay(1500);

  Serial.println("Interface ethernet iniciada...");

  pinMode(ledPin, OUTPUT);
  myservo.attach(7);
  lcd.begin(16, 2);

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

  for (int posicao = 0; posicao < 3; posicao++)
 {
   lcd.scrollDisplayLeft();
   delay(300);
 }

  client.loop();
}
