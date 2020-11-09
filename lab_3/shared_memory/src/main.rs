/// HUST OS Lab3 Implementation in Rust
use std::{
    io::{self, prelude::*, BufReader, BufWriter},
    fs::File,
    thread,
    sync::{Mutex, Arc},
    // time::Duration,
};
fn main() -> io::Result<()>{
    println!("Welcome to hust os lab in rust");

    let buffer = Arc::new(Mutex::new(Vec::new()));
    let eof = Arc::new(Mutex::new(0));
    let read_buf = Arc::clone(&buffer);
    let write_buf = Arc::clone(&buffer);
    let eof_1 = Arc::clone(&eof);
    let eof_2 = Arc::clone(&eof);
    
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
    println!("[Main_thread]exit.");
    Ok(())
}
