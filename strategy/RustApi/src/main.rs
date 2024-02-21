#[cxx::bridge]
pub mod ffi {
    extern "Rust" {

    }

    unsafe extern "C++" {
        include!("RustApi/src/test.hpp");
        type Test;

        fn new_test() -> UniquePtr<Test>;
        fn print(&self) -> ();
    }
}

fn main() {
    let test = ffi::new_test();
    test.print();
}
