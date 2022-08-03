@msg = constant [9 x i8] c"d is %d\0A\00"

declare i32 @printf(i8*, ...)

define i32 @main() {
  %a_ptr = alloca i32 ; %2
  %b_ptr = alloca i32 ; %3
  %c_ptr = alloca i32 ; %4

  store i32 1, i32* %a_ptr
  store i32 2, i32* %b_ptr
  store i32 3, i32* %c_ptr

  %a = load i32, i32* %a_ptr ; %6
  %cmp_res = icmp sgt i32 %a, 0 ; %7
  br i1 %cmp_res, label %true_branch, label %false_branch

true_branch:
  %b = load i32, i32* %b_ptr
  %c = load i32, i32* %c_ptr
  %sum = add i32 %b, %c
  br label %merge_point

false_branch:
  %b_1 = load i32, i32* %b_ptr
  %c_1 = load i32, i32* %c_ptr
  %dif = sub i32 %b, %c
  br label %merge_point

merge_point:
  %d = phi i32 [%sum, %true_branch], [%dif, %false_branch]
  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @msg, i64 0, i64 0), i32 %d)
  ret i32 0
}
