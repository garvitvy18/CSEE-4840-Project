module ppu_top(

    input logic clk,
    input logic reset,
    input logic [31:0] write_data,
    input logic [11:0] address,
    input logic write,
    input chipselect,
    output logic irq,

    output logic [7:0] VGA_R, VGA_G, VGA_B,
    output logic VGA_CLK, VGA_HS, VGA_VS, VGA_BLANK_n, VGA_SYNC_n
);

      addr_decode decoder(
        .addr(),
        .write_data(),
        .chip_select(),
        .rw_tile_buffer(), 
        .rw_tile_graphics(), 
        .rw_sprite_graphics(), 
        .rw_color_palettes(), 
        .rw_OAM(),
        .write_data_tile_buffer(), 
        .write_data_tile_graphics(), 
        .write_data_sprite_graphics(), 
        .write_data_OAM(),
        .write_data_color_palettes(),
        .addr_tile_buffer(),
        .addr_tile_graphics(), 
        .addr_sprite_graphics(),
        .addr_color_palettes(),
        .addr_OAM()
    );


    tile_buffer tile_buffer_mem (
        .clk(), 
        .reset(), 
        .rw_1(),
        .rw_2(),
        .write_data_1(), 
        .write_data_2(),
        .addr_1(), 
        .addr_2(),
        .read_data_1(), 
        .read_data_2()
    );

    tile_graphics tile_graphics_mem (
        .clk(), 
        .reset(), 
        .rw_1(), 
        .rw_2(),
        .write_data_1(), 
        .write_data_2(),
        .addr_1(),
        .addr_2(),
        .read_data_1(), 
        .read_data_2()
    );


    sprite_graphics sprite_graphics_mem (
        .clk(),
        .reset(), 
        .rw_1(), 
        .rw_2(),
        .write_data_1(), 
        .write_data_2(), 
        .addr_1(), 
        .addr_2(),
        .read_data_1(), 
        .read_data_2()
    );


    color_palettes color_palettes_mem(
        .clk(), 
        .reset(),
        .rw_1(), 
        .rw_2(),
        .write_data_1(), 
        .write_data_2(), 
        .addr_1(), 
        .addr_2(),
        .read_data_1(), 
        .read_data_2()
    );

    OAM OAM_mem (
        .clk(), 
        .reset(), 
        .rw_1(), 
        .rw_2(),
        .write_data_1(),
        .write_data_2(), 
        .addr_1(),
        .addr_2(),
        .read_data_1(), 
        .read_data_2()
    );

    PPU_asm asm( 
        .clk(),
        .reset(),
        .hcount(), 
        .vcount(), 
        .vblank(), 
        .hsync(), 
        .pixel_color(),
        .rw_tile_buffer(), 
        .rw_tile_graphics(), 
        .rw_sprite_graphics(), 
        .rw_color_palettes(), 
        .rw_OAM(),
        .write_data_tile_buffer(), 
        .write_data_tile_graphics(), 
        .write_data_sprite_graphics(), 
        .write_data_OAM(),
        .write_data_color_palettes(),
        .addr_tile_buffer(),
        .addr_tile_graphics(), 
        .addr_sprite_graphics(),
        .addr_color_palettes(),
        .addr_OAM(),
        .read_data_tile_buffer(), 
        .read_data_tile_graphics(), 
        .read_data_sprite_graphics(), 
        .read_data_OAM(),
        .read_data_color_palettes(),
        .shift_load_data(),
        .shift_enable(),
        .shift_load_sprite(), 
        .shift_load_background(),
        .priority_palette_data_out(),
        .priority_pixel_data_in(),
        .priority_palette_data_in()
    );

    combined_priority_encoder priority_encoder (
        .pixel_data_in(),
        .palette_data_in(),
        .pixel_data_out(),
        .palette_data_out()
    ):

    shift_register shift_registers (
        .load_data(),
        .enable(),
        .clk(), 
        .reset(), 
        .load(),
        .out_data()
    );

    vga (
        .clk(),
        .reset(),
        .pixel_color(),
        .VGA_R(), 
        .VGA_G(), 
        .VGA_B(),
        .VGA_CLK(), 
        .VGA_HS(), 
        .VGA_VS(), 
        .VGA_BLANK_n(), 
        .VGA_SYNC_n(),
        .hcount(),
        .vcount()
    );



endmodule