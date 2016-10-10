/* 
Pebble JS component of Custom Workout Timer. v4.0
Communicates with OWM, parses and stores workouts and handles workout requests from Pebble. 
Copyright © 2016 Fernando Trujano
                 trujano@mit.edu
*/

var VERSION = "4.0";
var MESSAGE_DELIMITER = "\t"; 
var MESSAGE_DATA_ROOT = 2; 
var MESSAGE_CHUNK_LENGTH = 25;

var MAX_MOVE_NAME_LENGTH = 39; 
var MAX_WORKOUT_NAME_LENGTH = 29; 

var MESSAGE_CHUNK_LENGTH; 


//100 for Pebble Time 


var DEV_MODE = false; //Set to false before Shipping! 
var BASE_URL = 'http://pebble.fernandotrujano.com';

var MessageQueue = require('./message-queue');

Pebble.addEventListener("ready", function(e){
  console.log("JS code running!"); 
  sendMessage("READY", "none", {}); 
  
  if (getWatchPlatform() === "aplite") MESSAGE_CHUNK_LENGTH = 25; 
  else MESSAGE_CHUNK_LENGTH = 100; 

  if (DEV_MODE) { 
    var sample_workout = '{"workouts":[{"moves":[{"name":"Move 1","value":6,"type":"time"},{"name":"Move 2","value":7,"type":"time"},{"name":"Move 3","value":7,"type":"time"}],"title":"Just Time"},{"moves":[{"name":"move 1","value":6,"type":"reps"},{"name":"move 2","value":5,"type":"reps"},{"name":"move 3","value":12,"type":"reps"}],"title":"Just reps"},{"moves":[{"name":"move 1","value":16,"type":"time"},{"name":"move 2 reps","value":4,"type":"reps"},{"name":"move 3 time","value":6,"type":"time"},{"name":"move 4 reps","value":14,"type":"reps"}],"title":"Both"}]}'; 
    parseAndAddWorkouts(sample_workout); 
  }
});

Pebble.addEventListener("showConfiguration", function(){ 
  console.log("Showing Configuration v" + VERSION);
  //Pebble.openURL(BASE_URL + "/user/home?info=" + Pebble.getAccountToken() + "," + VERSION);
  Pebble.openURL(BASE_URL + "/layout.html?info=" + Pebble.getAccountToken() + "," + VERSION);
});

/**
* Assambles and sends a message to Pebble
* @param: String: messageType: String representing the type of message
* @param: String: messageHeader:  Delimiter separated message Header
* @param: Object: message: Object with (key, value) pairs with integer keys starting at MESSAGE_DATA_ROOT
*/
function sendMessage(messageType, messageHeader, message) {
  message[0] = messageType; 
  message[1] = messageHeader; 
  console.log("sending message of type: " + messageType); 
  MessageQueue.sendAppMessage(message, function(e) { console.log("send yes"); }, function(e){console.log("ERROR: " + e.error.message); console.log("send no"); });
}
/**
* Constructs a workout message and sends it to Pebble
* @param String: WorkoutName: name of workout
* @param Array: moves: Array of moves as encoded by OWM
*/
function sendWorkout(workoutName, moves) {
  console.log("sending workout" + workoutName + " with moves: " + moves); 
  
  
  var messageType = "MOVES"; 
  var messageHeader; 
  var i, len_i, j, len_j, chunk, move, message, messageIndex;
  len_i = moves.length; 
  var numMessagesToSend = Math.max(0,Math.round(len_i/MESSAGE_CHUNK_LENGTH)-1); 
  
  //Split each message into chunks. 
  for (i=0; i<len_i; i+=MESSAGE_CHUNK_LENGTH) { 
    
    messageHeader= [safeWorkoutName(workoutName), Math.round(i/MESSAGE_CHUNK_LENGTH), numMessagesToSend].join(MESSAGE_DELIMITER); //Format: | WorkoutName | Message # | Total # of messages to expect |
    message = {}; 
    
    chunk = moves.slice(i, i+MESSAGE_CHUNK_LENGTH);
    
    messageIndex = MESSAGE_DATA_ROOT; //Start moves at MESSAGE_DATA_ROOT to make space for type and header
    //Add each move in chunk to message
    for (j=0, len_j=chunk.length; j<len_j; j++) { 
      move = chunk[j]; 
      message[messageIndex] = [safeMoveName(move.name), move.type, move.value].join(MESSAGE_DELIMITER); 
      messageIndex++; 
      console.log("constructing message");
    }
    
    sendMessage(messageType, messageHeader, message); 
  }
  
}


