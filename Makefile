CC=g++

CFLAGS=-std=c++20

TARGET=yoursql

SOURCES=$(wildcard src/core/*.cpp src/parser/*.c src/parser/*.cpp)

OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS:=$(OBJECTS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# 生成对象文件的规则
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理编译产生的文件
clean:
	rm -f $(OBJECTS) $(TARGET)

# 声明伪目标
.PHONY: all clean
