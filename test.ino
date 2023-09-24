#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>

// Define the pins for relay control
const int relay1Pin = D1; // Monitors status (active low)
const int relay2Pin = D2; // Controls Relay 2 (active low)


// Define global variables for timer value and relay state
int timerValue = 0;
int relayState = LOW;

// Define hardware timer object
os_timer_t myTimer;

ESP8266WebServer server(80);

void setup() {
  pinMode(relay1Pin, INPUT_PULLUP); // Relay 1 status monitoring (active low)
  pinMode(relay2Pin, OUTPUT);       // Relay 2 control (active low)
  // Set initial output to LOW
  digitalWrite(relay2Pin, HIGH);


  WiFiManager wifiManager;

  wifiManager.setCustomHeadElement("<style>header h1 { display: none !important; }</style>");
  wifiManager.setCustomHeadElement("<style>header { background-color: #f2f2f2 !important; padding: 20px !important; text-align: center !important; }</style>");
  wifiManager.setCustomHeadElement("<style>footer { display: none !important; }</style>");

  wifiManager.autoConnect("NodeMCU-Relay");

  ArduinoOTA.begin();
  ArduinoOTA.setHostname("NodeMCU-Relay-Control");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/relay2/on", HTTP_GET, turnOnRelay2);
  server.on("/relay2/off", HTTP_GET, turnOffRelay2);
  server.on("/timer", activateTimer);
  server.on("/status", HTTP_GET, sendStatus);
  server.begin();

  // Initialize the hardware timer
  os_timer_setfn(&myTimer, &handleTimer, NULL);
  os_timer_arm(&myTimer, 1000, true);
}


void ICACHE_RAM_ATTR handleTimer(void *pArg) {
  if (relayState == HIGH) {
    if (timerValue > 0) {
      timerValue--;
      if (timerValue == 0) {
        relayState = LOW;
        digitalWrite(output, HIGH);
      }
    }
  }
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}



// Define a function that will handle the root web page request
void handleRoot() {
  String html = "<html><head><style>";
  
  // Add CSS styles as before
  
  html += ".button {";
  html += "  display: inline-block;";
  html += "  padding: 10px 20px;";
  html += "  margin: 10px;";
  html += "  border: none;";
  html += "  border-radius: 5px;";
  html += "  font-size: 20px;";
  html += "}";
  
  html += ".button-on {";
  html += "  background-color: green;";
  html += "  color: white;";
  html += "}";
  
  html += ".button-off {";
  html += "  background-color: red;";
  html += "  color: white;";
  html += "}";
  
  html += "</style></head><body>";
  
  // Add HTML elements as before
  
  html += "<h1>ESP8266 Web Server</h1>";
  html += "<p>Relay 1 status: <span id='relay1'></span></p>";
  html += "<p>Relay 2 status: <span id='relay2'></span></p>";
  html += "<p>Enter duration in minutes (1-60) for Relay 2 timer: <input type='number' id='duration' min='1' max='60'></p>";
  html += "<p>Timer: <span id='timer'></span></p>";
  html += "<button id='relay2on' onclick='turnRelay2On()'>Turn Relay 2 ON</button>";
  html += "<button id='relay2off' onclick='turnRelay2Off()'>Turn Relay 2 OFF</button>";
  html += "<button id='activate' onclick='activateTimer()'>Activate Timer</button>";
  
  html += "<script>";
  
  // Add JavaScript functions as before
  
  
// Modify checkStatus function to read current timer value and relay state from global variables and send them as JSON data

html += "function checkStatus() {";
html += " var xhttp = new XMLHttpRequest();";
html += " xhttp.onreadystatechange = function() {";
html += "   if (this.readyState == 4 && this.status == 200) {";
html += "     var status = JSON.parse(this.responseText);"; // Parse the JSON response from the server
html += "     var relay1 = document.getElementById('relay1');"; // Get the span element for relay1 status
html += "     var relay2on = document.getElementById('relay2on');"; // Get the button element for turning relay2 on
html += "     var relay2off = document.getElementById('relay2off');"; // Get the button element for turning relay2 off

// Get the input element for entering duration

html += "     var duration = document.getElementById('duration');";

// Get the span element for displaying timer

html += "     var timer = document.getElementById('timer');";

// Get the button element for activating timer

html += "     var activate = document.getElementById('activate');";

html += "     if (status.relay1 == 'ON') {"; // If relay1 is on
html += "       relay1.className = 'button button-on';"; // Change the span class to button-on
html += "       relay1.innerHTML = 'ON';"; // Change the span text to ON
html += "     } else {"; // If relay1 is off
html += "       relay1.className = 'button button-off';"; // Change the span class to button-off
html += "       relay1.innerHTML = 'OFF';"; // Change the span text to OFF
html += "     }";
html += "     if (status.relay2 == 'ON') {"; // If relay2 is on
html += "       relay2on.disabled = true;"; // Disable the ON button
html += "       relay2off.disabled = false;"; // Enable the OFF button

// Disable the duration input and activate button

html += "       duration.disabled = true;";
html += "       activate.disabled = true;";

// Display the timer value

html += "       timer.innerHTML = formatTime(status.timer);";

html += "     } else {"; // If relay2 is off
html +="       relay2on.disabled = false;"; // Enable the ON button
html +="       relay2off.disabled = true;"; // Disable the OFF button

// Enable the duration input and activate button

html +="       duration.disabled = false;";
html +="       activate.disabled = false;";

// Clear the timer value

html +="       timer.innerHTML = '';";

html +="     }";
html +="   }";
html +=" };";
html +=" xhttp.open('GET', '/status', true);";
html +=" xhttp.send();";
html +="}";

// Add other JavaScript functions as before

// Define a function that will turn Relay 2 on and send a GET request to the server
function turnRelay2On() {
  // Create a new XMLHttpRequest object
  var xhttp = new XMLHttpRequest();
  
  // Define a callback function for when the request is completed
  xhttp.onreadystatechange = function() {
    // Check if the request was successful
    if (this.readyState == 4 && this.status == 200) {
      // Display a confirmation message
      alert(this.responseText);
    }
  };
  
  // Send a GET request to the server with the relay2/on path
  xhttp.open('GET', '/relay2/on', true);
  xhttp.send();
}

// Define a function that will turn Relay 2 off and send a GET request to the server
function turnRelay2Off() {
  // Create a new XMLHttpRequest object
  var xhttp = new XMLHttpRequest();
  
  // Define a callback function for when the request is completed
  xhttp.onreadystatechange = function() {
    // Check if the request was successful
    if (this.readyState == 4 && this.status == 200) {
      // Display a confirmation message
      alert(this.responseText);
    }
  };
  
  // Send a GET request to the server with the relay2/off path
  xhttp.open('GET', '/relay2/off', true);
  xhttp.send();
}

// Define a function that will activate the timer and send a GET request to the server with the duration parameter
function activateTimer() {
  // Get the duration value from the input element
  var duration = document.getElementById('duration').value;
  
  // Check if the duration is a valid number between 1 and 60
  if (isNaN(duration) || duration < 1 || duration > 60) {
    // Display an alert message
    alert('Invalid duration');
  } else {
    // Create a new XMLHttpRequest object
    var xhttp = new XMLHttpRequest();
    
    // Define a callback function for when the request is completed
    xhttp.onreadystatechange = function() {
      // Check if the request was successful
      if (this.readyState == 4 && this.status == 200) {
        // Display a confirmation message
        alert(this.responseText);
      }
    };
    
    // Send a GET request to the server with the timer path and the duration parameter
    xhttp.open('GET', '/timer?duration=' + duration, true);
    xhttp.send();
  }
}

// Define a function that will format a given number of seconds into hh:mm:ss format
function formatTime(seconds) {
  // Calculate hours, minutes, and seconds from seconds
  var hours = Math.floor(seconds / 3600);
  var minutes = Math.floor((seconds % 3600) / 60);
  var seconds = Math.floor(seconds % 60);
  
  // Add leading zeros if needed
  if (hours < 10) hours = '0' + hours;
  if (minutes < 10) minutes = '0' + minutes;
  if (seconds < 10) seconds = '0' + seconds;
  
  // Return the formatted time string
  return hours + ':' + minutes + ':' + seconds;
}
  
html += "</script>";
html += "</body></html>";
server.send(200, "text/html", html); // Send the HTML code to the browser
}

