LIB = ./obj/clstm.o ./obj/ctc.o ./obj/clstm_proto.o ./obj/clstm_prefab.o ./obj/extras.o ./obj/clstm.pb.o
EIG3:= $(shell pkg-config --cflags --libs eigen3)
DATE:="-DHGVERSION=\"$(shell date)\""
DEF= -DNODISPLAY=1 -DTHROW=throw
CV:=$(shell pkg-config --cflags --libs opencv)

.PHONY: all SciOCR clean

lib:
	protoc --cpp_out=./scr clstm.proto
	g++ --std=c++11 -Wno-unused-result -o ./obj/clstm.o -c -g -O3 -finline -g $(DATE) $(DEF) $(EIG3) ./scr/clstm.cc
	g++ --std=c++11 -Wno-unused-result -o ./obj/clstm.pb.o -c -g -O3 -finline -g $(DATE) $(DEF) $(EIG3) ./scr/clstm.pb.cc
	g++ --std=c++11 -Wno-unused-result -o ./obj/clstm_prefab.o -c -g -O3 -finline -g $(DATE) $(DEF) $(EIG3) ./scr/clstm_prefab.cc
	g++ --std=c++11 -Wno-unused-result -o ./obj/clstm_proto.o -c -g -O3 -finline -g $(DATE) $(DEF) $(EIG3) ./scr/clstm_proto.cc
	g++ --std=c++11 -Wno-unused-result -o ./obj/ctc.o -c -g -O3 -finline -g $(DATE) $(DEF) $(EIG3) ./scr/ctc.cc
	g++ --std=c++11 -Wno-unused-result -o ./obj/extras.o -c -g -O3 -finline -g $(DATE) $(DEF) $(EIG3) ./scr/extras.cc
	g++ --std=c++11 -Wno-unused-result -o ./obj/shredder.o -c -c -O3 -finline -g $(DATE) -lX11 $(CV) ./scr/shredder.cc  

	
SciOCR:
	g++ --std=c++11 -Wno-unused-result -o SciOCR ./scr/SciOCR.cc $(DATE) -lX11 $(CV) $(LIB) -lpng -lprotobuf $(DEF) $(EIG3) 


	
	