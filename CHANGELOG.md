Pebble-workout-timer
===================
###Changelog
* **v4.0**
   * Rewite Pebble Code
     * Redesigned Message Protocol
       * No phone required after workout start
     * Much faster and battery friendly
   * Miscellaneous bug fixes
* **v3.5**
   * Online Workout Manager
     * Completely remade backend in node.js
     * Faster + more secure
   * Timeline pins
   * Workout stats
   * Admin panel
* **v3.2**
  * Online Workout Manager
    * Reorder moves by clicking and dragging
    * Create longer moves
  * UI Changes
    * Show timer in minutes and seconds
  * Bug Fixes
    * Font to fit bug for a more consistant layout

* **v3.1**
  * Pebble Time Compatibility 
    * App now works with Pebble Time and makes use of color screen for various windows. 
  * Updated Layout
    * Move Name and Time larger and centered
    * Move Name font and size changes to accommodate larger moves
  * Bug Fixes
    * Fix bug where text would get cut off by status bar
    * Fix bug that would cause workouts to get stuck on loading after second move. 
    * Miscellaneous bug fixes
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
  * ~~Delete workouts from Pebble~~ (removed in v2.0)
  * Start timers from Pebble

