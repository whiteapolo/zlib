CC = cc
CFLAGS = -Iinclude -g -Wall -Wextra -O3
LIB = libzatar.a
OBJDIR = obj

# 1. Find all .c files in the root and all subdirectories of src/
SRC = $(shell find src -name "*.c")

# 2. Generate the list of object files (all placed directly in OBJDIR)
# This strips the path from the source file and adds the obj/ prefix
OBJ = $(addprefix $(OBJDIR)/, $(notdir $(SRC:.c=.o)))

# 3. Extract all unique source directories and add them to VPATH
# This tells make where to look for the .c files for the pattern rule below
SRCDIRS = $(sort $(dir $(SRC)))
VPATH = $(SRCDIRS)

all: $(LIB)

# Link the library from the flat list of objects
$(LIB): $(OBJ)
	ar rcs $@ $^

# Rule to compile any .c file into the flat obj/ folder
$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(LIB)

.PHONY: all clean