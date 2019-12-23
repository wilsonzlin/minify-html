pub struct CodeInPlace<'data> {
    data: &'data mut [u8],
    read_next: usize,
    // Offset of the next unwritten space.
    write_next: usize,
}

impl Code for CodeInPlace {

}
