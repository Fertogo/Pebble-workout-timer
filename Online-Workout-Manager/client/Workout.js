;
var React = require('react');
var MoveHolder = require('./MoveHolder');
var InlineEdit = require('./InlineEdit');

var Workout = React.createClass({
    getInitialState: function() {
        return {editing: false};
    },
    edit: function(){
        this.setState({'editing': !this.state.editing});
    },
    render: function() {
        var drop = null;
        if (this.state.editing) {
            return (
                <ul className="list-group">
                    <li className="list-group-item list-group-item-info" onClick={this.edit}>
                        <InlineEdit
                            text={this.props.workout.title}
                            onChange={this.props.renameWorkout} />
                        <div className="pull-right">
                            <button type="button" className="btn btn-link btn-xs" onClick={this.props.removeWorkout}>
                                <span className="glyphicon glyphicon-trash"></span>
                            </button>
                        </div>
                    </li>

                    {this.props.workout.moves.map(function (object, i) {
                        return <MoveHolder key={i} move={object} keyId={i}
                                           removeMove={this.props.removeMove.bind(this, i)}
                                           addMove={this.props.addMove.bind(this, i)}
                                           copyMove={this.props.copyMove.bind(this, i)}
                                           editMove={this.props.editMove.bind(this, i)}
                                           moveMove={this.props.moveMove.bind(this, i)}
                        />
                    }, this)}

                    <li className="list-group-item list-group-item-info">
                        <a onClick={this.props.addMove}><span className="glyphicon glyphicon-plus"></span> Add move</a>
                    </li>
                </ul>
            );


        } else {
            return (
                <ul className="list-group">
                    <li className="list-group-item list-group-item-info" onClick={this.edit}>
                        <InlineEdit
                            text={this.props.workout.title}
                            onChange={this.props.renameWorkout} />
                    </li>
                </ul>
            );
        }
    }
});

module.exports = Workout;
