# FG_EndlessRunner
 
Unreal Engine 5 Endless Runner with 3 lanes and randomly distibuted obstacles.

## Part 1
Uses 4 main classes:
1. RunnerCharacter.h
   - Handles logic for moving between lanes.
   - Handles input, move inputs are buffered (uses TQueue) so double presses moves you twice.
   - Takes damage and broadcasts OnDamage and OnDeath to subscribed classes.
 
2. LevelManager.h
   - Stores game variables for number of lanes, speed increases, points increase etc.
   - Handles logic for gamespeed and obstacle difficulty.
   - Handles spawning of FloorTiles, spawned floortiles are stored in a TDeque (double sided queue) to allow queue logic and give access to the tail and head.
   - Despawns oldest FloorTile when far enough behind player.
   - Handles saving highscore to disk.
   
3. FloorTile.h
   - Moves according to the LevelManager's gamespeed every tick.
   - Generates obstacles randomly with rules to prevent unfair placements.
   - Obstacle amount is decided by LevelManager's obstacle difficulty and random offset.

4. StaticObstacle.h
   - Handles collision detection with player and gives damage on overlap.

Effective uses of c++:
- All uses of pointers are wrapped with TObjectPtr for safer memory usage.
- Casting of classes makes usage of dynamic casts and are null checked to prevent bad memory access.
- Runtime values are marked Transient to ensure they are zeroed at start.

## Part 2
Adds Multiplayer:
- A bool inside the new game instance class determines if the game should spawn multiple players.
- Reworked the input management to where a master player controller sends input events to the two players. This is a workaround to enable multiple players to both use the keyboard.
- Players are cooperating meaning that both are on the same playfield and if either of them die the game ends.

Added Reward for Successful Dodges:
- When the player dodges right before they are hit an upcoming obstacle will be destroyed.
- Dodge detection is done by having a collider follow the player with its previous position. The time delay is developer specified and gives a good enough approximation for detecting successful dodges.
- Destroying of obstacles is done with a method on the GameState class that picks a random floortile that calls it's method to pick a random obstacle inside of it that then destroys itself. The random selection of the floortile is limited to be an index between 1/3 and 2/3 of the FloorTiles TDeque container's size.

Additional Changes:
- All header files uses forward declararations instead of includes when possible to reduce compile time.
- All LevelManager logic has been moved to a GameState class to make better usage of Unreal Engine's pipeline. Gives ability to get the class from other classes, removes the need to have an actor inside the level etc.
- Removed previous camera class and created a new invisible pawn that has the game camera instead.
- Save managing has been moved to an unreal interface to allow other gamemodes to load and save data without having to copy code.
