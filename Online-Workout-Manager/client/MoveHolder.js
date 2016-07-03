;
var React = require('react');
var Move = require('./Move');
var MoveForm = require('./MoveForm');

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
    done: function(data){
        this.setState({editing: false});
        this.props.editMove(data);
    },

    render: function() {
        return (
            <li className="list-group-item icons-on-hover">
                {this.state.editing ?
                    <MoveForm editMove={this.done} move={this.props.move} />:<Move {...this.props} onClick={this.edit} />
                }
            </li>
        );
    }
});

module.exports = MoveHolder;
