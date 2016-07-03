;
var React = require('react');
var Workout = require('./Workout');
var $ = require('jquery');
var DragDropContext = require('react-dnd').DragDropContext;
var HTML5Backend = require('react-dnd-html5-backend');

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
                this.setState({workouts: data.workouts});
            }.bind(this),
            error: function(xhr, status, err) {
                console.error(this.props.url, status, err.toString());
            }.bind(this)
        });
    },
    handleSave:function(){
        var w = {workouts: this.state.workouts};
        $.ajax({
            method: "PUT",
            url: '/user/workout/save',
            data: {
                "workouts" : this.state.workouts,
                "id" : this.props.token
            },
            processData: true,
            success: function(data, textStatus, jqXHR){
                document.location = "pebblejs://close#" +encodeURIComponent(JSON.stringify(w));
            },
            error: function(xhr, status, err) {
                console.error(this.props.url, status, err.toString());
            }.bind(this)
        });
    },
    handleCancel:function(){
        document.location = "pebblejs://close#";
    },
    removeWorkout:function(workoutKey){
        var w = this.state.workouts;
        w.splice(workoutKey, 1);
        this.setState({workouts: w});
    },
    renameWorkout:function(workoutKey, newName){
        var w = this.state.workouts;
        w[workoutKey].title = newName;
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
    editMove:function(workoutKey, moveKey, newData){
        var w = this.state.workouts;
        w[workoutKey].moves[moveKey] = newData;
        this.setState({workouts: w});
    },
    addMove:function(workoutKey){
        var w = this.state.workouts;
        w[workoutKey].moves.push({name:"new move", type:"reps", value:"0"});
        this.setState({workouts: w});
    },
    moveMove: function(workoutKey, targetMoveKey, moveKey){
        var w = this.state.workouts;
        var dragged = w[workoutKey].moves.splice(moveKey, 1);
        w[workoutKey].moves.splice(targetMoveKey, 0, dragged[0]);
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
                                    addMove={this.addMove.bind(this, i)}
                                    copyMove={this.copyMove.bind(this, i)}
                                    renameWorkout={this.renameWorkout.bind(this, i)}
                                    editMove={this.editMove.bind(this, i)}
                                    moveMove={this.moveMove.bind(this, i)}
                    />
                }, this)}

                <button type="button" className="btn btn-default btn-block" onClick={this.addWorkout}>Add Workout</button>

                <div className="row" style={{marginTop: "10px"}}>
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

module.exports = DragDropContext(HTML5Backend)(WorkoutList);
