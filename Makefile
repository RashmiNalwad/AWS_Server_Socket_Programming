CC = gcc
CFLAGS = -lsocket -lnsl -lresolv

TARGETS = client_compile aws_compile serverA_compile serverB_compile serverC_compile 

all: $(TARGETS)

client_compile: client.c
	$(CC) -o client client.c $(CFLAGS)

aws_compile: aws.c
	$(CC) -o aws_out aws.c $(CFLAGS)

serverA_compile: serverA.c
	$(CC) -o serverA_out serverA.c $(CFLAGS)

serverB_compile: serverB.c
	$(CC) -o serverB_out serverB.c $(CFLAGS)	

serverC_compile: serverC.c
	$(CC) -o serverC_out serverC.c $(CFLAGS)

aws : 
	./aws_out

serverA :  
	./serverA_out

serverB:  
	./serverB_out

serverC: 
	./serverC_out

clean:
	$(RM) client aws_out serverA_out serverB_out serverC_out

#.PHONY: serverA serverB serverC aws