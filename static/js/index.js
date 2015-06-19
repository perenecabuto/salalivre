$(document).ready(function() {
    var $container = $('.rooms-container');

    function getRooms() {
        $.getJSON('/rooms', function(data) {
            var rooms = data.rooms || [];
            for (var i = 0; i < rooms.length; i++) {
                var room = rooms[i];
                var roomId = room.name;
                var $roomEl = $('#' + roomId);

                if ($roomEl.get(0) === undefined) {
                    var template = $('#template').text();
                    $roomEl = $(template);
                    $roomEl.attr('id', roomId);
                    $roomEl.find('.title-text').text(room.name);
                    $container.append($roomEl);
                }

                var status = !room.alive ? 'offline' : (room.in_use ? 'in-use' : 'empty');
                $roomEl.find('.box-container').removeClass('offline').removeClass('in-use').removeClass('empty');
                $roomEl.find('.box-container').addClass(status);
            }
        });
    }

    function autoUpdateRooms() {
        getRooms();
        setTimeout(autoUpdateRooms, 1000);
    };

    autoUpdateRooms();
});
