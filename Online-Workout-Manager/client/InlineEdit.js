var React = require('react');

// could be a controlled input
var InlineEdit = React.createClass({
    getInitialState: function(){
        return {
            editing: this.props.editing,
            text: this.props.text
        };
    },
    propTypes:{
        text: React.PropTypes.string,
        editing: React.PropTypes.bool,
        onChange: React.PropTypes.func
    },

    getDefaultProps: function(){
        return {
            text: "Some Words",
            editing: false,
            onChange: function(){}
        };
    },
    handleEdit: function(e){
        e.stopPropagation();
        this.setState({editing: true});
    },
    handleFinish: function(){
        this.setState({editing: false});
        this.props.onChange(this.state.text);
    },
    _keyAction: function(e){
        if(e.keyCode === 13) {
            // Enter to save
            this.setState({text: e.target.value, editing: false});
            this.props.onChange(e.target.value);
        } else if(e.keyCode === 27) {
            // ESC to cancel
            this.handleFinish();
        }
    },
    render: function() {
        return(
            <span onClick={this.handleEdit}>
                {this.state.editing ? (<input
                    type="text"
                    onKeyDown={this._keyAction}
                    onBlur={this.handleFinish}
                    defaultValue={this.state.text}
                    ref="textField" />): this.state.text}
            </span>

        );
    }
});

module.exports = InlineEdit;
