comp=g++-5
flags=-std=c++11
FLCF=`fltk-config --cxxflags`
FLLF=`fltk-config --ldflags`

cpp11deadlocktimingvisualization: main.o primaryapplication.o executioncontrolbox.o inputcontrolgroup.o inputtable.o analysischart.o progressbar.o globalprogdata.o cppgenerator.o resultsanalyzer.o graph.o processdeadlock.o randgen.o
	$(comp) $(flags) main.o primaryapplication.o executioncontrolbox.o inputcontrolgroup.o inputtable.o analysischart.o progressbar.o globalprogdata.o cppgenerator.o resultsanalyzer.o graph.o processdeadlock.o randgen.o $(FLLF) -o cpp11deadlocktimingvisualization
main.o: main.cpp
	$(comp) $(flags) $(FLCF) -c main.cpp $(FLLF) -o main.o
primaryapplication.o: primaryapplication.cpp primaryapplication.h
	$(comp) $(flags) $(FLCF) -c primaryapplication.cpp $(FLLF) -o primaryapplication.o
executioncontrolbox.o: executioncontrolbox.cpp executioncontrolbox.h
	$(comp) $(flags) $(FLCF) -c executioncontrolbox.cpp $(FLLF) -o executioncontrolbox.o
inputcontrolgroup.o: inputcontrolgroup.cpp inputcontrolgroup.h
	$(comp) $(flags) $(FLCF) -c inputcontrolgroup.cpp $(FLLF) -o inputcontrolgroup.o
inputtable.o: inputtable.cpp inputtable.h fltkincludes.h
	$(comp) $(flags) $(FLCF) -c inputtable.cpp $(FLLF) -o inputtable.o
analysischart.o: analysischart.cpp analysischart.h fltkincludes.h
	$(comp) $(flags) $(FLCF) -c analysischart.cpp $(FLLF) -o analysischart.o
progressbar.o: progressbar.cpp progressbar.h fltkincludes.h
	$(comp) $(flags) $(FLCF) -c progressbar.cpp $(FLLF) -o progressbar.o
globalprogdata.o: globalprogdata.cpp globalprogdata.h
	$(comp) $(flags) -c globalprogdata.cpp -o globalprogdata.o
cppgenerator.o: cppgenerator.cpp cppgenerator.h generatedstringdata.h includes.h
	$(comp) $(flags) -c cppgenerator.cpp -o cppgenerator.o
resultsanalyzer.o: resultsanalyzer.cpp resultsanalyzer.h
	$(comp) $(flags) -c resultsanalyzer.cpp -o resultsanalyzer.o
graph.o: graph.cpp graph.h includes.h
	$(comp) $(flags) -c graph.cpp -o graph.o
processdeadlock.o: processdeadlock.cpp processdeadlock.h
	$(comp) $(flags) -c processdeadlock.cpp -o processdeadlock.o
randgen.o: randgen.cpp randgen.h includes.h
	$(comp) $(flags) -c randgen.cpp -o randgen.o

.PHONY: clean

clean: 
	rm main.o primaryapplication.o executioncontrolbox.o inputcontrolgroup.o inputtable.o analysischart.o progressbar.o globalprogdata.o cppgenerator.o resultsanalyzer.o graph.o processdeadlock.o randgen.o
