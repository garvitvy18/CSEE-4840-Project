module addr_decode (
    input logic [11:0] addr,
    input logic [31:0] write_data,
    input logic chip_select,
    input logic write,
     
    //RW Signals to Memories
    output logic rw_tile_buffer, 
    output logic rw_tile_graphics, 
    output logic rw_sprite_graphics, 
    output logic rw_color_palettes, 
    output logic rw_OAM,

    //Write Data to Memories
    output logic [31:0] write_data_tile_buffer, 
    output logic [31:0] write_data_tile_graphics, 
    output logic [31:0] write_data_sprite_graphics, 
    output logic [31:0] write_data_OAM,
    output logic [23:0] write_data_color_palettes,

    //Address Signals to Memories
    output logic [8:0] addr_tile_buffer,
    output logic [10:0] addr_tile_graphics, 
    output logic [10:0] addr_sprite_graphics,
    output logic [2:0] addr_color_palettes,
    output logic [7:0] addr_OAM
);
    always @(*) begin
        rw_tile_buffer <= 0;
        rw_tile_graphics <= 0;
        rw_sprite_graphics <= 0;
        rw_color_palettes <= 0;
        rw_OAM <= 0;
        write_data_tile_buffer <= 0;
        write_data_tile_graphics <= 0;
        write_data_sprite_graphics <= 0;
        write_data_OAM <= 0;
        write_data_color_palettes <= 0;
        addr_tile_buffer <= 0;
        addr_tile_graphics <= 0;
        addr_sprite_graphics <= 0;
        addr_color_palettes <= 0;
        addr_OAM <= 0;
        casez (addr[11:9])
            3'b00_: begin //Tile Buffer
                if(chip_select && write) begin
                    rw_tile_buffer <= 1;
                    addr_tile_buffer <= addr;
                    write_data_tile_buffer <= write_data;
                end
            end
            3'b01_: begin //Tile Graphics
                if(chip_select && write) begin
                    rw_tile_graphics <= 1;
                    addr_tile_graphics <= addr;
                    write_data_tile_graphics <= write_data;
                end
            end
            3'b10_: begin //Sprite Graphics
                if(chip_select && write) begin
                    rw_sprite_graphics <= 1;
                    addr_sprite_graphics <= addr;
                    write_data_sprite_graphics <= write_data;
                end
            end
            3'b110: begin //Color Palettes
                if(chip_select && write) begin
                    rw_color_palettes <= 1;
                    addr_color_palettes <= addr;
                    write_data_color_palettes <= write_data;
                end
            end
            3'b111: begin //OAM
                if(chip_select && write) begin
                    rw_OAM <= 1;
                    addr_OAM <= addr;
                    write_data_OAM <= write_data;
                end
            end
        endcase
    end
    
endmodule