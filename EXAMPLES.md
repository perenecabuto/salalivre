# Payload

    {
      "rooms": [
        {
          "alive": true,
          "in_use": false,
          "name": "1D"
        },
        {
          "alive": false,
          "in_use": false,
          "name": "1E"
        }
      ]
    }

# Create Room

    curl -X POST http://0.0.0.0:5000/rooms --data '{"name": "1D"}' -H "Content-Type: application/json"
    curl -X POST http://0.0.0.0:5000/rooms --data '{"name": "1E"}' -H "Content-Type: application/json"

# Get Rooms

    curl http://0.0.0.0:5000/rooms

# Check sensor alive

    curl http://0.0.0.0:5000/healthcheck/1D
    curl http://0.0.0.0:5000/rooms

    curl http://0.0.0.0:5000/healthcheck/1E
    curl http://0.0.0.0:5000/rooms

# Register event

    curl -X POST http://0.0.0.0:5000/room/1D/event
    curl -X POST http://0.0.0.0:5000/room/1D/event
    curl http://0.0.0.0:5000/rooms

    curl -X POST http://0.0.0.0:5000/room/1D/event
    curl http://0.0.0.0:5000/rooms

