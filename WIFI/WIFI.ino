#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <EMailSender.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define PING_PERIOD_DAYTIME 30000
#define PING_PERIOD_NIGHTTIME 60000
#define DOMAIN_EVENT_REPORTER "report-event"
#define REQUEST_TYPE_GET "GET"
#define REQUEST_TYPE_POST "POST"
#define MAX_EVENTS 16
#define EVENT_SENDER_ROUTINE_PERIOD  10000
//<editor-fold desc="HTML" defaultstate="collapsed">
char webpage[] = R"=====(
<html>
<head>
  <script>
    var Socket;
    var routerIP="79.119.34.62";
    var userID=0;
    var multiplier=5000;
    function init() {               /*window.location.hostname*/
                                    /*79.119.34.62*/
      Socket = new WebSocket('ws://' + routerIP + ':443/');
      Socket.onmessage = function(event){
        document.getElementById("rxConsole").value += event.data;
        document.getElementById("rxConsole").scrollTop = document.getElementById("rxConsole").scrollHeight; 
      }
      window.onbeforeunload = function()
      {
        Socket.send("ne a parasit "+userID);
      }
      Socket.onopen=function(event){
        userID=Math.floor(Math.random()*multiplier);
        setTimeout(function(){Socket.send("User "+userID);},500);
      }
      /*setTimeout(
    function() {
      Socket.send()
    }, 5000);*/
    }
    function sendText(){
      Socket.send(document.getElementById("txBar").value);
      document.getElementById("txBar").value = "";
    }
    function sendBrightness(){
      Socket.send("#"+document.getElementById("brightness").value);
    }
  </script>

</head>
<body onload="javascript:init()">
  <div>
    <textarea id="rxConsole" rows="10" cols="50"></textarea>
  </div>
  <hr/>
  <div>
    <input type="text" id="txBar" onkeydown="if(event.keyCode == 13) sendText();" />
  </div>
  <hr/>
  <div>
    <input type="range" min="0" max="1023" value="512" id="brightness" oninput="sendBrightness();" />
  </div>  
</body>
</html>
)=====";
//</editor-fold>

//<editor-fold desc="Variabile" defaultstate="collapsed">
ESP8266WebServer server;
WebSocketsServer webSocket = WebSocketsServer(443);
WebSocketsClient webSocketClient;
const long utcOffsetInSeconds = 0;
char daysOfTheWeek[7][12] = {"Duminica", "Luni", "Marti", "Miercuri", "Joi", "Vineri", "Sambata"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
uint8_t pin_led = 2;
char* ssid = "Madhouse";
char* password = "Freshmaker1";
//char* ssid="ES_6823";
//char* password="internet";
const char* DEFAULT_EXTERNAL_ROUTER_IP = "79.117.45.187";
String externalIP = DEFAULT_EXTERNAL_ROUTER_IP;
boolean EMAIL_ENABLED = true;
IPAddress staticIP(192, 168, 0, 69);
IPAddress gateway(192, 168, 0, 0);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);
String SMART_HOME_SYSTEM_USER = "Furtos Stelian Emanuel";
int pingPeriod = PING_PERIOD_NIGHTTIME;

//</editor-fold>

//<editor-fold desc="Gettere" defaultstate="collapsed">

String getIP(char data[], int n) {
    char ipMatcher[] = "dd";
    String ip = "";
    for (int i = 1; i < n; i++) {
        ipMatcher[0] = data[i - 1];
        ipMatcher[1] = data[i];
        if (strcmp(ipMatcher, "ip") == 0) {
            int j = i + 1;
            while (data[j++] != ':');
            while (!isdigit(data[j++]));
            j = j - 1;
            while (data[j] != '"') {
                ip += data[j];
                j++;
            }
            return ip;
        }
    }
    return "empty";
}

