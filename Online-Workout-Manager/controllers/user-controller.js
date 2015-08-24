var mongoose = require('mongoose');

UserController = {};

UserController.getWorkouts = function(req, res, next) {

    var userid = req.params.userid,
        version = req.params.version;

    console.log(userid);

    //TODO Check for old token
    mongoose.model('User').findOrCreate({ id: userid}, function (err, user) {
        if (err) {
          console.log(err);
          res.send('an error occured :(');
        }
        console.log("got user: ");
        user.currentVersion = version;
        res.json({"workouts" : user.workouts});
        user.save();
    });
};

/**
* Save all the workouts to the user
* @param userid
* @param workouts
*/
UserController.saveWorkouts = function(req, res, next){
    var workouts = req.body.workouts,
        userId = req.body.id;

    console.log("in user-controller saveWorkouts");
    mongoose.model('User').findOne({"id": userId}, function(err, user){
        if (err) {
           console.log("DB ERROR");
           res.sendStatus(500);
        } else if (user){
            console.log("about to save");

            // console.log(user)
            user.saveWorkouts(workouts, function(err){
                if (err) {
                    console.log("error");
                    res.sendStatus(500);
                }
                else {
                    console.log("workouts saved");
                    res.send("ok");
                }
            });
        } else {
           console.log("User not found");
           res.send(404);
        }
    });

};

UserController.completeWorkout = function(req, res, next) {
    console.log("usercontroller -> completeWorkout");

    console.log("workout completed request");
      var userId = req.params.id,
          workoutName = req.params.name,
          date = Date.now(), //TODO send Date in request
          timelineId = req.params.timelineId;
    console.log(userId);
    console.log(workoutName);

    mongoose.model('User').findOne({"id": userId}, function(err, user){
        if (err) {
            console.log("db error");
            res.sendStatus(404);
            return;
        }
        if (!user) {
            console.log("user " + userId + " not found");
            return res.sendStatus(404);
        }
        // console.log(user)
        user.completeWorkout(workoutName, date, timelineId, function(err){
            if (!err){
                console.log("workout marked as completed");
                res.send("ok");
            }
            else {
                res.send(err);
                console.log(err);
                return;
            }
        });

     });

};

module.exports = UserController;
