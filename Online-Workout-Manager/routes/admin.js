var express = require('express'),
    router = express.Router(),
    mongoose = require('mongoose'),
    adminController = require('../controllers/admin-controller'),

    bodyParser = require('body-parser'),
    app = express();

app.use(bodyParser.json());

/**
* Create a new user populate with contents
* @param user: token/id of the user
* @param workouts: list of workouts to import
*/
router.put('/import/user', adminController.importUser);

router.get('/scheduleStats', adminController.scheduleStats);

router.get('/getStats', adminController.getStats);


router.get('/', adminController.showStats);

router.get('/user/:id', adminController.getUser);

// delete user with :id
router.post('/user/delete/:id', adminController.deleteUser);


module.exports = router;
