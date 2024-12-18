obj-m += mytest.o
obj-m += mytest2.o
obj-m += mytest3.o

PWD := $(CURDIR)

all:
  make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
  make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
