CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
BIN      := sim
SRCS     := main.cpp simulator.cpp

$(BIN): $(SRCS) task.hpp scheduler.hpp scheduler_1.hpp metrics.hpp simulator.hpp
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: run clean
