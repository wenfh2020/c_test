CXX = g++
CPPFLAGS = -g -O0 -Wall

TARGETS = main

CPP_SRCS = $(foreach dir, ., $(wildcard $(dir)/*.cpp))
OBJS = $(patsubst %.cpp, %.o, $(CPP_SRCS))

$(TARGETS): $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $@ 

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $< -c -o $@

clean:
	rm -f $(OBJS) $(TARGETS)