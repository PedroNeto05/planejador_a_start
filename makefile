# Variáveis
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = planejador
SRCS = planejador.cpp planejador-main.cpp
HEADERS = planejador.h

# Regras
all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
