extern crate cc;

fn main() {
    cc::Build::new()
        .file("src/unix_fs.c")
        .compile("libunixfs.a");
}