all : mc_01 mc_01_02 mc_02 mc_02_03 mc_03 mc_04 jc_01

jc_01: json_class_01.cpp
	g++ -o jc_01  json_class_01.cpp -lcjson

mc_01: motor_class_01.cpp
	g++ -o mc_01 -l cjson motor_class_01.cpp -l cjson

mc_01_02: motor_class_01_02.cpp fields.h
	g++ -o mc_01_02 motor_class_01_02.cpp fields.cpp -l cjson

mc_02: motor_class_02.cpp fields.cpp fields.h
	g++ -o mc_02 motor_class_02.cpp fields.cpp -l cjson

mc_02_03: motor_class_02_03.cpp fields.cpp fields.h
	g++ -o mc_02_03 motor_class_02_03.cpp fields.cpp -l cjson

mc_03: motor_class_03.cpp fields.cpp fields.h
	g++ -o mc_03 -l cjson motor_class_03.cpp fields.cpp -l cjson
#server.cpp server.h
mc_04: motor_class_04.cpp fields.cpp fields.h motor_sock.cpp motor_sock.h
	g++ -o mc_04  motor_class_04.cpp fields.cpp motor_sock.cpp  -lpthread -lrt -l cjson
