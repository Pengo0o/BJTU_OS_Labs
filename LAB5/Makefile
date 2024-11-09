
CXX = g++
CXXFLAGS = -Wall -g


TARGETS = Compare.exe NoSynchronizationMechanism.exe Peterson.exe


SOURCES = Compare.cpp NoSynchronizationMechanism.cpp Peterson.cpp


all: $(TARGETS)

Compare.exe: Compare.cpp
	$(CXX) $(CXXFLAGS) -o Compare.exe Compare.cpp

NoSynchronizationMechanism.exe: NoSynchronizationMechanism.cpp
	$(CXX) $(CXXFLAGS) -o NoSynchronizationMechanism.exe NoSynchronizationMechanism.cpp

Peterson.exe: Peterson.cpp
	$(CXX) $(CXXFLAGS) -o Peterson.exe Peterson.cpp


clean:
	rm -f $(TARGETS)
