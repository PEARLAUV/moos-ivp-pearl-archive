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
include src/lib_gpsParser/CMakeFiles/gpsParser.dir/depend.make

# Include the progress variables for this target.
include src/lib_gpsParser/CMakeFiles/gpsParser.dir/progress.make

# Include the compile flags for this target's objects.
include src/lib_gpsParser/CMakeFiles/gpsParser.dir/flags.make

src/lib_gpsParser/CMakeFiles/gpsParser.dir/gpsParser.cpp.o: src/lib_gpsParser/CMakeFiles/gpsParser.dir/flags.make
src/lib_gpsParser/CMakeFiles/gpsParser.dir/gpsParser.cpp.o: ../src/lib_gpsParser/gpsParser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/erolland/moos-ivp-kfish/trunk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/lib_gpsParser/CMakeFiles/gpsParser.dir/gpsParser.cpp.o"
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_gpsParser && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gpsParser.dir/gpsParser.cpp.o -c /home/erolland/moos-ivp-kfish/trunk/src/lib_gpsParser/gpsParser.cpp

src/lib_gpsParser/CMakeFiles/gpsParser.dir/gpsParser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gpsParser.dir/gpsParser.cpp.i"
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_gpsParser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/erolland/moos-ivp-kfish/trunk/src/lib_gpsParser/gpsParser.cpp > CMakeFiles/gpsParser.dir/gpsParser.cpp.i

src/lib_gpsParser/CMakeFiles/gpsParser.dir/gpsParser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gpsParser.dir/gpsParser.cpp.s"
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_gpsParser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/erolland/moos-ivp-kfish/trunk/src/lib_gpsParser/gpsParser.cpp -o CMakeFiles/gpsParser.dir/gpsParser.cpp.s

# Object files for target gpsParser
gpsParser_OBJECTS = \
"CMakeFiles/gpsParser.dir/gpsParser.cpp.o"

# External object files for target gpsParser
gpsParser_EXTERNAL_OBJECTS =

../lib/libgpsParser.a: src/lib_gpsParser/CMakeFiles/gpsParser.dir/gpsParser.cpp.o
../lib/libgpsParser.a: src/lib_gpsParser/CMakeFiles/gpsParser.dir/build.make
../lib/libgpsParser.a: src/lib_gpsParser/CMakeFiles/gpsParser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/erolland/moos-ivp-kfish/trunk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../lib/libgpsParser.a"
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_gpsParser && $(CMAKE_COMMAND) -P CMakeFiles/gpsParser.dir/cmake_clean_target.cmake
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_gpsParser && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gpsParser.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/lib_gpsParser/CMakeFiles/gpsParser.dir/build: ../lib/libgpsParser.a

.PHONY : src/lib_gpsParser/CMakeFiles/gpsParser.dir/build

src/lib_gpsParser/CMakeFiles/gpsParser.dir/clean:
	cd /home/erolland/moos-ivp-kfish/trunk/build/src/lib_gpsParser && $(CMAKE_COMMAND) -P CMakeFiles/gpsParser.dir/cmake_clean.cmake
.PHONY : src/lib_gpsParser/CMakeFiles/gpsParser.dir/clean

src/lib_gpsParser/CMakeFiles/gpsParser.dir/depend:
	cd /home/erolland/moos-ivp-kfish/trunk/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/erolland/moos-ivp-kfish/trunk /home/erolland/moos-ivp-kfish/trunk/src/lib_gpsParser /home/erolland/moos-ivp-kfish/trunk/build /home/erolland/moos-ivp-kfish/trunk/build/src/lib_gpsParser /home/erolland/moos-ivp-kfish/trunk/build/src/lib_gpsParser/CMakeFiles/gpsParser.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/lib_gpsParser/CMakeFiles/gpsParser.dir/depend

