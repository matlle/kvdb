# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /home/matlle/Files/IDEs/clion-2019.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/matlle/Files/IDEs/clion-2019.2.4/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/matlle/Projects/c++/degree_project_ma/kvdb

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/kvdb.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/kvdb.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/kvdb.dir/flags.make

CMakeFiles/kvdb.dir/main.cpp.o: CMakeFiles/kvdb.dir/flags.make
CMakeFiles/kvdb.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/kvdb.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvdb.dir/main.cpp.o -c /home/matlle/Projects/c++/degree_project_ma/kvdb/main.cpp

CMakeFiles/kvdb.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvdb.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/matlle/Projects/c++/degree_project_ma/kvdb/main.cpp > CMakeFiles/kvdb.dir/main.cpp.i

CMakeFiles/kvdb.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvdb.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/matlle/Projects/c++/degree_project_ma/kvdb/main.cpp -o CMakeFiles/kvdb.dir/main.cpp.s

CMakeFiles/kvdb.dir/time/DateTime.cpp.o: CMakeFiles/kvdb.dir/flags.make
CMakeFiles/kvdb.dir/time/DateTime.cpp.o: ../time/DateTime.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/kvdb.dir/time/DateTime.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvdb.dir/time/DateTime.cpp.o -c /home/matlle/Projects/c++/degree_project_ma/kvdb/time/DateTime.cpp

CMakeFiles/kvdb.dir/time/DateTime.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvdb.dir/time/DateTime.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/matlle/Projects/c++/degree_project_ma/kvdb/time/DateTime.cpp > CMakeFiles/kvdb.dir/time/DateTime.cpp.i

CMakeFiles/kvdb.dir/time/DateTime.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvdb.dir/time/DateTime.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/matlle/Projects/c++/degree_project_ma/kvdb/time/DateTime.cpp -o CMakeFiles/kvdb.dir/time/DateTime.cpp.s

CMakeFiles/kvdb.dir/tree/BTree.cpp.o: CMakeFiles/kvdb.dir/flags.make
CMakeFiles/kvdb.dir/tree/BTree.cpp.o: ../tree/BTree.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/kvdb.dir/tree/BTree.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvdb.dir/tree/BTree.cpp.o -c /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/BTree.cpp

CMakeFiles/kvdb.dir/tree/BTree.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvdb.dir/tree/BTree.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/BTree.cpp > CMakeFiles/kvdb.dir/tree/BTree.cpp.i

CMakeFiles/kvdb.dir/tree/BTree.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvdb.dir/tree/BTree.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/BTree.cpp -o CMakeFiles/kvdb.dir/tree/BTree.cpp.s

CMakeFiles/kvdb.dir/tree/Node.cpp.o: CMakeFiles/kvdb.dir/flags.make
CMakeFiles/kvdb.dir/tree/Node.cpp.o: ../tree/Node.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/kvdb.dir/tree/Node.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvdb.dir/tree/Node.cpp.o -c /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/Node.cpp

CMakeFiles/kvdb.dir/tree/Node.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvdb.dir/tree/Node.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/Node.cpp > CMakeFiles/kvdb.dir/tree/Node.cpp.i

CMakeFiles/kvdb.dir/tree/Node.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvdb.dir/tree/Node.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/Node.cpp -o CMakeFiles/kvdb.dir/tree/Node.cpp.s

CMakeFiles/kvdb.dir/tree/Key.cpp.o: CMakeFiles/kvdb.dir/flags.make
CMakeFiles/kvdb.dir/tree/Key.cpp.o: ../tree/Key.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/kvdb.dir/tree/Key.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvdb.dir/tree/Key.cpp.o -c /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/Key.cpp

CMakeFiles/kvdb.dir/tree/Key.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvdb.dir/tree/Key.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/Key.cpp > CMakeFiles/kvdb.dir/tree/Key.cpp.i

CMakeFiles/kvdb.dir/tree/Key.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvdb.dir/tree/Key.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/Key.cpp -o CMakeFiles/kvdb.dir/tree/Key.cpp.s

CMakeFiles/kvdb.dir/tree/Value.cpp.o: CMakeFiles/kvdb.dir/flags.make
CMakeFiles/kvdb.dir/tree/Value.cpp.o: ../tree/Value.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/kvdb.dir/tree/Value.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvdb.dir/tree/Value.cpp.o -c /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/Value.cpp

CMakeFiles/kvdb.dir/tree/Value.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvdb.dir/tree/Value.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/Value.cpp > CMakeFiles/kvdb.dir/tree/Value.cpp.i

CMakeFiles/kvdb.dir/tree/Value.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvdb.dir/tree/Value.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/matlle/Projects/c++/degree_project_ma/kvdb/tree/Value.cpp -o CMakeFiles/kvdb.dir/tree/Value.cpp.s

