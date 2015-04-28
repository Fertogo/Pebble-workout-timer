$(document).ready(function(){

    function resetAddWorkoutForm(){
        console.log("Resetting Add Workout Form...");
        //Reset Form
        workout = {};
        workout.moves = [];
        workout.title = "";
        $('#add-total-time').html('');
        $("#titletext").html("");
        $("#title").val("");
        $("#move-name").val("");
        $("#minute-slider").val("0");
        $("#minute-slider").slider("refresh");
        $("#second-slider").val("0");
        $("#second-slider").slider("refresh");
        $('#move-list').html('');
        $('#move-list').listview('refresh');
        $("#error1").fadeOut();
        $("#error-add-workout").fadeOut();
     }

    var workout = {};
    workout.moves= [];
    var totalsecs = 0;


    $("#title").change(function(){
        $("#titletext").html("<h3>" + $("#title").val()+ "</h3>");
    });

    $("#move-type").change(function(){
        if ($("#move-type").is(':checked')) {  //Go into rep mode
            $("#min-slider-container").hide();
            $("#sec-slider-label").html("Repetitions");
        }
        else {
            $("#min-slider-container").show();
            $("#sec-slider-label").html("Seconds");
        }
    });

    // Add move the the workout based on current inputs
    $("#add-move").click(function(){
        var move = $("#move-name").val();
        var mins = $("#minute-slider").val();
        var secs = $("#second-slider").val(); //secs or reps if checked
        console.log(move);
        if (move.indexOf(',') > 0){
            $("#error1").html("Please don't user commas in move names").fadeIn();
        }
        else if (move == ""){
            $("#error1").html("Please enter a move title").fadeIn();
        }

        $("#move-type").is(':checked') ?  addRepMove(move, secs) : addTimeMove(move, mins, secs);

    });

    function addRepMove(move, reps){
        if (reps <= 0){
            $("#error1").html("You need at least one rep").fadeIn();
            return;
        }

        $("#move-list").append("<li>" + move + ' for ' + reps + (reps == 1 ? " rep" : " reps") + "</li>");

        workout.moves.push({
            "name" : move,
            "value" : parseInt(reps),
            "type" : "reps"
        });

        console.log(workout);
        resetMove();
    }

    function addTimeMove(move, mins, secs){
        if (!(mins > 0 || secs >0)){
            $("#error1").html("Please select a time for your move").fadeIn();
            return;
        }

        var time = (parseInt(mins) * 60)+parseInt(secs);
        console.log(time);

        totalsecs += time;
        var timetext = timeText(time);
        $("#move-list").append("<li>" + move +' for ' + timetext + "</li>");
        console.log(workout);

        workout.moves.push({  //Add to workout
            "name" : move,
            "value" : time,
            "type" : "time"
        });

        console.log("appended " + title);
        console.log(workout);

        $('#add-total-time').html("Total Time: "+timeText(totalsecs));
        resetMove();
    }

    function resetMove() {
        //Reset Values
        $("#move-name").val("");
        $("#minute-slider").val("0");
        $("#minute-slider").slider("refresh");
        $("#second-slider").val("0");
        $("#second-slider").slider("refresh");
        $('#move-list').listview('refresh');
        $("#error1").fadeOut();
    }


    //Save new workout if valid
    $("#add-workout-save-btn").click(function() {

        var title = $("#title").val();
        while (title.slice(-1)==" ") title = title.slice(0,-1); //Don't allow trailing spaces

        var same_title = false;
        $.each(json.workouts, function(index, workout){
            if (title == workout.title) same_title = true;
        });

        //Form Validation
        if (title == "") { //No title entered
            $("#error-add-workout").fadeIn();
            $("#error-add-workout").html("Please enter a title");
        }

        else if (same_title){ //Title already exists
            $("#error-add-workout").html("Workout title already exists. Please change.");
            $("#error-add-workout").fadeIn();
        }

        else if (title.indexOf(',') > 0){ //Valid title name (no commas)
            $("#error-add-workout").html("Plase don't use commas in your title");
            $("#error-add-workout").fadeIn();
        }

        else if ($("#move-list").html() == " "){  //No moves entered
            $("#error-add-workout").html('Please enter at least one move');
            $("#error-add-workout").fadeIn();
        }

        else {  //Looks good
            totalsecs = 0;
            workout.title = title;
            console.log(workout);
            json.workouts.push(workout);
            populateHTML();
            window.location = '#main';
            resetAddWorkoutForm();
        }

    });

    $("#add-workout-cancel-btn").click(function(){
        resetAddWorkoutForm();
    });
});
