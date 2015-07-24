import requests
import json
from os import listdir
from os.path import isfile, join

def importUsers(path):
    allworkouts = [ f for f in listdir(path) if isfile(join(path,f)) ]

    for token in allworkouts:
        user = token.split(".")[0]
        print user
        fullpath = path + "/" + token
        if (fullpath == path+"/.DS_Store" or fullpath == path + "/."): continue
        # print fullpath
        f = open(fullpath, "r")

        workouts = json.loads(f.read())
        if workouts == "": continue
        # print(workouts)
        # workouts = [{"name":"A","type":"reps","value":34},{"name":"B","type":"time","value":44}]
        # data = {
        #     "user" : user,
        #     "workouts" : workouts
        # }
        data = workouts
        data['user'] = user
        # data = {"user": "asdFadsf", "workouts": workouts}
        sendRequest(json.dumps(data))

def sendRequest(body_content):

    # print body_content
    headers = {'Content-type': 'application/json', 'Accept': 'text/plain'}
    r = requests.put("http://pebble-workout-timer.herokuapp.com/admin/import/user", data=body_content, headers=headers)

    if (r.status_code == 200):
        print "success"
    else :
        print("Error^")

importUsers('userworkouts')
