//Pebble JS Component of timer. 
//Sends workouts to watch app using Pebble.sendAppMesssage

//Correctly sends message
/*
Todo: 
  Make URL return JSON formated workout
  Stringify workout and send to watchapp
*/

Pebble.addEventListener("ready", function(e){
    console.log("JS code Started");  
});

Pebble.addEventListener("showConfiguration", function(){ 
  console.log("Showing Configuration");
  Pebble.openURL("http://www.fernandotrujano.com/pebble");
});

//After Closing settings view
Pebble.addEventListener("webviewclosed",
  function(e) {
    console.log("Configuration window returned: " + e.response);
    //Send Message to Pebble
    console.log("sending message");
    var transactionId = Pebble.sendAppMessage( { "0": 42, "1": e.response },function(e) {
      console.log("Successfully delivered message with transactionId="+ e.data.transactionId);
    },
  function(e) {
    console.log("Unable to deliver message with transactionId="+ e.data.transactionId + " Error is: " + e.error.message);
  }
);
   console.log(transactionId);
  }
);


//Recieve message from Pebble
Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + e.payload);   
  }
);


/*
var UI = require('ui');
var Vector2 = require('vector2');
  var wind = new UI.Window();
  var title = new UI.Text({
    position: new Vector2(0, 50),
    size: new Vector2(144, 30),
    font: 'gothic-24-bold',
    text: 'Text Anywhere!',
    textAlign: 'center'
  });
  wind.add(title);
  var time = new UI.Text({
    position: new Vector2(0, 100),
    size: new Vector2(144, 30),
    font: 'gothic-20-bold',
    text: 'Time here',
    textAlign: 'center'
  });
  time.text("test") ;
  //wind.add(time);

var time2 = new UI.TimeText({
  text: '%M:%S',
   position: new Vector2(0, 100),
    size: new Vector2(144, 30),
    font: 'gothic-20-bold',
    
    textAlign: 'center'
  });
wind.add(time2);
  wind.show();
*/





/*
var UI = require('ui');
var Vector2 = require('vector2');

var main = new UI.Card({
  title: 'Pebble.js',
  icon: 'images/menu_icon.png',
  subtitle: 'Hello World!',
  body: 'Press any button.'
});

main.show();

main.on('click', 'up', function(e) {
  var menu = new UI.Menu({
    sections: [{
      items: [{
        title: 'Pebble.js',
        icon: 'images/menu_icon.png',
        subtitle: 'Can do Menus'
      }, {
        title: 'Second Item',
        subtitle: 'Subtitle Text'
      }]
    }]
  });
  menu.on('select', function(e) {
    console.log('Selected item: ' + e.section + ' ' + e.item);
  });
  menu.show();
});

main.on('click', 'select', function(e) {
  var wind = new UI.Window();
  var textfield = new UI.Text({
    position: new Vector2(0, 50),
    size: new Vector2(144, 30),
    font: 'gothic-24-bold',
    text: 'Text Anywhere!',
    textAlign: 'center'
  });
  wind.add(textfield);
  wind.show();
});

main.on('click', 'down', function(e) {
  var card = new UI.Card();
  card.title('A Card');
  card.subtitle('Is a Window');
  card.body('The simplest window type in Pebble.js.');
  card.show();
});

*/
