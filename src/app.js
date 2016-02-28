//--------- js-message-queue ----------//
"use strict";(function(root,factory){if(typeof define==="function"&&define.amd){define([],factory)}else if(typeof module==="object"&&module.exports){module.exports=factory()}else{root.MessageQueue=factory()}})(this,function(){var RETRY_MAX=5;var queue=[];var sending=false;var timer=null;return{reset:reset,sendAppMessage:sendAppMessage,size:size};function reset(){queue=[];sending=false}function sendAppMessage(message,ack,nack){if(!isValidMessage(message)){return false}queue.push({message:message,ack:ack||null,nack:nack||null,attempts:0});setTimeout(function(){sendNextMessage()},1);return true}function size(){return queue.length}function isValidMessage(message){if(message!==Object(message)){return false}var keys=Object.keys(message);if(!keys.length){return false}for(var k=0;k<keys.length;k+=1){var validKey=/^[0-9a-zA-Z-_]*$/.test(keys[k]);if(!validKey){return false}var value=message[keys[k]];if(!validValue(value)){return false}}return true;function validValue(value){switch(typeof value){case"string":return true;case"number":return true;case"object":if(toString.call(value)==="[object Array]"){return true}}return false}}function sendNextMessage(){if(sending){return}var message=queue.shift();if(!message){return}message.attempts+=1;sending=true;Pebble.sendAppMessage(message.message,ack,nack);timer=setTimeout(function(){timeout()},1e3);function ack(){clearTimeout(timer);setTimeout(function(){sending=false;sendNextMessage()},200);if(message.ack){message.ack.apply(null,arguments)}}function nack(){clearTimeout(timer);if(message.attempts<RETRY_MAX){queue.unshift(message);setTimeout(function(){sending=false;sendNextMessage()},200*message.attempts)}else{if(message.nack){message.nack.apply(null,arguments)}}}function timeout(){setTimeout(function(){sending=false;sendNextMessage()},1e3);if(message.ack){message.ack.apply(null,arguments)}}}});

var VERSION = "4.0";
var MESSAGE_CHUNK_LENGTH = 100; 
var MESSAGE_DELIMITER = "\t"; 

var DEV_MODE = false; //Set to false before Shipping! 
var BASE_URL = 'http://pebble.fernandotrujano.com';


Pebble.addEventListener("ready", function(e){
  console.log("JS code running!"); 
  sendMessage("READY", "none", {}); 

  if (DEV_MODE) { 
    var sample_workout = '{"workouts":[{"moves":[{"name":"Move 1","value":6,"type":"time"},{"name":"Move 2","value":7,"type":"time"},{"name":"Move 3","value":7,"type":"time"}],"title":"Just Time"},{"moves":[{"name":"move 1","value":6,"type":"reps"},{"name":"move 2","value":5,"type":"reps"},{"name":"move 3","value":12,"type":"reps"}],"title":"Just reps"},{"moves":[{"name":"move 1","value":16,"type":"time"},{"name":"move 2 reps","value":4,"type":"reps"},{"name":"move 3 time","value":6,"type":"time"},{"name":"move 4 reps","value":14,"type":"reps"}],"title":"Both"}]}'; 
    parseAndAddWorkouts(sample_workout); 
  }
});

Pebble.addEventListener("showConfiguration", function(){ 
  console.log("Showing Configuration v" + VERSION);
  Pebble.openURL(BASE_URL + "/user/home?info=" + Pebble.getAccountToken() + "," + VERSION);
});