String getExternalIP() {
    WiFiClient client;
    if (!client.connect("api.ipify.org", 80)) {
        Serial.println("Nu am putut realiza conexiunea cu 'api.ipify.org' !");
    } else {
        int timeout = millis() + 5000;
        client.print("GET /?format=json HTTP/1.1\r\nHost: api.ipify.org\r\n\r\n");
        while (client.available() == 0) {
            if (timeout - millis() < 0) {
                Serial.println(">>> Client Timeout !");
                client.stop();
                return DEFAULT_EXTERNAL_ROUTER_IP;
            }
        }
        int size;
        while ((size = client.available()) > 0) {
            uint8_t* msg = (uint8_t*) malloc(size);
            size = client.read(msg, size);
            char ar[size];
            for (int i = 0; i < size; i++) {
                ar[i] = (char) msg[i];
            }
            String procesat = getIP(ar, size);
            if (!procesat.equals("empty")) {
                return procesat;
            }
            free(msg);
        }
    }
    return "empty";
}
/**
 * Nu sterge functia asta, daca cumva iei un modul nou si vezi ca nu se conecteaza la 
 * WiFi, incearca sa apelezi functia asta din setup, ar trebui sa se conecteze de data asta,
 * dupa care poti scoate apelul din setup si uploada noul cod 
 *
 * asa a functionat cu un modul
 * @param retry
 */
void getExternalIPRoutine(int retry) {
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    Serial.println();
    Serial.print("get external ip procedure");
    int lc = 0;
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        digitalWrite(pin_led, !digitalRead(pin_led));
        delay(500);
        if (lc > 20) {
            ESP.restart();
            return;
        }
        lc++;
    }
    externalIP = getExternalIP();
    if (externalIP.equals("empty")) {
        if (retry < 3) {
            getExternalIPRoutine(++retry);
        } else {
            externalIP = DEFAULT_EXTERNAL_ROUTER_IP;
            Serial.println("Nu am putut obtine IP-ul extern, resetam la cel default");
            sendErrorEmail("Dupa " + String(retry + 1) + " incercari de reconectare nu s-a putut extrage IP-ul extern al modulului<br>Resetat la default");
        }
    } else {
        Serial.println();
        Serial.println("IP extern extras cu succes");
        Serial.println(externalIP);
    }
}

String getTime() {
    timeClient.update();
    String timp = "";
    timp += String(daysOfTheWeek[timeClient.getDay()]) + ", ";
    timp += String(timeClient.getHours()) + ":";
    timp += String(timeClient.getMinutes()) + ":";
    timp += String(timeClient.getSeconds()) + " UTC";
    return timp;
}
//</editor-fold>

//<editor-fold desc="Utilitare" defaultstate="collapsed">

String editJavaScriptDeclaration(char ar[], String variable, String value) {
    String edited = "";
    edited += ar[0];
    edited += ar[1];
    int n = strlen(ar);
    char varMatcher[] = "fff";
    for (int i = 2; i < n; i++) {
        varMatcher[0] = ar[i - 2];
        varMatcher[1] = ar[i - 1];
        varMatcher[2] = ar[i];
        edited += ar[i];
        if (strcmp("var", varMatcher) == 0) {
            int j = i;
            while (ar[j++] == ' ');
            String varName = "";
            while (ar[j] != '=' && ar[j] != ';') {
                if (ar[j] != ' ') {
                    varName += ar[j];
                }
                j++;
            }
            if (varName.equals(variable)) {
                edited += " " + varName + "=" + value + ";";
                while (ar[j++] != ';');
                while (j < n) {
                    edited += ar[j];
                    j++;
                }
                break;
            }
        }
    }
    return edited;
}

void sendInitConnectionMail() {
    EMailSender emailSend("smarthomesystem.esp8266@gmail.com", "combinashs0451");
    EMailSender::EMailMessage message;
    message.subject = "Smart Home System Online";
    message.message = "Modulul ESP8266 tocmai s-a conectat la internet, acesta este IP-ul router-ului:<br><a href=http://" + externalIP + ">" + externalIP + "</a>";
    message.message += "<br>" + externalIP + "  " + getTime();

    mailReporter(emailSend.send("manel_1850@yahoo.ro", message));

}

void sendErrorEmail(String errorMessage) {
    EMailSender emailSend("smarthomesystem.esp8266@gmail.com", "combinashs0451");
    EMailSender::EMailMessage message;
    message.subject = "Smart Home System Error";
    message.message = errorMessage;
    mailReporter(emailSend.send("manel_1850@yahoo.ro", message));
}

