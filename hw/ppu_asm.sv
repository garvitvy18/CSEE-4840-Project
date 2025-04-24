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

    //Once per line
    logic [1279:0] background_line_buffer;
    logic [2:0] [31:0] sprite_graphics_buffer;
    logic [2:0] [6:0] sprites_on_line;

    //Once per frame
    logic [2:0] [23:0] color_palette_buffer;
    logic [6:0] [15:0] sprite_x_buffer;
    logic [6:0] [15:0] sprite_y_buffer;
    logic [6:0] sprite_palette_buffer;
    logic [6:0] [6:0] sprite_tile_id_buffer;
    logic [6:0] [1:0] sprite_rotation_buffer;

    //Vblank memory access pointers
    logic [7:0] cords_sprite_load;
    logic [7:0] palette_sprite_load;
    logic [3:0] palette_ram_pointer;
    

    //Hsync memory access pointers
    logic [5:0] background_line_pointer;
    logic [2:0] sprite_graphics_pointer;

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
            background_line_pointer <= 0;
            sprite_graphics_pointer <= 0;

        end

        //set buffers that fill once per frame
        else if (vblank) begin

            //fill color_palette_buffer
            if (palette_ram_pointer == 0) begin
                rw_color_palettes <= 0;
                addr_color_palettes <= palette_ram_pointer;
                palette_ram_pointer <= palette_ram_pointer + 1;
            end

            else if (palette_ram_pointer < 8) begin
                rw_color_palettes <= 0;
                addr_color_palettes <= palette_ram_pointer;
                color_palette_buffer[palette_ram_point - 1] = read_data_color_palettes;
                palette_ram_pointer <= palette_ram_pointer + 1;
            end

            else if (palette_ram_point = 8) begin
                rw_color_palettes <= 0;
                addr_color_palettes <= 0;
                color_palette_buffer[palette_ram_point - 1] = read_data_color_palettes;
                palette_ram_pointer <= palette_ram_pointer + 1;
            end

            else begin
                rw_color_palettes <= 0;
                addr_color_palettes <= 0;
            end
            
            //fill sprite_x_buffer and sprite_y_buffer
            if (cords_sprite_load == 0) begin
                rw_OAM <= 0;
                addr_OAM <= cords_sprite_load * 2 + 1;
                cords_sprite_load <= cords_sprite_load + 1;
            end

            else if (cords_sprite_load < 128) begin
                rw_OAM <= 0;
                addr_OAM <= cords_sprite_load * 2 + 1;
                sprite_x_buffer[cords_sprite_load - 1] <= read_data_OAM[15:0];
                sprite_y_buffer[cords_sprite_load - 1] <= read_data_OAM[31:16];
                cords_sprite_load <= cords_sprite_load + 1;
            end

            else if (cords_sprite_load == 128) begin
                rw_OAM <= 0;
                addr_OAM <= palette_sprite_load;
                sprite_x_buffer[cords_sprite_load - 1] <= read_data_OAM[15:0];
                sprite_y_buffer[cords_sprite_load - 1] <= read_data_OAM[31:16];
                cords_sprite_load <= cords_sprite_load + 1;
                palette_sprite_load <= palette_sprite_load + 1;
            end

            else if (palette_sprite_load < 128) begin
                rw_OAM <= 0;
                addr_OAM <= palette_sprite_load * 2;
                sprite_palette_buffer[palette_sprite_load - 1] <= read_data_OAM[7];
                sprite_tile_id_buffer[palette_sprite_load - 1] <= read_data_OAM[6:0];
                sprite_rotation_buffer[palette_sprite_load - 1] <= read_data_OAM[31:30];
                palette_sprite_load <= palette_sprite_load + 1;
            end

            else if (palette_sprite_load == 128) begin
                rw_OAM <= 0;
                addr_OAM <= 0;
                sprite_palette_buffer[palette_sprite_load - 1] <= read_data_OAM[7];
                sprite_tile_id_buffer[palette_sprite_load - 1] <= read_data_OAM[6:0];
                sprite_rotation_buffer[palette_sprite_load - 1] <= read_data_OAM[31:30];
                palette_sprite_load <= palette_sprite_load + 1;
            end

            else begin
                rw_OAM <= 0;
                addr_OAM <= 0;
            end

        end

        else if (hsync) begin

            //Load background tiles into buffer
            if (background_line_pointer == 0) begin
                rw_tile_graphics 

            end

            else if (background_line_pointer < 40) begin
                rw_tile_graphics
            end

            else if (background_line_pointer == 40) begin


            end
            
            //Detect which sprites are on the line


            //Calculate pointers based on rotation flags and what line we are on

                



        end
        else begin
            //Reset vblank memory pointers
            coords_sprite_load <= 0;
            palette_sprite_load <= 0;
            palette_ram_pointer <= 0;



        end
    end


endmodule


