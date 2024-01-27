use std::path::Path;
// use cxx_build::CFG;


fn get_abs_path(path: &str) {
    if let Ok(abs_path) = Path::new(path).canonicalize() {
        println!("Absolute path({}) is: {}", path, abs_path.display());
    } else {
        println!("Failed to get absolute path");
    }
}

fn main() {
    #[cfg(debug_assertions)]
    {
        get_abs_path("./");
        get_abs_path("src");
        get_abs_path("src/strategy.rs");
        get_abs_path("../../core/include/core");
        get_abs_path("./../CppApi/strategy.hpp");
        get_abs_path("./../CppApi");
        get_abs_path("./../../core/3rd/spdlog/include");
        get_abs_path("./../build/core");
    }

    // CFG.include_prefix = "../../core/include/core";
    cxx_build::bridge("./src/strategy.rs")
        .file("./../CppApi/strategy.hpp")
        .include("./../CppApi")
        .include("./../../core/include")
        .include("./../../core/3rd/spdlog/include")
        .flag("-std=c++17")
        .flag("-L/./../build/core")
        .flag("-lcore")
        .compile("RustApi");

    println!("cargo:rerun-if-changed=src/strategy.rs");
    println!("cargo:rerun-if-changed=./../CppApi/strategy.hpp");
}