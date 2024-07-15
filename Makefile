obj-m += greet.o

all:
	make -C /lib/modules/6.9.9-arch1-1/build M=$(PWD) modules
clean:
	make -C /lib/modules/6.9.9-arch1-1/build M=$(PWD) clean
