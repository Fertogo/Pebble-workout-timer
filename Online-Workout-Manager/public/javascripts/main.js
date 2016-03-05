
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

/**
* Populate the workouts HTML based on json.
* @param index index of workout to expand after populating
*/
function populateHTML(index) {

    $("#workout-list").html(''); //Reset div. Yes, I am redrawing everytime, could be optimized.
    $.each(json.workouts, function(i, workout) {  //Add workouts
            var maintotaltime = 0;
            var workout_title = workout.title.replace(/\s/g, '-');
            var jQuerySelectorsRe = new RegExp('[\\[!"#$%&\'()*+,.:;<=>?@^`{|}~\\]]',"g");
            var workout_selector = workout_title.replace(jQuerySelectorsRe, "\\$&");
            // var workout_selector = workout_title.replace(jQuerySelectorsRe, "-");

            var workoutHasReps = false;

            $.each(workout.moves, function(j, move){ maintotaltime += (move.type == "reps" ? 0 :parseInt(move.value)); if (move.type == "reps") workoutHasReps = true;  }); //Please optimize
            $("#workout-list").append('<div data-role="collapsible" id="workoutcollapsible' + i + '" > <h4>'+ workout.title + ' <span class="totaltime" style="float:right" id="total-time">'+ timeText(maintotaltime) + (workoutHasReps ? " + reps" : "")+' </span>  </h4> <ul class="move-list" id="' + workout_title+'" data-role="listview">');
            $.each(workout.moves, function(j, move){  //Add moves
                $("#"+workout_selector).append('<li data-icon="gear"> <a href="#edit-popup" id="'+ i + ','+j+ '" class="editlink" data-rel="popup" data-position-to="window" data-role="button"  data-transition="pop">'+move.name+' <small class="totaltile"> for ' + (move.type == "reps" ? move.value + " reps" : timeText(parseInt(move.value)))+'</small>   <p class="ui-li-aside">Edit Move</p> </a> </li> ');
                console.log(maintotaltime);
            });
                $("#"+workout_selector).append('<li><fieldset class="ui-grid-a"><div class="ui-block-a"><a href="#edit-popup" id="'+ i+ '" class="add-move-to-existing" data-rel="popup" data-position-to="window" data-role="button"  data-transition="pop"><button class="li-btn" data-mini="true" data-icon="plus" >Add Move</button></a> </div><div class="ui-block-b"><a href="#" id="'+ i+ '" class="delete-workout"><button data-mini="true" class="delete li-btn" data-icon="delete">Delete Workout</button></a> </div></fieldset></li>  ');

             $("#workout-list").append('</ul> </div>');//.trigger('create');

        });
        //Refresh
        $('#workout-list').collapsibleset().collapsibleset('refresh');//.trigger('create'); ;
        if (index) $('#workoutcollapsible'+index).collapsible("expand")

        $('.move-list').listview().listview('refresh').trigger('create');

        initMoveList();

        //initWorkoutList();
}
function initWorkoutList(){
    var workoutList = document.getElementById('workout-list');

    workoutList.addEventListener('slip:beforereorder', function(e){
        if (/editlink/.test(e.target.className)) e.preventDefault(); // Don't reorder moves, only workouts
        else {
            //Collapse workout if opened
            console.log("moving workout")
            console.log(e)
            $(e.target.parentElement.parentElement).collapsible("collapse")
        }
    }, false);

    workoutList.addEventListener('slip:beforeswipe', function(e){
       // if (e.target.nodeName == 'INPUT' || /demo-no-swipe/.test(e.target.className)) {
            e.preventDefault();
       // }
    }, false);

    workoutList.addEventListener('slip:beforewait', function(e){
        console.log("Workout Wait");
        console.log(e)

        if (e.target.className.indexOf('instant') > -1) e.preventDefault();
    }, false);

    workoutList.addEventListener('slip:afterswipe', function(e){
        e.target.parentNode.appendChild(e.target);
    }, false);

    workoutList.addEventListener('slip:reorder', function(e){
        e.target.parentNode.insertBefore(e.target, e.detail.insertBefore);
        console.log("Move from " + e.detail.originalIndex + " to " + e.detail.spliceIndex);
        json.workouts.move(e.detail.originalIndex, e.detail.spliceIndex);
        return false;
    }, false);

    workoutList.addEventListener('slip:swipe', function(e){
        return false;
    }, false);

    new Slip(workoutList);

}

function initMoveList(){
    $("#workout-list").children().each(function() {
        var moveList = this.getElementsByClassName("move-list")[0];
        console.log(moveList);

        moveList.addEventListener('slip:beforereorder', function(e){
            console.log("Before order Moves");
            console.log(e.target.id)

        }, false);

        moveList.addEventListener('slip:beforewait', function(e){
            console.log("Move Wait");
            console.log(e);

            if (e.target.className.indexOf('instant') > -1) e.preventDefault();
        }, false);

        moveList.addEventListener('slip:reorder', function(e){
            id = e.target.children[0].id.split(",")
            e.preventDefault() //Don't count as click (aka open Edit-popup)

            if (e.detail.spliceIndex >= json.workouts[id[0]].moves.length) return false

            e.target.parentNode.insertBefore(e.target, e.detail.insertBefore);
            console.log("Move from " + e.detail.originalIndex + " to " + e.detail.spliceIndex);
            console.log("On workout " + id[0] + " move " + id[1]);
            json.workouts[id[0]].moves.move(e.detail.originalIndex, e.detail.spliceIndex)
            populateHTML(id[0]);//Update all ID's
            return false;
        }, false);


        new Slip(moveList);
    })

}

Array.prototype.move = function(from, to) {
    this.splice(to, 0, this.splice(from, 1)[0]);
};

var json = {};

$(document).ready(function(){

    //Parse URL
    var parser = document.createElement('a');
    parser.href = document.URL;
    var urlinfo = parser.search.slice(6).split(','); //Split url into array of info. Current Format [token,version]
    var token = urlinfo[0]; //Unique to Pebble account and this app!
    var version = parseFloat(urlinfo[1]) ;
    var jsonstring = false;

    if (version < 3.0) {
        $("#update").show();
        if (confirm("New Version available. You must update to v3.x to continue. ")){
            document.location = "https://apps.getpebble.com/applications/53b8f8d4c09b06bcc7000007";
        }
        else {
            document.location = "pebblejs://close#"
            document.location = "https://apps.getpebble.com/applications/53b8f8d4c09b06bcc7000007";
        }
    }
    else if (version < 4.0){
        $("#update").show();
    }
    else {
        $("#heart").show();
        $("#new").show();


    }

    //Initialize Popup
    $( "#edit-popup" ).enhanceWithin().popup();
    $('.slider').slider();

    $.ajax({
      url: '/user/' + token + '/' + version,
      complete: function (response) {
        if (response.responseText){
            console.log("got a response! ");
            jsonstring = response.responseText.replace(/\\"/g, '"'); //Unescape escaped ""
            json = $.parseJSON(jsonstring);
            $("#loading").hide();
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

            info = {
                "workouts" : json.workouts,
                "id" : token
            }
            $.ajax({
              method: "PUT",
              url: '/user/workout/save',
              data: info,
              processData: true,
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
    });

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

