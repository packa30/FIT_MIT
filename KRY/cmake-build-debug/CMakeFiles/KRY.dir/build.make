# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/169/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/169/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/peter/Documents/II year/sem2/KRY"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/peter/Documents/II year/sem2/KRY/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/KRY.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/KRY.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/KRY.dir/flags.make

CMakeFiles/KRY.dir/main.cpp.o: CMakeFiles/KRY.dir/flags.make
CMakeFiles/KRY.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/peter/Documents/II year/sem2/KRY/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/KRY.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/KRY.dir/main.cpp.o -c "/home/peter/Documents/II year/sem2/KRY/main.cpp"

CMakeFiles/KRY.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/KRY.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/peter/Documents/II year/sem2/KRY/main.cpp" > CMakeFiles/KRY.dir/main.cpp.i

CMakeFiles/KRY.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/KRY.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/peter/Documents/II year/sem2/KRY/main.cpp" -o CMakeFiles/KRY.dir/main.cpp.s

CMakeFiles/KRY.dir/kasisk.cpp.o: CMakeFiles/KRY.dir/flags.make
CMakeFiles/KRY.dir/kasisk.cpp.o: ../kasisk.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/peter/Documents/II year/sem2/KRY/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/KRY.dir/kasisk.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/KRY.dir/kasisk.cpp.o -c "/home/peter/Documents/II year/sem2/KRY/kasisk.cpp"

CMakeFiles/KRY.dir/kasisk.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/KRY.dir/kasisk.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/peter/Documents/II year/sem2/KRY/kasisk.cpp" > CMakeFiles/KRY.dir/kasisk.cpp.i

CMakeFiles/KRY.dir/kasisk.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/KRY.dir/kasisk.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/peter/Documents/II year/sem2/KRY/kasisk.cpp" -o CMakeFiles/KRY.dir/kasisk.cpp.s

CMakeFiles/KRY.dir/friedman.cpp.o: CMakeFiles/KRY.dir/flags.make
CMakeFiles/KRY.dir/friedman.cpp.o: ../friedman.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/peter/Documents/II year/sem2/KRY/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/KRY.dir/friedman.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/KRY.dir/friedman.cpp.o -c "/home/peter/Documents/II year/sem2/KRY/friedman.cpp"

CMakeFiles/KRY.dir/friedman.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/KRY.dir/friedman.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/peter/Documents/II year/sem2/KRY/friedman.cpp" > CMakeFiles/KRY.dir/friedman.cpp.i

CMakeFiles/KRY.dir/friedman.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/KRY.dir/friedman.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/peter/Documents/II year/sem2/KRY/friedman.cpp" -o CMakeFiles/KRY.dir/friedman.cpp.s

# Object files for target KRY
KRY_OBJECTS = \
"CMakeFiles/KRY.dir/main.cpp.o" \
"CMakeFiles/KRY.dir/kasisk.cpp.o" \
"CMakeFiles/KRY.dir/friedman.cpp.o"

# External object files for target KRY
KRY_EXTERNAL_OBJECTS =

KRY: CMakeFiles/KRY.dir/main.cpp.o
KRY: CMakeFiles/KRY.dir/kasisk.cpp.o
KRY: CMakeFiles/KRY.dir/friedman.cpp.o
KRY: CMakeFiles/KRY.dir/build.make
KRY: CMakeFiles/KRY.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/peter/Documents/II year/sem2/KRY/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable KRY"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/KRY.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/KRY.dir/build: KRY
.PHONY : CMakeFiles/KRY.dir/build

CMakeFiles/KRY.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/KRY.dir/cmake_clean.cmake
.PHONY : CMakeFiles/KRY.dir/clean

CMakeFiles/KRY.dir/depend:
	cd "/home/peter/Documents/II year/sem2/KRY/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/peter/Documents/II year/sem2/KRY" "/home/peter/Documents/II year/sem2/KRY" "/home/peter/Documents/II year/sem2/KRY/cmake-build-debug" "/home/peter/Documents/II year/sem2/KRY/cmake-build-debug" "/home/peter/Documents/II year/sem2/KRY/cmake-build-debug/CMakeFiles/KRY.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/KRY.dir/depend
