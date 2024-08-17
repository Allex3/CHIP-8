#sources specifies which files to compile as part of the project
sources = source/main.c source/chip8.c source/peripherals.c
#headers specifies which headers we include in our sources files
headers = include/peripherals.h include/chip8.h
#we put headers here so we don't include with ./include/... in the .c files

#objects are files for where we compile the .c files
objects = build/main.o build/chip8.o build/peripherals.o

#CFLAGS has a flag for including the include folder that contains our headers
CFLAGS = -Iinclude -Wall -Wextra

#CC specifies which compiler we're using
CC = g++

#LIBS specifies the libraries we're linking against
LIBS = -lSDL2

#OBJ_NAME specifies the name of our exectuable
outfile = emulator

#This is the target that compiles our executable
all: bin/emulator.out

# NOTE FOR SELF: $@ is the name of the target being generated,
# like example.o in example.o: ... , the word before the colon :
#and $< is the first prerequisite, usually a source file, 
# prerequisites are what's after the colon on the first line
#and the other lines are executed

bin/emulator.out: $(objects) $(headers)
		@mkdir -p bin
		$(CC) $(CFLAGS) -o $@ $(objects) $(LIBS)

#to make the object files, the prerequisites for building the project
#this next target builds all the object files from all the source files
#the %.c gets the file with the shortest occurence of .c from the end of its name
#here would be any file, since they all end in .c
build/%.o: source/%.c
		@mkdir -p build
		$(CC) -c $< $(CFLAGS) -o $@ $(LIBS)

#this removes the current build
clean:
		rm -rf bin build
