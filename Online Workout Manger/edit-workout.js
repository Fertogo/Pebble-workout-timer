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

        $("#edit-title").val(move[0]);
        $("#edit-minute-slider").val((move[1] - move[1]%60)/60);
        $("#edit-second-slider").val(move[1]%60);

        $("#edit-second-slider").slider("refresh");
        $("#edit-minute-slider").slider("refresh");


        $("#edit-save-btn").val(id[0]+','+id[1]);
    });

    $(document).on('click', '.add-move-to-existing', function(){

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
        //Edit json
        var id = $(this).val().split(',');
        var title = $("#edit-title").val();
        console.log(title);
        var mins = $("#edit-minute-slider").val();
        var secs = $("#edit-second-slider").val();
        if (title != "" && (mins > 0 || secs >0) && !(title.indexOf(',') > 0)) {
            var time = (parseInt(mins)*60)+parseInt(secs);
            json.workouts[id[0]].moves[id[1]] = [title , time];
            console.log("Edited");
            populateHTML();
            console.log(id);
            $("#popup-error").fadeOut();
            $("#edit-popup").popup('close');
        }
        else $("#popup-error").fadeIn();
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
