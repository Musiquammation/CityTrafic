PORT ?= 3000

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
# Dossiers
# =========================
GAME_SRC_DIR   = game
GAME_BIN_DIR   = bin
SERVER_SRC_DIR = server
SERVER_BIN_DIR = bin/server

# uWebSockets
UWS_DIR = uWebSockets
UWS_INC = -I. -I$(UWS_DIR)/uSockets/src
UWS_LIB = $(UWS_DIR)/uSockets/uSockets.a
UWS_SYS_FLAGS = -lz -lssl -lcrypto -lpthread

# Destinations Emscripten
EMCC_FINAL_FOLDER_1 = client/wasm
EMCC_FINAL_FOLDER_2 = server/wasm

# =========================
# Détection des fichiers
# =========================

GAME_SRC := $(shell find $(GAME_SRC_DIR) -type f -name "*.cpp")

# --- GAME (test) ---
GAME_OBJ := $(patsubst $(GAME_SRC_DIR)/%.cpp,$(GAME_BIN_DIR)/%.o,$(GAME_SRC))
GAME_DEP := $(GAME_OBJ:.o=.d)

# --- GAME (pour serveur) ---
GAME_SERVER_OBJ := $(patsubst $(GAME_SRC_DIR)/%.cpp,$(SERVER_BIN_DIR)/game/%.o,$(GAME_SRC))
GAME_SERVER_DEP := $(GAME_SERVER_OBJ:.o=.d)

# --- SERVER ---
SERVER_SRC := $(shell find $(SERVER_SRC_DIR) -type f -name "*.cpp")
SERVER_OBJ := $(patsubst $(SERVER_SRC_DIR)/%.cpp,$(SERVER_BIN_DIR)/%.o,$(SERVER_SRC))
SERVER_DEP := $(SERVER_OBJ:.o=.d)

# =========================
# Cibles principales
# =========================

.PHONY: all clean server test emcc napi compile_lib

all: server test

# =========================
# SERVER
# =========================

server: $(SERVER_BIN_DIR)/server
	@./$(SERVER_BIN_DIR)/server $(PORT)

$(SERVER_BIN_DIR)/server: $(SERVER_OBJ) $(GAME_SERVER_OBJ)
	@mkdir -p $(SERVER_BIN_DIR)
	$(CXX) $(SERVER_OBJ) $(GAME_SERVER_OBJ) $(UWS_LIB) $(LDFLAGS_NATIVE) $(UWS_SYS_FLAGS) -o $@

# compile server sources
$(SERVER_BIN_DIR)/%.o: $(SERVER_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_NATIVE) $(UWS_INC) -I. -DCOMPILE_SERVER=1 -c $< -o $@

# compile game for server
$(SERVER_BIN_DIR)/game/%.o: $(GAME_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_NATIVE) -I. -DCOMPILE_SERVER=1 -c $< -o $@

# =========================
# TEST (game uniquement)
# =========================

test: $(GAME_BIN_DIR)/gametest
	@./$(GAME_BIN_DIR)/gametest

$(GAME_BIN_DIR)/gametest: $(GAME_OBJ)
	@mkdir -p $(GAME_BIN_DIR)
	$(CXX) $(GAME_OBJ) $(LDFLAGS_NATIVE) -o $@

$(GAME_BIN_DIR)/%.o: $(GAME_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_NATIVE) -DTESTING=1 -c $< -o $@

# =========================
# EMSCRIPTEN
# =========================

EMCC_FUNCS = Api_createApi Api_deleteApi Api_createSession Api_deleteSession Api_take Api_runFrames malloc free
EMCC_FUNCS_JSON = $(shell printf '"_%s",' $(EMCC_FUNCS) | sed 's/,$$//')
EMCC_OPTIMIZATION = -O3

EM_TARGET = $(GAME_BIN_DIR)/api.js
EM_OBJ    = $(patsubst $(GAME_SRC_DIR)/%.cpp,$(GAME_BIN_DIR)/emcc/%.o,$(GAME_SRC))
EM_DEP    = $(EM_OBJ:.o=.d)

EMFLAGS = -std=c++2b $(EMCC_OPTIMIZATION) \
    -sEXPORTED_FUNCTIONS='[$(EMCC_FUNCS_JSON)]' \
    -sEXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    -sMODULARIZE \
    -sEXPORT_ES6=1 \
    -sENVIRONMENT=worker,web \
    -sLLD_REPORT_UNDEFINED \
    -sALLOW_MEMORY_GROWTH=1

$(GAME_BIN_DIR)/emcc/%.o: $(GAME_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(EMXX) -std=c++2b -MMD -MP -c $(EMCC_OPTIMIZATION) -DTESTING=1 $< -o $@

emcc: $(EM_OBJ)
	@mkdir -p $(GAME_BIN_DIR) $(EMCC_FINAL_FOLDER_1) $(EMCC_FINAL_FOLDER_2)
	$(EMXX) $(EM_OBJ) $(EMFLAGS) -o $(EM_TARGET)
	cp $(EM_TARGET) $(EMCC_FINAL_FOLDER_1)/api.js
	cp $(GAME_BIN_DIR)/api.wasm $(EMCC_FINAL_FOLDER_1)/api.wasm
	cp $(EM_TARGET) $(EMCC_FINAL_FOLDER_2)/api.js
	cp $(GAME_BIN_DIR)/api.wasm $(EMCC_FINAL_FOLDER_2)/api.wasm

# =========================
# N-API
# =========================

NODE_ADDON_API_DIR := $(shell node -p "require('node-addon-api').include" 2>/dev/null || echo "")
NODE_INCLUDE_DIR   := $(shell node -p "require('node:path').join(process.execPath, '../../include/node')" 2>/dev/null || echo "")
OBJ_NAPI := $(patsubst $(GAME_SRC_DIR)/%.cpp,$(GAME_BIN_DIR)/napi/%.o,$(GAME_SRC))

$(GAME_BIN_DIR)/napi/%.o: $(GAME_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -fPIC -I$(NODE_ADDON_API_DIR) -I$(NODE_INCLUDE_DIR) -DPRODUCE_NAPI=1 -c $< -o $@

napi: $(OBJ_NAPI)
	@mkdir -p $(GAME_BIN_DIR)
	$(CXX) $(OBJ_NAPI) -shared -fPIC -o $(GAME_BIN_DIR)/addon.node $(LDFLAGS)

# =========================
# LIB DYNAMIQUE
# =========================

UNAME_S := $(shell uname -s)
LIB_EXT = so
ifeq ($(UNAME_S),Darwin)
    LIB_EXT = dylib
endif
ifeq ($(OS),Windows_NT)
    LIB_EXT = dll
endif

compile_lib:
	@mkdir -p $(GAME_BIN_DIR)
	$(CXX) -shared -fPIC $(GAME_SRC) $(CXXFLAGS_NATIVE) -o $(GAME_BIN_DIR)/api.$(LIB_EXT) $(LDFLAGS_NATIVE)

# =========================
# CLEAN
# =========================

clean:
	rm -rf $(GAME_BIN_DIR)

# =========================
# DEPENDANCES
# =========================

-include $(GAME_DEP)
-include $(GAME_SERVER_DEP)
-include $(SERVER_DEP)
-include $(EM_DEP)