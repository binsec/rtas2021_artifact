.PHONY: all clean

all : BasicIntegerOperation NumberBetween FloatInvariant Read

BasicIntegerOperation : BasicIntegerOperation.cpp ../host_floating.hh ../floating.hh ../host_floating.tcc ../floating.tcc ../integer.hh ../integer.tcc
	g++ -O3 -Wall -I../../../.. -o $@ BasicIntegerOperation.cpp

NumberBetween : NumberBetween.cpp ../host_floating.hh ../floating.hh ../host_floating.tcc ../floating.tcc ../integer.hh ../integer.tcc
	g++ -O3 -Wall -I../../../.. -o $@ NumberBetween.cpp

FloatInvariant : FloatInvariant.cpp ../host_floating.hh ../floating.hh ../host_floating.tcc ../floating.tcc ../integer.hh ../integer.tcc
	g++ -O3 -Wall -I../../../.. -o $@ FloatInvariant.cpp

Read : Read.cpp ../host_floating.hh ../floating.hh ../host_floating.tcc ../floating.tcc ../integer.hh ../integer.tcc
	g++ -O3 -Wall -I../../../.. -o $@ Read.cpp

clean :
	rm BasicIntegerOperation NumberBetween FloatInvariant Read

