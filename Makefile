# compiler setting
CC ?= gcc

# directories configuration
INC_DIR = ./inc
SRC_DIR = ./src
BUILD_DIR = ./build

# output files
APP_NAME=cenviroapp
LIB_NAME=libcenviro

# build flags
C_FLAGS += -I$(INC_DIR) -std=c99 -Wall
# flag for using 'usleep()'
C_FLAGS += -D_XOPEN_SOURCE=500

# linker flags
LD_FLAGS = -pthread

# list of files to be compiled into library
LIB_SRCS = $(SRC_DIR)/led.c

# list of library header files
LIB_HEADERS = $(INC_DIR)/cel_led.h
LIB_INSTALL_HEADERS =  $(LIB_HEADERS:$(INC_DIR)/%.h=$(BUILD_DIR)/%.h)

# list of library objects
LIB_OBJECTS = $(LIB_SRCS:.c=.o) 

#list of files to be compiled into application only
APP_SRCS = $(SRC_DIR)/main.c

# list of application specific object files
APP_OBJECTS = $(APP_SRCS:.c=.o) 

# targets' definition
.PHONY: default clean debug

default: $(BUILD_DIR)/$(LIB_NAME).a $(BUILD_DIR)/$(APP_NAME)

$(BUILD_DIR)/$(APP_NAME): $(BUILD_DIR)/$(LIB_NAME).a $(LIB_INSTALL_HEADERS) $(APP_OBJECTS)
	@echo "BINARY: $@"
	@$(CC) -I$(BUILD_DIR) $(C_FLAGS) -L$(BUILD_DIR) $(LD_FLAGS) $(APP_OBJECTS) -lcenviro -o $(BUILD_DIR)/$(APP_NAME)

$(BUILD_DIR)/$(LIB_NAME).a: $(BUILD_DIR) $(LIB_OBJECTS)
	@echo "LIBRARY: $@"
	@ar rcs $@ $(LIB_OBJECTS)

clean:
	@echo "CLEAN"
	@rm -f $(LIB_OBJECTS) $(APP_OBJECTS)
	@rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

debug: C_FLAGS += -g -DDEBUG
debug: default

$(BUILD_DIR)/%.h: $(INC_DIR)/%.h
	@echo "COPY $@"
	@cp $< $@

%.o: %.c
	@echo "OBJECT: $@"
	@$(CC) $(C_FLAGS) -c -o $@ $<