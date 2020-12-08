# <center>华中科技大学计算机科学与技术学院</center>  <center>操作系统实验报告</center>  <center>共享内存与进程同步</center>  
<center>计科校交 1801</center>  <center>车春池</center>  <center>U201816030</center> 
<center>指导老师：郑然</center>  
<center>2020-12-08</center>

## 实验目的
+ 掌握 Linux 下共享内存的概念与使用方法
+ 掌握环形缓冲的结构与使用方法
+ 掌握 Linux 下进程同步与通信的主要机制

## 实验内容
+ 利用多个共享内存构成的环形缓冲，将源文件复制到目标文件，实现两个进程的眷抄
+ 对下列参数设置不同的取值，统计程序并发执行的个体和总体执行时间，分析不同设置对缓冲结果和进程并发执行的性能影响，并分析其原因：信号灯的设置，缓冲区的个数和进程执行的相对速度

## 实验设计
### 开发环境
Linux 发行版 Manjaro 系统  
<img src = "../img/screenfetch.png" width = "70%">  

### 实验设计
本实验将使用现代系统语言 Rust 语言完成。  
#### Rust 语言简介
Rust 是由 Mozilla 主导开发的通用、编译型编程语言。设计准则为“安全、并发、实用”，支持函数式、並行式、程序式以及面向对象的编程风格。  
Rust 语言的本质在于`赋能`，无论你现在编写的是何种代码，Rust 能让你在更为广泛的编程领域走得更远。  
比如`系统层面`的工作，涉及内存管理，数据表示和并发等底层细节。从传统角度来看，这是一个神秘的编程领域，只为浸淫多年的极少数人所触及，也只有他们能避开那些臭名昭著的陷阱。即使再谨慎的实践者，亦唯恐代码出现漏洞，崩溃或损坏。  
Rust 破除了这些障碍，它通过所有权机制和生命周期和其他一些语言特性消除了旧的陷阱并提供了伴你一路同行的友好，精良的工具。想要深入底层控制的程序员可以使用 Rust，无需冒着常见的崩溃或安全漏洞的风险，也无需学习时常改变的工具链的最新知识，其语言本身更是被设计为自然而然的引导你编写出在运行速度和內存使用上都十分高效的可靠代码。  
Rust 并不局限于底层系统编程，其表现力和工效足于令人愉悦地写出 CLI 应用， web server 和很多其他类型的代码。  
#### 无畏并发
在大部分现代操作系统中，已执行的代码在一个`进程`中运行，操作系统则负责管理多个进程。在程序内部，也可以拥有多个同时运行的独立部分。运行这些独立部分的功能被称为`线程`（Thread）。  
将程序中的计算拆分为多个线程可以改善性能，因为程序可以同时进行多个任务，不过这也会增加复杂性。因为线程是同时运行的，所以无法预先保证不同线程中的代码的执行顺序，因此会导致诸如此类的问题：  
+ 竞争
+ 死锁
+ 不可重现 bug

Rust 尝试减轻使用线程的负面影响。不过在多线程上下文中编程仍需格外小心，同时其所要求的代码结构也不同于运行单线程的程序。  
很多编程语言提供了自己特殊的线程实现。编程语言提供的线程称为`绿色线程`，使用绿色线程的语言会在不同数量的 OS 线程的上下文去执行它们。  
Rust 为并发编程提供了消息传递和互斥锁的支持，这使得我们可以很方便地在进程之间通信和解决互斥问题。另外 Rust 的所有权机制和生命周期机制保证了这种并发是绝对安全的，很少出现崩溃现象，因此称为`无畏并发`。  

#### 实验的 Rust 代码实现
共享内存是进程间通信中最简单的方式之一。共享内存允许两个或更多进程访问同一块内存，当一个进程改变了这块内存空间中的内容的时候，其他进程都会察觉到这个改变。这属于消息传递的一种方式。  
虽然消息传递是一个很好的处理并发的方式，但并不是唯一一个。`Go`语言编程文档中有这样一句“do not communicate by sharing memory.”，不要通过共享内存来通讯。  
在某种程度上，任何编程语言中的通道都类似于单所有权，因为一旦将一个值送到通道中，将无法再使用这个值。共享内存类似于多所有权，多个线程可以同时访问相同的内存位置，虽然 Rust 可以通过智能指针使得多所有权成为可能，但这会增加额外的复杂性，因为需要以某种方式管理这些不同的所有者。Rust 的类型系统和所有权规则极大的协助了正确地管理这些所有权。  
下面将简单介绍一下在 Rust 语言中是如何进行并发编程和实现互斥效果的。  

