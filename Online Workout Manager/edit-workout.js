$(document).ready(function(){

    // Array Remove - By John Resig (MIT Licensed)
    Array.prototype.remove = function(from, to) {
      var rest = this.slice((to || from) + 1 || this.length);
      this.length = from < 0 ? this.length + from : from;
      return this.push.apply(this, rest);
    };

    $(document).on('click', '.editlink', function(){
        $("#edit-delete-btn").show();
        $( "#edit-popup" ).enhanceWithin().popup();
        $("#popup-text").html('<small> Edit Move </small> ');

        console.log("click");
        var id = this.id.replace(/id/,'').split(',');
        console.log(id);
        var move = json.workouts[id[0]].moves[id[1]];
        console.log(move);

        $("#edit-title").val(move.name);
        if (move.type == "reps"){
            $("#edit-minute-container").hide();
            $("#edit-second-slider").val(move.value);
            $("#edit-second-slider-label").text("Repetitions");
            $("#edit-move-type").prop('checked',true);

        }
        else {
            $("#edit-minute-container").show();
            $("#edit-minute-slider").val((move.value - move.value%60)/60);
            $("#edit-second-slider").val(move.value%60);
            $("#edit-second-slider-label").text("Seconds");
            $("#edit-move-type").prop('checked',false);

        }

        $("#edit-second-slider").slider("refresh");
        $("#edit-minute-slider").slider("refresh");
        $("#edit-move-type" ).flipswitch( "refresh" );

        $("#edit-save-btn").val(id[0]+','+id[1]);
    });

    $("#edit-move-type").change(function(){
        if ($("#edit-move-type").is(':checked')) {  //Go into rep mode
            $("#edit-minute-container").hide();
            $("#edit-second-slider-label").html("Repetitions");
        }
        else {
            $("#edit-minute-slider").val(0);
            $("#edit-minute-slider").slider("refresh");
            $("#edit-minute-container").show();
            $("#edit-second-slider-label").html("Seconds");
        }
    });
    $(document).on('click', '.add-move-to-existing', function(){

        $("#edit-move-type").prop('checked',false);
        $("#edit-move-type" ).flipswitch( "refresh" );

        $("#edit-delete-btn").hide();
        $( "#edit-popup" ).enhanceWithin().popup();
        $("#popup-text").html('<small>Add Move</small>');

        console.log("click. Trying to add move");
        var id = this.id.replace(/id/,'');
        console.log(id);
        $("#edit-title").val('');
        $("#edit-minute-slider").val(0);
        $("#edit-second-slider").val(0);
        $("#edit-second-slider").slider("refresh");
        $("#edit-minute-slider").slider("refresh");
        $("#edit-save-btn").val(id+','+json.workouts[id].moves.length);//Add new move to workout id[0]
    });

    $(document).on('click', '.delete-workout', function(){
        var id = this.id.replace(/id/,'')
        json.workouts.remove(parseInt(id));
        console.log("Removing workout with id: "+id);
        populateHTML();
    });

    $("#edit-save-btn").click(function(){
        console.log("Edit save")
        //Edit json
        var id = $(this).val().split(',');
        var title = $("#edit-title").val();
        console.log(title);
        var mins = $("#edit-minute-slider").val();
        var secs = $("#edit-second-slider").val();
        var move = json.workouts[id[0]].moves[id[1]] || {"type": "time", "value" : 0, "name" : ""};


        if ($("#edit-move-type").is(':checked')){ //TODO DRY
            if (secs <= 0 || title == "") {
                $("#popup-error").fadeIn();
                return;
            }
            move.name = title;
            move.value = secs;
            move.type = "reps";

            if (!json.workouts[id[0]].moves[id[1]]) json.workouts[id[0]].moves.push(move)

            console.log("Edited");
            populateHTML(id[0]);
            console.log(id);
            $("#popup-error").fadeOut();
            $("#edit-popup").popup('close');
        }

        else { //Regular Time workout
            if (title != "" && (mins > 0 || secs >0) && !(title.indexOf(',') > 0)) {
                var time = (parseInt(mins)*60)+parseInt(secs);
                move.name = title;
                move.value = time;
                move.type = "time";

                if (!json.workouts[id[0]].moves[id[1]]) json.workouts[id[0]].moves.push(move)
                console.log("Edited");
                populateHTML(id[0]);
                console.log(id);
                $("#popup-error").fadeOut();
                $("#edit-popup").popup('close');
            }
            else $("#popup-error").fadeIn();
        }
    });

    $("#edit-cancel-btn").click(function(){
        $("#popup-error").fadeOut();
        $("#edit-popup").popup('close');
    });

    $("#edit-delete-btn").click(function(){
        var id = $("#edit-save-btn").val().split(',');
        //Deleting the last move --> Delete workout
        if (json.workouts[id[0]].moves.length == 1)  json.workouts.remove(parseInt(id[0]));
        else json.workouts[id[0]].moves.remove(parseInt(id[1]));

        populateHTML();
        $("#edit-popup").popup('close');
    });

});
