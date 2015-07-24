var mongoose = require('mongoose');
var Timeline = require('pebble-api');

var userSchema = new mongoose.Schema({
    id: { type: 'String', required: true, index: true},
    timelineId: { type: 'String'},
    dateJoined: { type: 'Date', default: Date.now },
    lastActiveDate: { type: 'Date', default: Date.now },
    workouts : [{ _id: false,
        title : { type: 'String', required: true},
        datesCompleted: { type: ['Date'], required: false },
        moves: [{ _id: false,
            name : { type: 'String', required: true},
            type: {
                type: 'String',
                enum: {
                  values: 'reps time'.split(' '),
                },
                required: true
            },
            value: { type: 'Number', min: 0, required: true}
        }]
    }],
    currentVersion: {type: 'String'}
});


userSchema.set('autoIndex', false);

userSchema.statics.findOrCreate = function(parameters, callback){
    mongoose.model('User').findOne({"id": parameters.id}, function(err, user){
        if (err) {
            callback(err);
        } else if (user) {
            // console.log("user already exists")
            user.lastActiveDate = Date.now();
            callback(null, user);
        } else {
            // console.log("creating user")
            mongoose.model('User').create(parameters, callback);
        }
    });
};

/**
* save all workouts to the user, maintain dates completed
*/
userSchema.method('saveWorkouts', function(workouts, callback){
    console.log("saving user workouts..====================");

    // for i in this.workouts {
    //     this.workouts.[i].name =

    // }

    this.workouts = workouts;

    console.log("set");
    this.save(function(err){
        if (err) console.log(err);
        callback(err);
    });
});

//mark a workout as completed
userSchema.method('completeWorkout', function(workoutName, date, timelineId ,callback){
    console.log("User Model -> completeWorkout");
    var foundWorkout = false;
    this.lastActiveDate = Date.now();
    this.timelineId = timelineId;
    for (var i=0; i < this.workouts.length; i++){
        if (this.workouts[i].title == workoutName){
            foundWorkout = true;
            var workout = this.workouts[i];
            workout.datesCompleted.push(date);
            if (this.timelineId && this.timelineId != 'none') {
                var body = this.getPinBody(workout);
                console.log(body);
                this.sendTimelinePin(timelineId, "Workout Finished!", workoutName, body);
            }
            this.save(callback);
        }
    }
    if (!foundWorkout) callback("workout not found");
});

function totalWorkoutTime(workout){
    if (workout.length <= 0) return 0;
    return workout.moves.reduce(function(total, move){
                if (move.type === "reps") return total;
                return move.value + total;
            },0);
}

function totalWorkoutReps(workout){
    if (workout.length <= 0) return 0;
    return workout.moves.reduce(function(total, move){
                if (move.type === "time") return total;
                return move.value + total;
            },0);
}

// Convert secs to a string in the form "x hours and n minutes"
function prettyTime(totalSecs){
    if (totalSecs < 60) return totalSecs + " seconds";
    var hours, mins, secs, result;
    hours = parseInt( totalSecs / 3600 ) % 24;
    mins = parseInt( totalSecs / 60 ) % 60;
    secs = totalSecs % 60;
    hoursText = hours > 0 ? (hours +  (hours > 1 ? " hours" : " hour")) : "";
    if (mins > 0) minsText = ((hours > 0 ? (" and " ): "") + mins + " " + (mins > 1 ? "minutes" : "minute"));
    else minsText = "";
    minsText =  mins > 0 ? (minsText = ((hours > 0 ? (" and " ): "") + mins + " " + (mins > 1 ? "minutes" : "minute")) ) : ("");

    return hoursText + minsText;
}

//Returns the total time worked out since the given date
userSchema.method('timeWorkedSince', function (sinceDate) {
    var i, len, workout, workoutTime, timesCompletedSince, totalTime =0;
    for (i=0, len=this.workouts.length; i<len; i++) {
        workout = this.workouts[i];
        if (workout.datesCompleted.length <= 0) continue;
        workoutTime = totalWorkoutTime(workout);


        timesCompletedSince =
            workout.datesCompleted.reduce(function (total, date){
                if (date < sinceDate )  return total;
                return total + 1;
            },0);
        console.log("timesCompeltedSince: " + timesCompletedSince);
        totalTime += workoutTime * timesCompletedSince;
    }
    return totalTime;
});


//Returns the pin body for a completed workout
userSchema.method('getPinBody', function (workout){
    var lastWeek = new Date();
    lastWeek.setHours(lastWeek.getHours() - (7 * 24));

    var workoutTime = totalWorkoutTime(workout),
        workoutReps = totalWorkoutReps(workout),
        timeOrReps  = workoutTime > 0 ?
                      "Workout time: " + prettyTime(workoutTime) :
                      "Workout reps: " + workoutReps;

    return [
        timeOrReps ,"\n",
        "Times completed: ", workout.datesCompleted.length,"\n",
        "Total time this week: ", prettyTime(this.timeWorkedSince(lastWeek))
    ].join("");
});

userSchema.method('sendTimelinePin', function(timelineId, title, subtitle, body){
    var timeline = new Timeline();

    var pin = new Timeline.Pin({
      id: 'workout-complete-'+ this.id +"-"+parseInt(Math.random()*1000000),
      time: new Date(),
      duration: 10,
      layout: new Timeline.Pin.Layout({
        type: Timeline.Pin.LayoutType.GENERIC_PIN,
        tinyIcon: Timeline.Pin.Icon.PIN,
        title: title,
        subtitle: subtitle,
        body: body,
        primaryColor: "#FFFFFF",
        secondaryColor: "#AAFFFF",
        backgroundColor: "#0000AA"
      })
    });

    timeline.sendUserPin(timelineId, pin, function (err) {
      if (err) {
        console.log("Pin Error: ");
        console.log(timelineId);
        return console.error(err);
      }
      console.log(timelineId);
      console.log('Pin sent successfully!');
    });

});
module.exports = mongoose.model('User', userSchema);
