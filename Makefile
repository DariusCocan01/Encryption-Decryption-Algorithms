TARGET = main

RESULTSDIRECTORY = obj/results

CC = gcc

CFLAGS = -Wall -Iinclude

SRCDIR = src
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.c)

OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

$(shell mkdir -p $(OBJDIR) $(RESULTSDIRECTORY))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@$(CC) $(OBJECTS) -o $(RESULTSDIRECTORY)/$(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -f $(OBJDIR)/*.o $(RESULTSDIRECTORY)/$(TARGET)

run: $(TARGET)
	@echo "Running $(TARGET)..."
	@./$(RESULTSDIRECTORY)/$(TARGET)
