; ModuleID = 'my cool jit'
source_filename = "my cool jit"

@global1 = global double 2.000000e+00
@0 = private unnamed_addr constant [36 x i8] c"the value of global variable is %f\0A\00", align 1
@1 = private unnamed_addr constant [36 x i8] c"the value of global variable is %f\0A\00", align 1
@2 = private unnamed_addr constant [36 x i8] c"the value of global variable is %f\0A\00", align 1
@3 = private unnamed_addr constant [36 x i8] c"the value of global variable is %f\0A\00", align 1
@4 = private unnamed_addr constant [36 x i8] c"the value of global variable is %f\0A\00", align 1

define void @sub() {
entry:
  %0 = load double, double* @global1
  %subop = fsub double %0, 1.000000e+00
  store double %subop, double* @global1
  ret void
}

define void @add() {
entry:
  %0 = load double, double* @global1
  %addop = fadd double %0, 1.000000e+00
  store double %addop, double* @global1
  ret void
}

declare i32 @printf(i8*, ...)

declare i8* @printf.1(i8*, ...)

define void @main() {
entry:
  %0 = load double, double* @global1
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @0, i32 0, i32 0), double %0)
  call void @add()
  %2 = load double, double* @global1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @1, i32 0, i32 0), double %2)
  call void @add()
  %4 = load double, double* @global1
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @2, i32 0, i32 0), double %4)
  call void @sub()
  %6 = load double, double* @global1
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @3, i32 0, i32 0), double %6)
  call void @sub()
  %8 = load double, double* @global1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @4, i32 0, i32 0), double %8)
  ret void
}
