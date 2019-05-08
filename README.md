# Meowsterious

Meowsterious is an old-school style RPG adventure inspired by the likes of
Dwarf Fortress and Cataclysm Dark Days Ahead.

You take the role of a detective and are tasked with solving a murder by
exploring the environment, interacting with NPCs and scenes and collecting
items.

## Screenshots
![screenshot](https://github.com/VladLazar/Meowsterious/blob/master/screenshots/s1.jpg)
![screenshot](https://github.com/VladLazar/Meowsterious/blob/master/screenshots/s2.jpg)
![screenshot](https://github.com/VladLazar/Meowsterious/blob/master/screenshots/s3.jpg)

## Running the game
You probably will not be able to play the game if you do not own the same
piece of hardware that I've developed the game on. This is not because the
code is not portable (I hope!), but because the graphics library which I have
used was written specifically for this bit of hardware. Tough luck.

On the off chance that you do own a La Fortuna, connect it to your computer
via the USB cable and just run > sudo make. This should flask the game on your
board.

## Technical Acknowledgements
- lcd library: created by Steve Gunn under Creative Commons Attribution License

  It has been modified quite heavily, but much of the original code is still there.

- RIOS: original can be found here <http://www.cs.ucr.edu/~vahid/rios/rios_avr.htm>
	
  Modified by Klaus-Peter Zauner.

- input library: Copyright Peter Dannegger; adapted by Klaus-Peter Zauner.

- OSFS: Copyright Charles Baynham; available at https://github.com/charlesbaynham/OSFS

  Ported to AVR AT90USB1286 and C by me.