##### 使用 spawn 创建新线程
为了创建一个新线程，需要调用`thread::spawn`函数并传递一个闭包：  
```Rust
fn main() {
    thread::spawn(|| {
        for i in 1..10 {
            println!("hi number {} from the spawned thread!", i);
            thread::sleep(Duration::from_millis(1));
        }
    });

    for i in 1..5 {
        println!("hi number {} from the main thread!", i);
        thread::sleep(Duration::from_millis(1));
    }
}
```
每次运行结果都会有所不同，不过大体上看起来像这样：  
```
hi number 1 from the main thread!
hi number 1 from the spawned thread!
hi number 2 from the main thread!
hi number 2 from the spawned thread!
hi number 3 from the main thread!
hi number 3 from the spawned thread!
hi number 4 from the main thread!
hi number 4 from the spawned thread!
hi number 5 from the spawned thread!
```
`thread::sleep`调用强制线程停止执行一小段时间，这会允许其他不同的线程运行。这些线程可能会轮流运行，不过并不保证如此：这依赖操作系统如何调度线程。在这里，主线程首先打印，即便新创建线程的打印语句位于程序的开头，甚至即便我们告诉新建的线程打印直到 i 等于 9 ，它在主线程结束之前也只打印到了 5。  

##### 使用 join 等待所有线程结束
在上面的代码中，如果主线程结束了，子线程也会随之结束，我们可以通过调用`thread::spawn`返回的`JoinHandle`类型的`join`方法来确保新线程在`main`退出前结束运行：  
```Rust
fn main() {
    let handle = thread::spawn(|| {
    ...... // 中间代码省略
    handle.join().unwrap();
}
```

##### 互斥器 Mutex<T>
互斥器在任何时刻只允许一个线程访问某些数据，为了访问互斥器中的数据，线程首先需要获取互斥器的锁来表明其希望访问数据。互斥器通过锁系统来保护其数据。  
Example：  
```Rust
fn main() {
    let m = Mutex::new(5);

    {
        let mut num = m.lock().unwrap();
        *num = 6;
    }

    println!("m = {:?}", m);
}
```

##### 原子引用计数 Arc<T>
为了在线程之间共享所有权，达到无畏并发的目的，Rust 提供了原子引用计数的概念。关于这部分的概念涉及到很多 Rust 语言相关知识，因此这里只给出例子，详细的内容可以移步《Rust 程序设计语言 简体中文版》。  
```Rust
fn main() {
    let counter = Arc::new(Mutex::new(0));
    let mut handles = vec![];

    for _ in 0..10 {
        let counter = Arc::clone(&counter);
        let handle = thread::spawn(move || {
            let mut num = counter.lock().unwrap();

            *num += 1;
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }

    println!("Result: {}", *counter.lock().unwrap());
}
```

运行结果：  
```
Result: 10
```

## 实验调试
### 实验步骤
#### 新建项目
使用`Cargo`工具新建 Rust 项目：  
```Shell
cargo new --bin shared_memory
```
#### 编写代码
基于以下 Rust 库实现满足实验要求的代码：  
+ io: 输入输出
+ fs: 文件
+ thread: 线程
+ sync: 并发
+ time: 时间

实现效果是从一个文件读取文本数据同时写入到另一个文件，读线程负责读，写线程负责写，两个线程并发执行。  
#### 互斥与同步
在调试过程中，遇到过两个问题：  
+ 如何实现两个线程的互斥
+ 如何实现两个线程的同步

第一个问题通过 Rust 语言中的互斥器`Mutex<T>`很好的解决了。  
而第二个问题，我想到了几个办法，其中一个是通过`channel`来传递缓冲区是否空或满的消息。然而之前提到过消息传递不是线程之间通信的唯一办法，并且应该尽量避免使用这种办法，因为其增加了 CPU 的开销。  
我们可以设想一个这样的情景：你面前有一个盖着的木桶，你想知道这个木桶里面的状态，是没水，有水还是满了，你会怎么做？很简单的一个办法就是去把盖子揭开，看一眼就行了。  
回到这个实验中，我们可以这样做：读线程想要从缓冲区里面写数据的时候，先获得这个缓冲区的锁，然后判断这个缓冲区是否已经满了，如果没有，就往里面写数据，如果满了，就什么也不做。写线程也是类似的操作，只不过它是判断缓冲区是否为空，不为空即从里面读取数据而已。  
这样一来也可以解决同步问题。当然肯定有其他解决办法，在本实验中我是像前面说得那样做的。  

