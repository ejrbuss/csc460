from os.path import join

# This is a standard configuration file for mekpie

# the name of your project
name = 'project-1' 
# the .c file containing `main`
main = 'project-1.cpp'
# the c compiler configuration to use (gcc_clang, avr_gcc, or emscripten)
cc = avr_gcc(hardware='atmega2560', programmer='wiring', baud='115200')
# any libraries to load
libs = []
# additional compiler flags
flags = [
    # define clock rate
    '-DF_CPU=16000000L',
    # Need optimizations for delay to work 
    '-Os',
    # Reduce application size and optimize
    '-flto',
    '-ffunction-sections',
    '-fdata-sections',
    '-fno-fat-lto-objects',
    '-fuse-linker-plugin',
]

if options.release:
    flags += ['-Wall', '-O']
else:
    flags += ['-Wall', '-g']
