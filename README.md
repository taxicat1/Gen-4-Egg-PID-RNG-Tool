## Intro

A tool to RNG manipulate for specific PIDs when breeding in the generation 4 Pokemon games. Useful for gimmick PIDs that spell something out in hexadecimal, ASCII, or when converted to base 36.

## Usage

In Diamond, Pearl, and Platinum, egg PID RNG is well understood and manipulatable using coin flips and double taps on the Happiness Checker app. The program will output the target initial seed along with numbers of coin flips and double taps to perform. After that, the next generated egg will have the target PID.

In HeartGold and SoulSilver, a technique using Voltorb Flip is used instead. By starting a Voltorb Flip game and immediately quitting, the game will have used RNG calls to create the game board. After doing this some number of times, a rematchable trainer (who does not currently want a rematch) is repeatedly called using the PokeGear to advance the RNG more precisely (usually Youngster Joey, [but see here for a full list](https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9gear#Trainers)). If the RNG needs to be advanced one more time, you will have to reject the first egg and pick up the second one. The egg will then have the target PID.

## Compiling

```
gcc main.c dppt_searcher.c hgss_searcher.c input_tool.c mt_tool.c pid_tool.c voltorb_flip_tool.c -o Gen-4-Breeding-Specific-PID-RNG-Tool.exe -Ofast
```
