define void @add(i32* %ptr1, i32* %ptr2, i32* %val) {
entry:
  %0 = load i32, i32* %val
  %1 = load i32, i32* %ptr1
  %2 = add nsw i32 %1, %0
  store i32 %2, i32* %ptr1
  %3 = load i32, i32* %ptr2
  %4 = add nsw i32 %3, %0
  store i32 %4, i32* %ptr2
  ret void
}
