CFLAGS += -DMMEM_CONF_SIZE=512

OBJECTS =  unix/aiko_engine.o unix/io_serial.o
OBJECTS += expression.o parser.o primitives.o
OBJECTS += memory/list.o memory/mmem.o 

aiko_engine:	$(OBJECTS)
	gcc $^ -o $@

$(OBJECTS):	aiko_engine.h memory/list.h memory/mmem.h

clean:
	-rm -f $(OBJECTS)

clobber:	clean
	-rm -f aiko_engine

help :
	@echo ""
	@echo "make         - Build aiko_engine"
	@echo "make clean   - Remove binaries"
	@echo "make clobber - Remove all generated files"
	@echo "make help    - Display usage"
	@echo ""
