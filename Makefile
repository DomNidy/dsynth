CC=cc
CCFLAGS=-Wall -Wextra -std=c11
CCFLAGS_DEBUG=$(CCFLAGS) -g 
LDFLAGS=-lm

# Name of release app
APP_NAME = dsynth
# Name of debug app
APP_NAME_DEBUG = dsynth_debug

# Directory storing the release object files
OBJDIR=objs
# Directory storing the debug object files
OBJDIR_DEBUG=objs_debug

INCLUDES = -I.

.PHONY: dirs clean emit_debug

default: $(APP_NAME)

dirs:
	mkdir -p $(OBJDIR)/
	mkdir -p $(OBJDIR_DEBUG)/

clean:
	rm -rf $(OBJDIR) $(OBJDIR_DEBUG)

## Release section

# List of targets final app depends on
OBJS=$(OBJDIR)/main.o $(OBJDIR)/audiofile.o

# Rule for the final executable, links all objs together
$(APP_NAME): dirs $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(APP_NAME)

# Normal object file compilation
$(OBJDIR)/%.o: %.c
	$(CC) $< $(CCFLAGS) $(INCLUDES) -c -o $@

## Debug section

# List of debug targets the final debug app depends on
OBJS_DEBUG=$(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/audiofile.o

# List of assembly file targets we want to create from the debug obj files (these contain readable asm code for debugging)
DISASM_OBJS_DEBUG=$(OBJDIR_DEBUG)/main.asm $(OBJDIR_DEBUG)/audiofile.asm

# Rule for the final debug executable, links all objs together
$(APP_NAME_DEBUG): dirs $(OBJS_DEBUG)
	$(CC) $(OBJS_DEBUG) $(LDFLAGS) -o $(APP_NAME_DEBUG)

# Debug object file compilation, note the _debug directory is used
$(OBJDIR_DEBUG)/%.o: %.c
	$(CC) $< $(CCFLAGS_DEBUG) $(INCLUDES) -c -o $@

$(OBJDIR_DEBUG)/%.asm: $(OBJDIR_DEBUG)/%.o
	objdump $< -drwC -Mintel -S > $@

# Create debug executable, (compile the obj files with debug flags on)
# then run objdump on these obj files to create readable assembly code
emit_debug: $(APP_NAME_DEBUG) $(DISASM_OBJS_DEBUG)
