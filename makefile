CFLAGS += -DMMEM_CONF_SIZE=512

OBJECTS =  examples/unix/aiko_server.o
OBJECTS += src/aiko_engine.o
OBJECTS += src/unix/network.o
OBJECTS += src/unix/serial.o

OBJECTS += lisp/expression.o
OBJECTS += lisp/interface.o
OBJECTS += lisp/parser.o
OBJECTS += lisp/primitives.o
OBJECTS += memory/list.o
OBJECTS += memory/mmem.o 

aiko_server:	$(OBJECTS)
	gcc $^ -o $@

$(OBJECTS):	\
	include/aiko_engine.h   \
	include/compatibility.h \
	include/network.h       \
	include/serial.h        \
	lisp/lisp.h             \
	memory/list.h           \
	memory/mmem.h

clean:
	-rm -f $(OBJECTS)

clobber:	clean
	-rm -f aiko_server

help :
	@echo ""
	@echo "make         - Build aiko_server"
	@echo "make clean   - Remove binaries"
	@echo "make clobber - Remove all generated files"
	@echo "make help    - Display usage"
	@echo ""
