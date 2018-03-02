#CC  = ccache gcc
#CXX = ccache g++


CC  = ccache gcc
CXX = ccache g++
LD  = g++

# debug build
DEBUG = YES
#NOWARN		= -c -O0 -pthread 
DEBUG_CFLAGS    =  -fPIC -Wno-unknown-pragmas -Wno-format -Wall -g -pg -DDEBUG -m64 -Wno-deprecated
RELEASE_CFLAGS  =  -fPIC -Wno-unknown-pragmas -Wno-format -O3 -m64 -Wno-deprecated

DEBUG_CXXFLAGS   = ${DEBUG_CFLAGS} 
RELEASE_CXXFLAGS = ${RELEASE_CFLAGS}

DEBUG_LDFLAGS   = -g
RELEASE_LDFLAGS =

ifeq (YES, ${DEBUG})
	CFLAGS      = ${DEBUG_CFLAGS}
	CXXFLAGS    = ${DEBUG_CXXFLAGS}
	LDFLAGS     = ${DEBUG_LDFLAGS}
else
	CFLAGS      = ${RELEASE_CFLAGS}
	CXXFLAGS    = ${RELEASE_CXXFLAGS}
	LDFLAGS     = ${RELEASE_LDFLAGS}
endif

INCLUDE_DIRS = \
	-I${IG_HOME}/opt/include \
	-I/usr/local/include \
	-I./include \
	-I./include/env \
	-I./dependencies/libxml2-2.7.2/include \

LIBRARY_DIRS = \
	-L${IG_HOME}/opt/lib \
 	-L${IG_HOME}/opt/lib64 \
	-L/usr/lib64 \
	-L/lib64 \
	-L/usr/local/lib \
	-L/usr/lib \
	-L/usr/lib64/qt4 \
	-L./dependencies/libxml2-2.7.2/.libs \

IG_LIBRARY_DIRS = \
	-L./lib

LIBXML2_LIBS = \
	-lxml2 \


OSG_LIBS = \
	-lGL \
	-ltiff \
	-lOpenThreads \
	-losgViewer \
	-losgSim \
	-losgText \
	-losgGA \
	-losgDB \
	-losgFX \
	-losgUtil \
	-losgManipulator \
	-losg \
	-lGLU \
	-lm \
	-lpthread \
	-losgTerrain \
	-losgParticle \

QT_LIBS = \
	-lQtCore
	



ifeq (YES, ${DEBUG})
	TEST_OUTPUT = bin/lightingd
	
	

	
else
	TEST_OUTPUT = bin/lighting
endif

all:  ${TEST_OUTPUT}





TEST_SOURCES = \
		main.cpp \



TEST_OBJECTS = \
	$(addsuffix .o,$(basename ${TEST_SOURCES} )) \





${TEST_OUTPUT}: ${TEST_OBJECTS}
	${LD} -o $@ ${LDFLAGS} ${TEST_OBJECTS}     ${IG_LIBRARY_DIRS}  ${LIBRARY_DIRS} ${IG_LIBS} ${OSG_LIBS} ${LIBXML2_LIBS} 
%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCLUDE_DIRS} $< -o $@

%.o : %.c
	${CC} -c ${CFLAGS} ${INCLUDE_DIRS} $< -o $@





clean:
	-rm -f core ${OBJECTS}  ${TEST_OBJECTS} 
