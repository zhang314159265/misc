@msg = constant [11 x i8] c"sum is %f\0A\00"

declare i32 @printf(i8*, ...)

define i32 @main() {
  %iptr = alloca i32 ; %3, %5, %7
	%jptr = alloca i32 ; %4, %8
	%sumptr = alloca float ; %6
	%matptr = alloca [10 x [10 x float]] ; %2

	store i32 0, i32* %iptr
	br label %clear_outer_cond

clear_outer_cond: ; 9
  %i = load i32, i32* %iptr
	%cmpres = icmp slt i32 %i, 10
	br i1 %cmpres, label %clear_outer_body, label %clear_outer_exit

clear_outer_body: ; 12
  store i32 0, i32* %jptr
	br label %clear_inner_cond

clear_inner_cond: ; 13
  %j = load i32, i32* %jptr
	%cmpres_1 = icmp slt i32 %j, 10
	br i1  %cmpres_1, label %clear_inner_body, label %clear_inner_next

clear_inner_body: ; 16
  %i_1 = load i32, i32* %iptr
	%i_2 = sext i32 %i_1 to i64
	%matrow_ptr = getelementptr inbounds [10 x [10 x float]], [10 x [10 x float]]* %matptr, i64 0, i64 %i_2

	%j_1 = load i32, i32* %jptr
	%j_2 = sext i32 %j_1 to i64
	%item_ptr = getelementptr inbounds [10 x float], [10 x float]* %matrow_ptr, i64 0, i64 %j_2
	store float 0.0, float* %item_ptr

	; 23
	%j_3 = load i32, i32* %jptr
	%j_4 = add i32 %j_3, 1
	store i32 %j_4, i32* %jptr
	br label %clear_inner_cond

clear_inner_next: ; 26
  ; 27
	%i_3 = load i32, i32* %iptr
	%i_4 = add i32 %i_3, 1
	store i32 %i_4, i32* %iptr
	br label %clear_outer_cond

clear_outer_exit: ; 30
  store i32 0, i32* %iptr
	br label %set1_cond

set1_cond: ; 31
  %i_5 = load i32, i32* %iptr
	%cmpres_2 = icmp slt i32 %i_5, 10
	br i1 %cmpres_2, label %set1_body, label %set1_next

set1_body: ; 34
  %i_6 = load i32, i32* %iptr
	%i_7 = sext i32 %i_6 to i64
	%matrow_ptr_1 = getelementptr inbounds [10 x [10 x float]], [10 x [10 x float]]* %matptr, i64 0, i64 %i_7

	%i_8 = load i32, i32* %iptr
	%i_9 = sext i32 %i_8 to i64
	%item_ptr_1 = getelementptr inbounds [10 x float], [10 x float]* %matrow_ptr_1, i64 0, i64 %i_9
	store float 1.0, float* %item_ptr_1
	; 41

	%i_10 = load i32, i32* %iptr
	%i_11 = add i32 %i_10, 1
	store i32 %i_11, i32* %iptr
	br label %set1_cond

set1_next: ; 44
  store float 0.0, float* %sumptr
	store i32 0, i32* %iptr
	br label %sum_outer_cond

sum_outer_cond: ; 45
  %i_12 = load i32, i32* %iptr
	%cmpres_3 = icmp slt i32 %i_12, 10
	br i1 %cmpres_3, label %sum_outer_body, label %sum_outer_next

sum_outer_body: ; 48
  store i32 0, i32* %jptr
	br label %sum_inner_cond

sum_inner_cond: ; 49
 	%j_5 = load i32, i32* %jptr
	%cmpres_4 = icmp slt i32 %j_5, 10
	br i1 %cmpres_4, label %sum_inner_body, label %sum_inner_next

sum_inner_body: ; 52
  %i_13 = load i32, i32* %iptr
	%i_14 = sext i32 %i_13 to i64
	%matrow_ptr_2 = getelementptr inbounds [10 x [10 x float]], [10 x [10 x float]]* %matptr, i64 0, i64 %i_14

	%j_6 = load i32, i32* %jptr
	%j_7 = sext i32 %j_6 to i64
	%item_ptr_2 = getelementptr inbounds [10 x float], [10 x float]* %matrow_ptr_2, i64 0, i64 %j_7
	%item = load float, float* %item_ptr_2
	%sum = load float, float* %sumptr
	%sum_2 = fadd float %sum, %item
	store float %sum_2, float* %sumptr
	; 62

	%j_8 = load i32, i32* %jptr
	%j_9 = add i32 %j_8, 1
	store i32 %j_9, i32* %jptr
	br label %sum_inner_cond

sum_inner_next: ; 65
  ; 66
  %i_15 = load i32, i32* %iptr
	%i_16 = add i32 %i_15, 1
	store i32 %i_16, i32* %iptr
	br label %sum_outer_cond

sum_outer_next: ; 69
	%sum_3 = load float, float* %sumptr
	%sum_4 = fpext float %sum_3 to double
	call i32 (i8*, ...) @printf(i8* getelementptr inbounds([11 x i8], [11 x i8]* @msg, i64 0, i64 0), double %sum_4)
  ret i32 0
}
