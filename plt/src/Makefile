BASE_PATH := /ebs_plt/bob_new/base
COMM_PATH := ../../../common
INC_PATH := $(BASE_PATH) $(COMM_PATH)/aws_comm $(COMM_PATH)/aws_tbl $(COMM_PATH)/comm ./bussiness ./frame /ebs_plt/bob_new/common/proto /usr/local/include 
SRC_FILES := $(COMM_PATH)/aws_comm/*.cpp $(COMM_PATH)/aws_tbl/*.cpp $(COMM_PATH)/comm/*.cpp ./bussiness/*.cpp ./frame/*.cpp /ebs_plt/bob_new/common/proto/*.cc 
LIB_PATH := $(BASE_PATH)/lib

project.targets += login_center
project.extra_warning := -Wno-write-strings -fpermissive -Wno-unused-but-set-variable
project.optimize_flags :=
login_center.path := ../bin
login_center.name := login_center
login_center.defines := TIXML_USE_STL
login_center.sources := $(wildcard $(SRC_FILES))
login_center.includes := $(INC_PATH) .
login_center.ldadd := $(LIB_PATH)/libzookeeper_mt.a $(LIB_PATH)/libEncode.a $(LIB_PATH)/libSvrPub.a $(LIB_PATH)/libWtseBase.a $(LIB_PATH)/libjsoncpp.a $(LIB_PATH)/libzkutil.a $(LIB_PATH)/libprotobuf.a -lpthread -lcurl

include $(BASE_PATH)/Generic.mak

