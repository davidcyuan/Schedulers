CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2
BIN      := sim
SRCS     := main.cpp simulator.cpp
HDRS     := task.hpp scheduler.hpp schedulers.hpp metrics.hpp simulator.hpp

# Program arguments, given as bare words after `run`:
#   make run                  -> ./sim fifo
#   make run RoundRobin       -> ./sim RoundRobin
#   make run RoundRobin detailed  -> per-tick trace of the running task
# Defaults to fifo when no scheduler is named.
ifneq ($(filter run,$(MAKECMDGOALS)),)
ARGS := $(filter-out run,$(MAKECMDGOALS))
ARGS := $(if $(ARGS),$(ARGS),fifo)
# Swallow the extra words so make doesn't treat them as real targets.
$(foreach g,$(filter-out run,$(MAKECMDGOALS)),$(eval $(g):;@:))
else
ARGS := fifo
endif

$(BIN): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

run: $(BIN)
	@./$(BIN) $(ARGS)

clean:
	rm -f $(BIN)

.PHONY: run clean
