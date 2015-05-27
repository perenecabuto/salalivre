# coding: utf8

from datetime import datetime

from flask import Flask, render_template, request, jsonify
from flask.ext.pymongo import PyMongo

app = Flask(__name__)
mongo = PyMongo(app)

EVENT_TIME_INTERVAL = 30  # seconds
ALIVE_INTERVAL = 3 * 60 # seconds


@app.route("/")
def index():
    #mongo.db.rooms.remove()
    return render_template("index.html")


@app.route("/rooms", methods=['GET', 'POST'])
def rooms():
    if request.method == 'POST':
        if not request.json or 'name' not in request.json:
            return 'You must send the room name: {"name": ...}', 400

        name = request.json['name']
        room = mongo.db.rooms.find_one({'_id': name})
        if room:
            return "Room %s already exists" % name, 409

        mongo.db.rooms.insert({'_id': name, 'events': [], 'heathchecked_at': None})
        return "", 201

    rooms = mongo.db.rooms.find() or []
    response = []
    for room in rooms:
        empty = False
        alive = False

        if room['heathchecked_at']:
            pass

        response.append({
            'name': room['_id'],
            'empty': empty,
            'alive': alive,
        })

    return jsonify(rooms=list(response))


@app.route("/room/<name>", methods=['DELETE'])
def delete_room(name):
    mongo.db.rooms.remove({'_id': name})
    return "", 200


@app.route("/room/<name>/event", methods=['POST'])
def register_event(name):
    room = mongo.db.rooms.find_one({'_id': name})
    if room is None:
        return "", 404

    events = room.get('events', [])
    events.append(datetime.now())
    room = mongo.db.rooms.find_and_modify({'_id': name}, {'events': events[-10:]})
    return "",  201


@app.route("/heathcheck/<name>")
def heathcheck(name):
    room = mongo.db.rooms.find_one({'_id': name})
    if room is None:
        return "", 404

    room = mongo.db.rooms.find_and_modify({'_id': name}, {'heathchecked_at': datetime.now()})


if __name__ == "__main__":
    app.debug = True
    app.run("0.0.0.0", 5000)

