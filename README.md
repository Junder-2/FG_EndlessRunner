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
