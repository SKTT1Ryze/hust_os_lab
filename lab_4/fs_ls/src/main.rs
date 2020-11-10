/// HUST OS Lab4 Implementation in Rust
/// 
use std::{fs, io, path::PathBuf};
use unix::file::{stat_at, AtFlags};
fn main() -> io::Result<()>{
    println!("Welcome to hust os lab in rust");
    
    let mut entries = fs::read_dir("/home/hustccc/xv6-k210")?
        .map(|res| res.map(|e| e.path()))
        .collect::<Result<Vec<_>, io::Error>>()?;

    // The order in which `read_dir` returns entries is not guaranteed. If reproducible
    // ordering is required the entries should be explicitly sorted.

    entries.sort();
    walk(&entries);
    Ok(())
}

fn walk(entries: &Vec<PathBuf>) {
    for entry in entries {
        if entry.is_dir() {
            let mut new_entries = fs::read_dir(entry.to_str().unwrap()).unwrap()
                .map(|res| res.map(|e| e.path()))
                .collect::<Result<Vec<_>, io::Error>>().unwrap();
            new_entries.sort();
            walk(&new_entries);
        }
        // println!("{}", entry.to_str().unwrap());
        let stat = stat_at(None, entry.to_str().unwrap(), AtFlags::Follow);
        println!("{:?}", stat);

    }
}