void sendEventsEmail(String eventsMessage) {
    EMailSender emailSend("smarthomesystem.esp8266@gmail.com", "combinashs0451");
    EMailSender::EMailMessage message;
    message.subject = "Smart Home System Events";
    message.message = eventsMessage;
    mailReporter(emailSend.send("manel_1850@yahoo.ro", message));
}

void mailReporter(EMailSender::Response response) {
    Serial.println();
    Serial.println("Status trimitere mail: ");
    Serial.println(response.code);
    Serial.println(response.desc);
    Serial.println(response.status);
}

/**
 * @Deprecated
 * @param num
 * @param type
 * @param payload
 * @param length
 *  DEPRECATED
 */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        if (payload[0] == '#') {
            uint16_t brightness = (uint16_t) strtol((const char *) &payload[1], NULL, 10);
            brightness = 1024 - brightness;
            analogWrite(pin_led, brightness);
            Serial.print("brightness= ");
            Serial.println(brightness);
        } else {
            for (int i = 0; i < length; i++)
                Serial.print((char) payload[i]);
            Serial.println();
        }
    }
}
//</editor-fold>

void startServer() {

    digitalWrite(pin_led, LOW);
    getExternalIPRoutine(0);
    int lc = 0;
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        digitalWrite(pin_led, !digitalRead(pin_led));
        delay(500);
        if (lc > 20) {
            ESP.restart();
            return;
        }
        lc++;
    }
    Serial.println();
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("IP extern :");
    Serial.print(externalIP);
    timeClient.begin();
    digitalWrite(pin_led, LOW);

    server.on("/", []() {
        //server.send_P(200, "text/html", webpage);
        server.send(200, "text/plain", "Ai accesat shs-ul lui " + SMART_HOME_SYSTEM_USER);
        Serial.println("Request nou");
    });
    server.begin();

    for (int i = 0; i < 5; i++) {
        digitalWrite(pin_led, !digitalRead(pin_led));
        delay(250);
    }
    digitalWrite(pin_led, HIGH);

}

struct Eveniment {
    String timp;
    String descriere;
    int index;
public:

    Eveniment(String timp, String descriere, int index) {
        this->timp = timp;
        this->descriere = descriere;
        this->index = index;
    }

    Eveniment() {
        this->timp = "eroare";
        this->descriere = "eroare";
    }

    String pack() {
        //return timp + " -> " + descriere;
        return "{\"utc\":\"" + timp + "\",\"descriere\":\"" + descriere + "\"}";
    }
} evenimente[MAX_EVENTS];

int evenimenteCounter = 0;

void sendSmartHomeSystemRequest(String requestType, String subDomeniu, String json) {
    WiFiClient client;
    client.setTimeout(5000);
    if (client.connect("mysmarthomesystem.com", 80)) {
        Serial.println("Conectat cu mysmarthomesystem.com");
        int timeout = millis() + 5000;

        client.println(requestType + " /" + subDomeniu + " HTTP/1.1");
        client.println("Host: mysmarthomesystem.com");
        //client.println("User-Agent: Arduino/1.0");
        //client.println("Connection: close");
        client.println("Content-Type: application/json");
        
        client.print("Content-Length: ");
        client.println(json.length());
        client.println();
        client.print(json);
        while (client.available() == 0) {
            if (timeout - millis() < 0) {
                Serial.println(">>> Client Timeout !");
                client.stop();
                break;
            }
        }
        int size;
        while ((size = client.available()) > 0) {
            uint8_t* msg = (uint8_t*) malloc(size);
            size = client.read(msg, size);
            Serial.write(msg, size);
            free(msg);
        }
    }else{
        Serial.println("Conexiunea cu serverul a esuat");
    }
}

void addEvent(String timp, String descriere, int index) {
    if (evenimenteCounter >= MAX_EVENTS) {
        eventSenderRoutine();
    }
    evenimente[evenimenteCounter] = Eveniment(timp, descriere, index);
    evenimenteCounter++;
}

