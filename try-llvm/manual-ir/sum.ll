@fmtstr = constant [11 x i8] c"Sum is %d\0A\00"
declare i32 @printf(i8*, ...)

define i32 @main() {
  %iptr = alloca i32
  %sumptr = alloca i32

  store i32 0, i32* %iptr
  store i32 0, i32* %sumptr
  br label %l_cmp

l_cmp:
  %i = load i32, i32* %iptr 
  %cmpres = icmp sle i32 %i, 100
  br i1 %cmpres, label %l_true, label %l_false

l_true:
  %i_2 = load i32, i32* %iptr
  %sum = load i32, i32* %sumptr
  %sum_2 = add i32 %i_2, %sum
  store i32 %sum_2, i32* %sumptr
  br label %l_inc

l_inc:
  %i_3 = load i32, i32* %iptr
  %i_4 = add i32 %i_3, 1
  store i32 %i_4, i32* %iptr
  br label %l_cmp

l_false:
  %sum_final = load i32, i32* %sumptr
  %printf_rv = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @fmtstr, i64 0, i64 0), i32 %sum_final)
  ret i32 0
}
