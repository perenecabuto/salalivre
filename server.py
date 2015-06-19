# coding: utf-8

from datetime import datetime
import os

from gevent.wsgi import WSGIServer
from gevent import monkey
monkey.patch_all()

from flask import Flask, render_template, request, jsonify
from flask.ext.pymongo import PyMongo

app = Flask(__name__)
app.config["MONGO_URI"] = os.environ.get('DBAAS_MONGODB_ENDPOINT', 'mongodb://localhost:27017/salalivre')
mongo = PyMongo(app)

MAX_EVENTS_PER_ROOM = 10
NUM_OF_LAST_EVENTS = 3
MAX_EVENTS_INTERVAL = 1 * 60  # seconds
ALIVE_INTERVAL = 3 * 60  # seconds

@app.route("/")
def index():
    return render_template("index.html")


@app.route("/rooms", methods=['GET'])
def rooms():
    response = []
    rooms = mongo.db.rooms.find(sort=[('position', 1)]) or []
    for room in rooms:
        alive = False
        if room.get('healthchecked_at') is not None:
            alive = (datetime.now() - room['healthchecked_at'].replace(tzinfo=None)).seconds < ALIVE_INTERVAL

        in_use = False
        events = room.get('events', [])
        if len(events) >= NUM_OF_LAST_EVENTS:
            last_events = [e.replace(tzinfo=None) for e in events[-NUM_OF_LAST_EVENTS:]] + [datetime.now()]
            intervals = (last_events[i] - last_events[i-1] for i in xrange(len(last_events) - 1, 0, -1))
            in_use = all(i.seconds <= MAX_EVENTS_INTERVAL for i in intervals)

        response.append({
            'name': room['_id'],
            'in_use': in_use,
            'alive': alive,
            'position': room['position']
        })

    return jsonify(rooms=list(response))


@app.route("/room", methods=['POST'])
def create_room():
    if not request.json or 'name' not in request.json:
        return 'You must send the room name: {"name": ...}', 400

    name = request.json['name']

    room = mongo.db.rooms.find_one({'_id': name})
    if room:
        return "Room %s already exists" % name, 409

    if 'position' in request.json:
        position = request.json.get('position')
    else:
        last_room = mongo.db.rooms.find_one(sort=[('position', -1)])
        position = last_room['position'] if last_room else 0

    position = position + 1

    mongo.db.rooms.insert({'_id': name, 'events': [], 'position': position, 'healthchecked_at': None})
    return "", 201


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
    room = mongo.db.rooms.find_and_modify({'_id': name}, {
        'events': events[-MAX_EVENTS_PER_ROOM:],
        'healthchecked_at': datetime.now(),
    })
    return "", 201


@app.route("/healthcheck/<name>")
def healthcheck(name):
    room = mongo.db.rooms.find_one({'_id': name})
    if room is None:
        return "", 404

    room = mongo.db.rooms.find_and_modify({'_id': name}, {'healthchecked_at': datetime.now()})
    return "", 201


if __name__ == "__main__":
    port = int(os.environ.get('PORT', 5000))
    print "Starting server at port %s" % port
    app.debug = True
    WSGIServer(('0.0.0.0', port), app).serve_forever()

