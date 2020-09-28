#通用makefile模板
CC=gcc -g
#利用wildcard 可以过滤出来当前目录下的所有.c文件
SRCS=$(wildcard *.c)
#将提取出来的.c文件创建.o文件
OBJS=$(SRCS:.c=.o)
#gcc -MM 文件名     可以打印出当前文件编译时的依赖关系
#保存.d文件，内容为当前文件编译时的依赖关系
DEPS=$(SRCS:.c=.d)

#生成可执行程序的位置
#addprefix 添加前缀 main  ->  /framework/main
BIN := $(addprefix $(BULID_ROOT)/,$(BIN))

#生成.o文件的路径，将生成的.o文件放置到一个文件里
LINK_OBJ_DIR=$(BULID_ROOT)/link_obj
#将依赖关系放到一个文件
DEP_DIR=$(BULID_ROOT)/depend

#创建出来这两个临时文件 ，即创建保存.o和.d的文件的目录
$(shell mkdir -p $(LINK_OBJ_DIR))
$(shell mkdir -p $(DEP_DIR))

#将这两种目录里的文件都加上路径前缀
#main.o -> /framework/link_obj/main.o

OBJS:=$(addprefix $(LINK_OBJ_DIR)/,$(OBJS))
DEPS:=$(addprefix $(DEP_DIR)/,$(DEPS))

#变量定义完成后，将.o文件过滤出来形成依赖
LINK_OBJ=$(wildcard $(LINK_OBJ_DIR)/*.o)
LINK_OBJ += $(OBJS)

all: $(DEPS) $(OBJS) $(BIN)

$(BIN) : $(LINK_OBJ)
	$(CC) -o $@ $^

#-I指定头文件的路径
$(LINK_OBJ_DIR)/%.o : %.c
	$(CC) -c  $^ -o $@ -I$(INCLUDE_PATH)  

#先将每个文件对应的完整的.0文件名定向到目标文件
#再把依赖关系追加到后面
$(DEP_DIR)/%.d : %.c
	echo -n $(LINK_OBJ_DIR)/ > $@
	$(CC) -I$(INCLUDE_PATH) -MM $^ >> $@
