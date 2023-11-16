# Путь к компилятору
COMPILER = g++

# Флаги компиляции
FLAGS = \
 -Wno-unused-parameter -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal 						\
 -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-default -Weffc++ -Wmain 			\
 -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security 		\
 -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual 		\
 -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings

# Папка с объектами
BUILD_DIR = ./build

# Папка с исходниками и заголовками
SRC_DIR = ./source

# Имя исполняемого файла
BIN = run.exe

# Пути для директивы #include
INC_FLAGS = -I$(SRC_DIR)

# Флаги для директивы #define
D_FLAGS = -D_DEBUG -D_EJUDGE_CLIENT_

# Все исходники
CPP = $(shell find $(SRC_DIR) -type f -name "*.cpp")

# Все объекты
OBJ = $(CPP:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Файлы зависимостей
DEP = $(OBJ:%.o=%.d)

# Цель по умолчанию
all : run

# Собирает и запускает
# '@' отключает вывод самой команды перед исполнением
.PHONY : run
run : build
	@./$(BIN)

# Собирает и запускает под Valgrind (предварительно лучше собрать с оптимизацией -O0 или -O1)
.PHONY : memcheck
memcheck : build
	@valgrind --leak-check=yes --log-file="log.txt" ./$(BIN)

# Только собирает
.PHONY : build
build : $(BIN)

# Завершает сборку
$(BIN) : $(OBJ)
	@mkdir -p $(@D)
	@$(COMPILER) $^ -o $(BIN) -lsfml-graphics -lsfml-window -lsfml-system

# Подключает зависимости
# '-include' подключает файл, только если он существует
-include $(DEP)

# Собирает каждый объект в папке build
# mkdir копирует структуру source в build
# флаг -MMD генерирует файл с зависимостями
$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(COMPILER) $(FLAGS) $(INC_FLAGS) $(D_FLAGS) -MMD -c $< -o $@

# Удаляет результаты компиляции
.PHONY : clean
clean :
	@rm -rf $(BIN) $(BUILD_DIR)