// Define a function that will turn on Relay 2 and set relay state to HIGH and start hardware timer
void turnOnRelay2() {
  digitalWrite(relay2Pin, LOW);
  relayState = HIGH;
  
  // Set up hardware timer interrupt
  // The timer will trigger every second and call the handleTimer function
  os_timer_setfn(&myTimer, &handleTimer, NULL);
  os_timer_arm(&myTimer, 1000, true);
  
  server.send(200, "text/plain", "Relay 2 is ON");
}

// Define a function that will turn off Relay 2 and set relay state to LOW and stop hardware timer
void turnOffRelay2() {
  digitalWrite(relay2Pin, HIGH);
  relayState = LOW;
  
  // Stop hardware timer interrupt
  os_timer_disarm(&myTimer);
  
  server.send(200, "text/plain", "Relay 2 is OFF");
}

// Define a function that will activate the timer and set timer value and relay state to HIGH and start hardware timer
void activateTimer() {
  int duration = server.arg("duration").toInt();
  if (duration > 0) {
    duration = duration * 60;
    timerValue = duration;
    relayState = HIGH;
    digitalWrite(relay2Pin, LOW);
    
    // Set up hardware timer interrupt
    // The timer will trigger every second and call the handleTimer function
    os_timer_setfn(&myTimer, &handleTimer, NULL);
    os_timer_arm(&myTimer, 1000, true);
    
    server.send(200, "text/plain", "Timer is activated");
  } else {
    server.send(400, "text/plain", "Invalid duration");
  }
}

// Define a function that will send the JSON data with the timer value and relay status from global variables
void sendStatus() {
  // Declare a variable to hold the JSON data
  String jsonData;
  
  // Get the status of Relay 1 (ON or OFF)
  String relay1Status = digitalRead(relay1Pin) == LOW ? "ON" : "OFF";
  
  // Get the status of Relay 2 (ON or OFF)
  String relay2Status = relayState == HIGH ? "ON" : "OFF";
  
  // Format the JSON data with the timer value and relay status
  jsonData = "{\"relay1\": \"" + relay1Status + "\", \"relay2\": \"" + relay2Status + "\", \"timer\": \"" + timerValue + "\"}";
  
  // Send the JSON data with the content type as application/json
  server.send(200, "application/json", jsonData);
}

