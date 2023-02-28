fn main() {
    println!("cargo:rerun-if-changed=src/ffi.rs");

    cxx_build::bridge("mime-rs/ffi.rs")
        .includes(["c++/vendor/immer", "c++/include"])
        .file("c++/src/lib/mime.cc")
        .warnings(false)
        .flag_if_supported("-std=c++17")
        .opt_level(3)
        .compile("mime");
}
