# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.26

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\jonas\Documents\Koding\Workspaces\glWrapper

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\jonas\Documents\Koding\Workspaces\glWrapper\build

# Include any dependencies generated for this target.
include CMakeFiles/glWrapper.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/glWrapper.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/glWrapper.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/glWrapper.dir/flags.make

CMakeFiles/glWrapper.dir/source/glWrapper.cpp.obj: CMakeFiles/glWrapper.dir/flags.make
CMakeFiles/glWrapper.dir/source/glWrapper.cpp.obj: C:/Users/jonas/Documents/Koding/Workspaces/glWrapper/source/glWrapper.cpp
CMakeFiles/glWrapper.dir/source/glWrapper.cpp.obj: CMakeFiles/glWrapper.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\jonas\Documents\Koding\Workspaces\glWrapper\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/glWrapper.dir/source/glWrapper.cpp.obj"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/glWrapper.dir/source/glWrapper.cpp.obj -MF CMakeFiles\glWrapper.dir\source\glWrapper.cpp.obj.d -o CMakeFiles\glWrapper.dir\source\glWrapper.cpp.obj -c C:\Users\jonas\Documents\Koding\Workspaces\glWrapper\source\glWrapper.cpp

CMakeFiles/glWrapper.dir/source/glWrapper.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/glWrapper.dir/source/glWrapper.cpp.i"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\jonas\Documents\Koding\Workspaces\glWrapper\source\glWrapper.cpp > CMakeFiles\glWrapper.dir\source\glWrapper.cpp.i

CMakeFiles/glWrapper.dir/source/glWrapper.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/glWrapper.dir/source/glWrapper.cpp.s"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\jonas\Documents\Koding\Workspaces\glWrapper\source\glWrapper.cpp -o CMakeFiles\glWrapper.dir\source\glWrapper.cpp.s

# Object files for target glWrapper
glWrapper_OBJECTS = \
"CMakeFiles/glWrapper.dir/source/glWrapper.cpp.obj"

# External object files for target glWrapper
glWrapper_EXTERNAL_OBJECTS =

libglWrapper.a: CMakeFiles/glWrapper.dir/source/glWrapper.cpp.obj
libglWrapper.a: CMakeFiles/glWrapper.dir/build.make
libglWrapper.a: CMakeFiles/glWrapper.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\jonas\Documents\Koding\Workspaces\glWrapper\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libglWrapper.a"
	$(CMAKE_COMMAND) -P CMakeFiles\glWrapper.dir\cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\glWrapper.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/glWrapper.dir/build: libglWrapper.a
.PHONY : CMakeFiles/glWrapper.dir/build

CMakeFiles/glWrapper.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\glWrapper.dir\cmake_clean.cmake
.PHONY : CMakeFiles/glWrapper.dir/clean

CMakeFiles/glWrapper.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\jonas\Documents\Koding\Workspaces\glWrapper C:\Users\jonas\Documents\Koding\Workspaces\glWrapper C:\Users\jonas\Documents\Koding\Workspaces\glWrapper\build C:\Users\jonas\Documents\Koding\Workspaces\glWrapper\build C:\Users\jonas\Documents\Koding\Workspaces\glWrapper\build\CMakeFiles\glWrapper.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/glWrapper.dir/depend

