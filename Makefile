CC=cc
CCFLAGS=-Wall -Wextra -std=c11
LDFLAGS=-lm
APP_NAME = dsynth
OBJDIR=objs
INCLUDES = -I.

.PHONY: dirs clean

default: $(APP_NAME)

dirs:
	mkdir -p $(OBJDIR)/

clean:
	rm -rf $(OBJDIR)

# List of targets final app depends on
OBJS=$(OBJDIR)/main.o $(OBJDIR)/audiofile.o

# Rule for the final executable, links all objs together
$(APP_NAME): dirs $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(APP_NAME)

$(OBJDIR)/%.o: %.c
	$(CC) $< $(CCFLAGS) $(INCLUDES) -c -o $@
