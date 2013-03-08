include ../config-host.mak

DEFINES=-I. -I.. -I$(SRC_PATH) -I$(SRC_PATH)/shared -I$(SRC_PATH)/slirp -I$(SRC_PATH)/shared/hooks -I$(SRC_PATH)/i386-softmmu -I$(SRC_PATH)/target-i386 -I$(SRC_PATH)/fpu -I$(SRC_PATH)/shared/xed2/include
DEFINES+=-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE 
DEFINES+=-I$(SRC_PATH)/shared/sleuthkit/src/fstools -I$(SRC_PATH)/shared/sleuthkit/src/auxtools -I$(SRC_PATH)/shared/sleuthkit/src/imgtools -DLINUX2 -DTRACE_ENABLED

CC=gcc
CPP=g++
CFLAGS=-Wall -O2 -fPIC 
# CFLAGS=-Wall -O2 -g -fPIC 
# CFLAGS=-Wall -g -fPIC 
LDFLAGS=-g -shared 
LIBS=-L/usr/local/lib -lbfd -lllconf
LIBS+=-L$(SRC_PATH)/shared/sleuthkit/lib -lfstools -limgtools -lauxtools 
LIBS+= -lcrypto -L$(SRC_PATH)/shared/sleuthkit/src/afflib/lib/ -lafflib -L$(SRC_PATH)/shared/sleuthkit/src/libewf -lewf
LIBS+= -L$(SRC_PATH)/shared/xed2/lib -lxed 
OBJS=commands.o trace.o operandinfo.o conditions.o network.o errdet.o conf.o tracecap.o ../shared/procmod.o ../shared/read_linux.o readwrite.o #special_propagation.o
OBJS+=../shared/hooks/function_map.o ../shared/hookapi.o hook_helpers.o 
OBJS+=../shared/hooks/hook_plugin_loader.o ext_hooks.o
OBJS+=../shared/reduce_taint.o

all: tracecap.so ini/main.ini 

sleuthkit:
	$(MAKE) -C $(SRC_PATH)/shared/sleuthkit

hooks:
	$(MAKE) -C $(SRC_PATH)/shared/hooks/hook_plugins protos_hooks

ini/main.ini: ini/main.ini.in
	@perl -pe 's[SRC_PATH][$(SRC_PATH)]g' $< >$@

%.o: %.c 
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $<

%.o: %.cpp
	$(CPP) $(CFLAGS) $(DEFINES) -c -o $@ $<

tracecap.so: $(OBJS)
	$(CPP) $(LDFLAGS) $^ -o $@ $(LIBS)
	ar cru libtracecap.a $@


clean:
	rm -f *.o  *.so *.a *~ $(PLUGIN) ../shared/*.o ../shared/hooks/*.o

