all : libnmchelper.so test.out Event.out

libnmchelper.so:../../src/CachedAffair.cpp ../../src/NMCDevice.cpp ../../src/NMCDeviceImpl.cpp ../../src/NmcMessage.cpp ../../src/NMCUser.cpp \
	../../src/JtDevcieSearch.cpp ../../src/jtprintf.cpp ../../src/JtEventConnect.cpp ../../src/JtEventServer.cpp ../../src/JtEventTimer.cpp ../../src/JtEventPairPipe.cpp \
	../../src/JtStreamSender.cpp ../../src/PushStreamPeer.cpp ../../src/jtmutex.cpp 
	g++ -std=c++11 -g -fPIC -Wl,-rpath,./ -Wl,-Bsymbolic -fvisibility=hidden -Wall -shared -o2 -o libnmchelper.so -L../lib/x64  -I../../include -I../innerinclude  ../../src/NMCDevice.cpp \
	../../src/NMCDeviceImpl.cpp ../../src/NmcMessage.cpp ../../src/NMCUser.cpp ../../src/JtDevcieSearch.cpp ../../src/jtprintf.cpp \
	../../src/JtEventConnect.cpp ../../src/CachedAffair.cpp ../../src/JtEventServer.cpp ../../src/JtEventTimer.cpp ../../src/JtEventPairPipe.cpp ../../src/JtStreamSender.cpp \
	../../src/PushStreamPeer.cpp ../../src/jtmutex.cpp  \
	 -lpthread -ldl -lrt -ltinyxml -levent_core -levent_extra -levent_pthreads -ljthread
	chmod +x *.so
	cp ../../include/*.h ./

test.out:Test.cpp
	g++ -g -Wall -o test.out Test.cpp ./libnmchelper.so -ldl -lpthread -lrt

Event.out: TestEvent.cpp ../../src/jtprintf.cpp ../../src/JtEventConnect.cpp ../../src/JtEventServer.cpp ../../src/JtEventTimer.cpp \
	../../src/JtEventPairPipe.cpp ../../src/jtmutex.cpp  ../../src/CachedAffair.cpp
	g++ -std=c++11 -g3 -Wall -o Event.out TestEvent.cpp ../../src/jtprintf.cpp ../../src/JtEventConnect.cpp ../../src/JtEventServer.cpp \
	../../src/JtEventTimer.cpp ../../src/JtEventPairPipe.cpp ../../src/jtmutex.cpp ../../src/CachedAffair.cpp \
	-L../lib/x64  -I../../include -I../innerinclude -lpthread -ldl -lrt -ltinyxml -levent_core -levent_extra -levent_pthreads -ljthread

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