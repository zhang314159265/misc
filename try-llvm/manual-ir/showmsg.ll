@msg = constant [13 x i8] c"LLVM is COOL!"

declare i32 @puts(i8*)

define i32 @main() {
  call i32 @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @msg, i64 0, i64 0))
  ret i32 0
}
