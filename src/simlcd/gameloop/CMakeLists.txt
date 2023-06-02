set(gameloop_source_files
    gameloop.c
    gameloop.h
)


set(FREETYPE_INCLUDE_DIR /usr/include/freetype2)
include_directories("." ${FREETYPE_INCLUDE_DIR})

add_library(gameloop STATIC ${gameloop_source_files})



target_compile_definitions(gameloop PRIVATE SIMMAP_ALL)
