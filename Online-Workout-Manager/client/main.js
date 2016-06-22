React = require('react');
ReactDOM = require('react-dom');
$ = require('jquery');

//Parse URL
var parser = document.createElement('a');
parser.href = document.URL;
var urlinfo = parser.search.slice(6).split(','); //Split url into array of info. Current Format [token,version]
var token = urlinfo[0]; //Unique to Pebble account and this app!
var version = parseFloat(urlinfo[1]) ;

var url = '/user/' + token + '/' + version;

/**
 * REEEACCCTT
 */

var MoveForm = React.createClass({
    render: function() {
        return (
            <div>
                <div className="form-horizontal">
                    <div className="form-group">
                        <label className="col-sm-2 control-label">Name</label>
                        <div className="col-sm-10">
                            <input type="text" className="form-control" placeholder="Name" defaultValue={this.props.move.name}/>
                        </div>
                    </div>
                    <div className="form-group">
                        <label className="col-sm-2 control-label">Type</label>
                        <div className="col-sm-10">
                            <div className="btn-group">
                                <button type="button" className="btn btn-default">Reps</button>
                                <button type="button" className="btn btn-default">Time</button>
                            </div>
                        </div>
                    </div>
                    <div className="form-group">
                        <label className="col-sm-2 control-label">Value</label>
                        <div className="col-sm-10">
                            <input type="number" className="form-control" placeholder="Reps" defaultValue={this.props.move.value}/>
                        </div>
                    </div>
                </div>
            </div>
        );
    }
});

var Move = React.createClass({
    render: function() {
        return (
            <div onClick={this.props.onClick}>
                {this.props.move.name}
                <div className="pull-right">
                    <button type="button" className="btn btn-link btn-xs hidden" onClick={this.props.removeMove}>
                        <span className="glyphicon glyphicon-trash"></span>
                    </button>
                    <button type="button" className="btn btn-link btn-xs hidden" onClick={this.props.copyMove}>
                        <span className="glyphicon glyphicon-duplicate"></span>
                    </button>
                </div>
            </div>
        );
    }
});

var MoveHolder = React.createClass({
    getDefaultProps: function(){return {
        'editAtInit':false
    }},
    getInitialState: function() {
        return {editing:this.props.editAtInit};
    },
    edit: function(){
        this.setState({editing: true});
    },
    render: function() {
        return (
            <li className="list-group-item icons-on-hover" onClick={this.edit}>
                {this.state.editing ?
                    <MoveForm {...this.props} />:<Move {...this.props} />
                }
            </li>
        );
    }
});


var Workout = React.createClass({
    getInitialState: function() {
        return {dropped: false};
    },
    toggle: function(){
        this.setState({'dropped': !this.state.dropped});
    },
    render: function() {
        var drop = null;
        if (this.state.dropped) {
            drop = this.props.workout.moves.map(function (object, i) {
                return <MoveHolder key={i} move={object}
                                   removeMove={this.props.removeMove.bind(this, i)}
                                   moveMove={this.props.moveMove.bind(this, i)}
                                   addMove={this.props.addMove.bind(this, i)}
                                   copyMove={this.props.copyMove.bind(this, i)}
                />
            }, this);
        }
        // <span className="glyphicon glyphicon-menu-hamburger"></span>
        return (
            <ul className="list-group">
                <li className="list-group-item active" onClick={this.toggle}>
                    {this.props.workout.title}
                </li>
                {drop}
                {!this.state.dropped?'':<li className="list-group-item list-group-item-info">
                    <a onClick={this.props.removeWorkout}>remove workout</a> | <a onClick={this.props.addMove}>new step</a>
                </li>}
            </ul>
        );
    }
});

var WorkoutList = React.createClass({
    getInitialState: function() {
        return {workouts:[]};
    },
    componentDidMount: function() {
        $.ajax({
            url: this.props.url,
            dataType: 'json',
            cache: false,
            success: function(data) {
                console.log(data);
                this.setState({workouts: data.workouts});
            }.bind(this),
            error: function(xhr, status, err) {
                console.error(this.props.url, status, err.toString());
            }.bind(this)
        });
    },
    handleSave:function(){
        $.ajax({
            method: "PUT",
            url: '/user/workout/save',
            data: {
                "workouts" : this.state.workouts,
                "id" : token
            },
            processData: true,
            success: function(data, textStatus, jqXHR){
                //document.location = "pebblejs://close#" +encodeURIComponent(JSON.stringify(json)) ;
            },
            error: function(xhr, status, err) {
                console.error(this.props.url, status, err.toString());
            }.bind(this)
        });
    },
    handleCancel:function(){},
    removeWorkout:function(workoutKey){
        var w = this.state.workouts;
        w.splice(workoutKey, 1);
        this.setState({workouts: w});
    },
    addWorkout:function(){
        var w = this.state.workouts;
        w.push({moves:[], title:"New workout", datesCompleted:[]});
        this.setState({workouts: w});
    },
    removeMove:function(workoutKey, moveKey){
        var w = this.state.workouts;
        w[workoutKey].moves.splice(moveKey, 1);
        this.setState({workouts: w});
    },
    moveMove:function(workoutKey, moveKey, newMoveKey){

    },
    addMove:function(workoutKey){
        var w = this.state.workouts;
        w[workoutKey].moves.push({name:"new move", type:"reps", value:"0"});
        this.setState({workouts: w});
    },
    copyMove:function(workoutKey, moveKey){
        var w = this.state.workouts;
        var copy = w[workoutKey].moves[moveKey];
        w[workoutKey].moves.splice(moveKey, 0, copy);
        this.setState({workouts: w});
    },
    render: function() {
        return (
            <div className="workoutList">
                {this.state.workouts.map(function(object, i){
                    return <Workout key={i} workout={object}
                                    removeWorkout={this.removeWorkout.bind(this, i)}
                                    removeMove={this.removeMove.bind(this, i)}
                                    moveMove={this.moveMove.bind(this, i)}
                                    addMove={this.addMove.bind(this, i)}
                                    copyMove={this.copyMove.bind(this, i)}
                    />
                }, this)}

                <button type="button" className="btn btn-default btn-block" onClick={this.addWorkout}>Add Workout</button>

                <div className="row">
                    <div className="col-xs-6">
                        <button type="button" className="btn btn-link btn-block" onClick={this.handleCancel}>Cancel</button>
                    </div>
                    <div className="col-xs-6">
                        <button type="button" className="btn btn-primary btn-block" onClick={this.handleSave}>Save</button>
                    </div>

                </div>
            </div>
        );
    }
});

// Mount
ReactDOM.render(
    <WorkoutList url={url} />,
    document.getElementById('Holder')
);
