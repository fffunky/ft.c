TARGET_EXEC := ft

SRC_DIR := ./src
BUILD_DIR := ./build

# find all the c files
srcs := $(shell find $(SRC_DIR) -name '*.c')

# prepends BUILD_DIR and appends .o to every src file
objs := $(srcs:%=$(BUILD_DIR)/%.o)

# turns .o files into .d files
deps := $(objs:.o=.d)

# get all the directories nested in SRC_DIR
INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# -MMD and -MP generate makefiles for us.
CFLAGS := -MMD -MP $(INC_FLAGS) 

# executable build step
$(TARGET_EXEC): $(objs)
	$(CC) $(objs) -o $@ $(LDFLAGS)

# build step for src
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm ft
	rm -r $(BUILD_DIR)

-include $(DEPS)
