all: player test

purge: clean
	-rm libegg.so player

clean:
	-rm egg-player-marshallers.[ch] *.o

player: player.o libegg.so
	gcc -g -rdynamic -o player `pkg-config --libs gtk+-2.0 gstreamer-0.10 gstreamer-interfaces-0.10` -L. $^

player.o: player.c egg-player.h
	gcc -g -c `pkg-config --cflags gtk+-2.0 gstreamer-0.10 gstreamer-interfaces-0.10` -I. $<

test: test.o libegg.so
	gcc -g -rdynamic -o test `pkg-config --libs gtk+-2.0 gstreamer-0.10 gstreamer-interfaces-0.10` -L. $^

test.o: test.c egg-player.h
	gcc -g -c `pkg-config --cflags gtk+-2.0 gstreamer-0.10 gstreamer-interfaces-0.10` -I. $<

libegg.so: egg-player.o egg-player-marshallers.o
	gcc -g -shared -o libegg.so `pkg-config --libs gtk+-2.0 gstreamer-0.10 gstreamer-interfaces-0.10` $^

egg-player.o: egg-player.c egg-player.h egg-player-marshallers.h
	gcc -fpic -g -c `pkg-config --cflags gtk+-2.0 gstreamer-0.10 gstreamer-interfaces-0.10` -I. $<

egg-player-marshallers.o: egg-player-marshallers.c
	gcc -fpic -g -c `pkg-config --cflags gtk+-2.0 gstreamer-0.10 gstreamer-interfaces-0.10` $<

egg-player-marshallers.c: egg-player-marshallers.list
	glib-genmarshal --prefix _egg_player_marshal --body $< > $@

egg-player-marshallers.h: egg-player-marshallers.list
	glib-genmarshal --prefix _egg_player_marshal --header $< > $@
