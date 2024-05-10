# Путь к компилятору
COMPILER = g++

# Флаги компиляции
FLAGS = -Og \
 -Wno-unused-parameter -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal 						\
 -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-default -Weffc++ -Wmain 						\
 -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wctor-dtor-privacy -Wempty-body -Wformat-security 					\
 -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual 		\
 -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings \

# Папка с объектами
BUILD_DIR = ./build

# Папка с исходниками и заголовками
SRC_DIR = ./source

# Название папки с исходниками плагинов
PLUGIN_SRC_DIR = plugins

# Название папки, где лежат скомпилированные плагины
PLUGIN_BIN_DIR = Plugins

# Директория для логов
LOG_DIR = ./log

# Имя исполняемого файла
BIN = ./run.out

# Пути для директивы #include
INC_FLAGS = -I$(SRC_DIR)

# Флаги для директивы #define
D_FLAGS = -D_DEBUG -D_EJUDGE_CLIENT_

# Все исходники плагинов
DLL_CPP = $(shell find $(SRC_DIR)/$(PLUGIN_SRC_DIR) -type f -name "*.cpp")

# Все исходники кроме плагинов
CPP := $(shell find $(SRC_DIR) -type f -name "*.cpp")
CPP := $(filter-out $(DLL_CPP), $(CPP))

# Все объекты кроме плагинов
OBJ = $(CPP:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Объекты плагинов
DLL_OBJ = $(DLL_CPP:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Файлы динамических библиотек плагинов
DLL_SO = $(DLL_CPP:$(SRC_DIR)/$(PLUGIN_SRC_DIR)/%.cpp=$(PLUGIN_BIN_DIR)/%.so)

# Файлы зависимостей
DEP = $(OBJ:%.o=%.d)

# Цель по умолчанию
# Собирает и запускает
# '@' отключает вывод самой команды перед исполнением
all : build
	@./$(BIN)

# Собирает и запускает под Valgrind (предварительно лучше собрать с оптимизацией -O0 или -O1)
.PHONY : memcheck
memcheck : build
	@valgrind --leak-check=yes --log-file="$(LOG_DIR)/memcheck-log.txt" ./$(BIN)

# Только собирает
.PHONY : build
build : $(BIN)

# Завершает сборку
# Также создает папку для логов
# Также собирает объекты подключаемых плагины
# Указывать файлы нужно в порядке зависимостей друг от друга
$(BIN) : $(OBJ) $(DLL_OBJ)
	@mkdir -p $(LOG_DIR)
	@mkdir -p $(@D)
	@$(COMPILER) $(OBJ) -o $(BIN) -lsfml-graphics -lsfml-window -lsfml-system

# Cобирает все подключаемые плагины
.PHONY : plugins
plugins : $(DLL_SO)

# Подключает зависимости
# '-include' подключает файл, только если он существует
-include $(DEP)

# Собирает каждый объект в папке build
# mkdir копирует структуру source в build
# флаг -MMD генерирует файл с зависимостями
$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(COMPILER) $(FLAGS) $(INC_FLAGS) $(D_FLAGS) -MMD -fPIC -c $< -o $@

# Собирает каждый плагин в динамическую библиотеку
# Указывать файлы нужно в порядке зависимостей друг от друга
$(PLUGIN_BIN_DIR)/%.so : $(BUILD_DIR)/$(PLUGIN_SRC_DIR)/%.o $(OBJ)
	@mkdir -p $(@D)
	@$(COMPILER) -shared -o $@ $^

# Удаляет результаты компиляции
.PHONY : clean
clean :
	@rm -rf $(BIN) $(BUILD_DIR)
