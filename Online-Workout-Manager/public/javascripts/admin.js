console.log(stats)

/**
* Creates a line graph to render given stat
* limit, how many dates to render.
*/
function renderStat(stats, limit){
    var dates = [],
        values = [];

    if (limit > stats.data.length) limit = stats.data.length

    var i, len, stat, date;
    len=stats.data.length;
    for (i=len-limit ; i < len; i++) {
        stat = stats.data[i];
        console.log(i)
        dates.push( new Date(stat.date).toDateString());
        values.push(stat.value)
    }

    var ctx = document.getElementById(stats.name).getContext("2d");

    var data = {
        labels: dates,
        datasets: [
            {
                label: stats.name,
                fillColor: "rgba(220,220,220,0.2)",
                strokeColor: "rgba(220,220,220,1)",
                pointColor: "rgba(220,220,220,1)",
                pointStrokeColor: "#fff",
                pointHighlightFill: "#fff",
                pointHighlightStroke: "rgba(220,220,220,1)",
                data: values
            }
        ]
    };

    var myLineChart = new Chart(ctx).Line(data);


}

var totalUsers = stats[0];

stats.map(function(stat){
    renderStat(stat, 500)
})

// renderStat(totalUsers, totalUsers.data.length-1)
// var dates = [],
//     totalWorkouts = [];

// var i, len, stat, date;
// for (i=0, len=totalWorkoutStats.data.length; i < len; i++) {
//     stat = totalWorkoutStats.data[i];
//     console.log(i)
//     dates.push( new Date(stat.date).toDateString());
//     totalWorkouts.push(stat.value)
// }

// console.log(dates)
// console.log(totalWorkouts)
// var ctx = document.getElementById("myChart").getContext("2d");



// var myLineChart = new Chart(ctx).Line(data);

