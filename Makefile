CC = cc
CFLAGS = -Iinclude -g -Wall -Wextra -O3
LIB = libzatar.a
OBJDIR = obj
SRC = $(shell find src -name "*.c")
OBJ = $(addprefix $(OBJDIR)/, $(notdir $(SRC:.c=.o)))
SRCDIRS = $(sort $(dir $(SRC)))
VPATH = $(SRCDIRS)

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(LIB)

.PHONY: all clean