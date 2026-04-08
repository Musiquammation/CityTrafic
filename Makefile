# =========================
# Compilateurs
# =========================
CXX  = g++
EMXX = em++

# =========================
# Flags de base
# =========================
CXXFLAGS = -Wextra -Werror -Wconversion -Wswitch -g -std=c++20 -MMD -MP
LDFLAGS  =

# =========================
# SANITIZER
# =========================
SANITIZE ?= 1

ifeq ($(SANITIZE),1)
	SAN_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer
	CXXFLAGS += $(SAN_FLAGS)
	LDFLAGS  += $(SAN_FLAGS)
endif

# =========================
# Exported functions (Emscripten)
# =========================
EMCC_FUNCS = Api_create Api_delete Api_frame Api_take
EMCC_FUNCS_JSON = $(shell printf '"_%s",' $(EMCC_FUNCS) | sed 's/,$$//')

# =========================
# Flags Emscripten
# =========================
EMFLAGS = -std=c++20 -O3 \
          -sEXPORTED_FUNCTIONS='[$(EMCC_FUNCS_JSON)]' \
          -sEXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
          -sMODULARIZE \
          -sENVIRONMENT=web \
          -sALLOW_MEMORY_GROWTH=1

# =========================
# Variables de test
# =========================
TESTING_MACRO ?= 1

ifeq ($(TESTING_MACRO),1)
	CXXFLAGS += -DTESTING=1
	EMFLAGS  += -DTESTING=1
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
# Exécutables
# =========================
TARGET    = $(BIN_DIR)/gametest
EM_TARGET = $(BIN_DIR)/api.js

# =========================
# Règle par défaut
# =========================
all: $(TARGET)

# =========================
# Build natif
# =========================
$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# =========================
# Test
# =========================
test: $(TARGET)
	@$(TARGET)

# =========================
# Build Emscripten
# =========================
emcc:
	@mkdir -p $(BIN_DIR)
	$(EMXX) $(SRC) $(EMFLAGS) -o $(EM_TARGET)

# =========================
# Debug
# =========================
gdb: $(TARGET)
	gdb --args $(TARGET) $(ARGS)

# =========================
# Clean
# =========================
clean:
	rm -rf $(BIN_DIR)

# =========================
# Dépendances auto
# =========================
-include $(DEP)