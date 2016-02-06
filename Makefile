CC=icc
LIBS=-lgsl -lgslcblas -lm

KKtransform:
	$(CC) KKreader.cpp KKtransform.cpp -o $@ $(LIBS)