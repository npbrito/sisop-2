TARGET = myClient
CC     = gcc
CFLAGS = -D_POSIX_C_SOURCE=200809L -Wall -I.

LINKER = gcc
LFLAGS = -Wall -I.

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm        = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(rm) $(BINDIR)/$(TARGET) $(OBJECTS)