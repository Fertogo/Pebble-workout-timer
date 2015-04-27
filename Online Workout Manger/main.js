
function timeText(time){
    secs = time%60;
    mins = (time-secs)/60;
    //Format time to text nicely b/c grammar Nazis
    var mintext = "minutes";
    if (mins == 1) mintext = "minute";
    var sectext = "seconds";
    if (secs == 1) sectext = "second";
    if (mins > 0){
            if (secs > 0) return  mins + " " + mintext + " and " + secs + " " + sectext +".";
            else return  mins + " " + mintext +".";
        }
        else return secs + " " + sectext + ".";
    return "Error";
}

function populateHTML() {

    $("#workout-list").html(''); //Reset div. Yes, I am redrawing everytime, could be optimized.
    $.each(json.workouts, function(i, workout) {  //Add workouts
            var maintotaltime = 0;
            var workout_title = workout.title.replace(/\s/g, '-');
            var jQuerySelectorsRe = new RegExp('[\\[!"#$%&\'()*+,.:;<=>?@^`{|}~\\]]',"g");
            var workout_selector = workout_title.replace(jQuerySelectorsRe, "\\$&");

            $.each(workout.moves, function(j, move){ maintotaltime += parseInt(move[1]) }); //Please optimize
            $("#workout-list").append('<div data-role="collapsible" id="workoutcollapsible" > <h4>'+ workout.title + ' <span class="totaltime" style="float:right" id="total-time">'+ timeText(maintotaltime)+' </span>  </h4> <ul class="move-list" id="' + workout_title+'" data-role="listview">');
            $.each(workout.moves, function(j, move){  //Add moves
                $("#"+workout_selector).append('<li data-icon="gear"> <a href="#edit-popup" id="'+ i + ','+j+ '" class="editlink" data-rel="popup" data-position-to="window" data-role="button"  data-transition="pop">'+move[0]+' <small class="totaltile"> for ' + timeText(parseInt(move[1]))+'</small>   <p class="ui-li-aside">Edit Move</p> </a> </li> ');
                console.log(maintotaltime);
            });
                $("#"+workout_selector).append('<li><fieldset class="ui-grid-a"><div class="ui-block-a"><a href="#edit-popup" id="'+ i+ '" class="add-move-to-existing" data-rel="popup" data-position-to="window" data-role="button"  data-transition="pop"><button class="li-btn" data-mini="true" data-icon="plus" >Add Move</button></a> </div><div class="ui-block-b"><a href="#" id="'+ i+ '" class="delete-workout"><button data-mini="true" class="delete li-btn" data-icon="delete">Delete Workout</button></a> </div></fieldset></li>  ');

             $("#workout-list").append('</ul> </div>');//.trigger('create');

        });
        //Refresh
        $('#workout-list').collapsibleset().collapsibleset('refresh');//.trigger('create'); ;

        $('.move-list').listview().listview('refresh').trigger('create');
}

var json = {};

$(document).ready(function(){

    //Parse URL
    var parser = document.createElement('a');
    parser.href = document.URL;
    var urlinfo = parser.search.slice(6).split(','); //Split url into array of info. Current Format [token,version]
    var token = urlinfo[0]; //Unique to Pebble account and this app!
    var version = urlinfo[1];
    var jsonstring = false;

    if (version != "2.7") $("#update").show();

    //Initialize Popup
    $( "#edit-popup" ).enhanceWithin().popup();
    $('.slider').slider();

    $.ajax({
      url:'read.php?token='+ token,
      complete: function (response) {
        if (response.responseText){
            console.log("got a response! ");
            jsonstring = response.responseText.replace(/\\"/g, '"'); //Unescape escaped ""
            json = $.parseJSON(jsonstring);
            $("#loading").hide();
            //Populate HTML
            populateHTML();
        }
      },

      error: function () {
          console.log("Something went wrong. Can't connect");
      },
    });

    $("#save-btn").click(function() {
        if (json.workouts.length < 1) $("#main-error").fadeIn();

        else {
            //Submit json file to server
            $.ajax({
              type: "POST",
              url: 'save.php?token='+token,
              data: {"data": JSON.stringify(json)},
              success: function(data, textStatus, jqXHR){
                console.log("Server received message");
                console.log(textStatus, data, jqXHR);
                $("#success").fadeIn();
                $("#main-error").hide();
                document.location = "pebblejs://close#" +encodeURIComponent(JSON.stringify(json)) ;
                },

              complete: function(){console.log("Send message to PHP")},
              error: function(jqXHR, textStatus, errorThrown) {
                console.log("Error: you are weak. ");
                console.log(textStatus, errorThrown, jqXHR.responseText);
                }
            });

            console.log("ajax funciton called");
            console.log(json);
        }
    })

    $("#cancel-btn").click(function() {
        document.location = "pebblejs://close#";
    });

    $.getJSON(
        "http://api.bitly.com/v3/shorten?callback=?",
        {
            "format": "json",
            "apiKey": "R_ceb591091d2c79f818d71577d992ae28",  //Anyone can see this from "Network" so no need to hide it :(.
            "login": "fertogo",
            "longUrl": $(location).attr('href')
        },
        function(response){
            $("#bitly-link").html('<a href="'+response.data.url+'"">'+response.data.url+'</a>');
        }
    );
});

//Analytics
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-52590022-1', 'auto');
  ga('send', 'pageview');

