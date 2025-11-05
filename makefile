CC=clang
CFLAGS=-O3 -Wall -framework CoreFoundation
SRCDIR=functions
OBJDIR=obj
BINDIR=bin
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))
TARGET=main

.PHONY: all clean run

all: $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): main.c $(OBJECTS)
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o $(BINDIR)/$(TARGET)

run: $(TARGET)
	./$(BINDIR)/$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)