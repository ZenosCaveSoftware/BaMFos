.section .init
	# gcc will nicely put the contents of crtend.o's .init section here.
	addq $8, %rsp
	ret

.section .fini
	# gcc will nicely put the contents of crtend.o's .fini section here.
	addq $8, %rsp
	ret
