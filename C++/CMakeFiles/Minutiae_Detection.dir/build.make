# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/cmake-gui

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/montura/Git/LSS/C++

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/montura/Git/LSS/C++

# Include any dependencies generated for this target.
include CMakeFiles/Minutiae_Detection.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Minutiae_Detection.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Minutiae_Detection.dir/flags.make

CMakeFiles/Minutiae_Detection.dir/main.cpp.o: CMakeFiles/Minutiae_Detection.dir/flags.make
CMakeFiles/Minutiae_Detection.dir/main.cpp.o: main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/montura/Git/LSS/C++/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Minutiae_Detection.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Minutiae_Detection.dir/main.cpp.o -c /home/montura/Git/LSS/C++/main.cpp

CMakeFiles/Minutiae_Detection.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Minutiae_Detection.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/montura/Git/LSS/C++/main.cpp > CMakeFiles/Minutiae_Detection.dir/main.cpp.i

CMakeFiles/Minutiae_Detection.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Minutiae_Detection.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/montura/Git/LSS/C++/main.cpp -o CMakeFiles/Minutiae_Detection.dir/main.cpp.s

CMakeFiles/Minutiae_Detection.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/Minutiae_Detection.dir/main.cpp.o.requires

CMakeFiles/Minutiae_Detection.dir/main.cpp.o.provides: CMakeFiles/Minutiae_Detection.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/Minutiae_Detection.dir/build.make CMakeFiles/Minutiae_Detection.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/Minutiae_Detection.dir/main.cpp.o.provides

CMakeFiles/Minutiae_Detection.dir/main.cpp.o.provides.build: CMakeFiles/Minutiae_Detection.dir/main.cpp.o

# Object files for target Minutiae_Detection
Minutiae_Detection_OBJECTS = \
"CMakeFiles/Minutiae_Detection.dir/main.cpp.o"

# External object files for target Minutiae_Detection
Minutiae_Detection_EXTERNAL_OBJECTS =

Minutiae_Detection: CMakeFiles/Minutiae_Detection.dir/main.cpp.o
Minutiae_Detection: CMakeFiles/Minutiae_Detection.dir/build.make
Minutiae_Detection: /usr/local/lib/libopencv_videostab.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_videoio.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_video.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_superres.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_stitching.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_shape.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_photo.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_objdetect.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_ml.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_imgproc.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_imgcodecs.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_highgui.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_flann.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_features2d.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_core.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_calib3d.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_features2d.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_ml.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_highgui.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_videoio.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_imgcodecs.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_flann.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_video.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_imgproc.so.3.1.0
Minutiae_Detection: /usr/local/lib/libopencv_core.so.3.1.0
Minutiae_Detection: CMakeFiles/Minutiae_Detection.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable Minutiae_Detection"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Minutiae_Detection.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Minutiae_Detection.dir/build: Minutiae_Detection
.PHONY : CMakeFiles/Minutiae_Detection.dir/build

CMakeFiles/Minutiae_Detection.dir/requires: CMakeFiles/Minutiae_Detection.dir/main.cpp.o.requires
.PHONY : CMakeFiles/Minutiae_Detection.dir/requires

CMakeFiles/Minutiae_Detection.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Minutiae_Detection.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Minutiae_Detection.dir/clean

CMakeFiles/Minutiae_Detection.dir/depend:
	cd /home/montura/Git/LSS/C++ && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/montura/Git/LSS/C++ /home/montura/Git/LSS/C++ /home/montura/Git/LSS/C++ /home/montura/Git/LSS/C++ /home/montura/Git/LSS/C++/CMakeFiles/Minutiae_Detection.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Minutiae_Detection.dir/depend

