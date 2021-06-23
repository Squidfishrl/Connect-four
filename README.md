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

With features to come:
- bot opponents
- stat tracking
- error logging

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


## How we made it
[Connect Four](https://github.com/Squidfishrl/Connect-four) was developed as a [TUES](https://www.elsys-bg.org/) school project for 10th grade.

Work was split between 2 people:
- [Ivaylo Genchev / Squidfishrl](https://github.com/Squidfishrl) - Created game board, implemented settings and made the main menu
- [Daniel Paunov / McAwesome123](https://github.com/McAwesome123) - Created game logic
