module shift_register(
    input logic [31:0] load_data,
    input logic enable, clk, reset, load,
    output logic [1:0] out_data
);

logic [31:0] shift_buffer;

always @(posedge clk) begin
    if (reset) begin 
        shift_buffer <= 0;
        out_data <= 0;
    end
    else if (load) shift_buffer <= load_data;
    else if (enable) begin
        out_data <= shift_buffer[1:0];
        shift_buffer <= shift_buffer >> 2;
    end
end

endmodule

module shift_register_block(
    input logic [31:0] load_data [8:0],
    input logic clk, reset, load_sprite, load_background,
    input logic [8:0] enable,
    output logic [1:0] out_data [8:0] 
);

    shift_register sprite_shift[7:0] (load_data[7:0], enable[7:0], clk, reset, load_sprite, out_data[7:0]);
    shift_register background_shift (load_data[8], enable[8], clk, reset, load_background, out_data[8]);

endmodule