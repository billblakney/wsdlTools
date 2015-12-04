######################################################################
# Automatically generated by qmake (2.00a) Thu Sep 7 15:54:07 2006
######################################################################
 
#TEMPLATE = app
TEMPLATE = lib
TARGET = IecHeaderParser
DESTDIR       = $(LIB_DIR)
QMAKE_CLEAN   = (LIB_DIR)/libIecHeaderParser*
QMAKE_CLEAN  += debug/* release/*
QMAKE_CLEAN  += $(CLIRCAR_H)
CONFIG       += debug_and_release
DEPENDPATH += .

INCLUDES  = $(BOOST_INC)

QMAKE_CXX = g++
QMAKE_CC = g++

QMAKE_POST_LINK = copyClirCarH.sh

LIBS = -L$(BOOST_DIR)/lib -lboost_regex
 
# Input
HEADERS += Field.hh StructorBuilder.hh Structure.hh Enum.hh
SOURCES += main.cc Field.cc StructorBuilder.cc Structure.cc Enum.cc

# Flex/bison specifics
 
#QMAKE_LEX = flex
#QMAKE_YACC = bison
 
#QMAKE_YACCFLAGS = -d -o y.tab.c
#QMAKE_YACC_HEADER =
#QMAKE_YACC_SOURCE =
 
#LEXSOURCES += parser.l
#YACCSOURCES += parser.y
FLEXSOURCES = parser.l
BISONSOURCES = parser.y
 
flex.commands = flex ${QMAKE_FILE_IN}
flex.input = FLEXSOURCES
flex.output = lex.yy.c
flex.variable_out = SOURCES
flex.depends = y.tab.h
flex.name = flex
QMAKE_EXTRA_COMPILERS += flex
 
bison.commands = bison -d -t -y ${QMAKE_FILE_IN} && mv y.tab.c y.tab.cpp
bison.input = BISONSOURCES
bison.output = y.tab.cpp
bison.variable_out = SOURCES
bison.name = bison
QMAKE_EXTRA_COMPILERS += bison
 
bisonheader.commands = @true
bisonheader.input = BISONSOURCES
bisonheader.output = y.tab.h
bisonheader.variable_out = HEADERS
bisonheader.name = bison header
bisonheader.depends = y.tab.cpp
QMAKE_EXTRA_COMPILERS += bisonheader
