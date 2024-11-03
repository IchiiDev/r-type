# R-Type

A game engine that roars!

*For Documentation, check out [CONTRIBUTING.md](CONTRIBUTING.md) and [the repository's wiki](https://github.com/IchiiDev/r-type/wiki)*

## Build

Resolve dependencies:
```bash
git submodule update --init --recursive
```

Build the project:
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
# or
cmake --build build --parallel # For parallel build (faster)
```

## Use

Once the project is compiled or [the release is downloaded](https://github.com/IchiiDev/r-type/releases), you can run the server and/or the client.

### Server

```bash
cd build
./R-Type-server/R-Type-server
```

### Client

```bash
cd build
./R-Type-client/R-Type-client
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
- [Asio](https://github.com/chriskohlhoff/asio)
- [Box2D](https://github.com/erincatto/box2d)
- [Poly2Tri](https://github.com/jhasse/poly2tri)

## License
[MIT License](LICENSE)

## Supported Platforms
- Windows
- Linux


