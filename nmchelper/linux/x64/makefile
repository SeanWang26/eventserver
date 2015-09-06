all : libnmchelper.so test.out

libnmchelper.so:../../src/CachedAffair.cpp ../../src/NMCDevice.cpp ../../src/NMCDeviceImpl.cpp ../../src/NmcMessage.cpp ../../src/NMCUser.cpp \
	../../src/JtDevcieSearch.cpp ../../src/jtprintf.cpp ../../src/JtEventConnect.cpp ../../src/JtEventServer.cpp ../../src/JtEventTimer.cpp \
	../../src/JtStreamSender.cpp ../../src/PushStreamPeer.cpp
	g++ -g -fPIC -Wl,-rpath,./ -Wall -shared -o2 -o libnmchelper.so -L../lib/x64  -I../../include -I../innerinclude  ../../src/NMCDevice.cpp \
	../../src/NMCDeviceImpl.cpp ../../src/NmcMessage.cpp ../../src/NMCUser.cpp ../../src/JtDevcieSearch.cpp ../../src/jtprintf.cpp \
	../../src/JtEventConnect.cpp ../../src/CachedAffair.cpp ../../src/JtEventServer.cpp ../../src/JtEventTimer.cpp ../../src/JtStreamSender.cpp  ../../src/PushStreamPeer.cpp \
	 -lpthread -ldl -lrt -ltinyxml -levent_core -levent_extra -levent_pthreads -ljthread
	chmod +x *.so
	cp ../../include/*.h ./

test.out:Test.cpp
	g++ -g -Wall -o test.out Test.cpp ./libnmchelper.so -ldl -lpthread -lrt

gtest.out:gtest.cpp
	g++ `pkg-config --cflags gtk+-2.0` -Wall -o gtest.out gtest.cpp `pkg-config --libs gtk+-2.0` -lpthread -lm -lX11

release:
	mkdir nmclinuxx64
	cp *.so ./nmclinuxx64
	cp ../../include/NMCUser.h ./nmclinuxx64/
	cp ../../include/nmctype.h ./nmclinuxx64/
	cp ../../include/jntype.h ./nmclinuxx64/
	cp Test.cpp ./nmclinuxx64
	cp *.out ./nmclinuxx64
	cp makefile.test ./nmclinuxx64/makefile
	tar zcf nmclinuxx64.tar.gz ./nmclinuxx64
	rm -r ./nmclinuxx64

.PHONY : all

clean:
	rm -f libnmchelper.so