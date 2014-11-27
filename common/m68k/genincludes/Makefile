CROSS=yes
MOUNT_DIR=.
ROOT_DIR=../../..

ifeq ($(CROSS),yes)
	bin-prefix=m68k-atari-mint-
else
	bin-prefix=
endif

NATIVECC=gcc
CC=${bin-prefix}gcc

default: quakedef68k-nq.i quakedef68k-qw.i

quakedef68k-nq.i: genasmheaders quakeasmheaders.gen
	$(MOUNT_DIR)/genasmheaders quakeasmheaders.gen $@ 0 "$(NATIVECC) -I$(ROOT_DIR)/NQ -I$(ROOT_DIR)/include -DNQ_HACK"
	
quakedef68k-qw.i: genasmheaders quakeasmheaders.gen
	$(MOUNT_DIR)/genasmheaders quakeasmheaders.gen $@ 0 "$(NATIVECC) -I$(ROOT_DIR)/QW/client -I$(ROOT_DIR)/include -DQW_HACK"

genasmheaders: genasmheaders.c
	$(NATIVECC) -o $@ genasmheaders.c

clean:
	rm -f *.BAK *.bak *~
	rm -f gendefs gendefs.c genasmheaders quakedef68k-nq.i quakedef68k-qw.i

clean2:
	rm -f *.BAK *.bak *~
	rm -f gendefs gendefs.c genasmheaders
