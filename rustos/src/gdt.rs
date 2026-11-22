// CyberLab OS - Global Descriptor Table (GDT)
// Configures Flat 32-bit Memory Model for Kernel Code & Data Segments

#[repr(C, packed)]
pub struct GdtEntry {
    limit_low: u16,
    base_low: u16,
    base_middle: u8,
    access: u8,
    granularity: u8,
    base_high: u8,
}

#[repr(C, packed)]
pub struct GdtPtr {
    limit: u16,
    base: u32,
}

static mut GDT: [GdtEntry; 3] = [
    // 0x00: Null Descriptor
    GdtEntry {
        limit_low: 0,
        base_low: 0,
        base_middle: 0,
        access: 0,
        granularity: 0,
        base_high: 0,
    },
    // 0x08: Kernel Code Segment (Base=0, Limit=4GB, Executable/Read, Ring 0)
    GdtEntry {
        limit_low: 0xFFFF,
        base_low: 0,
        base_middle: 0,
        access: 0x9A,      // Present, Ring 0, Code, Executable, Readable
        granularity: 0xCF, // 4KB granularity, 32-bit protected mode
        base_high: 0,
    },
    // 0x10: Kernel Data Segment (Base=0, Limit=4GB, Read/Write, Ring 0)
    GdtEntry {
        limit_low: 0xFFFF,
        base_low: 0,
        base_middle: 0,
        access: 0x92,      // Present, Ring 0, Data, Read/Write
        granularity: 0xCF, // 4KB granularity, 32-bit protected mode
        base_high: 0,
    },
];

static mut GDT_PTR: GdtPtr = GdtPtr {
    limit: 0,
    base: 0,
};

pub unsafe fn init() {
    GDT_PTR.limit = (core::mem::size_of::<[GdtEntry; 3]>() - 1) as u16;
    GDT_PTR.base = GDT.as_ptr() as u32;

    core::arch::asm!(
        "lgdt [{0}]",
        "mov ax, 0x10",
        "mov ds, ax",
        "mov es, ax",
        "mov fs, ax",
        "mov gs, ax",
        "mov ss, ax",
        "ljmp 0x08, 2f",
        "2:",
        in(reg) &GDT_PTR,
        options(att_syntax)
    );
}
