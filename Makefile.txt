CC = g++
LDFLAGS = `pkg-config --libs ibsimu-1.0.6`
CXXFLAGS = -Wall -g `pkg-config --cflags ibsimu-1.0.6`

einzel: einzel.o
	$(CC) -o einzel einzel.o $(LDFLAGS)
einzel.o: einzel.cpp

clean:
	$(RM) *.o einzel 

