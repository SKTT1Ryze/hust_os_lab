/// HUST OS Lab2 Implementation in Rust

use std::{
    thread,
    sync::{Mutex, Arc},
};

fn main() {
    println!("Welcome to hust os lab in rust");
    let counter = Arc::new(Mutex::new(0));
    let counter_1 = Arc::clone(&counter);
    let counter_2 = Arc::clone(&counter);
    let counter_3 = Arc::clone(&counter);
    
    let handle_2 = thread::spawn(move || {
        loop {
            let count = counter_2.lock().unwrap();
            if *count > 100 {
                break;
            }
            if *count % 2 == 0 {
                println!("[Print_thread_1]counter: {}", *count);
            }
            if *count == 100 {
                break;
            }
        }
        println!("[Print_thread_1]exit.");
    });

    let handle_3 = thread::spawn(move || {
        loop {
            let count = counter_3.lock().unwrap();
            if *count >= 100 {
                break;
            }
            if *count % 2 != 0 {
                println!("[Print_thread_2]counter: {}", *count);
            }
        }
        println!("[Print_thread_2]exit.");
    });

    let handle_1 = thread::spawn(move || {
        loop {
            let mut count = counter_1.lock().unwrap();
            if *count >= 100 {
                break;
            }
            *count += 1;
        }
        println!("[Calculate_thread]exit.");
    });

    

    handle_1.join().unwrap();
    handle_2.join().unwrap();
    handle_3.join().unwrap();
    println!("[Main_thread]exit.");
}
