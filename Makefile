BUILD_DIR	:= build
SRC_DIR 	:= src
TEST_DIR	:= tests

ENTRY		:= main.c
LIB			:= enl.so

SRCS 		:= $(shell find $(SRC_DIR)/*.c)
OBJS 		:= $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS 		:= $(OBJS:.o=.d)

TESTS 		:= $(shell find $(TEST_DIR)/*.c)
TEST_EXES 	:= $(TESTS:$(TEST_DIR)/%.c=$(BUILD_DIR)/%)

DEPFLAGS 	:= -MMD -MP
CFLAGS		:= -g -Wall -std=c11 -D_XOPEN_SOURCE -march=native -fPIC
LDFLAGS		:= -lm

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.
.SECONDEXPANSION: $(BUILD_DIR)/%.o

# Specific rules

lib: $(OBJS)
	$(CC) -shared -fPIC -Wl,-soname,enl.so -o enl.so $(OBJS) -lc

tests: $(OBJS) $(TEST_EXES)
	$(TEST_DIR)/test.sh $(TEST_EXES)

build: $(OBJS);

build-tests: $(TEST_EXES);

clean:
	$(RM) $(OBJS) $(LIB)

clean-deps:
	$(RM) -r $(DEPS)

clean-all:
	$(RM) -r $(BUILD_DIR) tmp $(LIB)

### Specific tests

test-sandbox: build/sandbox
	$(TEST_DIR)/test.sh build/sandbox

### General rules

$(BUILD_DIR)/.:
	mkdir -p $@

$(BUILD_DIR)%/.:
	mkdir -p $@

$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c | $$(@D)/.
	$(CC) $(CFLAGS) $(DEPFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/%: $(TEST_DIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $< $(LDFLAGS)

-include $(DEPS)
.PHONY: all main tests build-tests build clean clean-deps clean-all test-sandbox
