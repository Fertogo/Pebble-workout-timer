//Pebble JS Component of timer. 
//By Fernando Trujano
//    trujano@mit.edu
// 12/10/2014
var version = "2.6";
//Sends workouts to watch app using Pebble.sendAppMesssage

var counter = 0; 
//Parses Json and sets multiple timers 
function setTimers(moves) { 
  //moves is supposed to be a 2D Array, but for some reason it is a string. Here is my fix. 
  var moveslist = moves.split(','); //Now this is a 1D Array
  console.log("On set timers. Move: " + moveslist[counter] + " Time: " +moveslist[counter+1] ); 
  sendMessage("move" ,moveslist[counter] , moveslist[counter+1]); //SendMessage("move",move,time)
  
  counter+=2; //Since moves is 1D
  window.localStorage.setItem("currentMoveCounter", counter); 
  console.log("Timer set, incrementing counter to " + counter); 
}

function sendMessage(type, message1, message2){ 
  //Send Message to Pebble 
      console.log("sending  message to watchapp " + type +" : "+ message1 + " : " + message2); 
      Pebble.sendAppMessage( { "0": type, "1": message1, "2": message2 },
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
  Pebble.openURL("http://fernandotrujano.com/pebble/index.html?info="+Pebble.getAccountToken()+','+version); 
});

//After Closing settings view
//Send Title(key) to watch app (Persiant Memory), and save JSON in Internal Memory
Pebble.addEventListener("webviewclosed",
  function(e) {
    if (e.response != "CANCELLED") {
      console.log("Configuration window returned: " + e.response); //Site should return {"workouts":[{"moves":[["move1",180]],"title":"AbRipperX"},{"moves":[["sdfsdfasdasdfaaaaaaa",68]],"title":"sdfsdf"},{"moves":[["sdf",60]],"title":"sdff"},{"moves":[["asdfasdf",64]],"title":"asdfdsfa"}]}

      var json; 
      json = JSON.parse(decodeURIComponent(e.response));  
     // json = JSON.parse('{"workouts":[{"moves":[["move1",180]],"title":"1"},{"moves":[["sdfsdfasdasdfaaaaaaa",68]],"title":"2"},{"moves":[["sdf",60]],"title":"3"},{"moves":[["asdfasdf",64]],"title":"4"}]}');
      var title; 
      var moves; 
      var workouts = []; 
      console.log(json); 
      window.localStorage.clear(); 
      //Add Each workout
      for (var workout in json.workouts) {    
        title = json.workouts[workout].title; 
        moves = json.workouts[workout].moves; 
        workouts.push(title); //Add to workouts array to be sent to Pebble 
        window.localStorage.setItem(title, moves);     //Save workout on local storage
      }
      
      console.log("Local Storage Set");
      sendMessage("workouts", workouts.join()); //Send workout list to Pebble
      console.log(workouts); 

    }
  }
);


//Tries to advance to the next workout. If there are no more workouts, it notifies the Pebble.   
function advanceWorkout(){ 
        if( counter+1 < moves.split(',').length) //If there is at least one move left
          {   setTimers(moves); } 
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
      moves = window.localStorage.getItem(currentWorkoutName); 
      counter = parseInt(window.localStorage.getItem("currentMoveCounter"));   
      if (goBack) counter -= 2; //go back a move since we are preloading moves 
}

//Starts a new workout based on given name
function startNewWorkout(workoutName){ 
        moves = window.localStorage.getItem(workoutName);
        counter = 0; //Reset counter
        window.localStorage.setItem("currentMoveCounter", counter); 
        window.localStorage.setItem("currentWorkoutName", workoutName); 
        setTimers(moves);  
}

var moves = "";
//Recieve message from Pebble
Pebble.addEventListener("appmessage",
  function(e) {
    for (var type in e.payload){
          console.log("Received message: " + e.payload[type] + " of type: " + type);       
    }
    
    //if (e.payload[1] == "resumeWorkout"){ 
    if ("RESUME" in e.payload){
      console.log("RESUME WORKOUT MESSAGE"); 
      console.log();
      
      e.payload["RESUME"] === "true" ? goBack = true : goBack = false; 
      restoreWorkout(goBack);       
      advanceWorkout();                
    }
    
    //else if (e.payload[1] == "restoreWorkout"){ 
    else if ("RESTORE" in e.payload){
      restoreWorkout(); 
    }
    
    //else if (e.payload[1] != "done"){ //Begin Workout  
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
