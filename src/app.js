//Pebble JS Component of timer. 
//By Fernando Trujano
//    trujano@mit.edu
// 6/30/2014

//Sends workouts to watch app using Pebble.sendAppMesssage

var counter = 0; 
//Parses Json and sets multiple timers 
function setTimers(moves) { 
  //moves is supposed to be a 2D Array, but for some reason it is a string. Here is my fix. 
  var moveslist = moves.split(','); //Now this is a 1D Array
  console.log(moves);
  console.log("On set timers. Move: " + moveslist[counter] + " Time: " +moveslist[counter+1] ); 
  sendMessage(moveslist[counter] , moveslist[counter+1]); //SendMessage(move,time)
  
  counter+=2; //Since moves is 1D
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
});

Pebble.addEventListener("showConfiguration", function(){ 
  console.log("Showing Configuration");
  Pebble.openURL("http://www.fernandotrujano.com/pebble");
});

//After Closing settings view
//Send Title(key) to watch app (Persiant Memory), and save JSON in Internal Memory
Pebble.addEventListener("webviewclosed",
  function(e) {
    if (e.response != "CANCELLED") {
      console.log("Configuration window returned: " + e.response); //Site should return {"title": "ABX", "moves": [["Crunchy Frog", "10"], ["move", "time"] ... ]}
      var title = JSON.parse(decodeURIComponent(e.response)).title; 
      var moves = [];
      moves = JSON.parse(decodeURIComponent(e.response)).moves;  // For some reson moves is saving as a string instead of 2D array. 
     
      window.localStorage.setItem(title, moves); 
      console.log("Local Storage Set");
      console.log(title); 
      console.log(moves[0][0]); 

      sendMessage("workout", title); 
    }
  }
);

//Recieve message from Pebble
var moves = "";

Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + e.payload[1]); 
    
    if(e.payload[1].split(',')[0]=="delete") { 
      //Remove workout from internal storage
      window.localStorage.removeItem(e.payload[1].split(',')[1]); 
      console.log("Deleted item from storage:"+ e.payload[1].split(',')[1]);
    }
  
    else if (e.payload[1] != "done"){ //Begin Workout     
        moves = window.localStorage.getItem(e.payload[1]);
        counter = 0; //Reset counter
        setTimers(moves);      
    }

    else { 
      //console.log("Name was done, about to set another timer with moves: "+ moves + " and counter: "+ counter); 
        if( counter+1 < moves.split(',').length) //If there is at least one move left
          {   setTimers(moves); } 
      else sendMessage("end", ""); 
    } 
  }
);
