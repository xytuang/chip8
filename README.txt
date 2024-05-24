Resources/Documentation for Chip8
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

Instructions to build Chip8 and play pong:

1. Clone the repo using this command: git clone git@github.com:xytuang/chip8.git

2. In the terminal, enter the project folder. Make a new directory called
"build", by using this command: mkdir build

3. Enter this new build folder using the command: cd build

4. Once in the folder, type the command: cmake ..

5. Followed by the command: cmake --build .

***DO NOT FORGET THE "." at the end of the command! It is part of the command!

6. You should get a chip8 executable on success.

7. Enter the following command into the command line: ./chip8 ../roms/PONG

***The above example is for playing pong. Other games in the roms folder include
tetris and space invaders. 

Enter "./chip8 ../roms/TETRIS" for tetris and " ./chip8 ../roms/INVADERS" for
space invaders.

