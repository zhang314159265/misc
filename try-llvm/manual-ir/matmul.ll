@fmtitem = constant [4 x i8] c" %f\00"
@fmtnl = constant [2 x i8] c"\0A\00"

@const_mat1 = constant [2 x [2 x double]] [[2 x double] [double 2.0, double 1.0], [2 x double] [double 1.0, double 1.0]]
@const_mat2 = constant [2 x [2 x double]] [[2 x double] [double 3.0, double 2.0], [2 x double] [double 2.0, double 1.0]]

define i32 @main() {
  %iptr = alloca i32 ; %5
	%jptr = alloca i32 ; %6
	%kptr = alloca i32 ; %7
	%mat1_ptr = alloca [2 x [2 x double]] ; %2
  %mat2_ptr = alloca [2 x [2 x double]] ; %3
	%matres_ptr = alloca [2 x [2 x double]] ; %4

	%flat_mat1_ptr = bitcast [2 x [2 x double]]* %mat1_ptr to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %flat_mat1_ptr, i8* bitcast ([2 x [2 x double]]* @const_mat1 to i8*), i64 32, i1 false)
  %flat_mat2_ptr = bitcast [2 x [2 x double]]* %mat2_ptr to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %flat_mat2_ptr, i8* bitcast ([2 x [2 x double]]* @const_mat2 to i8*), i64 32, i1 false)

	store i32 0, i32* %iptr
	br label %mm_i_cond

mm_i_cond: ; 10
	%i = load i32, i32* %iptr
	%cc = icmp slt i32 %i, 2
	br i1 %cc, label %mm_i_body, label %mm_i_next

mm_i_body: ; 13
	store i32 0, i32* %jptr
	br label %mm_j_cond

mm_j_cond: ; 14
  %j = load i32, i32* %jptr
	%cc_1 = icmp slt i32 %j, 2
	br i1 %cc_1, label %mm_j_body, label %mm_j_next

mm_j_body: ; 17
	%i_1 = load i32, i32* %iptr
	%i_2 = sext i32 %i_1 to i64
	%matres_rowptr = getelementptr inbounds [2 x [2 x double]], [2 x [2 x double]]* %matres_ptr, i64 0, i64 %i_2
	%j_1 = load i32, i32* %jptr
	%j_2 = sext i32 %j_1 to i64
	%matres_itemptr = getelementptr inbounds [2 x double], [2 x double]* %matres_rowptr, i64 0, i64 %j_2
	store double 0.0, double* %matres_itemptr
	store i32 0, i32* %kptr

	br label %mm_k_cond

mm_k_cond: ; 24
	%k = load i32, i32* %kptr
	%cc_2 = icmp slt i32 %k, 2
	br i1 %cc_2, label %mm_k_body, label %mm_k_next

mm_k_body: ; 27
  %i_3 = load i32, i32* %iptr
	%i_4 = sext i32 %i_3 to i64
	%mat1_rowptr = getelementptr inbounds [2 x [2 x double]], [2 x [2 x double]]* %mat1_ptr, i64 0, i64 %i_4
  %k_1 = load i32, i32* %kptr
  %k_2 = sext i32 %k_1 to i64
  %mat1_itemptr = getelementptr inbounds [2 x double], [2 x double]* %mat1_rowptr, i64 0, i64 %k_2
  %mat1_item = load double, double* %mat1_itemptr ; %34

  %k_3 = load i32, i32* %kptr
	%k_4 = sext i32 %k_3 to i64
	%mat2_rowptr = getelementptr inbounds [2 x [2 x double]], [2 x [2 x double]]* %mat2_ptr, i64 0, i64 %k_4
  %j_3 = load i32, i32* %jptr
  %j_4 = sext i32 %j_3 to i64
  %mat2_itemptr = getelementptr inbounds [2 x double], [2 x double]* %mat2_rowptr, i64 0, i64 %j_4
  %mat2_item = load double, double* %mat2_itemptr ; %41

  %mulres = fmul double %mat1_item, %mat2_item
  %i_5 = load i32, i32* %iptr
	%i_6 = sext i32 %i_5 to i64
	%matres_rowptr_1 = getelementptr inbounds [2 x [2 x double]], [2 x [2 x double]]* %matres_ptr, i64 0, i64 %i_6
  %j_5 = load i32, i32* %jptr
  %j_6 = sext i32 %j_5 to i64
  %matres_itemptr_1 = getelementptr inbounds [2 x double], [2 x double]* %matres_rowptr_1, i64 0, i64 %j_6
  %matres_item = load double, double* %matres_itemptr_1
  %sumres = fadd double %matres_item, %mulres
  store double %sumres, double* %matres_itemptr_1

  ; 51
  %k_5 = load i32, i32* %kptr
  %k_6 = add i32 %k_5, 1
  store i32 %k_6, i32* %kptr
  br label %mm_k_cond

mm_k_next: ; 54
  ; 55
  %j_7 = load i32, i32* %jptr
  %j_8 = add i32 %j_7, 1
  store i32 %j_8, i32* %jptr
  br label %mm_j_cond

mm_j_next: ; 58
  ; 59
  %i_7 = load i32, i32* %iptr
  %i_8 = add i32 %i_7, 1
  store i32 %i_8, i32* %iptr
  br label %mm_i_cond

mm_i_next: ; 62
  store i32 0, i32* %iptr
  br label %pr_i_cond

pr_i_cond: ; 63
  %i_9 = load i32, i32* %iptr
  %cc_3 = icmp slt i32 %i_9, 2
  br i1 %cc_3, label %pr_i_body, label %pr_i_next

pr_i_body: ; 66
  store i32 0, i32* %jptr
  br label %pr_j_cond

pr_j_cond: ; 67
  %j_9 = load i32, i32* %jptr
  %cc_4 = icmp slt i32 %j_9, 2
  br i1 %cc_4, label %pr_j_body, label %pr_j_next

pr_j_body: ; 70
  %i_10 = load i32, i32* %iptr
	%i_11 = sext i32 %i_10 to i64
	%matres_rowptr_2 = getelementptr inbounds [2 x [2 x double]], [2 x [2 x double]]* %matres_ptr, i64 0, i64 %i_11
  %j_10 = load i32, i32* %jptr
  %j_11 = sext i32 %j_10 to i64
  %matres_itemptr_2 = getelementptr inbounds [2 x double], [2 x double]* %matres_rowptr_2, i64 0, i64 %j_11
  %matres_item_1 = load double, double* %matres_itemptr_2
  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @fmtitem, i64 0, i64 0), double %matres_item_1)
  ; 79
  %j_12 = load i32, i32* %jptr
  %j_13 = add i32 %j_12, 1
  store i32 %j_13, i32* %jptr
  br label %pr_j_cond ; 67

pr_j_next: ; 82
  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @fmtnl, i64 0, i64 0))

  ; 84
  %i_12 = load i32, i32* %iptr
  %i_13 = add i32 %i_12, 1
  store i32 %i_13, i32* %iptr
  br label %pr_i_cond

pr_i_next: ; 87
  ret i32 0
}

declare i32 @printf(i8*, ...)
declare void @llvm.memcpy.p0i8.p0i8.i64(i8*, i8*, i64, i1)
