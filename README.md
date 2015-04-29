Pebble-workout-timer
====================
![Banner](http://i.imgur.com/jvBNU81.png)

##Description
Easily create custom interval timers (with names) for your workouts. 
This app will time you and alert you when to change moves based on your own workout program. Example: Plank for 2 minutes, pushups for 45 seconds... etc. 

After installing, locate this app on the phone's Pebble app, then click on the settings icon. You can add workouts and moves from there. 


###Changelog
* **v3.0**
##### This update changes the way workouts are stored. See README and `migrate-workouts.py`

  * New Move type: Repetitions
    * Can now set a move with reps, click next to advance one rep
    * Long press next to go to next move
  * Updated Online Workout Manager
    * Allows for easy adding of rep moves
  * Migrated all workouts to new format
    * This is required and makes the update to 3.0 mandatory. 
  * Code clean-up and bug fixes
* **v2.7**
  * Fix Memory Issues
    * App will not crash on longer workouts
  * Fix Wakeup bug
    * Wakeups only scheduled when exiting app
    * Fixes repeating move bug
  * Phone not connected notification
* **v2.6**
  * Move preloading
    * Next move preloads during current move -
    * Significant performance improvement
  * Extended move names
    * Long moves can now take up two lines
  * Miscellaneous bug fixes and improvements
    * Backlight turns on when move changes
    * Fixed pause and stop bug
* **v2.5**
  * Background timers
     * Timers continue to run even if the app is closed
  * New controls during timer. 
     * Stop, play/pause and next
  * Improved UI
     * Added ActionBar for new controls
     * Added loading window
  * Performance Improvements
  * Bug Fixes
* **v2.0**
  * Online Workout Manager
    * Delete existing workouts/moves
    * Add moves to existing workouts
    * Edit excisitng moves (name + time)
    * Delete workouts
    * Display total workout time
    * Manage workouts from computer 
* **v1.0**
  * Initial Release - Custom workout interval timer
  * Add workouts from phone
  * Delete workouts from Pebble (removed in v2.0)
  * Start timers from Pebble

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
                  "value" : 5, //secs
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
