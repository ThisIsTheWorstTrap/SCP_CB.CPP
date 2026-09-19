# Rooms saving format

## The rooms themselves

All the rooms must be described within the rooms.ini with their size, comonness, model path, and zone they need to spawn in (and also the items that are inside, but i'll implement that later).
This is important, as the game will loop through the rooms in this file to get all the informations to create the map and place the models correctly.

rooms.ini format : 
```
[start]
mesh=models/map/173.m3d
size=20,20,30
event=173guard

[room3_lcz]
mesh=models/map/room3_lcz.m3d
size=10,10,15
event=None
```

Note that the size are not correct there, its just an example.

## Map saving

The map will be saved in map_system/map.ini (temporary) for testing. It will be later included in the save file.
The format is the following : 
```
[rooms]
start=9,15
room3_lcz=9,14
room3_106_lcz=10,14

[start]
event=173guard

[room3_lcz]
event=none

[room3_106_lcz]
event=106sinkhole
```

Ideally, the save will be made in the same order than the rooms are created (from right to left, and from bottom to top).