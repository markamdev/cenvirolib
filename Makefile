# compiler setting
CC ?= gcc

# directories configuration
INC_DIR = ./inc
SRC_DIR = ./src
BUILD_DIR = ./build

# output files
DEMO_NAME=cenvirodemo
METEO_NAME=meteo-app
LIB_NAME=libcenviro

# build flags
C_FLAGS += -I$(INC_DIR) -std=c99 -Wall
# flag for using 'usleep()'
C_FLAGS += -D_XOPEN_SOURCE=500

# linker flags
LD_FLAGS = -pthread

# list of files to be compiled into library
LIB_SRCS = $(SRC_DIR)/led.c $(SRC_DIR)/weather.c $(SRC_DIR)/cenviro.c

# list of library header files
LIB_HEADERS = $(INC_DIR)/cenviro.h
LIB_INSTALL_HEADERS =  $(LIB_HEADERS:$(INC_DIR)/%.h=$(BUILD_DIR)/%.h)

# list of library objects
LIB_OBJS = $(LIB_SRCS:.c=.o)

#list of files to be compiled into demo application
DEMO_SRCS = apps/demo/main.c
# list of demo application object files
DEMO_OBJS = $(DEMO_SRCS:.c=.o)

# list of files to be compiled into simple meteo application
METEO_SRCS = apps/meteo/main.c
# list of meteo object files
METEO_OBJS = $(METEO_SRCS:.c=.o)

# targets' definition
.PHONY: default clean debug demo meteo

default: $(BUILD_DIR)/$(LIB_NAME).a demo

demo: $(BUILD_DIR)/$(DEMO_NAME)

meteo: $(BUILD_DIR)/$(METEO_NAME)

# demo application
$(BUILD_DIR)/$(DEMO_NAME): $(BUILD_DIR)/$(LIB_NAME).a $(LIB_INSTALL_HEADERS) $(DEMO_OBJS)
	@echo "BINARY: $@"
	@$(CC) -I$(BUILD_DIR) $(C_FLAGS) -L$(BUILD_DIR) $(LD_FLAGS) $(DEMO_OBJS) -lcenviro -o $(BUILD_DIR)/$(DEMO_NAME)

# meteo sample app
$(BUILD_DIR)/$(METEO_NAME): $(BUILD_DIR)/$(LIB_NAME).a $(LIB_INSTALL_HEADERS) $(METEO_OBJS)
	@echo "BINARY: $@"
	@$(CC) -I$(BUILD_DIR) $(C_FLAGS) -L$(BUILD_DIR) $(LD_FLAGS) $(METEO_OBJS) -lcenviro -o $(BUILD_DIR)/$(METEO_NAME)

# library compilation
$(BUILD_DIR)/$(LIB_NAME).a: $(BUILD_DIR) $(LIB_OBJS)
	@echo "LIBRARY: $@"
	@ar rcs $@ $(LIB_OBJS)

clean:
	@echo "CLEAN"
	@rm -f $(LIB_OBJS) $(DEMO_OBJS)
	@rm -rf $(BUILD_DIR)

# output directory creation
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

debug: C_FLAGS += -g -DDEBUG
debug: default

# header files copying
$(BUILD_DIR)/%.h: $(INC_DIR)/%.h
	@echo "COPY $@"
	@cp $< $@

# object files creation from C files
%.o: %.c
	@echo "OBJECT: $@"
	@$(CC) $(C_FLAGS) -c -o $@ $<
