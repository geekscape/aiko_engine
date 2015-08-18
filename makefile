CFLAGS += -DMMEM_CONF_SIZE=512
CFLAGS += -Iinclude
CFLAGS += -Ilisp

OBJECTS  = src/common/aiko_engine.o
OBJECTS += src/unix/network.o
OBJECTS += src/unix/serial.o
OBJECTS += src/unix/timer.o

OBJECTS += lisp/expression.o
OBJECTS += lisp/interface.o
OBJECTS += lisp/parser.o
OBJECTS += lisp/primitives.o
OBJECTS += memory/list.o
OBJECTS += memory/mmem.o 

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
	lisp/lisp.h                  \
	memory/list.h                \
	memory/mmem.h                \
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
