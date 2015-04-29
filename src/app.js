//Pebble JS Component of timer. 
//By Fernando Trujano
//    trujano@mit.edu
// 04/29/2015
var version = "3.0";
//Sends workouts to watch app using Pebble.sendAppMesssage

var counter = 0; 


function sendMessage(type, message1, message2, message3){ 
  //Send Message to Pebble 
  console.log("sending  message to watchapp " + type +" : "+ message1 + " : " + message2 + " : " + message3); 
  Pebble.sendAppMessage( { "0": type, "1": message1, "2": message2, "3" : message3 },
        function(e) { console.log("Successfully delivered message");  },
        function(e) { console.log("Unable to deliver message " + " Error is: " + e.error.message); }                        
      );
}

Pebble.addEventListener("ready", function(e){
    console.log("JS code running!");  
    sendMessage("ready"); 
});

Pebble.addEventListener("showConfiguration", function(){ 
  console.log("Showing Configuration v" + version);
  console.log(version); 
  Pebble.openURL("http://fernandotrujano.com/pebble/workout-manager.html?info="+Pebble.getAccountToken()+','+version); 
   // Pebble.openURL("http://fernandotrujano.com/pebble/new/workout-manager.html?info=migrate,2.7"); 

});

//After Closing settings view
//Send Title(key) to watch app (Persiant Memory), and save JSON in Internal Memory
Pebble.addEventListener("webviewclosed",
  function(e) {
    if (e.response != "CANCELLED") {
      console.log("Configuration window returned: " + e.response); //See repo for json structure

      var json; 
      json = JSON.parse(decodeURIComponent(e.response));  
      //json = JSON.parse('{"workouts":[{"moves":[{"name":"Move 1","value":6,"type":"time"},{"name":"Move 2","value":7,"type":"time"},{"name":"Move 3","value":7,"type":"time"}],"title":"Just Time"},{"moves":[{"name":"move 1","value":6,"type":"reps"},{"name":"move 2","value":5,"type":"reps"},{"name":"move 3","value":12,"type":"reps"}],"title":"Just reps"},{"moves":[{"name":"move 1","value":16,"type":"time"},{"name":"move 2 reps","value":4,"type":"reps"},{"name":"move 3 time","value":6,"type":"time"},{"name":"move 4 reps","value":14,"type":"reps"}],"title":"Both"}]}')
      
      var title; 
      var moves; 
      var workouts = []; 
      
      console.log(json); 
      window.localStorage.clear(); 
      //Add Each workout
      for (var workout in json.workouts) {    
        title = json.workouts[workout].title; 
        moves = JSON.stringify(json.workouts[workout].moves); //Concert to string before saving!
        workouts.push(title); //Add to workouts array to be sent to Pebble 
        window.localStorage.setItem(title, moves);     //Save workout on local storage
      }
      
      console.log("Local Storage Set");
      sendMessage("workouts", workouts.join()); //Send workout list to Pebble
      console.log(workouts); 

    }
  }
);

/**
* Sends messages to Pebble with next timer to set
* @param moves: list of objects representing moves {name: ,value: , type: }
**/
function setTimers(moves) { 
  console.log("On set timers. Move: " + moves[counter].name + " Time: " + moves[counter].value + " Type: " + moves[counter].type ); 
  
  sendMessage("move", moves[counter].name, moves[counter].value.toString(), moves[counter].type); 
  counter += 1; 
  window.localStorage.setItem("currentMoveCounter", counter); 
  
  console.log("Message set, incrementing counter to " + counter); 
}

//Tries to advance to the next workout. If there are no more workouts, it notifies the Pebble.   
function advanceWorkout(){ 

      if( counter+1 <= moves.length){   //If there is at least one move left
          setTimers(moves);
        } 
      else { 
        window.localStorage.removeItem("currentMoveCounter");
        window.localStorage.removeItem("currentWorkoutName");
        sendMessage("end"); 
      }
}

//Restores the state of the current workout
// @param goBack 
function restoreWorkout(goBack){
      currentWorkoutName = window.localStorage.getItem("currentWorkoutName");
      moves = JSON.parse(window.localStorage.getItem(currentWorkoutName)); 
      counter = parseInt(window.localStorage.getItem("currentMoveCounter"));   
      if (goBack) counter -= 1; //go back a move since we are preloading moves 
}

//Starts a new workout based on given name
function startNewWorkout(workoutName){ 
        console.log("Starting new workout")
        
        moves = JSON.parse(window.localStorage.getItem(workoutName)); //LocalStorage saves items as stings, need to convert

        counter = 0; //Reset counter
        window.localStorage.setItem("currentMoveCounter", counter); 
        window.localStorage.setItem("currentWorkoutName", workoutName); 
        setTimers(moves);  
}

// var moves = "";
//Recieve message from Pebble
Pebble.addEventListener("appmessage",
  function(e) {
    for (var type in e.payload){
          console.log("Received message: " + e.payload[type] + " of type: " + type);       
    }
    
    if ("RESUME" in e.payload){
      console.log("RESUME WORKOUT MESSAGE"); 
      console.log();
      
      e.payload["RESUME"] === "true" ? goBack = true : goBack = false; 
      restoreWorkout(goBack);       
      advanceWorkout();                
    }
    
    else if ("RESTORE" in e.payload){
      restoreWorkout(); 
    }
    
      else if ("WORKOUT" in e.payload){
        console.log("RECEIVED WORKOUT MESSAGE:"); 
        startNewWorkout(e.payload["WORKOUT"]); 
    }

    //else  { 
    else if ("DONE" in e.payload){
      //console.log("Name was done, ab+out to set another timer with moves: "+ moves + " and counter: "+ counter); 
      advanceWorkout(); 
    } 
  }
);

/*
* Message Protocol:
*   Pebble sends the following messages to the phone:
*      Type: RESUME - Tells the phone to restore the state of the workout and go to the next move
*      Type: RESTORE - Tells the phone to restore the sate of the workout (current workout, current move)
*      Type: DONE" - Tells the phone that the current move is done, and it should advance to the next move
*      Type: WORKOUT Data: workout-name - Tells the phone the name of a specific workout to start. 
*  In all of these cases (except "restoreWorkout"), Pebble expects a reply from the phone with either a move and time, or "end" 
*/
