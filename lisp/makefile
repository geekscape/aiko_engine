CFLAGS += -DMMEM_CONF_SIZE=512

OBJECTS =  ../unix/lisp_engine.o ../unix/io_buffer.o
OBJECTS += expression.o parser.o primitives.o
OBJECTS += ../memory/list.o ../memory/mmem.o 

lisp_engine:	$(OBJECTS)
	gcc $^ -o $@

$(OBJECTS):	lisp_engine.h ../memory/list.h ../memory/mmem.h

clean:
	-rm -f $(OBJECTS)

clobber:	clean
	-rm -f lisp_engine

help :
	@echo ""
	@echo "make         - Build lisp_engine"
	@echo "make clean   - Remove binaries"
	@echo "make clobber - Remove all generated files"
	@echo "make help    - Display usage"
	@echo ""
