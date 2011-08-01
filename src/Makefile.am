bin_PROGRAMS = engine testGUI
engine_SOURCES = \
				 engine.c \
				 engine.h \
				 comm.c \
				 comm.h \
				 utilities.c \
				 utilities.h \
				 coordinate.c \
				 coordinate.h \
				 librobot.c \
				 librobot.h \
				 global.h \
				 actions.c \
				 actions.h \
				 robot.c \
				 robot.h

engine_LDFLAGS = -lm
engine_CFLAGS = -Wall

testGUI_SOURCES = testGUI.c \
				  global.h \
				  comm.c \
				  comm.h \
				  coordinate.h \
				  coordinate.c \
				  utilities.h \
				  utilities.c

testGUI_LDFLAGS = -lglut -lGLU -lm

