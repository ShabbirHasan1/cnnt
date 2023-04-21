# cnnt

> 科楠科技 FPGA行情 rust FFI 轮询demo

### Tips

- code_stack可能使用NoHashMap性能更高
- 要生成绑定得bindings,需要打开`feature`里面得 `debug`,然后在./target/debug/build/cnnt/out/下面找到`fifo_bindings.rs`

### 项目原理

利用提供的头文件以及转写`c函数`来提供可供rust调用的`API`,参见./linked_build/下面的文件

### 如何运行

由于cargo一般安装在用户, 但是FPGA需要sudo权限运行, 所以此处使用以下命令使用

```bash
cargo build  --release --example fifo && sudo taskset -c 2,3 ./target/release/examples/fifo
```
