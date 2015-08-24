var mongoose = require('mongoose');
var schedule = require('node-schedule');


AdminController = {};

AdminController.importUser = function(req,res,next) {
    var workouts = req.body.workouts,
        id       = req.body.user;
    console.log(id);

    mongoose.model('User').findOrCreate({"id": id}, function(err, user){
        // console.log("user created")
        user.workouts = workouts;
        user.save(function(err){
            if (!err) {
                // console.log("user saved successfully!")
                res.send('ok');
            }
            else {
                console.log(err);
                res.sendStatus(500);
            }
        });

    });

};


AdminController.deleteUser = function(req, res, next){
    console.log("AdminController > delteUser");
    var id = req.params.id;
    console.log(id);

    mongoose.model('User').findOne({"id": id}, function(err, user){
        if (err) {
            console.log("dberror");
        } else if (user) {
            console.log("user already exists");
            user.remove(function(err){
              if (!err) {
                console.log("user removed!");
                res.send('ok');
              }
            });
        } else {
            console.log("user not found");
        }
    });
};

AdminController.getUser = function(req, res, next){
    var id = req.params.id;
    mongoose.model('User').findOne({"id": id}, function(err, user){
        if (err) {
            console.log("dberror");
        } else if (user) {
            console.log("user already exists");
            res.json(user);
        } else {
            console.log("user not found");
            res.send('user not found');
        }
    });
};

AdminController.getStats = function ( req, res, next){
    getStats(req, res, next);
    res.send("getting stats...");
}


function countAllWorkouts(callback) {
    //Count total number of workouts


    mongoose.model('User').aggregate(
        { $project: { workouts: 1 }},
        { $unwind: "$workouts" },
        { $group: { _id: "result", count: { $sum: 1 }}}, function(err, data){
            var count = data[0].count;
            if (err) return callback(-1);
            callback(count);
        }
    );
}

function countAllUsers(callback){
    //Get totalUsers
    mongoose.model('User').count({}, function(err, count){
        if (err) return callback(-1);
        callback(count);
    });
}

function countAllMoves(callback){
    //total number of moves
    mongoose.model('User').aggregate(
        { $project: { workouts: 1 }},
        { $unwind: "$workouts" },
        { $unwind: "$workouts.moves" },
        { $group: { _id: "result", count: { $sum: 1 }}},
        function(err, data){
            if (err) callback(-1);
            callback(data[0].count);
        }
    );

}

function countTotalMoveTime(callback){
    //total values of time moves
    mongoose.model('User').aggregate(
        { $project: { workouts: 1 }},
        { $unwind: "$workouts" },
        { $unwind: "$workouts.moves" },
        { $match : {"workouts.moves.type" : "time"}},
        { $group: { _id: "result", count: { $sum: "$workouts.moves.value" }}},
        function(err, data){
            if (err) callback(-1);
            callback(data[0].count);
        }
    )
}

function countTotalWorkedOutTime(callback){
    mongoose.model('User').find({}, function(err, users){
        var sinceDate = new Date();
        sinceDate.setYear('2010');
        var i, len, user, totalTime = 0;
        for (i=0, len=users.length; i<len; i++) {
            user = users[i];
            totalTime += user.timeWorkedSince(sinceDate)
        }
        console.log(totalTime)
        callback(totalTime);
    })
}
AdminController.showStats = function(req,res,next) {
    mongoose.model('Stat').find({}, function(err, stats) {
        if (err) return res.send('DB Error');
        console.log(stats);
        // res.json(stats)
        res.render('admin', {"stats" : stats});
    });
};

function scheduleStats() {
    console.log("Scheduling stats");
    var j = schedule.scheduleJob('0 0 0 * * *', function(){ //Every day
        console.log("*** GETTING STATS ***");
        getStats();
    });
}

AdminController.scheduleStats = function(req, res, next){
    scheduleStats();
    res.send('scheduled');
};

function getStats(req,res,next) {
    //Get totalUsers
    mongoose.model('Stat').findOrCreate({"name" : "totalUsers"}, function(err, stat){
        if (err) return console.log(err);
        countAllUsers(function(totalUsers){
            if (err) return console.log(err);
            data = {
                "value" : totalUsers,
                "date" : new Date()
            };
            stat.data.push(data);

            stat.save(function(err){
                if (err) res.send(err);
                console.log("totalUsers : " + totalUsers);
            });
        });
    });

    mongoose.model('Stat').findOrCreate({"name" : "totalWorkouts"}, function(err, stat){
        if (err) return console.log(err);
        countAllWorkouts(function(totalWorkouts){
            stat.data.push({
                "value" : totalWorkouts,
                "date"  : new Date()
            });

            stat.save(function(err){
                if (err) return console.log(err)
                console.log("totalWorkouts : " + totalWorkouts);
            });
        });
    });

    mongoose.model('Stat').findOrCreate({"name" : "totalMoves"}, function(err, stat){
        if (err) return res.send(err);
        countAllMoves(function(totalMoves){
            stat.data.push({
                "value" : totalMoves,
                "date"  : new Date()
            });

            stat.save(function(err){
                if (err) return res.send(err);
                console.log("totalMoves : " + totalMoves);
            });
        });
    });

    mongoose.model('Stat').findOrCreate({"name" : "totalMoveTime"}, function(err, stat){
        if (err) return console.log(err);
        countTotalMoveTime(function(totalMoveTime){
            stat.data.push({
                "value" : totalMoveTime,
                "date"  : new Date()
            });
            stat.save(function(err){
                if (err) return console.log(err);
                console.log("totalMoveTime : " + totalMoveTime);
            });
        });
    });

    mongoose.model('Stat').findOrCreate({"name" : "totalTimeWorkedOut"}, function(err, stat){
        if (err) return console.log(err);
        countTotalWorkedOutTime(function(totalTime){
            stat.data.push({
                "value" : totalTime,
                "date"  : new Date()
            });
            stat.save(function(err) {
                if (err) return console.log(err);
                console.log("totalTimeWorkedOut: " + totalTime);
            });
        });
    });

    console.log("Got stats");
    // res.send("I think I got them")
    // db.getCollection('users').aggregate(
    //     { $project: { workouts: 1 }},
    //     { $unwind: "$workouts" },
    //     { $group: { _id: "result", count: { $sum: 1 }}}
    // );

    //

    //db.getCollection('users').find({"currentVersion": {$ne: "3.5"}})

    //How many workouts per user
    // db.getCollection('users').aggregate(
    //    [
    //       {
    //          $project: {

    //             workouts: { $size: "$workouts" }
    //          }
    //       }
    //    ]
    // )

    //TODO total seconds worked out since Date

    //get num ppl that joined after certain data
    // db.getCollection('users').count({
    //     "dateJoined" : {"$gte": ISODate("2015-07-20T00:00:00Z")}
    // })
}


module.exports = AdminController;
