include ../../problems.mk

test: $(TARGETS)
	./problem5-1 ojvtpuvg > got
	echo 4543c154 > expected
	diff -ru got expected
	./problem5-2 ojvtpuvg > got
	echo 1050cbbd > expected
	diff -ru got expected
	rm got expected
