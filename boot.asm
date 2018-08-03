	org	07c00h	;将程序加载到绝对地址07c00h 处
	mov	ax, cs	;将cs 赋值给予 ax寄存器
	mov	ds, ax
	mov	es, ax
	call	ShowStr ;调用显示子符串函数
	jmp	$
ShowStr:
	mov	ax, BootMessage	;ES:BP字符串地址
	mov	bp, ax		;CX 字符串长度
	mov	cx, 16
	mov	ax, 01301h	;AH =13 AL=01h
	mov	bx, 000ch	;页号为零BH=0 BL=0ch 黑底红字
	mov	dl, 0
	int	10h	;int 10号中断
	ret
BootMessage:	db	"Hello, OS World!"
times	510-($-$$)	db 0;
dw	0xaa55		;结束标志
