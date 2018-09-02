
#help for make
#http://www.cnblogs.com/wang_yb/p/3990952.html
#https://segmentfault.com/a/1190000000349917
#http://blog.csdn.net/cuiyifang/article/details/7910268

#to use gprof to analyse efficience of the program:
#http://blog.chinaunix.net/uid-25194149-id-3215487.html
#to use doxygen+graphviz+htmlhelp to generate document from source code:
#http://www.doxygen.nl/
#(also include good comments norm)
#http://blog.csdn.net/u010740725/article/details/51387810

#TODO:the dependences are not complete!

#compile parameters

CC = g++
MPICC = mpicxx


#the optimazition level of gcc/g++
#http://blog.csdn.net/hit_090420216/article/details/44900215
#NOTICE: -O2 is recommended, while -O3 is dangerous
#when developing, not use -O because it will disturb the normal 
#routine. use it for test and release.
CFLAGS = -c -Wall -g #-pg #-O2

CFLAGS += -DNO_THREAD
#CFLAGS += -DDEBUG_TRACK
#CFLAGS += -DMAIN_LOG
CFLAGS += -DSTD_TRACK
#CFLAGS += -DSTD_MAIN
CFLAGS += -DQUERYSTR
CFLAGS += -DPTR_MATCH
CFLAGS += -DSPAN
#CFLAGS += -DI_AND_D
#CFLAGS += -DTUPLE
#CFLAGS += -DUNCOMP
#CFLAGS += -DCOLORING
#CFLAGS += -DREDCC
#CFLAGS += -DGLOBAL_COMMENT
#CFLAGS += -DNUM_MATCH
#CFLAGS += -DSELECTIVE


EXEFLAG = -g #-pg #-O2
LDFLAG = -pthread

# paths

homedir =  timingsubg/

rdfdir = $(homedir)rdf/
timingdir = $(homedir)timing/
msforestdir = $(homedir)msforest/
utildir = $(homedir)util/
gstreamdir = $(homedir)gstream/

objdir = $(homedir)rdf/.objs/

exedir = $(homedir)rdf/bin/

# objects

rdfsubgobj = $(objdir)rdfstream.o $(objdir)rdfdedge.o $(objdir)rdfqedge.o $(objdir)rdfquery.o

timingobj = $(objdir)timingsubg.o $(objdir)timingconf.o $(objdir)query.o $(objdir)edge.o

msforestobj = $(objdir)msforest.o $(objdir)tenode.o $(objdir)msnode.o $(objdir)nodeop.o $(objdir)lockreq.o

gstreamobj = $(objdir)gstream.o

matchobj = $(objdir)match.o

gtransactionobj = $(objdir)gtransaction.o

utilobj = $(objdir)util.o $(objdir)runtime.o


objfile = $(rdfsubgobj) $(timingobj) $(msforestobj) $(gstreamobj) $(matchobj) $(gtransactionobj) $(utilobj)
	 

def64IO = -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

all: tirdf   

#timing
tirdf: $(objdir)timain.o $(objfile)
	$(CC) $(EXEFLAG) ${LDFLAG} -o $(exedir)tirdf $(objdir)timain.o $(objfile)

#<obj:rdf>

$(objdir)timain.o: $(rdfdir)timing_rdf.cpp 
	$(CC) $(CFLAGS) $(rdfdir)timing_rdf.cpp -o $(objdir)timain.o 

$(objdir)rdfstream.o: $(rdfdir)rdfstream.cpp $(rdfdir)rdfstream.h 
	$(CC) $(CFLAGS) $(rdfdir)rdfstream.cpp -o $(objdir)rdfstream.o

$(objdir)rdfquery.o: $(rdfdir)rdfquery.cpp $(rdfdir)rdfquery.h
	$(CC) $(CFLAGS) $(rdfdir)rdfquery.cpp -o $(objdir)rdfquery.o

