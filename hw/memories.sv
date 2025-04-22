module tile_buffer(
    input logic clk, reset, rw,
    input logic [31:0] write_data, 
    input [8:0] addr,
    output logic [31:0] read_data
);

    logic tile_buffer_array [299:0] [31:0];

endmodule 

module tile_graphics(
    input logic clk, reset, rw,
    input logic [31:0] write_data, 
    input [10:0] addr,
    output logic [31:0] read_data
);

    logic tile_graphics_array [2047:0] [31:0];

endmodule 

module sprite_graphics(
    input logic clk, reset, rw,
    input logic [31:0] write_data, 
    input [10:0] addr,
    output logic [31:0] read_data
);

    logic sprite_graphics_array [2047:0] [31:0];

endmodule 

module color_palettes(
    input logic clk, reset, rw,
    input logic [23:0] write_data, 
    input [2:0] addr,
    output logic [23:0] read_data
);

    logic color_palette_array [7:0] [23:0];

endmodule

module OAM (
    input logic clk, reset, rw,
    input logic [31:0] write_data, 
    input [7:0] addr,
    output logic [31:0] read_data
)

    logic OAM_array [255:0] [31:0];

endmodule