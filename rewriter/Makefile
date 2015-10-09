#-------------------------------------------------------------------------------
# Park Seong U (qkrtjddn938@naver.com)
#-------------------------------------------------------------------------------

LLVM_SRC_PATH := llvm/llvm
LLVM_BUILD_PATH := llvm/build
LLVM_BIN_PATH := $(LLVM_BUILD_PATH)/Release+Asserts/bin

# Run make BUILD_NINJA=1 to enable these paths
#ifdef BUILD_NINJA
#	LLVM_BUILD_PATH := $$your-ninja-build
#	LLVM_BIN_PATH 	:= $(LLVM_BUILD_PATH)/bin
#endif

$(info -----------------------------------------------)
$(info Using LLVM_SRC_PATH = $(LLVM_SRC_PATH))
$(info Using LLVM_BUILD_PATH = $(LLVM_BUILD_PATH))
$(info -----------------------------------------------)

CXX := g++
CXXFLAGS := -fno-rtti -O0 -g 

LLVM_CXXFLAGS := `$(LLVM_BIN_PATH)/llvm-config --cxxflags`
LLVM_LDFLAGS := `$(LLVM_BIN_PATH)/llvm-config --ldflags --libs --system-libs`

CLANG_INCLUDES := \
	-I$(LLVM_SRC_PATH)/tools/clang/include \
	-I$(LLVM_BUILD_PATH)/tools/clang/include \
	-I$(LLVM_SRC_PATH)/include \
	-I$(LLVM_BUILD_PATH)/include \
	-Iinc

# List of Clang libraries to link. The proper -L will be provided by the
# call to llvm-config
# Note that I'm using -Wl,--{start|end}-group around the Clang libs; this is
# because there are circular dependencies that make the correct order difficult
# to specify and maintain. The linker group options make the linking somewhat
# slower, but IMHO they're still perfectly fine for tools that link with Clang.
CLANG_LIBS := \
	-Wl,--start-group \
	-lclangAST \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangASTMatchers \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangTooling \
	-Wl,--end-group 

# Internal paths in this project: where to find sources, and where to put
# build artifacts.
SRC_DIR := src
LIB_DIR := inc

MAINOBJ=$(SRC_DIR)/Main.o

SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(filter-out $(MAINOBJ), $(SOURCES:%.cpp=%.o))
	
.PHONY: all
all: make_build autoCodeRewriter

.PHONY: make_build
make_build:
$(OBJS) : $(SRC_DIR)/%.o : $(SRC_DIR)/%.cpp  $(LIB_DIR)/%.h
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $< -c -o $@

$(SRC_DIR)/Main.o : $(SRC_DIR)/Main.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $< -c -o $@

autoCodeRewriter : $(OBJS) $(SRC_DIR)/Main.o
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -lm -o $@

.PHONY: clean
clean:
	rm -f $(SRC_DIR)/*.o
	rm -f autoCodeRewriter
