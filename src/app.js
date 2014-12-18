//Pebble JS Component of timer. 
//By Fernando Trujano
//    trujano@mit.edu
// 12/10/2014
var version = "2.5";
//Sends workouts to watch app using Pebble.sendAppMesssage

var counter = 0; 
//Parses Json and sets multiple timers 
function setTimers(moves) { 
  //moves is supposed to be a 2D Array, but for some reason it is a string. Here is my fix. 
  var moveslist = moves.split(','); //Now this is a 1D Array
  console.log("On set timers. Move: " + moveslist[counter] + " Time: " +moveslist[counter+1] ); 
  sendMessage(moveslist[counter] , moveslist[counter+1]); //SendMessage(move,time)
  
  counter+=2; //Since moves is 1D
  window.localStorage.setItem("currentMoveCounter", counter); 
  console.log("Timer set, incrementing counter to " + counter); 
}

function sendMessage(type, message){ 
  //Send Message to Pebble 
    console.log("sending  message to watchapp " + type +" : "+ message); 
      Pebble.sendAppMessage( { "0": type, "1": message  },
        function(e) { console.log("Successfully delivered message");  },
        function(e) { console.log("Unable to deliver message " + " Error is: " + e.error.message); }                        
      );
}

Pebble.addEventListener("ready", function(e){
    console.log("JS code running!");  
    sendMessage("ready"); 
});

Pebble.addEventListener("showConfiguration", function(){ 
  console.log("Showing Configuration v2.0");
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

        sendMessage("end", ""); 
      }
}

var moves = "";
//Recieve message from Pebble
Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + e.payload[1]); 
    
    if(e.payload[1].split(',')[0]=="delete") { 
      //Remove workout from internal storage
      window.localStorage.removeItem(e.payload[1].split(',')[1]); 
      console.log("Deleted item from storage:"+ e.payload[1].split(',')[1]);
    }
      
    else if (e.payload[1] == "resumeWorkout"){ 
      console.log("RESUME WORKOUT MESSAGE"); 
      currentWorkoutName = window.localStorage.getItem("currentWorkoutName");
      moves = window.localStorage.getItem(currentWorkoutName); 
      counter = parseInt(window.localStorage.getItem("currentMoveCounter"));       
      advanceWorkout();           
      
    }
    else if (e.payload[1] != "done"){ //Begin Workout     
        moves = window.localStorage.getItem(e.payload[1]);
        counter = 0; //Reset counter
        window.localStorage.setItem("currentMoveCounter", counter); 
        window.localStorage.setItem("currentWorkoutName", e.payload[1]); 

        setTimers(moves);      
    }

    else { 
      //console.log("Name was done, about to set another timer with moves: "+ moves + " and counter: "+ counter); 
      advanceWorkout(); 
    } 
  }
);
