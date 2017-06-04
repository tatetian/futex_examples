.PHONY: all 

C_SRCS := $(sort $(wildcard *.c))
C_OBJS := $(C_SRCS:.c=.o)
COMMON_OBJS := mutex.o futex.o condvar.o
TEST_SRCS := $(sort $(wildcard test_*.c))
BINS := $(TEST_SRCS:.c=)

C_FLAGS = -std=c11 -Wall -O2 -fPIC -lpthread

all: $(BINS)

$(BINS): %: %.o $(COMMON_OBJS)
	@$(CC) $^ -o $@ $(C_FLAGS)
	@echo "LINK => $@"

$(C_OBJS): %.o: %.c
	@$(CC) $(C_FLAGS) -c $< -o $@
	@echo "CC <= $@"

clean:
	@$(RM) -f $(BINS) $(C_OBJS)
