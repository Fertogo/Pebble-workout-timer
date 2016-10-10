;
var React = require('react');
var PropTypes = React.PropTypes;
var ItemTypes = require('./Constants').ItemTypes;
var DragSource = require('react-dnd').DragSource;

var knightSource = {
    beginDrag: function (props) {
        return {keyId: props.keyId};
    }
};

function collect(connect, monitor) {
    return {
        connectDragSource: connect.dragSource(),
        isDragging: monitor.isDragging()
    }
}

var Move = React.createClass({
    propTypes: {
        keyId: PropTypes.number.isRequired,
        connectDragSource: PropTypes.func.isRequired,
        isDragging: PropTypes.bool.isRequired
    },
    render: function() {
        var connectDragSource = this.props.connectDragSource;
        var isDragging = this.props.isDragging;
        return connectDragSource(
            <div style={{
                    opacity: isDragging ? 0.5 : 1
                 }}
                 >
                <span onClick={this.props.onClick}>
                    {this.props.move.type == "reps" ? <span className="glyphicon glyphicon-repeat"></span> : <span className="glyphicon glyphicon-time"></span>}&nbsp;
                    {this.props.move.name}&nbsp;
                    {this.props.move.type == "reps" ? "x" + this.props.move.value : this.props.move.value + "s"}
                </span>
                <div className="pull-right">
                    <button type="button" className="btn btn-link btn-xs" onClick={this.props.onClick}>
                        <span className="glyphicon glyphicon-pencil"></span>
                    </button>
                    <button type="button" className="btn btn-link btn-xs" onClick={this.props.copyMove}>
                        <span className="glyphicon glyphicon-duplicate"></span>
                    </button>
                    <button type="button" className="btn btn-link btn-xs" onClick={this.props.removeMove}>
                        <span className="glyphicon glyphicon-trash"></span>
                    </button>
                </div>
            </div>
        );
    }
});

module.exports = DragSource(ItemTypes.KNIGHT, knightSource, collect)(Move);
