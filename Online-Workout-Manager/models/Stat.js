var mongoose = require('mongoose');

var statSchema = new mongoose.Schema({
    name : {type: String, required: true},
    data : [{ _id : false,
        date : {type: Date},
        label : {type: String},
        value : {type: Number }
    }]

});

statSchema.statics.findOrCreate = function(parameters, callback){
    mongoose.model('Stat').findOne(parameters, function(err, stat){
        if (err) {
            callback(err);
        } else if (stat) {
            callback(null, stat);
        } else {
            // console.log("creating stat")
            mongoose.model('Stat').create(parameters, callback);
        }
    });
};

module.exports = mongoose.model('Stat', statSchema);
