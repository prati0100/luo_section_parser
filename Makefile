CC := gcc
CFLAGS := -Wall -Wextra -O2
LDFLAGS = -lelf

TARGET := list_versions
HEADERS := luo_abi.h

$(TARGET): $(TARGET).c $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)
