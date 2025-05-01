module tile_buffer(
    input logic clk, reset, rw,
    input logic [31:0] write_data, 
    input logic [8:0] addr,
    output logic [31:0] read_data
);

    logic tile_buffer_array [299:0] [31:0];

    always @(posedge clk) begin
        if (rw) tile_buffer_array[addr] <= write_data;
        else read_data <= tile_buffer_array[addr];
    end

endmodule 

module tile_graphics(
    input logic clk, reset, rw,
    input logic [31:0] write_data, 
    input [10:0] addr,
    output logic [31:0] read_data
);

    logic tile_graphics_array [2047:0] [31:0];

    always @(posedge clk) begin
            if (rw) tile_graphics_array[addr] <= write_data;
            else read_data <= tile_graphics_array[addr];
    end

endmodule 

module sprite_graphics(
    input logic clk, reset, rw,
    input logic [31:0] write_data, 
    input [10:0] addr,
    output logic [31:0] read_data
);

    logic sprite_graphics_array [2047:0] [31:0];

    always @(posedge clk) begin
            if (rw) sprite_graphics_array[addr] <= write_data;
            else read_data <= sprite_graphics_array[addr];
    end

endmodule 

module color_palettes(
    input logic clk, reset, rw,
    input logic [23:0] write_data, 
    input [2:0] addr,
    output logic [23:0] read_data
);

    logic color_palette_array [7:0] [23:0];

    always @(posedge clk) begin
            if (rw) color_palette_array[addr] <= write_data;
            else read_data <= color_palette_array[addr];
    end

endmodule

module OAM (
    input logic clk, reset, rw,
    input logic [31:0] write_data, 
    input [7:0] addr,
    output logic [31:0] read_data
)

    logic OAM_array [255:0] [31:0];

    always @(posedge clk) begin
            if (rw) OAM_array[addr] <= write_data;
            else read_data <= OAM_array[addr];
    end

endmodule