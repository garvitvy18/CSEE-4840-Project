module ppu_top(

    input logic clk,
    input logic reset,
    input logic [31:0] write_data,
    input logic write,
    input chipselect,
    output logic irq,

    output logic [7:0] VGA_R, VGA_G, VGA_B,
    output logic VGA_CLK, VGA_HS, VGA_VS, VGA_BLANK_n, VGA_SYNC_n
);

    PPU_asm asm (
       
    );

    tile_buffer tile_buffer_mem (
        .clk(),
        .reset(),
        .rw(),
        .write_data(),
        .addr(),
        .read_data()
    );

    tile_graphics tile_graphics_mem (
        .clk(),
        .reset(),
        .rw(),
        .write_data(),
        .addr(),
        .read_data()
    );

    sprite_graphics sprite_graphics_mem (
        .clk(),
        .reset(),
        .rw(),
        .write_data(),
        .addr(),
        .read_data()
    );

    color_palettes color_palettes_mem (
        .clk(),
        .reset(),
        .rw(),
        .write_data(),
        .addr(),
        .read_data()
    );

    OAM oam_mem (
        .clk(),
        .reset(),
        .rw(),
        .write_data(),
        .addr(),
        .read_data()
    );

    combined_priority_encoder priority_encoder (
        .pixel_data_in(),
        .palette_data_in(),
        .pixel_data_out(),
        .palette_data_out()
    );

    shift_register_block shift_regs (
        .load_data(),
        .enable(),
        .clk(),
        .reset(),
        .load(),
        .out_data()
    );

    vga vga_cont (
        .clk(),
        .reset(),
        .pixel_color(),
        .
    );



endmodule