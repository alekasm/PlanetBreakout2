# Planet Breakout 2

Thank you for playing Planet Breakout 2, and open-source breakout clone game written in C++ for Windows using Direct2D and XAudio2. You may use this source code in other open-source projects, or free projects, but please attribute credit to Aleksander Krimsky. You may not use this project for any commercial use, including ad-monetized software. 


## Modding 
All directories specified below are relative to the game directory. If you installed this game via Steam, then you can find the game directory under `C:\Program Files (x86)\Steam\steamapps\common\`.

### Creating a Mission
There are various mods you can make to the game which are super simple. As you know the game comes with a built-in level editor. If you would like to create a **mission** from these levels, make a new folder under the `assets/levels` directory and name it what you want the new *mission* name to be. For instance if you want a *Mars* mission, create a `assets/levels/Mars` directory then copy your levels there. Levels are sorted by name when you play them in the game.  

### Adding Bricks, Bats, and Balls
All new brick sprites should be placed under `assets/sprites/bricks`, be exactly `48x16px`, and be a `.png` type of image - transparency is allowed. All other sprites including bats and balls go under `assets/sprites`. Balls are `16x16px` and bats are`64x16px`, again only `.png` and transparency is allowed. You can instantly start using the new bricks in the level editor, however to use new bats and balls see **Customizing a Mission**.

### Customizing a Mission
After you start the mission at least once in the game, you will notice a new *mission.cfg* file which is created. This file is used to store highscores along with various values specific to that mission. You may override the name of the mission, or change the sprite of the bat or ball. By default, `bat:bat` and `ball:ball`. If you want to load your own custom sprite for these, place a `.png` file under `assets/sprites` then specify the sprite in the *mission.cfg* **without** the `.png` suffix. For instance, you place `ball3.png` under `asset/sprites` and in your mission config you set `ball:ball3`.

