CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE -Iinclude

TARGET = dupfinder

SOURCES = \
    src/main.c \
    src/scanner.c \
    src/hash_utils.c \
    src/grouper.c \
    src/ui.c

OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
