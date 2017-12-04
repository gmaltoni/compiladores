INCFLG=-I./include
CFLAGS=$(INCFLG) $(OPTFLG)
OPTFLG=-g

mypas: mypas.o gettoken.o parser.o myasm.o keywords.o symtab.o

clean:
	$(RM) *.o
mostlyclean: clean
	$(RM) *~
indent:
	indent -orig -nuts -ts3 *.[ch]
