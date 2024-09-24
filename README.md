# R-Type

A game engine that roars!

*For Documentation, check out [CONTRIBUTING.md]()*

## Build

Resolve dependencies:
```bash
git submodule update --init --recursive
```

Build the project:
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Project Description

The R-Type is a project revolving around building a correct and robust architecture for a game engine. The project is divided into several parts, each part focusing on a specific aspect of the engine. The project is divided into the following parts:
- Graphical Module
- Network Module
- Physics Module
- Server Side & Client Side game implementation

This project features a R-Type clone made from scratch in C++ using our game engine.

## Authors
- Jean-Baptiste STOQUER
- Yohan DECAMPS
- Eliot MARTIN
- Ethan THIERRY

## Dependencies

- [SFML](https://www.sfml-dev.org/)

## License
TODO: Add license

## Supported Platforms
- Windows
- Linux


