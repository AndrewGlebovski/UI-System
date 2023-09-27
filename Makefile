# Путь к компилятору
COMPILER=g++

# Флаги компиляции
FLAGS=-Wno-unused-parameter -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_

# Папка с объектами
BIN_DIR=binary

# Папка с исходниками и заголовками
SRC_DIR=source

# Имя исполняемого файла
EXE_NAME=run.exe


all: $(BIN_DIR) $(EXE_NAME)


# Завершение сборки
$(EXE_NAME): $(addprefix $(BIN_DIR)/, main.o) 
	$(COMPILER) $^ -o $(EXE_NAME)


# Предварительная сборка main.cpp
$(BIN_DIR)/main.o: $(addprefix $(SRC_DIR)/, main.cpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Создание папки для объектников, если она еще не существует
$(BIN_DIR):
	mkdir $@
