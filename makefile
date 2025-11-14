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

${BINDIR}:
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): main.c $(OBJECTS) ${BINDIR}
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o $(BINDIR)/$(TARGET)

run: $(TARGET)
	./$(BINDIR)/$(TARGET)

clean:
	rm -r $(OBJDIR) $(BINDIR)