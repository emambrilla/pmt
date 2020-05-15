SOURCE=./source
HDRS=./hdrs

all: $(MYTARGET)/mb_read $(MYTARGET)/msg_send $(MYTARGET)/pipe $(MYTARGET)/start_pipe $(MYTARGET)/msg_read

clean:
	@echo Borrando...
	@$(MYRM) $(MYTARGET)/libpop.o
	@$(MYRM) $(MYTARGET)/libsmtp.o
	@$(MYRM) $(MYTARGET)/libpipe.o
	@$(MYRM) $(MYTARGET)/libmsg.o
	@$(MYRM) $(MYTARGET)/libpmt.a
	@$(MYRM) $(MYTARGET)/mb_read$(MYEXE_SUFFIX)
	@$(MYRM) $(MYTARGET)/msg_send$(MYEXE_SUFFIX)
	@$(MYRM) $(MYTARGET)/pipe$(MYEXE_SUFFIX)
	@$(MYRM) $(MYTARGET)/start_pipe
	@$(MYRM) $(MYTARGET)/msg_read$(MYEXE_SUFFIX)

$(MYTARGET)/libpmt.a: $(MYTARGET)/libpop.o $(MYTARGET)/libsmtp.o $(MYTARGET)/libpipe.o $(MYTARGET)/libmsg.o
	@echo Creando libpmt.a
	@$(MYAR) -r $(MYTARGET)/libpmt.a $(MYTARGET)/libpop.o
	@$(MYAR) -r $(MYTARGET)/libpmt.a $(MYTARGET)/libsmtp.o
	@$(MYAR) -r $(MYTARGET)/libpmt.a $(MYTARGET)/libpipe.o
	@$(MYAR) -r $(MYTARGET)/libpmt.a $(MYTARGET)/libmsg.o

$(MYTARGET)/libmsg.o: $(HDRS)/libmsg.h $(SOURCE)/libmsg.cc
	@echo Compilando libmsg.o
	@$(MYCC) $(SOURCE)/libmsg.cc -c -D_$(MYOS) $(MYCC_OPTS) -o $(MYTARGET)/libmsg.o -I$(HDRS)

$(MYTARGET)/libpop.o: $(HDRS)/libpop.h $(SOURCE)/libpop.cc
	@echo Compilando libpop.o
	@$(MYCC) $(SOURCE)/libpop.cc -c -D_$(MYOS) $(MYCC_OPTS) -o $(MYTARGET)/libpop.o -I$(HDRS)

$(MYTARGET)/libsmtp.o: $(HDRS)/libsmtp.h $(SOURCE)/libsmtp.cc
	@echo Compilando libsmtp.o
	@$(MYCC) $(SOURCE)/libsmtp.cc -c -D_$(MYOS) $(MYCC_OPTS) -o $(MYTARGET)/libsmtp.o -I$(HDRS)

$(MYTARGET)/libpipe.o: $(HDRS)/libpipe.h $(SOURCE)/libpipe.cc
	@echo Compilando libpipe.o
	@$(MYCC) $(SOURCE)/libpipe.cc -c -D_$(MYOS) $(MYCC_OPTS) -o $(MYTARGET)/libpipe.o -I$(HDRS)

$(MYTARGET)/mb_read: $(SOURCE)/mb_read.cc $(MYTARGET)/libpmt.a
	@echo Compilando mb_read
	@$(MYCC) $(SOURCE)/mb_read.cc -o $(MYTARGET)/mb_read $(MYCC_OPTS) -D_$(MYOS) -I$(HDRS) $(MYSOCKET_LIBS) $(MYTARGET)/libpmt.a

$(MYTARGET)/pipe: $(SOURCE)/pipe.cc $(MYTARGET)/libpmt.a
	@echo Compilando pipe
	@$(MYCC) $(SOURCE)/pipe.cc -o $(MYTARGET)/pipe -D_$(MYOS) $(MYCC_OPTS) -I$(HDRS) $(MYSOCKET_LIBS) $(MYTARGET)/libpmt.a

$(MYTARGET)/msg_send: $(SOURCE)/msg_send.cc $(MYTARGET)/libpmt.a
	@echo Compilando msg_send
	@$(MYCC) $(SOURCE)/msg_send.cc -o $(MYTARGET)/msg_send $(MYCC_OPTS) -D_$(MYOS) -I$(HDRS) $(MYSOCKET_LIBS) $(MYTARGET)/libpmt.a

$(MYTARGET)/msg_read: $(SOURCE)/msg_read.cc $(MYTARGET)/libpmt.a
	@echo Compilando msg_read
	@$(MYCC) $(SOURCE)/msg_read.cc -o $(MYTARGET)/msg_read $(MYCC_OPTS) -D_$(MYOS) -I$(HDRS) $(MYSOCKET_LIBS) $(MYTARGET)/libpmt.a

$(MYTARGET)/start_pipe: $(SOURCE)/start_pipe.sh
	@echo Generando start_pipe
	@cp $(SOURCE)/start_pipe.sh $(MYTARGET)/start_pipe
	@chmod +x $(MYTARGET)/start_pipe