$(objdir)rdfqedge.o: $(rdfdir)RdfQedge.cpp $(rdfdir)RdfQedge.h 
	$(CC) $(CFLAGS) $(rdfdir)RdfQedge.cpp -o $(objdir)rdfqedge.o

$(objdir)rdfdedge.o: $(rdfdir)RdfDedge.cpp $(rdfdir)RdfDedge.h 
	$(CC) $(CFLAGS) $(rdfdir)RdfDedge.cpp -o $(objdir)rdfdedge.o

#</obj:rdf>

#<obj:timing>

$(objdir)timingsubg.o: $(timingdir)timingsubg.cpp $(timingdir)timingsubg.h 
	$(CC) $(CFLAGS) $(timingdir)timingsubg.cpp -o $(objdir)timingsubg.o 

$(objdir)timingconf.o: $(timingdir)timingconf.cpp $(timingdir)timingconf.h 
	$(CC) $(CFLAGS) $(timingdir)timingconf.cpp -o $(objdir)timingconf.o

$(objdir)query.o: $(timingdir)query.cpp $(timingdir)query.h 
	$(CC) $(CFLAGS) $(timingdir)query.cpp -o $(objdir)query.o

$(objdir)gtransaction.o: $(timingdir)gtransaction.h $(timingdir)gtransaction.cpp 
	$(CC) $(CFLAGS) $(timingdir)gtransaction.cpp -o $(objdir)gtransaction.o

$(objdir)match.o: $(timingdir)match.cpp $(timingdir)match.h
	$(CC) $(CFLAGS) $(timingdir)match.cpp -o $(objdir)match.o

$(objdir)edge.o: $(timingdir)edge.cpp $(timingdir)edge.h
	$(CC) $(CFLAGS) $(timingdir)edge.cpp -o $(objdir)edge.o

#</obj:timing>

#<obj:msforest>

$(objdir)msforest.o: $(msforestdir)msforest.cpp $(msforestdir)msforest.h 
	$(CC) $(CFLAGS) $(msforestdir)msforest.cpp -o $(objdir)msforest.o

$(objdir)tenode.o: $(msforestdir)teNode.cpp $(msforestdir)teNode.h 
	$(CC) $(CFLAGS) $(msforestdir)teNode.cpp -o $(objdir)tenode.o

$(objdir)msnode.o: $(msforestdir)msNode.cpp $(msforestdir)msNode.h 
	$(CC) $(CFLAGS) $(msforestdir)msNode.cpp -o $(objdir)msnode.o

$(objdir)nodeop.o: $(msforestdir)nodeOP.cpp $(msforestdir)nodeOP.h 
	$(CC) $(CFLAGS)  $(msforestdir)nodeOP.cpp -o $(objdir)nodeop.o

$(objdir)lockreq.o: $(msforestdir)lockReq.cpp $(msforestdir)lockReq.h 
	$(CC) $(CFLAGS) $(msforestdir)lockReq.cpp -o $(objdir)lockreq.o

#</obj:msforest>

#<obj:gstream>

$(objdir)gstream.o: $(gstreamdir)gstream.h $(gstreamdir)gstream.cpp 
	$(CC) $(CFLAGS) $(gstreamdir)gstream.cpp -o $(objdir)gstream.o

#</obj:gstream>

#<obj:other>

$(objdir)util.o: $(utildir)util.h $(utildir)util.cpp $(utildir)list_T.h
	$(CC) $(CFLAGS) $(utildir)util.cpp -o $(objdir)util.o

#$(objdir)list.o:  $(utildir)list_T.h 
#	$(CC) $(CFLAGS) $(utildir)list_T.h -o $(objdir)list.o

$(objdir)runtime.o:  $(utildir)runtime.h $(utildir)runtime.cpp
	$(CC) $(CFLAGS) $(utildir)runtime.cpp -o $(objdir)runtime.o

#</obj:other>

clean:
	rm -rf $(exedir)* $(objdir)*.o
	#rm -rf .project .cproject .settings   just for eclipse
	#rm -rf cscope* just for vim


