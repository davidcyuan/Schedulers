CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2
BIN      := sim
SRCS     := main.cpp simulator.cpp
HDRS     := task.hpp scheduler.hpp schedulers.hpp metrics.hpp simulator.hpp

# Scheduler to run, given as a bare argument: `make run fifo`.
# Defaults to fifo when no name is supplied.
ifneq ($(filter run,$(MAKECMDGOALS)),)
SCHED := $(filter-out run,$(MAKECMDGOALS))
SCHED := $(if $(SCHED),$(SCHED),fifo)
# Swallow the scheduler name so make doesn't treat it as a real target.
$(foreach g,$(filter-out run,$(MAKECMDGOALS)),$(eval $(g):;@:))
else
SCHED := fifo
endif

$(BIN): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

run: $(BIN)
	@./$(BIN) $(SCHED)

clean:
	rm -f $(BIN)

.PHONY: run clean
