/// HUST OS Lab4 Implementation in Rust
/// 
extern crate libc;
use std::{fs, io, path::PathBuf};
use libc::c_char;
use std::ffi::CString;

extern {
    // fn hello();
    #[allow(unused_variables)]
    fn print_stat(path: *const c_char);
}
fn main() -> io::Result<()>{
    println!("Welcome to hust os lab in rust");
    
    let mut entries = fs::read_dir("./")?
        .map(|res| res.map(|e| e.path()))
        .collect::<Result<Vec<_>, io::Error>>()?;

    let mut files = Vec::new();
    // The order in which `read_dir` returns entries is not guaranteed. If reproducible
    // ordering is required the entries should be explicitly sorted.

    entries.sort();
    walk(&entries, &mut files);
    for file in files {
        unsafe { print_stat(CString::new(file.as_str()).unwrap().as_ptr()); }
        // println!("{}", file);
    }

    // unsafe {
    //     hello();
    // }
    Ok(())
}

fn walk(entries: &Vec<PathBuf>, files: &mut Vec<String>) {
    for entry in entries {
        if entry.is_dir() {
            let mut new_entries = fs::read_dir(entry.to_str().unwrap()).unwrap()
                .map(|res| res.map(|e| e.path()))
                .collect::<Result<Vec<_>, io::Error>>().unwrap();
            new_entries.sort();
            walk(&new_entries, files);
        }
        files.push(String::from(entry.to_str().unwrap()));
        // println!("{}", entry.to_str().unwrap());
    }
}