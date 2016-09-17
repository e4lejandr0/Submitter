CXX_FLAGS=-Wall -std=c++11
BUILD_DIR=build
OBJ_DIR=build/obj
SRC_DIR=src
SRCS:=$(shell find src -name '*.cpp')
OBJS=$(subst .cpp,.o,$(subst src/,$(OBJ_DIR)/,$(SRCS)))
TARGETS=$(subst src/,,$(subst .cpp,, $(SRCS)))
BIN_NAME=Submitter

all: $(TARGETS)
	$(CXX) $(CXX_FLAGS) $(OBJS) -o $(BUILD_DIR)/$(BIN_NAME)
$(TARGETS):
	@mkdir -p build/obj
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/$@.cpp -o $(OBJ_DIR)/$@.o

.PHONY: clean
clean:
	@echo 'Removing build files...'
	@rm -rf $(BUILD_DIR)
.PHONY: cleanobj
cleanobj:
	@echo 'Removing object files...'
	@rm -rf $(OBJ_DIR)

