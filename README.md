Pebble-workout-timer
====================
![Banner](http://i.imgur.com/jvBNU81.png)

##Description
Easily create custom interval timers (with names) for your workouts.  This app will time you and alert you when to change moves based on your own workout program. Example: Plank for 2 minutes, 10 biceps curls, pushups for 45 seconds... etc.   You can add create workouts and add moves from your phone or your computer. 

Moves can be time (plank for 1 minute) or rep based (10 pullups). In a time move, you set the length and Pebble will advance to the next move when the countdown timer ends. On a rep move, you can manually advance when you are done with a set/rep. Pebble will advance when you've done all the reps.

After installing, locate this app on the phone's Pebble app, then click on the settings icon. You can add workouts and moves from there. 

###Code
The online workout manager creates a json file with an array of workouts. Format 
```javascript
{
    "workouts": [
        {
            "title": "Workout Title",
            "moves": [
                {
                  "name" : "Move 1", 
                  "value" : 19, //secs
                  "type" : "time"
                },
                {
                  "name" : "Move 2", 
                  "value" : 5, //reps
                  "type" : "reps"
                }
            ]
        },
        {
            "title": "Workout Title",
            "moves": [
                //...
            ]
        }
        //...
    ]
}
```
All of the JSON is edited locally. When the save button is clicked, the server is updated and the JSON gets sent to the Pebble. `app.js` parses this JSON. The moves get saved on `localStorage` using the title as key. An array of all the titles is then sent to `main.c`, where they are displayed. `main.c` communicates with `app.js` to request moves and times.
