//Pebble JS Component of timer. 
//Sends workouts to watch app using Pebble.sendAppMesssage

//Correctly sends message
/*
Todo: 
  Make URL return JSON formated workout
  Stringify workout and send to watchapp
*/

Pebble.addEventListener("ready", function(e){
    console.log("JS code running");  
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
      console.log("Configuration window returned: " + e.response);
      var title = JSON.parse(decodeURIComponent(e.response)).title; 
      window.localStorage.setItem(title, e.response); 
      console.log("Local Storage Set");
      console.log(title); 
      var jsonstring = window.localStorage.getItem(title);  
      //var json = JSON.parse(decodeURIComponent(jsonstring)); 
      // console.log(title.dd); 
      
      //Send Message to Pebble
      console.log("sending message to watchapp");
      Pebble.sendAppMessage( { "0": "workout", "1": title },
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
    
    var name = e.payload; 
    var time = window.localStorage.getItem(name); 
    
    //Send Message to Pebble 
    console.log("sending timer message to watchapp");
      Pebble.sendAppMessage( { "0": name, "1": time  },
        function(e) {
          console.log("Successfully delivered message");  
        },
        function(e) {
          console.log("Unable to deliver message " + " Error is: " + e.error.message);
        }                        
      );
  }
);








