;
var React = require('react');
var Move = require('./Move');
var MoveForm = require('./MoveForm');
var ItemTypes = require('./Constants').ItemTypes;
var DropTarget = require('react-dnd').DropTarget;
var PropTypes = React.PropTypes;

var squareTarget = {
    drop: function (props, monitor) {
        props.moveMove(monitor.getItem().keyId)
    }
};

function collect(connect, monitor) {
    return {
        connectDropTarget: connect.dropTarget(),
        isOver: monitor.isOver()
    };
}

var MoveHolder = React.createClass({
    propTypes: {
        keyId: PropTypes.number.isRequired,
        isOver: PropTypes.bool.isRequired,
        moveMove: PropTypes.func.isRequired
    },
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
        // var key = this.props.key;
        var connectDropTarget = this.props.connectDropTarget;
        var isOver = this.props.isOver;
        return connectDropTarget(
            <li className="list-group-item icons-on-hover" style={{
                position: 'relative',
                width: '100%',
                height: '100%'
              }}>
                {this.state.editing ?
                    <MoveForm editMove={this.done} move={this.props.move} />:<Move {...this.props} onClick={this.edit} />
                }
                {isOver &&
                <div style={{
                    position: 'absolute',
                    bottom: 0,
                    left: 0,
                    height: '10%',
                    width: '100%',
                    zIndex: 1,
                    opacity: 0.5,
                    backgroundColor: 'yellow',
                  }} />
                }
            </li>
        );
    }
});

module.exports = DropTarget(ItemTypes.KNIGHT, squareTarget, collect)(MoveHolder);
