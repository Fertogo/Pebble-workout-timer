from os import listdir
from os.path import isfile, join
import json

def migrateWorkouts(path):
    '''
    Migrates all workouts in given path to the new format, as specified on the GitHub Repo at:
    https://github.com/Fertogo/Pebble-workout-timer
    '''

    def pprint(stuff):
        print json.dumps(stuff, sort_keys=True, indent=4, separators=(',',': '))

    allworkouts = [ f for f in listdir(path) if isfile(join(path,f)) ]
    for token in allworkouts:
        fullpath = path + "/" + token
        if (fullpath == "userworkouts/.DS_Store" or fullpath == "userworkouts/."): continue
        print fullpath
        f = open(fullpath, "r")

        workouts = json.loads(f.read())
        if workouts == "": continue
        #pprint(workouts)
        for workout in workouts["workouts"]:
            newMoves =[];
            for move in workout["moves"]:
                if type(move) is not list: continue #ignore already processed files
                newMove = {};
                newMove["type"] = "time"
                newMove["name"] = move[0]
                newMove["value"] = move[1]
                newMoves.append(newMove)
            workout["moves"] = newMoves
        f.close()
        f = open(fullpath, "w")
        f.write(json.dumps(workouts))

migrateWorkouts("userworkouts")
