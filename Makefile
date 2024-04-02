# C++ Makefile for HW5
#
# Replace the LastName-FirstName in the package section with
# your actual first and last name.

CXX=gcc
CXXFLAGS=-Wall -Wextra -Wpedantic -Werror -Wfatal-errors -Winit-self -Wshadow -Wcomment -Wold-style-cast -D_GLIBCXX_DEBUG -fno-diagnostics-show-option

all: Scheduler.c
	$(CXX) $(CXXFLAGS) -o Scheduler Scheduler.c

clean:
	rm -f Scheduler *.o *.zip

package:
	zip Crosbie-Graham-HW5.zip README.txt Makefile Scheduler.c
