# Map generation

The map will be random, with a seed system the can go up to 2^32.
To decide the seed at random, the game will take the elapsed time since the launch in nanoseconds, mod 4294967296.

The map has a default size of 19x19 grid.

## 1 : The base layout

The grid will be first initialized to 0 everywhere.

First, it will place the starting room at [MAP_SIZE-2][MAP_SIZE//2], then another room above it, at [MAP_SIZE-3][MAP_SIZE//2].

DISCLAIMER : when I say "place a room", I will temporary initialize the grid cell to 1, indicating there is a room there.

Then, it will generate two numbers, one in the range [0, MAP_SIZE//4] (x), and one in the range [(3*MAP_SIZE)//4, MAP_SIZE-1] (width) and will make a hallway on the MAP_SIZE-3 row between these two numbers.

Then, it will generate from 3 to 5 hallways depending on the difference between width and x. These rows will have a length between 3 and 4. They will be placed at random between x and width, with 
one restraint : two hallways cannot be adjacent to each other.

