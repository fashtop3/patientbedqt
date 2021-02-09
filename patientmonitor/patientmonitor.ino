#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED_GREEN D5
#define LED_RED D6
// button controls
#define BTN_ATTENTION D1
#define BTN_DISTRESS D2
#define BTN_CLEAR D3

//mqtt message buffer size
#define MSG_BUFFER_SIZE  (50)
#define DISTRESS_DELAY 60000
#define DISTRESS_CLEAR_DELAY 20000

//Spectranet_LTE-3EAA
const char* ssid = "BedWiFi"; //"Bed_Monitor";
const char* password = "AEE3D9LT";
const char* mqtt_server = "192.168.8.101"; //"broker.mqtt-dashboard.com";
String clientId = "bed01";
String topic = "monitor/patient/bed";
String attention_msg = "attention/" + clientId;
String distress_msg = "distress/" + clientId;
String clear_msg = "clear/" + clientId;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
bool blocker = false;
unsigned long currentMillis;
unsigned long startMillis = 0;

char msg[MSG_BUFFER_SIZE];
int value = 0;

//function prototypes
void distress_action();
void attention_action();
void setup_wifi();
void setup_pins();
void setModuleBlocker();
bool clearModuleBlocker();

String reply = "";
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  for (uint8_t i = 0; i < length; i++)
  {
    reply += (char)payload[i];
  }

  if (reply == "clear/" + clientId) {
    Serial.print("Subscribe message: ");
    Serial.println(reply);
    clearModuleBlocker();
    Serial.println("MODULE RESET");
  }
  reply = "";
  Serial.println();
}

void setup() {
  setup_pins();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


void loop() {
  currentMillis = millis();

  if (!client.connected()) {
    reconnect();
  }

  if (!blocker) {
    if (!digitalRead(BTN_ATTENTION)) {
      attentionAction();
    }

    if (!digitalRead(BTN_DISTRESS)) {

      if (startMillis <= 0) {
        startMillis = currentMillis;
      }
      else {
        if ((currentMillis - startMillis) >= DISTRESS_DELAY) {
          distressAction();
        }
      }
    }
    else {
      startMillis = 0;
    }
  }
  else {
    if (!digitalRead(BTN_CLEAR)) {
      clearAction();
    }
  }
}


void clearAction() {
  Serial.println("Clear button pressed");
  startMillis = 0;
  Serial.print("Publish message (clear): ");
  Serial.println(clientId);
  //  String topic = "monitor/" + String(clientId) + "/clear";
  client.publish(topic.c_str(), clear_msg.c_str());
  clearModuleBlocker();
}

void distressAction() {
  Serial.println("Distress button pressed");
  startMillis = 0;
  Serial.print("Publish message (distress): ");
  Serial.println(clientId);
  //  String topic = "monitor/" + String(clientId) + "/distress";
  client.publish(topic.c_str(), distress_msg.c_str());
  setModuleBlocker();

  Serial.println("Expects admin clear override from user client");
  startMillis = millis();
  while (blocker) {
    // if admin doesn't clear after DISTRESS_DELAY sec then auto clear
    if ((millis() - startMillis) >= DISTRESS_CLEAR_DELAY) {
      Serial.println("Distress auto clear: " + String(DISTRESS_CLEAR_DELAY));
      clearAction();
      break;
    }
    client.loop();
  }
}


void attentionAction() {
  Serial.println("Attention button pressed");
  Serial.print("Publish message (attention): ");
  Serial.println(clientId);
  //  String topic = "monitor/" + String(clientId) + "/attention";
  client.publish(topic.c_str(), attention_msg.c_str());
  setModuleBlocker();
}

/**
   ESP wifi module configurations
*/
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  digitalWrite(LED_GREEN, HIGH);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_GREEN, LOW);
}

/**
   Board pin configurations
*/
void setup_pins() {
  //  control buttons
  pinMode(BTN_ATTENTION, INPUT_PULLUP);
  pinMode(BTN_DISTRESS, INPUT_PULLUP);
  pinMode(BTN_CLEAR, INPUT_PULLUP);
  //  Led pins
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }

  client.subscribe("reply/patient/bed");
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}

void setModuleBlocker() {
  blocker = true;
}

bool clearModuleBlocker() {
  blocker = false;
  return !blocker;
}