#### 运行结果
```
Welcome to hust os lab in rust
[Reader]read line: #include "include/types.h"

[Reader]read line: #include "include/param.h"

[Reader]read line: #include "include/memlayout.h"

[Writer]write: #include "include/types.h"

[Writer]write: #include "include/param.h"

[Writer]write: #include "include/memlayout.h"

[Reader]read line: #include "include/riscv.h"

[Reader]read line: #include "include/defs.h"

[Writer]write: #include "include/riscv.h"

[Writer]write: #include "include/defs.h"

[Reader]read line: #include "include/sbi.h"

[Writer]write: #include "include/sbi.h"

[Reader]read line: #include "include/sdcard.h"

[Writer]write: #include "include/sdcard.h"

[Reader]read line: #include "include/fpioa.h"

[Reader]read line: 

[Writer]write: #include "include/fpioa.h"

[Writer]write: 

[Reader]exit.
[Writer]exit.
[Main_thread]exit, cost 434 micro-seconds
```
### 实验调试及心得
本次实验我完全使用 Rust 语言实现，之所以这么做，是因为想进行一些新的尝试。如果使用 C 语言的话，我当然也能完成这次实验，但是这比起操作系统实验，更像是 C 语言实验，
根本没有想要写的动力。因此我尝试用 Rust 语言来完成这一次实验，并尽可能地满足实验要求，贴近实验目的。最终尽管有些地方没有办法，大部分实验要求都能得到满足。  
我在写实验的时候，经常会思考 Rust 语言和 C 语言之间的区别，在本实验中，Rust 提供的各种语言机制在设计和实现两个方面都明显优于 C 语言，写出的 Rust 的代码即简洁性能又高。  
我现在还是在学生时代，想要尽可能地去接触一些新的东西。  

## 实验代码
```Rust
/// HUST OS Lab3 Implementation in Rust
#[allow(unused_imports)]
use std::{
    io::{self, prelude::*, BufReader, BufWriter},
    fs::File,
    thread,
    sync::{Mutex, Arc},
    time::{Instant, Duration},
};
fn main() -> io::Result<()>{
    println!("Welcome to hust os lab in rust");

    let buffer = Arc::new(Mutex::new(Vec::new()));
    let eof = Arc::new(Mutex::new(0));
    let read_buf = Arc::clone(&buffer);
    let write_buf = Arc::clone(&buffer);
    let eof_1 = Arc::clone(&eof);
    let eof_2 = Arc::clone(&eof);
    let now = Instant::now();

    let reader_handle = thread::spawn(move || {
        let f_read = File::open("source.txt").unwrap();
        let mut reader = BufReader::new(f_read);
        loop {
            {
                let mut buf = read_buf.lock().unwrap();
                let mut line = String::new();
                match reader.read_line(&mut line) {
                    Ok(_) => {
                        if line.is_empty() {
                            break;
                        }
                        println!("[Reader]read line: {}", line);
                    },
                    Err(_) => break,
                }
                buf.push(line.clone());
            }
            
            // thread::sleep(Duration::from_millis(10));
        }
        *eof_1.lock().unwrap() = 1;
        println!("[Reader]exit.");
    });
    
    let writer_handle = thread::spawn(move || {
        let f_write = File::create("destination.txt").unwrap();
        let mut writer = BufWriter::new(f_write);
        loop {
            {
                let mut buf = write_buf.lock().unwrap();
                if buf.is_empty() {
                    if *eof_2.lock().unwrap() == 1 {
                        break;
                    } else {
                        continue;
                    }
                }
                let line = buf.remove(0);
                writer.write(&line.as_bytes()).unwrap();
                println!("[Writer]write: {}", line);
            }       
            // thread::sleep(Duration::from_millis(10));
        }
        println!("[Writer]exit.");
    });

    reader_handle.join().unwrap();
    writer_handle.join().unwrap();
    println!("[Main_thread]exit, cost {} micro-seconds", now.elapsed().as_micros());
    Ok(())
}
```