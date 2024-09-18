#include <ESP8266WiFi.h>

const char* ssid = "IIT-Mandi-WiFi";
const char* password = "wifi@iit";

int IN_1 = 5;  
int IN_2 = 4;  
int IN_3 = 0; 
int IN_4 = 2;  
int ENA = 14;   
int ENB = 12;   

int SPEED = 600;   
int speed_Coeff = 3;

WiFiServer server(80);
bool forwardPressed = false;
bool backwardPressed = false;
bool leftPressed = false;
bool rightPressed = false;

String command = "0"; 

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);

  analogWrite(ENA, SPEED);  
  analogWrite(ENB, SPEED);  

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    while (!client.available()) {
      delay(1);
    }
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();
    updateButtonStates(request);
    buildCommand();

    sendHTML(client);

    Serial.println("Client disconnected");
  }
  Serial.println("Command: " + command);
  delay(100);
  if (command == "F") Forward();      
  else if (command == "B") Backward();
  else if (command == "R") TurnRight();
  else if (command == "L") TurnLeft();
  else if (command == "P") ForwardLeft();
  else if (command == "S") BackwardLeft();
  else if (command == "Z") ForwardRight();
  else if (command == "Q") BackwardRight();
  else if (command == "0") Stop();
}

void updateButtonStates(String request) {
  if (request.indexOf("/FORWARD") != -1) forwardPressed = true;
  if (request.indexOf("/BACKWARD") != -1) backwardPressed = true;
  if (request.indexOf("/LEFT") != -1) leftPressed = true;
  if (request.indexOf("/RIGHT") != -1) rightPressed = true;

  if (request.indexOf("/RELEASE_FORWARD") != -1) forwardPressed = false;
  if (request.indexOf("/RELEASE_BACKWARD") != -1) backwardPressed = false;
  if (request.indexOf("/RELEASE_LEFT") != -1) leftPressed = false;
  if (request.indexOf("/RELEASE_RIGHT") != -1) rightPressed = false;
}

void buildCommand() {
  if (forwardPressed && rightPressed) {
    command = "P";
  } else if (backwardPressed && rightPressed) {
    command = "Q"; 
  } else if (forwardPressed && leftPressed) {
    command = "Z";
  } else if (backwardPressed && leftPressed) {
    command = "S";
  } else {
    command = "";  
    if (forwardPressed) command += "F";
    if (backwardPressed) command += "B";
    if (leftPressed) command += "L";
    if (rightPressed) command += "R";
    
    if (command.length() == 0) command = "0";
  }
}

void sendHTML(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println(createHTMLHead());
  client.println("<body>");
  client.println(createHTMLBody());
  client.println("<script>");
  client.println(createHTMLScript());
  client.println("</script>");
  client.println("</body>");
  client.println("</html>");
}

String createHTMLHead() {
  String html = "<head><title>ESP8266 Robot Control</title>";
  html += "<style>";
  html += "body { margin: 0; padding: 0; background-color: #f3f3f3; }";
  html += ".grid-container { display: grid; grid-template-columns: repeat(3, 1fr); grid-template-rows: repeat(3, 1fr); height: 100vh; width: 100vw; }";
  html += ".grid-item { display: flex; justify-content: center; align-items: center; }";
  html += ".button { width: 90%; height: 90%; font-size: 2rem; background-color: #333; color: white; border: none; border-radius: 10px; touch-action: manipulation; }";
  html += ".arrow { font-size: 3rem; }";
  html += "</style>";
  html += "</head>";
  return html;
}

