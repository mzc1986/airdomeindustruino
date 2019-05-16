/*
  Basic MQTT example for Industruino D21G with Ethernet module
  based on the mqtt_basic example of the pubsubclient library

  This sketch demonstrates the basic capabilities of the library.
  It connects to an MQTT server as 'industruinoClient'
  - publishes "hello" to the topic "industruinoOutTopic"
  - publishes on_time to the topic "industruinoOutTopic" every 5 seconds
  - subscribes to the topic "industruinoInTopic", and switches on/off LCD backlight on 1/0

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

*/

#include <SPI.h>
#include <Ethernet2.h>        // Industruino version of Ethernet2
#include <PubSubClient.h>

#include <UC1701.h>
static UC1701 lcd;

// Update these with values suitable for your network.
byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };  // or use Industruino unique MAC
IPAddress ip(192, 168, 0, 177);
IPAddress server(192, 168, 0, 17);
//char server[] = "m24.cloudmqtt.com";
//char user[] = "nhkyubdaa";
//char pass[] = "vwT_dPtiECLfa";

EthernetClient ethClient;
PubSubClient client(ethClient);
unsigned long timestamp;    // for regular publishing

void setup()
{
  SerialUSB.begin(115200);
  pinMode(26, OUTPUT);      // LCD backlight
  digitalWrite(26, HIGH);

  lcd.begin();
  lcd.print("Industruino MQTT demo");
  SerialUSB.println("Industruino MQTT demo");

  client.setServer(server, 1883);
  //client.setServer("test.mosquitto.org", 1883);   // public test broker
  //client.setServer(server,15416);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  SerialUSB.print("Ethernet started on IP: ");
  SerialUSB.println(Ethernet.localIP());
  lcd.setCursor(0, 1);
  lcd.print("thisIP:");
  lcd.print(Ethernet.localIP());
  lcd.setCursor(0, 2);
  lcd.print("server:");
  lcd.setCursor(0, 3);
  lcd.print(server);
  lcd.print(":1883");
}

int val = 1001;

void loop() {
  if (millis() - timestamp > 1000) {        // every 5 seconds do this
    if (!client.connected()) {
      reconnect();
    } else {
      //String message;
      //message = "on_time(ms):";
      //message += String(millis());
      SerialUSB.println("SENDING [industruinoOutTopic]: " + val);
      lcd.setCursor(0, 5);
      String message = String(val);
      lcd.print("SENT " + message + "      ");
      int len = message.length() + 1;
      char buf [len];
      message.toCharArray(buf, len);
      String val = "22";
      client.publish("industruinoOutTopic/1", (char*) val.c_str());
      val = "28";
      client.publish("industruinoOutTopic/2", (char*) val.c_str());
      val = "25";
      client.publish("industruinoOutTopic/3", (char*) val.c_str());
      val = "16";
      client.publish("industruinoOutTopic/4", (char*) val.c_str());
      val = "101325";
      client.publish("industruinoOutTopic/5", (char*) val.c_str());
      val = "0";
      client.publish("industruinoOutTopic/6", (char*) val.c_str());
    }
    timestamp = millis();
  }
  client.loop();
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    SerialUSB.print("Attempting MQTT connection to server: ");
    SerialUSB.print(server);
    SerialUSB.print(" >>> ");
    lcd.setCursor(0, 5);
    lcd.print("connecting...      ");
    // Attempt to connect
    if (client.connect("industruinoClient")) {
      SerialUSB.println("connected");
      // Once connected, publish an announcement...
      client.publish("industruinoOutTopic", "hello from industruino");
      lcd.setCursor(0, 5);
      lcd.print("SENT hello      ");
      // ... and resubscribe
      client.subscribe("industruinoInTopic/6");
      client.subscribe("industruinoInTopic/7");
    } else {
      SerialUSB.print("failed, rc=");
      SerialUSB.print(client.state());
      SerialUSB.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  SerialUSB.print("RECEIVING [");
  SerialUSB.print(topic);
  SerialUSB.print("]: ");
  lcd.setCursor(0, 5);
  lcd.print("RECEIVED ");
  for (int i = 0; i < length; i++) {
    SerialUSB.print((char)payload[i]);
    lcd.print((char)payload[i]);
  }
  lcd.print("              ");
  SerialUSB.println();

  if ((char)payload[0] == '0') {
    digitalWrite(26, LOW);
  }
  if ((char)payload[0] == '1') {
    digitalWrite(26, HIGH);
  }
}
