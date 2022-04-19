define double @sum(double* %ptr, i32 %n) {
entry:
  %result = alloca double ; double result
  store double 0.0, double* %result ; result = 0.0
  br label %comp1

comp1:
  %i = phi i32 [0, %entry], [%12, %ipp]
  %0 = sub nsw i32 %n, 1 ; n - 1
  %1 = icmp slt i32 %i, %0 ; i < n - 1
  %2 = add nsw i32 %i, 1 ; i + 1
  br i1 %1, label %comp2, label %comp4

comp2:
  %j = phi i32 [%2, %comp1], [%11, %jpp]
  %3 = icmp slt i32 %j, %n ; j < n
  br i1 %3, label %comp3, label %ipp

comp3:
  %4 = getelementptr inbounds double, double* %ptr, i32 %i ; ptr + i
  %5 = load double, double* %4 ; ptr[i]
  %6 = fmul double %5, %5 ; ptr[i] * ptr[i]
  %7 = getelementptr inbounds double, double* %ptr, i32 %j ; ptr + j
  %8 = load double, double* %7 ; ptr[j]
  %9 = fmul double %8, %8 ; ptr[j] * ptr[j]
  %10 = fcmp olt double %6, %9 ; ptr[i] * ptr[i] < ptr[j] * ptr[j]
  br i1 %10, label %swap, label %jpp

swap:
  store double %8, double* %4 ; ptr[i] = ptr[j]
  store double %5, double* %7 ; ptr[j] = ptr[i]
  br label %jpp

jpp:
  %11 = add nsw i32 %j, 1 ; j + 1
  br label %comp2

ipp:
  %12 = add nsw i32 %i, 1 ; i + 1
  br label %comp1

comp4:
  %k = phi i32 [0, %comp1], [%18, %acc]
  %13 = icmp slt i32 %k, %n ; i < n
  %14 = load double, double* %result ; result
  br i1 %13, label %acc, label %end

acc:
  %15 = getelementptr inbounds double, double* %ptr, i32 %k ; ptr + k
  %16 = load double, double* %15 ; ptr[k]
  %17 = fadd double %14, %16 ; result + ptr[k]
  store double %17, double* %result ; result = result + ptr[k]
  %18 = add i32 %k, 1 ; k + 1
  br label %comp4

end:
  ret double %14 ; return result
}
