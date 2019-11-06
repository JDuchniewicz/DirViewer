# Linux makefile which keeps track of dependencies
# automatically creates BUILD_DIR folder if needed

TARGET_EXEC ?= DirViewer
BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
LIB_DIRS ?= ./libs
INC_DIRS ?= ./headers

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name  *.c)
SRCS := $(SRCS) $(shell find $(LIB_DIRS) -name *.cpp -or -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(INC_DIRS) -type d)
INC_DIRS := $(INC_DIRS) $(shell find $(LIB_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
LINK_FLAGS := -lGL `pkg-config --static --libs glfw3`

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP  `pkg-config --cflags glfw3`
CXXFLAGS := $(CXXFLAGS) -g -Wall -Wformat -std=c++17

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@  $(LINK_FLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