String createHTMLBody() {
  String html = "<div class='grid-container'>";
  html += "<div class='grid-item'></div>";
  html += "<div class='grid-item'><button class='button' ontouchstart=\"sendCommand('/FORWARD')\" ontouchend=\"sendCommand('/RELEASE_FORWARD')\" onmousedown=\"sendCommand('/FORWARD')\" onmouseup=\"sendCommand('/RELEASE_FORWARD')\"><span class='arrow'>&#9650;</span></button></div>";
  html += "<div class='grid-item'></div>";
  html += "<div class='grid-item'><button class='button' ontouchstart=\"sendCommand('/LEFT')\" ontouchend=\"sendCommand('/RELEASE_LEFT')\" onmousedown=\"sendCommand('/LEFT')\" onmouseup=\"sendCommand('/RELEASE_LEFT')\"><span class='arrow'>&#9664;</span></button></div>";
  html += "<div class='grid-item'></div>";
  html += "<div class='grid-item'><button class='button' ontouchstart=\"sendCommand('/RIGHT')\" ontouchend=\"sendCommand('/RELEASE_RIGHT')\" onmousedown=\"sendCommand('/RIGHT')\" onmouseup=\"sendCommand('/RELEASE_RIGHT')\"><span class='arrow'>&#9654;</span></button></div>";
  html += "<div class='grid-item'></div>";
  html += "<div class='grid-item'><button class='button' ontouchstart=\"sendCommand('/BACKWARD')\" ontouchend=\"sendCommand('/RELEASE_BACKWARD')\" onmousedown=\"sendCommand('/BACKWARD')\" onmouseup=\"sendCommand('/RELEASE_BACKWARD')\"><span class='arrow'>&#9660;</span></button></div>";
  html += "<div class='grid-item'></div>";
  html += "</div>";
  return html;
}

String createHTMLScript() {
  String script = "function sendCommand(command) {";
  script += "  var xhttp = new XMLHttpRequest();";
  script += "  xhttp.open('GET', command, true);";
  script += "  xhttp.send();";
  script += "}";
  return script;
}


void Forward() { 
   Serial.println("Forward"); 
   digitalWrite(IN_1, HIGH);
   digitalWrite(IN_2, LOW);
   digitalWrite(IN_3,HIGH);
   digitalWrite(IN_4,LOW);
   analogWrite(ENA, SPEED);
   analogWrite(ENB, SPEED);
}

void Backward() {
  Serial.println("Backward");
   digitalWrite(IN_1, LOW);
   digitalWrite(IN_2, HIGH);
   digitalWrite(IN_3,LOW);
   digitalWrite(IN_4,HIGH);
   analogWrite(ENA, SPEED);
   analogWrite(ENB, SPEED-100);
}

void TurnRight() {
  Serial.println("Turn Right");
   digitalWrite(IN_1, HIGH);
   digitalWrite(IN_2, LOW);
   digitalWrite(IN_3,LOW);
   digitalWrite(IN_4,HIGH);
   analogWrite(ENA, SPEED);
   analogWrite(ENB, SPEED);
}

void TurnLeft() {
  Serial.println("Turn Left");
   digitalWrite(IN_1, LOW);
   digitalWrite(IN_2, HIGH);
   digitalWrite(IN_3,HIGH);
   digitalWrite(IN_4,LOW);
   analogWrite(ENA, SPEED);
   analogWrite(ENB, SPEED);
}

void ForwardLeft() {
  Serial.println("Forward Left");
  digitalWrite(IN_1, HIGH);
   digitalWrite(IN_2, LOW);
   digitalWrite(IN_3,HIGH);
   digitalWrite(IN_4,LOW);
  analogWrite(ENA, SPEED);
   analogWrite(ENB, 100);
}

void ForwardRight() {
  Serial.println("Forward Right");
  digitalWrite(IN_1, HIGH);
   digitalWrite(IN_2, LOW);
   digitalWrite(IN_3,HIGH);
   digitalWrite(IN_4,LOW);
  analogWrite(ENA, 100);
  analogWrite(ENB, SPEED);
}

void BackwardLeft() {
  Serial.println("Backward Left");
  digitalWrite(IN_1, LOW);
   digitalWrite(IN_2, HIGH);
   digitalWrite(IN_3,LOW);
   digitalWrite(IN_4,HIGH);
  analogWrite(ENA, 100);
  analogWrite(ENB, SPEED);
}

void BackwardRight() {
  Serial.println("Backward Right");
  digitalWrite(IN_1, LOW);
   digitalWrite(IN_2, HIGH);
   digitalWrite(IN_3,LOW);
   digitalWrite(IN_4,HIGH);
  analogWrite(ENA, SPEED);
  analogWrite(ENB, 100);
}

void Stop() {
  Serial.println("Stop");
   digitalWrite(IN_1, LOW);
   digitalWrite(IN_2, LOW);
   digitalWrite(IN_3,LOW);
   digitalWrite(IN_4,LOW);
   analogWrite(ENA, SPEED);
   analogWrite(ENB, SPEED);
}