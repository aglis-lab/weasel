# THREAD CALL

We use posix thread. To create thread we simply just the function needed to be called as a thread process.

```
fun Test() {
  // Do something heavy
}

fun TestWithParams(age int) {
  // Do something heavy
}

fun main() {
  // Without Parameters
  let runningThread1 = Thread(Test)

  // With Parameters
  let age = 12
  let runningThread2 = Thread(fun() {
    TestWithParams(age)
  })

  // Wait Thread to finish
  runningThread1.await
  runningThread2.await

  // Or Use Multiple await
  Thread::all([
    runningThread1,
    runningThread2
  ]).await
}
```
