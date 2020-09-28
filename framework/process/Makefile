# 总的Makefile编写，目的是到每个目录中去执行每个目录的子makefile文件
export BULID_ROOT=$(shell pwd)
# 指定编译的总路径，用export将其设置为环境变量中的全局变量，可以为多个文件使用，否则就只能在这一个文件中使用
export INCLUDE_PATH=$(BULID_ROOT)/include

#每一个需要编译的子目录
export BULID_PATH = $(BULID_ROOT)/dal \
					$(BULID_ROOT)/src 

#接着就是编译语句的设计
#主要是执行每个子目录下BULID_PATH的makefile
#@表示不显示命令
#make -C 指定要编译那个目录中的makefile
all: 
	@for dir in $(BULID_PATH); \
	do \
	   make -C $$dir; \
	done

clean:
	rm -rf $(BULID_ROOT)/depend
	rm -rf $(BULID_ROOT)/link_obj
	rm -rf main
