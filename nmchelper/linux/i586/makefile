all : libnmchelper.so test.out

libnmchelper.so:../../src/CachedAffair.cpp ../../src/NMCDevice.cpp ../../src/NMCDeviceImpl.cpp ../../src/NmcMessage.cpp ../../src/NMCUser.cpp \
	../../src/JtDevcieSearch.cpp ../../src/jtprintf.cpp ../../src/JtEventConnect.cpp ../../src/JtEventServer.cpp ../../src/JtEventTimer.cpp ../../src/JtEventPairPipe.cpp \
	../../src/JtStreamSender.cpp ../../src/PushStreamPeer.cpp ../../src/jtmutex.cpp  
	g++ -std=c++0x -g -fPIC -Wl,-rpath,./ -Wl,-Bsymbolic -Wall -fvisibility=hidden -shared -o2 -o libnmchelper.so -L../lib/i586 -I../../include -I../innerinclude  ../../src/NMCDevice.cpp \
	../../src/NMCDeviceImpl.cpp ../../src/NmcMessage.cpp ../../src/NMCUser.cpp ../../src/JtDevcieSearch.cpp ../../src/jtprintf.cpp \
	../../src/JtEventConnect.cpp ../../src/CachedAffair.cpp ../../src/JtEventServer.cpp ../../src/JtEventTimer.cpp ../../src/JtEventPairPipe.cpp ../../src/JtStreamSender.cpp \
	../../src/PushStreamPeer.cpp  ../../src/jtmutex.cpp \
	 -lpthread -ldl -lrt -ltinyxml -levent_core -levent_extra -levent_openssl -levent_pthreads -ljthread
	chmod +x *.so
	cp ../../include/*.h ./

#  -ltinyxml  ../../src/tinyxml2.cpp -fPIC -Wl,-Bsymbolic
#test.out:Test.cpp  
#	g++ -g -Wall -o test.out Test.cpp ./libjfront.so -ldl -lpthread

test.out:Test.cpp tinyxml2.cpp
	g++ -g -Wall -o test.out Test.cpp  tinyxml2.cpp ./libnmchelper.so -ldl -lpthread  -lrt

gtest.out:gtest.cpp
	g++ -g -Wall -o gtest.out gtest.cpp `pkg-config --cflags --libs gtk+-2.0` -ldl -lpthread

release:
	mkdir nmclinuxi586
	cp *.so ./nmclinuxi586
	cp ../../include/NMCUser.h ./nmclinuxi586/
	cp ../../include/nmctype.h ./nmclinuxi586/
	cp ../../include/jntype.h ./nmclinuxi586/
	cp Test.cpp ./nmclinuxi586
	cp *.out ./nmclinuxi586
	cp makefile.test ./nmclinuxi586/makefile
	tar zcf nmclinuxi586.tar.gz ./nmclinuxi586
	rm -r ./nmclinuxi586

.PHONY : all

clean:
	rm -f libnmchelper.so