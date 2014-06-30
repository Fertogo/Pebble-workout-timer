//Pebble JS Component of timer. 
//Sends workouts to watch app using Pebble.sendAppMesssage

//Correctly sends message
/*
Todo: 
  Make URL return JSON formated workout
  Stringify workout and send to watchapp
*/

Pebble.addEventListener("ready", function(e){
    console.log("JS code Started fo real");  
});

Pebble.addEventListener("showConfiguration", function(){ 
  console.log("Showing Configuration fo real");
  Pebble.openURL("http://www.fernandotrujano.com/pebble");
});

//After Closing settings view
Pebble.addEventListener("webviewclosed",
  function(e) {
    console.log("Configuration window returned: " + e.response);
    //Send Message to Pebble
   if (e.response != "CANCELLED") {
    console.log("sending message");
    Pebble.sendAppMessage( { "0": 42, "1": e.response },
      function(e) {
        console.log("Successfully delivered message");  
      },
      function(e) {
        console.log("Unable to deliver message " + " Error is: " + e.error.message);
      }
                          
    );
   }
  }
);


//Recieve message from Pebble
Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + e.payload);   
  }
);








