module PPU_asm( 
    input logic clk,
    input logic reset,
    input logic [31:0] write_data,
    input logic write,
    input chipselect,

    input logic [10:0] hcount,
    input logic [9:0] vcount,
    input logic vblank,
    input logic hsync,

    output logic rw_tile_buffer, rw_tile_graphics, rw_sprite_graphics, rw_color_palettes, rw_OAM,
    output logic [31:0] write_data_tile_buffer, write_data_tile_graphics, write_data_sprite_graphics, write_data_OAM,
    output logic [23:0] write_data_color_palettes,
    output logic [8:0] addr_tile_buffer,
    output logic [10:0] addr_tile_graphics, addr_sprite_graphics,
    output logic [2:0] addr_color_palettes,
    output logic [7:0] addr_OAM,

    input logic [31:0] read_data_tile_buffer, read_data_tile_graphics, read_data_sprite_graphics, read_data_OAM,
    input logic [23:0] read_data_color_palettes
);

    logic [10:0] prev_hcount;
    logic [9:0] prev_vcount;

    //Once per line
    logic [1279:0] background_line_buffer;
    logic [6:0] [6:0] sprite_on_line_buffer;

    //Once per frame
    logic [6:0] [15:0] sprite_x_buffer;
    logic [6:0] [15:0] sprite_y_buffer;
    logic [127:0] sprite_palette_buffer;
    logic [6:0] [23:0] color_palette_buffer;

    //Vblank memory access pointers
    logic [6:0] coords_sprite_load;
    logic [6:0] palette_sprite_load;
    logic [2:0] palette_ram_pointer;


    always @(posedge clk) begin
        
        //reset 
        if (reset) begin
            rw_tile_buffer <= 0;
            rw_tile_graphic <= 0;
            rw_sprite_graphics <= 0;
            rw_color_palettes <= 0;
            rw_OAM <= 0;
            prev_hcount <= 0;
            prev_vcount <= 0;
            coords_sprite_load <= 0;
            palette_sprite_load <= 0;
            palette_ram_pointer <= 0;
        end

        //set buffers that fill once per frame
        else if (vblank) begin
            
            //fill sprite_x_buffer

            //fill sprite_y_buffer

            //fill sprite_palette_buffer

            //fill color_palette_buffer

        end

        else if (hsync) begin
            

        end
        else begin
            //Reset vblank memory pointers
            coords_sprite_load <= 0;
            palette_sprite_load <= 0;
            palette_ram_pointer <= 0;
        end
    end


endmodule


