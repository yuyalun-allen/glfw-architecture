# Architecture

An example of how I like to architect applications in C++, as part of a [YouTube mini-series](https://youtube.com/playlist?list=PLlrATfBNZ98cpX2LuxLnLyLEmfD2FPpRA).

## Build

Generate project files/build using CMake. I like to make a directory called `build` at the root and then run
```
cmake ..
```
from that directory. This will generate relevant project files (eg. Visual Studio for me) which you can then use to build and run.

## Notes
I chose to use OpenGL (and GLFW) for this due to popular demand [after running a poll](https://www.youtube.com/post/UgkxP9IU8D8UjH8szUipCS3QkJJQOc_cdb0k), however these concepts mostly translate to any other libraries/rendering APIs you may be using. OpenGL and GLFW is simply used as an example, substitute what you like.

## License
This repository uses [The Unlicense](https://unlicense.org/), so feel free to use this however you like.