

SOURCE_FILES = config.h batt_notifier.c
OUTPUT_FILE = batt_notifier
CC = gcc
CFLAGS = -o 

EXEC_DIR = /usr/local/bin/

all: batt_notifier

batt_notifier: $(SOURCE_FILES)
	$(CC) $(CFLAGS) $(OUTPUT_FILE) $(SOURCE_FILES)

install: all 
	cp $(OUTPUT_FILE) $(EXEC_DIR)
clean:
	rm -f $(OUTPUT_FILE)
