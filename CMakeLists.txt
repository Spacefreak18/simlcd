
# minimum CMake version required for C++20 support, among other things
cmake_minimum_required(VERSION 3.15)

# detect if simlcd is being used as a sub-project of another CMake project
if(NOT DEFINED PROJECT_NAME)
    set(simlcd_SUBPROJECT OFF)
else()
    set(simlcd_SUBPROJECT ON)
endif()


SET_SOURCE_FILES_PROPERTIES( src/simlcd.c PROPERTIES LANGUAGE C)
#set(CMAKE_BUILD_TYPE Debug)


project(simlcd)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed -ldl")
set(FREETYPE_INCLUDE_DIR /usr/include/freetype2)


add_subdirectory(src/simlcd/gameloop)
add_subdirectory(src/simlcd/simulatorapi)
add_subdirectory(src/simlcd/fbgfx)
add_subdirectory(src/simlcd/helper)
add_subdirectory(src/simlcd/slog)

add_executable(simlcd src/simlcd/simlcd.c)
target_include_directories(simlcd PUBLIC config ${FREETYPE_INCLUDE_DIR})
target_link_libraries(simlcd xdg-basedir m argtable2 config gameloop helper slog simulatorapi fbgfx freetype)


# used for enabling additional compiler options if supported
include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    message(STATUS "[simlcd] Checking if compiler supports warning flag '${flag}'")
    check_cxx_compiler_flag("${flag}" flag_supported)
    if(flag_supported)
        message(STATUS "[simlcd] Enabling warning flag '${flag}'")
        target_compile_options(simlcd INTERFACE "${flag}")
    endif()
    unset(flag_supported CACHE)
endfunction()

# enable a large amount of extra warnings, regardless of build mode
if (MSVC) # MSVC supports different warning options to GCC/Clang
    enable_cxx_compiler_flag_if_supported("/W3")  # set warning level 3
    # if tests are enabled, enable converting all warnings to errors too
    if (ENABLE_TESTS)
        # add_compile_options(/WX)
        enable_cxx_compiler_flag_if_supported("/WX")
    endif()
else() # GCC/Clang warning option
    # NOTE: GCC and Clang support most of the same options, but neither supports all
    # of the others'. By only enabling them if supported, we get graceful failure
    # when trying to enable unsupported flags
    # e.g. at the time of writing, GCC does not support -Wdocumentation
    #
    # enable all warnings about 'questionable constructs'

    if(analyze)
        message("-- Analyzer is on")
        target_compile_options(simlcd PRIVATE -fanalyzer -Wno-analyzer-possible-null-argument -Wno-analyzer-possible-null-dereference)
    endif()

    enable_cxx_compiler_flag_if_supported("-Wall")
    enable_cxx_compiler_flag_if_supported("-W")
    # issue 'pedantic' warnings for strict ISO compliance
    enable_cxx_compiler_flag_if_supported("-pedantic")
    # enable 'extra' strict warnings
    enable_cxx_compiler_flag_if_supported("-Wextra")
    # enable sign conversion warnings
    enable_cxx_compiler_flag_if_supported("-Wsign-conversion")
    # enable warnings about mistakes in Doxygen documentation
    enable_cxx_compiler_flag_if_supported("-Wdocumentation")
    # if tests are enabled, enable converting all warnings to errors too
    if (ENABLE_TESTS)
        enable_cxx_compiler_flag_if_supported("-Werror")
        # exclude the following kinds of warnings from being converted into errors
        # unknown-pragma is useful to have as a warning but not as an error, if you have
        # pragmas which are for the consumption of one compiler only
        enable_cxx_compiler_flag_if_supported("-Wno-error=unknown-pragmas")
        # unused variable and function warnings are helpful but we don't need them as errors
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-function")
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-variable")
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-parameter")
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-private-field")
        enable_cxx_compiler_flag_if_supported("-Wno-error=unused-but-set-variable")
    endif()
endif()

# library
# unit tests --only enable if requested AND we're not building as a sub-project
if(ENABLE_TESTS AND NOT simlcd_SUBPROJECT)
    message(STATUS "[simlcd] Unit Tests Enabled")
    add_subdirectory(tests)
    enable_testing()
endif()
