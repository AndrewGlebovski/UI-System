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
$(EXE_NAME): $(addprefix $(BIN_DIR)/, main.o vector.o ui-system.o ui-base.o canvas.o button.o scrollbar.o) 
	$(COMPILER) $^ -o $(EXE_NAME) -lsfml-graphics -lsfml-window -lsfml-system


# Предварительная сборка main.cpp
$(BIN_DIR)/main.o: $(addprefix $(SRC_DIR)/, main.cpp configs.hpp ui-system.hpp vector.hpp list.hpp style.hpp ui-base.hpp button.hpp scrollbar.hpp canvas.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка vector.cpp
$(BIN_DIR)/vector.o: $(addprefix $(SRC_DIR)/, vector.cpp vector.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка ui-system.cpp
$(BIN_DIR)/ui-system.o: $(addprefix $(SRC_DIR)/, ui-system.cpp ui-system.hpp vector.hpp list.hpp style.hpp key-id.hpp ui-base.hpp button.hpp configs.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка ui-base.cpp
$(BIN_DIR)/ui-base.o: $(addprefix $(SRC_DIR)/, ui-base.cpp ui-base.hpp vector.hpp list.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка canvas.cpp
$(BIN_DIR)/canvas.o: $(addprefix $(SRC_DIR)/, canvas.cpp canvas.hpp vector.hpp list.hpp ui-base.hpp key-id.hpp configs.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка button.cpp
$(BIN_DIR)/button.o: $(addprefix $(SRC_DIR)/, button.cpp button.hpp vector.hpp list.hpp ui-base.hpp style.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка scrollbar.cpp
$(BIN_DIR)/scrollbar.o: $(addprefix $(SRC_DIR)/, scrollbar.cpp scrollbar.hpp vector.hpp list.hpp ui-base.hpp style.hpp)
	$(COMPILER) $(FLAGS) -c $< -o $@



# Создание папки для объектников, если она еще не существует
$(BIN_DIR):
	mkdir $@
