/// HUST OS Lab2 Implementation in Rust

use std::{
    thread,
    time,
    sync::{Mutex, Arc},
};

const THREAD_NUM: i32 = 5;
const TICKET_NUM: i32 = 100;

fn main() {
    println!("Welcome to hust os lab in rust");
    let tickets = Arc::new(Mutex::new(TICKET_NUM));
    let mut handles = vec![];
    for i in 0..THREAD_NUM {
        let tickets = Arc::clone(&tickets);
        let handle = thread::spawn(move || {
            let mut buy_tickets = 0;
            loop {
                {
                    let mut ticket = tickets.lock().unwrap();
                    if *ticket <= 0 {
                        break;
                    }
                    *ticket -= 1;
                    buy_tickets += 1;
                    println!("Thread {} buy a ticket, remain {} tickets.", i, *ticket);
                    }
                // let sleep_time = time::Duration::from_millis(100);
                // thread::sleep(sleep_time);
            }
            println!("Thread {} exit with {} tickets", i, buy_tickets);
        });
        handles.push(handle);
    }
    for handle in handles {
        handle.join().unwrap();
    }
    println!("Main thread exit, remain {} tickets.", *tickets.lock().unwrap());
}
