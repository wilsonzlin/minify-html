use minify_html::{minify, Cfg};

fn main() {
    let html = b"<div onClick=\"test()\" className=\"foo\" dataTest=\"bar\">Test</div>";
    
    // Test with preserve_attribute_case = false (default)
    let mut cfg_default = Cfg::new();
    cfg_default.preserve_attribute_case = false;
    let result_default = minify(html, &cfg_default);
    println!("With preserve_attribute_case=false:");
    println!("{}", String::from_utf8_lossy(&result_default));
    
    // Test with preserve_attribute_case = true
    let mut cfg_preserve = Cfg::new();
    cfg_preserve.preserve_attribute_case = true;
    let result_preserve = minify(html, &cfg_preserve);
    println!("\nWith preserve_attribute_case=true:");
    println!("{}", String::from_utf8_lossy(&result_preserve));
}
