fn main() {
    cxx_build::bridge("src/main.rs")
        .file("./../CppApi/strategy.hpp")
        .include("./../../core/include")
        .include("./../../core/3rd/spdlog/include")
        .flag("-std=c++17")
        .compile("RustApi");

    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=./../CppApi/strategy.hpp");
}