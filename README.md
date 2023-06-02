# simlcd
Mini USB lcd framebuffer device dashboard ui creation tool

## Features
- Create custom dashboards using config file ( not yet implemented, as it exists it is the hardcoded design pictured here )
- Through the fbgfx library one should be able to rotate a screen clockwise 90 degrees, and support any type of framebuffer device.
- Modular design for support with various titles through simapi library

[![f1wheel1.jpg](https://i.postimg.cc/BnC18WJX/f1wheel1.jpg)](https://postimg.cc/BjXvyVL0)

## Dependencies
- freetype2
- argtable2
- libconfig
- [slog](https://github.com/kala13x/slog) (static)
- [wine-linux-shm-adapter](https://github.com/spacefreak18/simshmbridge) - for sims that need shared memory mapping like AC.
- [simapi](https://github.com/spacefreak18/simapi)
- [fbgfx](https://github.com/spacefreak18/fbgfx)

## Building
This code depends on the shared memory data headers in the simapi [repo](https://github.com/spacefreak18/simapi). When pulling lastest if the submodule does not download run:
```
git submodule sync --recursive
git submodule update --init --recursive
```
Then to compile simply:
```
mkdir build; cd build
cmake ..
make
```

## Run

```
FBDEV=/dev/fb1 FBROTATE=1 ./simlcd
```

## ToDo
 - much, much more
