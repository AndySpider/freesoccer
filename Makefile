all: engine testGUI
engine: engine.c engine.h comm.c comm.h utilities.c utilities.h coordinate.c coordinate.h librobot.c librobot.h global.h actions.h actions.c robot.c robot.h
	gcc engine.c comm.c utilities.c coordinate.c actions.c librobot.c robot.c  -lm -o engine -g -Wall

testGUI: testGUI.c comm.c comm.h
	gcc testGUI.c comm.c -lglut -lGLU -o testGUI -g
