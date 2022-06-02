#Makefile

CC = gcc
CFLAGS = -g -Wall -lm

P3_1191027_BasilMari_4: P3_1191027_BasilMari_4.c Makefile
	$(CC) -o P3_1191027_BasilMari_4 P3_1191027_BasilMari_4.c $(CFLAGS)
