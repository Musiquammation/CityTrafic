# =========================
# Compilateurs
# =========================
CXX = g++
EMXX = em++

# Exported functions for emcc 
EMCC_FUNCS = Api_create Api_delete Api_frame Api_reserve
EMCC_FUNCS_JSON = $(shell printf '"_%s",' $(EMCC_FUNCS) | sed 's/,$$//')

# =========================
# Flags
# =========================
CXXFLAGS = -Wextra -Werror -g -std=c++20 -MMD -MP
EMFLAGS = -sEXPORTED_FUNCTIONS='[$(EMCC_FUNCS_JSON)]' \
          -sEXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
          -sMODULARIZE -sENVIRONMENT=web

# =========================
# Variables de test
# =========================
TESTING_MACRO ?= 1

ifeq ($(TESTING_MACRO),1)
	CXXFLAGS += -DTESTING=1
	EMFLAGS += -DTESTING=1
endif

# =========================
# Dossiers
# =========================
SRC_DIR = game
BIN_DIR = game-bin

# =========================
# Fichiers
# =========================
SRC := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

# =========================
# Exécutable
# =========================
TARGET = $(BIN_DIR)/gametest

# =========================
# Règle par défaut
# =========================
all: $(TARGET)

# Link final
$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJ) -o $@

# Compilation .cpp → .o
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# =========================
# TEST
# =========================
test: $(TARGET)
	@$(TARGET)

# =========================
# EMCC BUILD
# =========================
EM_TARGET = $(BIN_DIR)/api.js

emcc:
	@mkdir -p $(BIN_DIR)
	$(EMXX) $(SRC) $(EMFLAGS) -o $(EM_TARGET)

# =========================
# CLEAN
# =========================
clean:
	rm -rf $(BIN_DIR)

# Inclure dépendances auto
-include $(DEP)