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
	CXXFLAGS_NATIVE = $(CXXFLAGS) $(SAN_FLAGS)
	LDFLAGS_NATIVE  = $(LDFLAGS) $(SAN_FLAGS)
else
	CXXFLAGS_NATIVE = $(CXXFLAGS)
	LDFLAGS_NATIVE  = $(LDFLAGS)
endif

# =========================
# Variables de test
# =========================
TESTING_MACRO ?= 1

ifeq ($(TESTING_MACRO),1)
	CXXFLAGS_NATIVE += -DTESTING=1
endif

# =========================
# Dossiers
# =========================
SRC_DIR = game
BIN_DIR = game-bin

# =========================
# Emscripten output final
# =========================
EMCC_FINAL_FOLDER = client/api

# =========================
# Fichiers
# =========================
SRC := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

# =========================
# Plateforme / Extension bibliothèque
# =========================
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	LIB_EXT = so
endif
ifeq ($(UNAME_S),Darwin)
	LIB_EXT = dylib
endif
ifeq ($(OS),Windows_NT)
	LIB_EXT = dll
endif

LIB_TARGET = $(BIN_DIR)/api.$(LIB_EXT)

# =========================
# Exécutables / lib
# =========================
TARGET     = $(BIN_DIR)/gametest
EM_TARGET  = $(BIN_DIR)/api.js
NAPI_TARGET = $(BIN_DIR)/addon.node

# =========================
# Node / N-API includes
# =========================
NODE_ADDON_API_DIR := $(shell node -p "require('node-addon-api').include")
NODE_INCLUDE_DIR := $(shell node -p "require('node:path').join(process.execPath, '../../include/node')")

# =========================
# Règle par défaut
# =========================
all: $(TARGET)

# =========================
# Compilation des .o natifs
# =========================
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_NATIVE) -c $< -o $@

# =========================
# Compilation des .o N-API
# =========================
OBJ_NAPI := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/napi/%.o,$(SRC))

$(BIN_DIR)/napi/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -fPIC \
	      -I$(NODE_ADDON_API_DIR) \
	      -I$(NODE_INCLUDE_DIR) \
	      -DPRODUCE_NAPI=1 \
	      -c $< -o $@

# =========================
# Build natif
# =========================
$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJ) $(LDFLAGS_NATIVE) -o $@

# =========================
# Build lib dynamique
# =========================
compile_lib: $(SRC)
	@mkdir -p $(BIN_DIR)
ifeq ($(LIB_EXT),dll)
	$(CXX) -shared -o $(LIB_TARGET) $(SRC) $(CXXFLAGS_NATIVE) $(LDFLAGS_NATIVE)
else
	$(CXX) -shared -fPIC $(SRC) $(CXXFLAGS_NATIVE) -o $(LIB_TARGET) $(LDFLAGS_NATIVE)
endif

# =========================
# Build Emscripten (temp)
# =========================
EMCC_FUNCS = Api_createApi Api_deleteApi Api_createSession Api_deleteSession Api_take malloc free

EMCC_FUNCS_JSON = $(shell printf '"_%s",' $(EMCC_FUNCS) | sed 's/,$$//')

EMFLAGS = -std=c++20 -O3 \
          -sEXPORTED_FUNCTIONS='[$(EMCC_FUNCS_JSON)]' \
          -sEXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
          -sMODULARIZE \
		  -sEXPORT_ES6=1
          -sENVIRONMENT=web \
          -sALLOW_MEMORY_GROWTH=1

emccTmp:
	@mkdir -p $(BIN_DIR)
	$(EMXX) $(SRC) $(EMFLAGS) -o $(EM_TARGET)

# =========================
# Build final Emscripten + copy
# =========================
emcc: emccTmp
	@mkdir -p $(EMCC_FINAL_FOLDER)
	cp $(EM_TARGET) $(EMCC_FINAL_FOLDER)/api.js
	cp $(BIN_DIR)/api.wasm $(EMCC_FINAL_FOLDER)/api.wasm

# =========================
# Build N-API
# =========================
napi: $(OBJ_NAPI)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJ_NAPI) -shared -fPIC -o $(NAPI_TARGET) $(LDFLAGS)

# =========================
# Test
# =========================
test: $(TARGET)
	@$(TARGET)

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