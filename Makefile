BUILD_DIR	:= build
SRC_DIR 	:= src
TEST_DIR	:= tests

ENTRY		:= main.c
EXE			:= test

SRCS 		:= $(shell find $(SRC_DIR)/*.c)
OBJS 		:= $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS 		:= $(OBJS:.o=.d)

DEPFLAGS 	:= -MMD -MP
CXXFLAGS	:= -g -O3 -Wall -std=c11 -march=native
LDFLAGS		:= -lm

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.
.SECONDEXPANSION: $(BUILD_DIR)/%.o

# Specific rules

all: build-tests main;

main: $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) $(ENTRY) $(LDFLAGS) -o $(EXE)

build: $(OBJS);

clean:
	$(RM) $(OBJS) $(EXE)

clean-deps:
	$(RM) -r $(DEPS)

clean-all:
	$(RM) -r $(BUILD_DIR) tmp $(EXE)

### Specific tests

### General rules

$(BUILD_DIR)/.:
	mkdir -p $@

$(BUILD_DIR)%/.:
	mkdir -p $@

$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c | $$(@D)/.
	$(CC) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $< $(LDFLAGS)

-include $(DEPS)
.PHONY: all main tests memcheck build-tests build clean clean-deps clean-all
