LIB_CODE_BASE=../openGLHelper-starterCode

TSMV_CXX_SRC=main.cpp
TSMV_HEADER=
TSMV_OBJ=$(notdir $(patsubst %.cpp,%.o,$(TSMV_CXX_SRC)))

BMP_CODE_SRC=Bmp.cpp
BMP_HEADER=Bmp.h
BMP_OBJ=$(notdir $(patsubst %.cpp,%.o,$(BMP_CODE_SRC)))

UTIL_CODE_SRC=Util.cpp
UTIL_HEADER=Util.h
UTIL_OBJ=$(notdir $(patsubst %.cpp,%.o,$(UTIL_CODE_SRC)))

HTTPLIB_HEADER=httplib.h


LIB_CODE_CXX_SRC=$(wildcard $(LIB_CODE_BASE)/*.cpp)
LIB_CODE_HEADER=$(wildcard $(LIB_CODE_BASE)/*.h)
LIB_CODE_OBJ=$(notdir $(patsubst %.cpp,%.o,$(LIB_CODE_CXX_SRC)))

IMAGE_LIB_SRC=$(wildcard ../external/imageIO/*.cpp)
IMAGE_LIB_HEADER=$(wildcard ../external/imageIO/*.h)
IMAGE_LIB_OBJ=$(notdir $(patsubst %.cpp,%.o,$(IMAGE_LIB_SRC)))

HEADER=$(TSMV_HEADER) ${BMP_HEADER} ${UTIL_HEADER} ${HTTPLIB_HEADER} $(LIB_CODE_HEADER) $(IMAGE_LIB_HEADER)
CXX_OBJ=$(TSMV_OBJ) ${BMP_OBJ} ${UTIL_OBJ} $(LIB_CODE_OBJ) $(IMAGE_LIB_OBJ)

CXX=g++
TARGET=main
CXXFLAGS=-DGLM_FORCE_RADIANS -std=c++11
OPT=-O3

UNAME_S=$(shell uname -s)

ifeq ($(UNAME_S),Linux)
  PLATFORM=Linux
  INCLUDE=-I../external/glm/ -I$(LIB_CODE_BASE) -I../external/imageIO
  LIB=-lGLEW -lGL -lglut -ljpeg
  LDFLAGS=
else
  PLATFORM=Mac OS
  INCLUDE=-I../external/glm/ -I$(LIB_CODE_BASE) -I../external/imageIO -I../external/jpeg-9a-mac/include -I../external/openssl@1.1/include 
  LIB=-framework OpenGL -framework GLUT ../external/jpeg-9a-mac/lib/libjpeg.a ../external/openssl@1.1/lib/libssl.a ../external/openssl@1.1/lib/libcrypto.a -lstdc++
  CXXFLAGS+= -Wno-deprecated-declarations
  LDFLAGS=-Wl,-w
endif

all: $(TARGET)

$(TARGET): $(CXX_OBJ)
	$(CXX) $(LDFLAGS) $^ $(OPT) $(LIB) -o $@

$(TSMV_OBJ):%.o: %.cpp $(HEADER)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

$(BMP_OBJ):%.o: %.cpp $(BMP)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

$(UTIL_OBJ):%.o: %.cpp $(UTIL)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

$(LIB_CODE_OBJ):%.o: $(LIB_CODE_BASE)/%.cpp $(LIB_CODE_HEADER)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

$(IMAGE_LIB_OBJ):%.o: ../external/imageIO/%.cpp $(IMAGE_LIB_HEADER)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

clean:
	rm -rf *.o $(TARGET)
