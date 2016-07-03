;
var React = require('react');

var Move = React.createClass({
    render: function() {
        return (
            <div onClick={this.props.onClick}>
                {this.props.move.type == "reps" ? <span className="glyphicon glyphicon-repeat"></span> : <span className="glyphicon glyphicon-time"></span>}&nbsp;
                {this.props.move.name}&nbsp;
                {this.props.move.type == "reps" ? "x" + this.props.move.value : this.props.move.value + "s"}

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

module.exports = Move;
