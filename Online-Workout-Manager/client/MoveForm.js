;
var React = require('react');

var MoveForm = React.createClass({
    handleChange: function(nameOfChange, event) {
        var data = {};
        data[nameOfChange] = event.target.value;
        this.setState(data);
    },
    getInitialState: function(){
        return {
            name: this.props.move.name,
            type: this.props.move.type,
            value: this.props.move.value
        };
    },
    handleSave: function(){
        this.props.editMove({
            name:this.state.name,
            type:this.state.type,
            value:this.state.value
        });
    },
    setType: function(type){
        this.setState({type: type});
    },
    render: function() {
        return (
            <div className="form-inline">
                <div className="form-group">
                    <div className="btn-group" role="group">
                        <button type="button"
                                className={"btn " + (this.state.type == "reps" ? "btn-info" : "btn-default")}
                                onClick={this.setType.bind(this, "reps")}>
                            <span className="glyphicon glyphicon-repeat"></span> Reps
                        </button>
                        <button type="button"
                                className={"btn " + (this.state.type == "time" ? "btn-info" : "btn-default")}
                                onClick={this.setType.bind(this, "time")}>
                            <span className="glyphicon glyphicon-time"></span> Time
                        </button>
                    </div>
                </div>
                <div className="form-group">
                    <input type="text" className="form-control" placeholder="What" onChange={this.handleChange.bind(this, 'name')} value={this.state.name}/>
                </div>
                <div className="form-group">
                    <input type="number" className="form-control" placeholder="Value" onChange={this.handleChange.bind(this, 'value')} value={this.state.value}/>
                </div>
                <button type="button" className="btn btn-success" onClick={this.handleSave}>Done</button>
            </div>
        );
    }
});

module.exports = MoveForm;