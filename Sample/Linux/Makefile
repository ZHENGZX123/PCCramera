
OBJS_SERVER = SEP2PAppSDKDemo.o
LIBS_SERVER = -L../../Libs/Linux/x86 -L./SE_VideoCodec/Lib/Linux/x86 -lSEP2P_AppSDK -lSE_VideoCodec -lpthread

CFLAGS = -I../../Include -I./SE_VideoCodec/Include -DLINUX -DAPP_VIDEO_SUPPORT_DECODE -c
CXX = g++


PROS = SEP2PAppSDKDemo

all: $(PROS)

.cpp.o:
	$(CXX) $(CFLAGS) $<

SEP2PAppSDKDemo: $(OBJS_SERVER)
	$(CXX) -Wall -Wl,-rpath=./ -o $@ $^ $(LIBS_SERVER)
	cp ./SE_VideoCodec/Lib/Linux/x86/*.so ./
	cp ../../Libs/Linux/x86/*.so ./
clean:
	rm -rf $(PROS) $(OBJS_SERVER) ./*.bmp ./libSEP2P_AppSDK.so ./libSE_VideoCodec.so
