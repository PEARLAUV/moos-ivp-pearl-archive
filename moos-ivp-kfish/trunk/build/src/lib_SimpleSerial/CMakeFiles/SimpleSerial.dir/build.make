# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/erolland/moos-ivp-kfish/trunk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/erolland/moos-ivp-kfish/trunk/build

# Include any dependencies generated for this target.
include src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/depend.make

# Include the progress variables for this target.
include src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/progress.make

# Include the compile flags for this target's objects.
include src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/flags.make

src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/SerialComms.cpp.o: src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/flags.make
src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/SerialComms.cpp.o: ../src/lib_SimpleSerial/SerialComms.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/erolland/moos-ivp-kfish/trunk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/SerialComms.cpp.o"
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_SimpleSerial && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SimpleSerial.dir/SerialComms.cpp.o -c /home/erolland/moos-ivp-kfish/trunk/src/lib_SimpleSerial/SerialComms.cpp

src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/SerialComms.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleSerial.dir/SerialComms.cpp.i"
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_SimpleSerial && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/erolland/moos-ivp-kfish/trunk/src/lib_SimpleSerial/SerialComms.cpp > CMakeFiles/SimpleSerial.dir/SerialComms.cpp.i

src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/SerialComms.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleSerial.dir/SerialComms.cpp.s"
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_SimpleSerial && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/erolland/moos-ivp-kfish/trunk/src/lib_SimpleSerial/SerialComms.cpp -o CMakeFiles/SimpleSerial.dir/SerialComms.cpp.s

# Object files for target SimpleSerial
SimpleSerial_OBJECTS = \
"CMakeFiles/SimpleSerial.dir/SerialComms.cpp.o"

# External object files for target SimpleSerial
SimpleSerial_EXTERNAL_OBJECTS =

../lib/libSimpleSerial.a: src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/SerialComms.cpp.o
../lib/libSimpleSerial.a: src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/build.make
../lib/libSimpleSerial.a: src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/erolland/moos-ivp-kfish/trunk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../lib/libSimpleSerial.a"
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_SimpleSerial && $(CMAKE_COMMAND) -P CMakeFiles/SimpleSerial.dir/cmake_clean_target.cmake
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_SimpleSerial && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SimpleSerial.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/build: ../lib/libSimpleSerial.a

.PHONY : src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/build

src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/clean:
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_SimpleSerial && $(CMAKE_COMMAND) -P CMakeFiles/SimpleSerial.dir/cmake_clean.cmake
.PHONY : src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/clean

src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/depend:
	cd /home/erolland/moos-ivp-kfish/trunk/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/erolland/moos-ivp-kfish/trunk /home/erolland/moos-ivp-kfish/trunk/src/lib_SimpleSerial /home/erolland/moos-ivp-kfish/trunk/build /home/erolland/moos-ivp-kfish/trunk/build/src/lib_SimpleSerial /home/erolland/moos-ivp-kfish/trunk/build/src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/lib_SimpleSerial/CMakeFiles/SimpleSerial.dir/depend
