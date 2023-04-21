#[cfg(feature = "debug")]
extern crate bindgen;

use cc::Build;
#[cfg(feature = "debug")]
use std::env;

use std::path::PathBuf;


/// 生成C函数支持
///
#[cfg(all(feature = "fifo"))]
fn fifo_build_link(c: &PathBuf) {
    let function_path = c.join("fifo.cpp");
    Build::new()
        .file(function_path.clone())
        .cpp(true)
        .flag("-fno-strict-aliasing")
        .warnings(true)
        .out_dir(c)
        .compile("fifo");
}

/// 生成可以快速调用得映射函数绑定
///
#[cfg(all(feature = "debug", feature = "fifo"))]
fn fifo_build_bindings(c: &PathBuf) {
    let header_path = c.join("fifo.hpp");
    let bindings = bindgen::Builder::default()
        .header(header_path.to_string_lossy().to_string())
        .derive_default(true)
        .derive_debug(true)
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result or panic on failure.
        .expect("Unable to generate bindings");
    // Write the bindings to the $OUT_DIR/fifi_bindings file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("fifo_bindings"))
        .expect("Couldn't write bindings!");
}

fn main() {
    let c = PathBuf::from(std::env::var("CARGO_MANIFEST_DIR").unwrap()).join("linked_build");

    #[cfg(feature = "fifo")]
    fifo_build_link(&c);

    #[cfg(all(feature = "debug", feature = "fifo"))]
    fifo_build_bindings(&c);

    // 指定c++的版本
    #[cfg(feature = "fifo")]
    println!("cargo:rustc-link-search=native=/usr/lib/gcc/x86_64-redhat-linux/11/");

    #[cfg(feature = "fifo")]
    println!("cargo:rustc-link-lib=stdc++");

    #[cfg(feature = "fifo")]
    println!("cargo:rustc-link-search=native={}", c.to_string_lossy().to_string());

    #[cfg(feature = "fifo")]
    println!("cargo:rustc-link-lib=fifo");

    #[cfg(feature = "fifo")]
    println!("cargo:rustc-link-lib=boost_std");
}