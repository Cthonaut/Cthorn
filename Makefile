CXX = g++
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lSDL2 -lvulkan -ldl -lpthread -lwayland-client -lXxf86vm -lXrandr -lXi

TARGET = Vulkan
SOURCES = $(wildcard src/*.cpp src/imgui/*.cpp)
HEADERS = $(wildcard include/*.hpp include/imgui/*.h)
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all test clean

all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) main.cpp $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)
