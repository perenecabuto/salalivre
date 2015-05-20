# coding: utf8

from datetime import datetime

from flask import Flask, render_template, request, jsonify
from flask.ext.pymongo import PyMongo
from bson.json_util import dumps

app = Flask(__name__)
mongo = PyMongo(app)


@app.route("/")
def index():
    #mongo.db.rooms.remove()
    return render_template("index.html")


@app.route("/rooms", methods=['GET', 'POST'])
def rooms():
    if request.method == 'POST':
        if not request.json or '_id' not in request.json:
            return 'You must send the room id: {"_id": ...}', 400

        _id = request.json['_id']
        room = mongo.db.rooms.find_one({'_id': _id})
        if room:
            return "Room %s already exists" % _id, 409

        mongo.db.rooms.insert({'_id': _id})
        return "", 201

    rooms = mongo.db.rooms.find() or []
    return jsonify(rooms=list(rooms))


@app.route("/room/<_id>", methods=['GET', 'DELETE'])
def room(_id):
    if request.method == 'GET':
        room = mongo.db.rooms.find_one_or_404({'_id': _id})
        return dumps(room)
    elif request.method == 'DELETE':
        return mongo.db.rooms.remove({'_id': _id})


@app.route("/room/<_id>/event", methods=['POST'])
def register_event(_id):
    room = mongo.db.rooms.find_and_modify({'_id': _id}, {'$push': {'events': datetime.now().isoformat()}})
    return jsonify(room=room), 404 if room is None else 201


if __name__ == "__main__":
    app.debug = True
    app.run("0.0.0.0", 5000)