function sendMessage(messageType, messageHeader, message) {
  message[0] = messageType; 
  message[1] = messageHeader; 
  console.log("sending message of type: " + messageType); 
  MessageQueue.sendAppMessage(message, function(e) { console.log("send yes"); }, function(e){console.log("ERROR: " + e.error.message); console.log("send no"); });
}
function sendWorkout(workoutName, moves) {
  console.log("sending workout" + workoutName + " with moves: " + moves); 
  
  if (moves.length > MESSAGE_CHUNK_LENGTH) return; //TODO Only sending one chunk for now. 
  
  var messageType = "MOVES"; 
  var messageHeader = [workoutName].join(MESSAGE_DELIMITER); 
  var i, len_i, j, len_j, chunk, move, message, messageIndex;
  
  //Split each message into chunks. 
  for (i=0, len_i=moves.length; i<len_i; i+=MESSAGE_CHUNK_LENGTH) { 
    message = {}; 
    
    chunk = moves.slice(i, i+MESSAGE_CHUNK_LENGTH);
    
    messageIndex = 2; //Start moves at 2 
    //Add each move in chunk to message
    for (j=0, len_j=chunk.length; j<len_j; j++) { 
      move = chunk[j]; 
      message[messageIndex] = [move.name, move.type, move.value].join(MESSAGE_DELIMITER); 
      messageIndex++; 
      console.log("constructing message");
    }
    
    sendMessage(messageType, messageHeader, message);   
  }
  
}

function parseAndAddWorkouts(jsonString) { 
  console.log("Configuration window returned: " + jsonString); //See repo for json structure

  var json; 
  json = JSON.parse(decodeURIComponent(jsonString));  

  var workoutTitle,moves, workoutInfo, messageIndex=2, message={}; 

  localStorage.clear(); 
  //Add Each workout
  for (var workout in json.workouts) {    
    workoutTitle = json.workouts[workout].title; 
    workoutInfo = [workoutTitle].join(MESSAGE_DELIMITER); 
    moves = JSON.stringify(json.workouts[workout].moves); //Concert to string before saving!
    message[messageIndex] = workoutInfo; 
    messageIndex++; 
    localStorage.setItem(workoutTitle, moves);     //Save workout on local storage
    console.log("set " + workoutTitle + "to " + moves); 
  }

  console.log("Local Storage Set");
  
  sendMessage("WORKOUTS", "NONE", message); 
    

}


//After Closing settings view
//Send Title(key) to watch app (Persiant Memory), and save JSON in Internal Memory
Pebble.addEventListener("webviewclosed",function(e) {
    if (e.response != "CANCELLED") {
      parseAndAddWorkouts(e.response); 
    }
  }
);

//Workout requested from phone
function workoutMessageHandler(workoutTitle) { 
  console.log("Workout Requested: " + workoutTitle); 
  var moves = localStorage.getItem(workoutTitle); 
  console.log(moves); 
  moves = JSON.parse(moves); 

  sendWorkout(workoutTitle, moves); 
}

//TODO Call this from Pebble side
function workoutDoneMessageHandler(workoutTitle) { 
  workoutCompleted(workoutTitle); 

}


function workoutCompleted(title){ 
  console.log("sending request");

  if (Pebble.getActiveWatchInfo && Pebble.getActiveWatchInfo().firmware.major >= 3) { 
    Pebble.getTimelineToken(function (timelineToken) {
        var url = [ 
              BASE_URL,
              "/user/workout/completed/" , 
              encodeURIComponent(Pebble.getAccountToken()), "/",
              encodeURIComponent(title), "/",
              timelineToken 
        ].join("");
        console.log("URL: " + url);
        postWorkoutCompleted(url);

      },function (error) { 
        console.log('Error getting timeline token: ' + error);
      }
    );
  }

  else { 
      var url = BASE_URL + '/user/workout/completed/' + encodeURIComponent(  Pebble.getAccountToken() )  + '/' + encodeURIComponent(localStorage.getItem("currentWorkoutName")) + "/none"; 
      postWorkoutCompleted(url);
  }
  

}

function postWorkoutCompleted(url){ 
  //Send request to server
  console.log(url);
  var req = new XMLHttpRequest();
  req.open('POST', url, true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      if(req.status == 200) {
        console.log("Workout recorded"); 
      } else { console.log('Error'); }
    }
  }; 
  req.send(null);

}

//Recieve message from Pebble
Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Recieved something on phone---");
    for (var type in e.payload){
        console.log("Received message: " + e.payload[type] + " of type: " + type);
        if (type === 'WORKOUT') workoutMessageHandler(e.payload[type]); 
        if (type === 'WORKOUT_DONE') workoutDoneMessageHandler(e.payload[type]);           
    }
  }
);