/**
* Parses workout, save them to localStorage and send titles to Pebble
* @param String: jsonString: JSON string to parse 
*/
function parseAndAddWorkouts(jsonString) { 
  console.log("Configuration window returned: " + jsonString); //See repo for json structure

  var json; 
  json = JSON.parse(decodeURIComponent(jsonString));  

  var workoutTitle,moves, workoutInfo, messageIndex=MESSAGE_DATA_ROOT, message={}; 

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

/**
* Handles message requests from Pebble by finding and sending requested workout
* @param String: workoutName: Name of requested workout
*/
function workoutMessageHandler(workoutName) { 
  console.log("Workout Requested: " + workoutName); 
  var moves = localStorage.getItem(workoutName); 
  console.log(moves); 
  moves = JSON.parse(moves); 

  sendWorkout(workoutName, moves); 
}

/**
* Handles workoutDone messages
* @param String: workoutName: Name of recently completed workout
*/
function workoutDoneMessageHandler(workoutName) { 
  workoutCompleted(workoutName); 
}

/**
* Logs completed workouts to OWM
* @param String: workoutName: Name of recently completed workout
*/
function workoutCompleted(workoutName){ 
  console.log("sending request");

  if (Pebble.getActiveWatchInfo && Pebble.getActiveWatchInfo().firmware.major >= 3) { 
    Pebble.getTimelineToken(function(timelineToken) {
        var url = [ 
              BASE_URL,
              "/user/workout/completed/" , 
              encodeURIComponent(Pebble.getAccountToken()), "/",
              encodeURIComponent(workoutName), "/",
              timelineToken 
        ].join("");
        console.log("URL: " + url);
        postRequest(url);

      },function (error) { 
        console.log('Error getting timeline token: ' + error);
      }
    );
  }

  else { 
      var url = BASE_URL + '/user/workout/completed/' + encodeURIComponent(  Pebble.getAccountToken() )  + '/' + encodeURIComponent(workoutName) + "/none"; 
      postRequest(url);
  }
  
}

/**
* Makes a POST request to given URL
* @param String: url: URL to make request to 
*/
function postRequest(url){ 
  //Send request to server
  console.log(url);
  var req = new XMLHttpRequest();
  req.open('POST', url, true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      if(req.status == 200) {
        console.log("POST Request went through"); 
      } else { console.log('Error'); }
    }
  }; 
  req.send(null);
}

/**
* Parses messages received from Pebble. Called when phone receives a message
* @param e: Event. (see Pebble API for details)
*/
function parsePebbleMessage(e) { 
   console.log("Recieved something on phone---");
    for (var type in e.payload){
        console.log("Received message: " + e.payload[type] + " of type: " + type);
        if (type == 0 /*'WORKOUT'*/) workoutMessageHandler(e.payload[type]); 
        if (type == 1 /*'WORKOUT_DONE'*/) workoutDoneMessageHandler(e.payload[type]);           
    }
}


/**
* Returns a move name that can be safely sent to Pebble
* @param string moveName. Move to "sanitize"
*/
function safeMoveName(moveName) { 
  if (moveName.length <= MAX_MOVE_NAME_LENGTH) return moveName; 
  return moveName.slice(0,MAX_MOVE_NAME_LENGTH); 
}

/**
* Returns a workout name that can be safely sent to Pebble
* @param string workoutName. Workout to "sanitize"
*/
function safeWorkoutName(workoutName) { 
  if (workoutName.length <= MAX_WORKOUT_NAME_LENGTH) return workoutName; 
  return workoutName.slice(0,MAX_WORKOUT_NAME_LENGTH); 
}

/**
* Safely get the platform of the running watch
* @returns String representing platform
*/ 
function getWatchPlatform() { 
  return Pebble.getActiveWatchInfo ? Pebble.getActiveWatchInfo().platform : "aplite"; 
}

Pebble.addEventListener("appmessage", parsePebbleMessage);

