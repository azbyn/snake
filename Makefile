CXX := cc_args clang++
SRC_DIR := .
BUILD_DIR := build
TARGET := snake

SRCS := $(shell find $(SRC_DIR) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

#THIS_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))



CPPFLAGS := $(INC_FLAGS)
CPPFLAGS += -Wall -Wextra -g -pg -std=c++11 -Wpedantic -ferror-limit=1 \
-Wno-unused-const-variable
#CPPFLAGS += -g -std=c++17 -Weverything \
-Wno-c++98-compat \
-Wno-c++98-compat-pedantic \
-Wno-sign-conversion \
-Wno-unused-macros \
-Wno-format-nonliteral \
-Wno-shorten-64-to-32 \
-Wno-unused-const-variable \
-Wno-missing-variable-declarations \
-Wno-old-style-cast \
-Wno-shadow-field-in-constructor \
-Wno-padded \
-ferror-limit=1

CPPFLAGS += -DVERSION=\"0.0.0.1\" #`pkg-config --cflags ...`  -I...



LDFLAGS := -pthread -lncurses #-l...

$(TARGET): $(OBJS)
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS)


$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	@$(CXX) $(CPPFLAGS) -c $< -o $@


.PHONY: clean
clean:
	@$(RM) -r $(BUILD_DIR) $(TARGET)

-include $(DEPS)

MKDIR_P ?= mkdir -p
