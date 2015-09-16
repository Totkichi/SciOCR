LIB = ./lib/obj/clstm.o ./lib/obj/clstm.pb.o ./lib/obj/clstm_prefab.o ./lib/obj/clstm_proto.o ./lib/obj/ctc.o ./lib/obj/extras.o ./lib/obj/image_processing.o ./lib/obj/OCR.o
EIG3:= $(shell pkg-config --cflags --libs eigen3)
DATE:="-DHGVERSION=\"$(shell date)\""
DEF= -DNODISPLAY=1 -DTHROW=throw
CV:=$(shell pkg-config --cflags --libs opencv)
PY:=$(shell pkg-config --cflags --libs python2)

.PHONY: all SciOCR lib clean

lib:
	protoc --cpp_out=./scr clstm.proto
	g++ --std=c++11 -Wno-deprecated-declarations -o ./lib/obj/clstm.o -c -fPIC -O2 $(DATE) $(DEF) $(EIG3) ./scr/clstm.cc
	g++ --std=c++11 -Wno-deprecated-declarations -o ./lib/obj/clstm.pb.o -c -fPIC -O2 $(DATE) $(DEF) $(EIG3) ./scr/clstm.pb.cc
	g++ --std=c++11 -Wno-deprecated-declarations -o ./lib/obj/clstm_prefab.o -c -fPIC -O2 $(DATE) $(DEF) $(EIG3) ./scr/clstm_prefab.cc
	g++ --std=c++11 -Wno-deprecated-declarations -o ./lib/obj/clstm_proto.o -c -fPIC -O2 $(DATE) $(DEF) $(EIG3) ./scr/clstm_proto.cc
	g++ --std=c++11 -Wno-deprecated-declarations -o ./lib/obj/ctc.o -c -fPIC -O2 $(DATE) $(DEF) $(EIG3) ./scr/ctc.cc
	g++ --std=c++11 -Wno-deprecated-declarations -o ./lib/obj/extras.o -c -fPIC -O2 $(DATE) $(DEF) $(EIG3) ./scr/extras.cc
	g++ --std=c++11 -Wno-unused-result -o ./lib/obj/image_processing.o -c  -fPIC -O2 $(DATE) -lX11 $(CV) ./scr/image_processing.cc
	g++ --std=c++11 -Wno-deprecated-declarations -o ./lib/obj/OCR.o -c -fPIC -O2 $(DATE) $(DEF) $(EIG3) ./scr/OCR.cc

shredder:
	g++ --std=c++11 -Wno-unused-result -o ./lib/obj/image_processing.o -c  -fPIC -O2 $(DATE) -lX11 $(CV) ./scr/image_processing.cc


console_SciOCR:
	g++ --std=c++11 -Wno-unused-result -o console_SciOCR ./scr/SciOCR.cc $(DATE) -lX11 $(CV) $(LIB) -lpng -lprotobuf $(DEF) $(EIG3) 

Sci:
	g++ --std=c++11 -Wno-deprecated-declarations -fPIC -O2 -c ./scr/wrap.cc -o ./lib/obj/wrap.o $(EIG3) $(DEF) -lX11 $(CV) $(PY) -lboost_python -lprotobuf
	g++ --std=c++11 -shared -o ./lib/SciOCR.so $(LIB) ./lib/obj/wrap.o $(EIG3) $(DEF) -lX11 $(CV) $(PY) -lboost_python -lprotobuf
	
	