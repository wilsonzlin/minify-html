pub struct CodeOutOfPlace<'src, 'out> {
    src: &'src [u8],
    src_next: usize,

    out: &'out mut [u8],
    out_next: usize,
}

impl Code for CodeOutOfPlace {

}
