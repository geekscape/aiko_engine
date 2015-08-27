CFLAGS += -DMMEM_CONF_SIZE=512
CFLAGS += -Iinclude

OBJECTS  = src/common/aiko_engine.o
OBJECTS += src/unix/engine.o
OBJECTS += src/unix/network.o
OBJECTS += src/unix/serial.o
OBJECTS += src/unix/timer.o

OBJECTS += src/common/lisp/expression.o
OBJECTS += src/common/lisp/interface.o
OBJECTS += src/common/lisp/parser.o
OBJECTS += src/common/lisp/primitives.o
OBJECTS += src/common/lisp/utility.o
OBJECTS += src/common/memory/list.o
OBJECTS += src/common/memory/mmem.o 

AIKO_SERVER_OBJECTS  = examples/unix/aiko_server.o
AIKO_SERVER_OBJECTS += examples/common/aiko_server/lisp_extend.o

AIKO_TIMER_OBJECTS   = examples/unix/aiko_timer.o

all:	aiko_server aiko_timer

aiko_server:	$(AIKO_SERVER_OBJECTS) $(OBJECTS)
	gcc $^ -o $@

aiko_timer:	$(AIKO_TIMER_OBJECTS) $(OBJECTS)
	gcc $^ -o $@

$(OBJECTS):	\
	include/aiko_engine.h        \
	include/aiko_compatibility.h \
	include/aiko_network.h       \
	include/aiko_serial.h        \
	include/lisp.h               \
	include/memory/list.h        \
	include/memory/mmem.h        \
	examples/common/aiko_server/lisp_extend.h

clean:
	-rm -f $(OBJECTS)
	-rm -f $(AIKO_SERVER_OBJECTS)
	-rm -f $(AIKO_TIMER_OBJECTS)

clobber:	clean
	-rm -f aiko_server aiko_timer

help :
	@echo ""
	@echo "make         - Build Unix examples
	@echo "make clean   - Remove binaries"
	@echo "make clobber - Remove all generated files"
	@echo "make help    - Display usage"
	@echo ""
