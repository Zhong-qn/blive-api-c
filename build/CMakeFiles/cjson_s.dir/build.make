# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = D:/CMake/bin/cmake.exe

# The command to remove a file.
RM = D:/CMake/bin/cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = E:/work/pdj/supports/blive-api-c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:/work/pdj/supports/blive-api-c/build

# Include any dependencies generated for this target.
include CMakeFiles/cjson_s.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/cjson_s.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/cjson_s.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cjson_s.dir/flags.make

CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj: CMakeFiles/cjson_s.dir/flags.make
CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj: CMakeFiles/cjson_s.dir/includes_C.rsp
CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj: E:/work/pdj/supports/blive-api-c/external_supports/cJSON/cJSON.c
CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj: CMakeFiles/cjson_s.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:/work/pdj/supports/blive-api-c/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj -MF CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj.d -o CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj -c E:/work/pdj/supports/blive-api-c/external_supports/cJSON/cJSON.c

CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.i"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E E:/work/pdj/supports/blive-api-c/external_supports/cJSON/cJSON.c > CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.i

CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.s"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S E:/work/pdj/supports/blive-api-c/external_supports/cJSON/cJSON.c -o CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.s

CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj: CMakeFiles/cjson_s.dir/flags.make
CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj: CMakeFiles/cjson_s.dir/includes_C.rsp
CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj: E:/work/pdj/supports/blive-api-c/external_supports/cJSON/cJSON_Utils.c
CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj: CMakeFiles/cjson_s.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:/work/pdj/supports/blive-api-c/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj -MF CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj.d -o CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj -c E:/work/pdj/supports/blive-api-c/external_supports/cJSON/cJSON_Utils.c

CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.i"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E E:/work/pdj/supports/blive-api-c/external_supports/cJSON/cJSON_Utils.c > CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.i

CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.s"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S E:/work/pdj/supports/blive-api-c/external_supports/cJSON/cJSON_Utils.c -o CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.s

# Object files for target cjson_s
cjson_s_OBJECTS = \
"CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj" \
"CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj"

# External object files for target cjson_s
cjson_s_EXTERNAL_OBJECTS =

output/lib/libcjson_s.a: CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON.c.obj
output/lib/libcjson_s.a: CMakeFiles/cjson_s.dir/external_supports/cJSON/cJSON_Utils.c.obj
output/lib/libcjson_s.a: CMakeFiles/cjson_s.dir/build.make
output/lib/libcjson_s.a: CMakeFiles/cjson_s.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=E:/work/pdj/supports/blive-api-c/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library output/lib/libcjson_s.a"
	$(CMAKE_COMMAND) -P CMakeFiles/cjson_s.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cjson_s.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cjson_s.dir/build: output/lib/libcjson_s.a
.PHONY : CMakeFiles/cjson_s.dir/build

CMakeFiles/cjson_s.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cjson_s.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cjson_s.dir/clean

CMakeFiles/cjson_s.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" E:/work/pdj/supports/blive-api-c E:/work/pdj/supports/blive-api-c E:/work/pdj/supports/blive-api-c/build E:/work/pdj/supports/blive-api-c/build E:/work/pdj/supports/blive-api-c/build/CMakeFiles/cjson_s.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cjson_s.dir/depend

