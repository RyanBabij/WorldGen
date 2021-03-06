# WorldGen
Program for creating a random world map. The map can then be imported into another program and used for a game. Each pixel is a tile. For some games you can generate a local map for each tile. Each tile is given a seed for this purpose.

### How does it work?

The core algorithm is the [diamond-square algorithm](https://en.wikipedia.org/wiki/Diamond-square_algorithm), with some small modifications to generate better looking maps. It outputs the world as a PNG and .txt file. There are several command-line options. It is still a work in progress but looks quite nice as it is right now.

The complexity of the map is achieved by layering multiple instances of the diamond-square algorithm.

The program generates the heightmap, then determines the land/sea cutoff point based on the desired amount of land. It then uses a smoothing algorithm to smooth out the coasts. The smoothing makes the coastlines much more attractive. The heightmap is discarded and the map merely represents land or ocean. If a heightmap were to be needed, it should probably be overlaid seperately from the landmass map.

The program then uses similar techniques to place the biomes. To make the maps a little realistic, snow is biased to the top and bottom of the map, desert is biased to the top and bottom third of the map, and jungles are biased to the equator. Realism is secondary to gameplay, however.

Icecaps are added to the north and south of the map, and are intended to prevent the need for y-wrapping, because it is just confusing for gameplay. However x-wrapping is a command-line option.

The program is designed to be reasonably fast, and on my rig it generates a 1025x1025 map in about 3 seconds. In my games, 1 pixel is meant to represent 25km<sup>2</sup>. Therefore this program generates an area of 26,265,625km<sup>2</sup> in roughly 3 seconds. This is enough area to cover Western Europe, North Africa, and Eastern Europe up to the Caspian Sea, which I think is a reasonable amount of space for most grand-strategy games. The program can generate landmasses several times larger than Earth, but it can take a few minutes and generate gigabytes of data.

### Sample Output

One of the PNGs generated by the algorithm. Normally the PNG is uncompressed to make the generator run faster, but I have compressed it here with OptiPNG. Each pixel has a colour which represents a biome. For example the blue is ocean, the dark green is jungle, etc. The colours can represent anything really. The PNG is just for viewing purposes, so you can see if you have created a nice map. The program also outputs a text file which contains the world data.

### Dependencies

This program, like all of my programs, depends on the [Wildcat](https://github.com/RyanBabij/Wildcat) code library. You must include that if you want to compile my program.

### Compile instructions

I compile with:
g++ Driver.cpp -o a -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -I [PATH TO WILDCAT] -std=c++0x -O0

And build with:
g++ Driver.cpp -o a -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -I [PATH TO WILDCAT] -std=c++0x -Os -m32 -s -shared-libstdc++

And if I'm building for myself:
g++ Driver.cpp -o a -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -I [PATH TO WILDCAT] -std=c++0x -Ofast -s -shared-libstdc++ -mtune=native -ffast-math
