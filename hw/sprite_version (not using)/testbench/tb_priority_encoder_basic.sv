`timescale 1ns/1ps

module tb_priority_encoder_basic;
  // DUT I/Os
  logic [1:0] pixel_data_in   [8:0];
  logic       palette_data_in [8:0];
  logic [1:0] pixel_data_out;
  logic       palette_data_out;

  // Instantiate your encoder (no `include`—compile DUT separately with +incdir)
  combined_priority_encoder dut (
    .pixel_data_in   (pixel_data_in),
    .palette_data_in (palette_data_in),
    .pixel_data_out  (pixel_data_out),
    .palette_data_out(palette_data_out)
  );
/*Note: to run this just go to project directory 
# 1) Compile both DUT and TB
vlog +incdir+./hw ./hw/priority_encoder.sv ./testbech/tb_priority_encoder_basic.sv

# 2) Launch the simulator in GUI mode
vsim work.tb_priority_encoder_basic

3) once GUI opens # at vsim> 
add wave -recursive /*
run -all

click No when it says do you wanna finish, cuz its gonna exit

*/  
  integer i;

  initial begin
    // --- Test 1: single non-transparent at index 2 ---
    for (i = 0; i < 9; i = i + 1) begin
      pixel_data_in[i]   = 2'b00;
      palette_data_in[i] = 1'b0;
    end
    pixel_data_in[2]   = 2'b11;
    palette_data_in[2] = 1'b1;
    #1;
    $display("Test1 (idx=2): palette_out=%b, pixel_out=%b",
             palette_data_out, pixel_data_out);

    // --- Test 2: slots 1 and 3 both non-transparent (should pick 1) ---
    for (i = 0; i < 9; i = i + 1) begin
      pixel_data_in[i]   = 2'b00;
      palette_data_in[i] = 1'b0;
    end
    pixel_data_in[1]   = 2'b01; palette_data_in[1] = 1'b1;
    pixel_data_in[3]   = 2'b10; palette_data_in[3] = 1'b1;
    #1;
    $display("Test2 (idx=1&3): palette_out=%b, pixel_out=%b",
             palette_data_out, pixel_data_out);

    $finish;
  end
endmodule
