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


endmodule