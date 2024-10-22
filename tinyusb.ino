#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

const char* ap_ssid = "EPS";
const char* ap_password = "12345678";

USBHIDKeyboard Keyboard;

WebServer server(80);

#define EXECUTE_BUTTON_PIN 14

String queuedSequence = "";

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html><html><body><h1>ESP32 Keyboard</h1>
  <div><input type="text" id="command" placeholder="Enter command"><button id="addStep">Add Step</button></div>
  <h2>Steps:</h2><ul id="stepsList"></ul>
  <button id="executeSteps">Execute Steps</button>
  <button id="clearSteps">Clear Steps</button>
  <button id="queueSteps">Queue Steps for Button Execution</button>
  <div><input type="file" id="fileInput"><button id="uploadFile">Upload and Execute</button></div>
  <p id="status"></p>
  <script>
  var steps=[];

  document.getElementById('addStep').addEventListener('click',function(){
    var cmd=document.getElementById('command').value.trim();
    if(cmd){
      steps.push(cmd);
      var li=document.createElement('li');
      li.textContent=cmd+' ';
      var removeBtn=document.createElement('button');
      removeBtn.textContent='Remove';
      removeBtn.addEventListener('click',function(){
        var index=Array.from(document.getElementById('stepsList').children).indexOf(li);
        steps.splice(index,1);
        li.remove();
      });
      li.appendChild(removeBtn);
      document.getElementById('stepsList').appendChild(li);
      document.getElementById('command').value='';
    }
  });

  document.getElementById('executeSteps').addEventListener('click',function(){
    if(steps.length>0){
      var sequence=steps.join(';');
      fetch('/send',{
        method:'POST',
        headers:{'Content-Type':'application/x-www-form-urlencoded'},
        body:'sequence='+encodeURIComponent(sequence)
      }).then(r=>r.text()).then(d=>{
        document.getElementById('status').innerText=d;
      });
    }else{
      alert('No steps to execute');
    }
  });

  document.getElementById('clearSteps').addEventListener('click',function(){
    steps=[];
    document.getElementById('stepsList').innerHTML='';
    document.getElementById('status').innerText = "Steps cleared.";
  });

  document.getElementById('queueSteps').addEventListener('click',function(){
    if(steps.length>0){
      var sequence=steps.join(';');
      fetch('/queue',{
        method:'POST',
        headers:{'Content-Type':'application/x-www-form-urlencoded'},
        body:'sequence='+encodeURIComponent(sequence)
      }).then(r=>r.text()).then(d=>{
        document.getElementById('status').innerText=d;
      });
    }else{
      alert('No steps to queue');
    }
  });

  document.getElementById('uploadFile').addEventListener('click',function(){
    var fileInput = document.getElementById('fileInput');
    var file = fileInput.files[0];
    if (file) {
      var reader = new FileReader();
      reader.onload = function(e) {
        var text = e.target.result;
        var lines = text.split('\n');
        steps = lines.map(line => line.trim()).filter(line => line.length > 0);
        document.getElementById('stepsList').innerHTML = '';
        steps.forEach(function(cmd) {
          var li = document.createElement('li');
          li.textContent = cmd + ' ';
          var removeBtn = document.createElement('button');
          removeBtn.textContent = 'Remove';
          removeBtn.addEventListener('click', function() {
            var index = Array.from(document.getElementById('stepsList').children).indexOf(li);
            steps.splice(index, 1);
            li.remove();
          });
          li.appendChild(removeBtn);
          document.getElementById('stepsList').appendChild(li);
        });
        document.getElementById('status').innerText = "Steps loaded from file.";
      };
      reader.readAsText(file);
    } else {
      alert('No file selected');
    }
  });
  </script></body></html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleSendCommand() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("sequence")) {
      String body = server.arg("sequence");
      if (body.length() > 0) {
        parseAndExecuteSequence(body.c_str());
        server.send(200, "text/plain", "Sequence executed!");
      } else {
        server.send(400, "text/plain", "No sequence received.");
      }
    } else {
      server.send(400, "text/plain", "Missing sequence parameter.");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleQueueCommand() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("sequence")) {
      String body = server.arg("sequence");
      if (body.length() > 0) {
        queuedSequence = body;
        server.send(200, "text/plain", "Sequence queued!");
      } else {
        server.send(400, "text/plain", "No sequence received.");
      }
    } else {
      server.send(400, "text/plain", "Missing sequence parameter.");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void setup() {
  Serial.begin(115200);

  Keyboard.begin();
  USB.begin();

  WiFi.softAP(ap_ssid, ap_password);

  server.on("/", handleRoot);
  server.on("/send", handleSendCommand);
  server.on("/queue", handleQueueCommand);
  server.begin();

  pinMode(EXECUTE_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  server.handleClient();

  if (digitalRead(EXECUTE_BUTTON_PIN) == LOW) {
    delay(50);
    if (digitalRead(EXECUTE_BUTTON_PIN) == LOW && queuedSequence.length() > 0) {
      parseAndExecuteSequence(queuedSequence.c_str());
      while (digitalRead(EXECUTE_BUTTON_PIN) == LOW) {
        delay(10);
      }
    }
  }
}

void parseAndExecuteSequence(const char* sequence) {
  char seqCopy[1024];
  strncpy(seqCopy, sequence, sizeof(seqCopy) - 1);
  seqCopy[sizeof(seqCopy) - 1] = '\0';
  char* saveptr1;
  char* command = strtok_r(seqCopy, ";", &saveptr1);
  while (command != NULL) {
    executeCommand(command);
    command = strtok_r(NULL, ";", &saveptr1);
  }
}

void executeCommand(const char* command) {
  if (strcasecmp(command, "press win+r") == 0) {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    delay(200);
    Keyboard.releaseAll();
  } else if (strcasecmp(command, "press win+d") == 0) {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('d');
    delay(200);
    Keyboard.releaseAll();
  } else if (strcasecmp(command, "press ctrl+t") == 0) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('t');
    delay(200);
    Keyboard.releaseAll();
  } else if (strcasecmp(command, "press ctrl+w") == 0) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('w');
    delay(200);
    Keyboard.releaseAll();
  } else if (strcasecmp(command, "press ctrl+alt+t") == 0) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press('t');
    delay(200);
    Keyboard.releaseAll();
  } else if (strcasecmp(command, "press alt+f4") == 0) {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_F4);
    delay(200);
    Keyboard.releaseAll();
  } else if (strcasecmp(command, "press win") == 0) {
    Keyboard.press(KEY_LEFT_GUI);
    delay(200);
    Keyboard.releaseAll();
  } else if (strcasecmp(command, "press enter") == 0) {
    Keyboard.press(KEY_RETURN);
    delay(200);
    Keyboard.releaseAll();
  } else {
    Keyboard.print(command);
  }
}
