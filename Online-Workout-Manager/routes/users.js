var express = require('express'),
    router  = express.Router(),
    userController = require('../controllers/user-controller'),
    mongoose   = require('mongoose'),
    bodyParser = require('body-parser'),
    app = express();

app.use(bodyParser.json());

// Get workouts for :userid
router.get('/:userid/:version', userController.getWorkouts);

// Show the workout-manager homepage
router.get('/home', userController.renderHome);

/* Save users workout */
router.put('/workout/save', userController.saveWorkouts);

// Mark a workout as completed
router.post('/workout/completed/:id/:name/:timelineId', userController.completeWorkout);

module.exports = router;
