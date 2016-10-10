;
var React = require('react');
var ReactDOM = require('react-dom');
var WorkoutList = require('./WorkoutList');

//Parse URL
var parser = document.createElement('a');
parser.href = document.URL;
var urlinfo = parser.search.slice(6).split(','); //Split url into array of info. Current Format [token,version]
var token = urlinfo[0]; //Unique to Pebble account and this app!
var version = parseFloat(urlinfo[1]) ;
var url = '/user/' + token + '/' + version;

// Mount
ReactDOM.render(
    <WorkoutList url={url} token={token} />,
    document.getElementById('Holder')
);