void checkIfEvent(String ar) {
    switch (ar[0]) {
        case 'u':
            if (ar[1] == '1') {
                evenimente[evenimenteCounter] = Eveniment(getTime(), "Usa inchisa", evenimenteCounter);
                evenimenteCounter++;
            } else if (ar[1] == '0') {
                evenimente[evenimenteCounter] = Eveniment(getTime(), "Usa deschisa", evenimenteCounter);
                evenimenteCounter++;
            } else {
                evenimente[evenimenteCounter] = Eveniment(getTime(), "Usa cuantica :) ", evenimenteCounter);
                evenimenteCounter++;
            }
            break;
        case 'y':
            if (ar[1] == '1') {
                evenimente[evenimenteCounter] = Eveniment(getTime(), "Lumina aprinsa", evenimenteCounter);
                evenimenteCounter++;
            } else if (ar[1] == '0') {
                evenimente[evenimenteCounter] = Eveniment(getTime(), "Lumina stinsa", evenimenteCounter);
                evenimenteCounter++;
            } else {
                evenimente[evenimenteCounter] = Eveniment(getTime(), "Lumina cuantica :)", evenimenteCounter);
                evenimenteCounter++;
            }
            break;
    }
    if (evenimenteCounter >= MAX_EVENTS) {
        eventSenderRoutine();
    }
}

void eventSenderRoutine() {
    if (evenimenteCounter > 0) {
        String msg = "";
        for (int i = 0; i < evenimenteCounter; i++) {
            msg += evenimente[i].pack() + "\n";
        }
        String json = "{";
        json += "\"tip\":\"Evenimente\",\r\n";
        json += "\"length\": 1,\r\n";
        json += "\"user\": \"" + SMART_HOME_SYSTEM_USER + "\"" + ",\r\n";
        json += "\"continut\" : [\r\n";
        for (int i = 0; i < evenimenteCounter; i++) {
            json += evenimente[i].pack()+ (i < evenimenteCounter - 1 ? "," : "") + "\r\n";
        }
        json += "]\r\n}";
        evenimenteCounter = 0;
        Serial.println(json);
        sendSmartHomeSystemRequest(REQUEST_TYPE_POST, DOMAIN_EVENT_REPORTER, json);
    }
}

void pingSenderRoutine() {
    String json = "{";
    json += "\"tip\":\"Ping\",\r\n";
    json += "\"length\": " + String(1) + ",\r\n";
    json += "\"user\": \"" + SMART_HOME_SYSTEM_USER + "\"" + ",\r\n";
    json += "\"continut\" : [{\"ping_period\":\"" + String(pingPeriod) + "\"}]\r\n}";
    sendSmartHomeSystemRequest(REQUEST_TYPE_POST, DOMAIN_EVENT_REPORTER, json);
}

void setup() {
    pinMode(pin_led, OUTPUT);
    Serial.begin(115200);
    
    int lc = 0;

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        digitalWrite(pin_led, !digitalRead(pin_led));
        delay(500);
        if (lc > 20) {
            ESP.restart();
        }
        lc++;
    }

    Serial.println("\nConnectat la WiFi");
    Serial.print("Adresa IP :");
    Serial.print(WiFi.localIP());
    Serial.println();
    addEvent(getTime(), "Conexiune la internet", 0);
    delay(500);
    pingSenderRoutine();
    for (int i = 0; i < 5; i++) {
        digitalWrite(pin_led, !digitalRead(pin_led));
        delay(250);
    }
    digitalWrite(pin_led, HIGH);
}

unsigned long previousMillis_eventSender = 0;
unsigned long previousMillis_pingSender = 0;
String data = "";

void loop() {

    if (Serial.available() > 0) {
        byte readByte = Serial.read();
        char c[] = {(char) readByte};
        //webSocket.broadcastTXT(c, sizeof (c)); // de ce mai am asta aici?
        if (readByte != 13 && readByte != 10) {
            data += (char) readByte;
        } else {
            checkIfEvent(data);
            data = "";
        }
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis_eventSender >= EVENT_SENDER_ROUTINE_PERIOD) {
        previousMillis_eventSender = currentMillis;
        eventSenderRoutine();
    }
    if (currentMillis - previousMillis_pingSender >= pingPeriod) {
        previousMillis_pingSender = currentMillis;
        pingSenderRoutine();
    }
}