CMakeFiles/kvdb.dir/cli/Cli.cpp.o: CMakeFiles/kvdb.dir/flags.make
CMakeFiles/kvdb.dir/cli/Cli.cpp.o: ../cli/Cli.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/kvdb.dir/cli/Cli.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvdb.dir/cli/Cli.cpp.o -c /home/matlle/Projects/c++/degree_project_ma/kvdb/cli/Cli.cpp

CMakeFiles/kvdb.dir/cli/Cli.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvdb.dir/cli/Cli.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/matlle/Projects/c++/degree_project_ma/kvdb/cli/Cli.cpp > CMakeFiles/kvdb.dir/cli/Cli.cpp.i

CMakeFiles/kvdb.dir/cli/Cli.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvdb.dir/cli/Cli.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/matlle/Projects/c++/degree_project_ma/kvdb/cli/Cli.cpp -o CMakeFiles/kvdb.dir/cli/Cli.cpp.s

CMakeFiles/kvdb.dir/db/Database.cpp.o: CMakeFiles/kvdb.dir/flags.make
CMakeFiles/kvdb.dir/db/Database.cpp.o: ../db/Database.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/kvdb.dir/db/Database.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvdb.dir/db/Database.cpp.o -c /home/matlle/Projects/c++/degree_project_ma/kvdb/db/Database.cpp

CMakeFiles/kvdb.dir/db/Database.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvdb.dir/db/Database.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/matlle/Projects/c++/degree_project_ma/kvdb/db/Database.cpp > CMakeFiles/kvdb.dir/db/Database.cpp.i

CMakeFiles/kvdb.dir/db/Database.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvdb.dir/db/Database.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/matlle/Projects/c++/degree_project_ma/kvdb/db/Database.cpp -o CMakeFiles/kvdb.dir/db/Database.cpp.s

CMakeFiles/kvdb.dir/db/Table.cpp.o: CMakeFiles/kvdb.dir/flags.make
CMakeFiles/kvdb.dir/db/Table.cpp.o: ../db/Table.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/kvdb.dir/db/Table.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvdb.dir/db/Table.cpp.o -c /home/matlle/Projects/c++/degree_project_ma/kvdb/db/Table.cpp

CMakeFiles/kvdb.dir/db/Table.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvdb.dir/db/Table.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/matlle/Projects/c++/degree_project_ma/kvdb/db/Table.cpp > CMakeFiles/kvdb.dir/db/Table.cpp.i

CMakeFiles/kvdb.dir/db/Table.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvdb.dir/db/Table.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/matlle/Projects/c++/degree_project_ma/kvdb/db/Table.cpp -o CMakeFiles/kvdb.dir/db/Table.cpp.s

# Object files for target kvdb
kvdb_OBJECTS = \
"CMakeFiles/kvdb.dir/main.cpp.o" \
"CMakeFiles/kvdb.dir/time/DateTime.cpp.o" \
"CMakeFiles/kvdb.dir/tree/BTree.cpp.o" \
"CMakeFiles/kvdb.dir/tree/Node.cpp.o" \
"CMakeFiles/kvdb.dir/tree/Key.cpp.o" \
"CMakeFiles/kvdb.dir/tree/Value.cpp.o" \
"CMakeFiles/kvdb.dir/cli/Cli.cpp.o" \
"CMakeFiles/kvdb.dir/db/Database.cpp.o" \
"CMakeFiles/kvdb.dir/db/Table.cpp.o"

# External object files for target kvdb
kvdb_EXTERNAL_OBJECTS =

kvdb: CMakeFiles/kvdb.dir/main.cpp.o
kvdb: CMakeFiles/kvdb.dir/time/DateTime.cpp.o
kvdb: CMakeFiles/kvdb.dir/tree/BTree.cpp.o
kvdb: CMakeFiles/kvdb.dir/tree/Node.cpp.o
kvdb: CMakeFiles/kvdb.dir/tree/Key.cpp.o
kvdb: CMakeFiles/kvdb.dir/tree/Value.cpp.o
kvdb: CMakeFiles/kvdb.dir/cli/Cli.cpp.o
kvdb: CMakeFiles/kvdb.dir/db/Database.cpp.o
kvdb: CMakeFiles/kvdb.dir/db/Table.cpp.o
kvdb: CMakeFiles/kvdb.dir/build.make
kvdb: CMakeFiles/kvdb.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX executable kvdb"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kvdb.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/kvdb.dir/build: kvdb

.PHONY : CMakeFiles/kvdb.dir/build

CMakeFiles/kvdb.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/kvdb.dir/cmake_clean.cmake
.PHONY : CMakeFiles/kvdb.dir/clean

CMakeFiles/kvdb.dir/depend:
	cd /home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/matlle/Projects/c++/degree_project_ma/kvdb /home/matlle/Projects/c++/degree_project_ma/kvdb /home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug /home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug /home/matlle/Projects/c++/degree_project_ma/kvdb/cmake-build-debug/CMakeFiles/kvdb.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/kvdb.dir/depend

