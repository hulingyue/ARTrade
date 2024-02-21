fn main() {
    cxx_build::bridge("src/main.rs")
        .file("src/test.hpp")
        .flag_if_supported("-std=c++17")
        .compile("RustApi");

    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/test.hpp");
}