## Introduction
[Connect Four](https://en.wikipedia.org/wiki/Connect_Four) is a 2 player connection board game, in which the players choose a colour and then take turns dropping coloured discs into a seven-column, six-row vertically suspended grid. The pieces fall straight down, occupying the lowest available space within the column. The objective of the game is to be the first to form a horizontal, vertical, or diagonal line of four of one's own discs.


![text](/assets/md/connectFourDemoClassic.gif)


This projects purpose is to recreate [Connect Four](https://en.wikipedia.org/wiki/Connect_Four) on a digital enviroment and build upon it.

Currently it offers:
- 2+ players per game
- customizable board sizes
- customizable colouring and symbols
- changable win conditions
- game logging
- error logging
- bot opponents
- stat tracking

## Installation and Usage
``` console
# clone the repo
$ git clone https://github.com/Squidfishrl/Connect-four

# change the working directory to connect four
$ cd Connect_four

# run launch script
$ ./run/launchLinux.sh
```

## Requirements

##### - any Linux OS
[Connect Four](https://github.com/Squidfishrl/Connect-four) is developed under Linux. Using it under any other OS would have undefined behaviour and would likely not work.

##### - GCC compiler
Required for automated launch. Game might not work with different compilers / compiler settings.

#### - console using Bash shell
Required for automated launch and error logging.
