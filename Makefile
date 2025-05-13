# Define the compiler to use
CC = gcc

# Define the flags for the compiler

# Define the name of the final executable
TARGET = a.out

# List all source files
SRCS = main.c \
       display.c \
       input.c \
       hop.c \
       reveal.c \
       log.c \
       sys_commands.c \
       proclore.c \
       seek.c \
       io_redirection.c\
       pipes.c\
       activities.c\
       signals.c\
       fg_bg.c\
       iMan.c\


# List all object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# The default target that will be built when you just run `make`
all: $(TARGET)

# Rule to build the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to build object files from source files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets are not files
.PHONY: all clean
