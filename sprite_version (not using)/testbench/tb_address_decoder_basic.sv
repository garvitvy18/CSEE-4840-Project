`timescale 1ns/1ps

module tb_address_decoder_basic;
  // Inputs
  logic [11:0] addr;
  logic [31:0] write_data;
  logic        chip_select;
  logic        write;
  // Outputs
  logic        rw_tile_buffer;
  logic        rw_tile_graphics;
  logic        rw_sprite_graphics;
  logic        rw_color_palettes;
  logic        rw_OAM;

  logic [31:0] write_data_tile_buffer;
  logic [31:0] write_data_tile_graphics;
  logic [31:0] write_data_sprite_graphics;
  logic [31:0] write_data_OAM;
  logic [23:0] write_data_color_palettes;

  logic [ 8:0] addr_tile_buffer;
  logic [10:0] addr_tile_graphics;
  logic [10:0] addr_sprite_graphics;
  logic [ 2:0] addr_color_palettes;
  logic [ 7:0] addr_OAM;

  // Instantiate DUT (compile with +incdir+./hw)
  addr_decode dut (
    .addr                     (addr),
    .write_data               (write_data),
    .chip_select              (chip_select),
    .write                    (write),

    .rw_tile_buffer           (rw_tile_buffer),
    .rw_tile_graphics         (rw_tile_graphics),
    .rw_sprite_graphics       (rw_sprite_graphics),
    .rw_color_palettes        (rw_color_palettes),
    .rw_OAM                   (rw_OAM),

    .write_data_tile_buffer       (write_data_tile_buffer),
    .write_data_tile_graphics     (write_data_tile_graphics),
    .write_data_sprite_graphics   (write_data_sprite_graphics),
    .write_data_OAM               (write_data_OAM),
    .write_data_color_palettes    (write_data_color_palettes),

    .addr_tile_buffer         (addr_tile_buffer),
    .addr_tile_graphics       (addr_tile_graphics),
    .addr_sprite_graphics     (addr_sprite_graphics),
    .addr_color_palettes      (addr_color_palettes),
    .addr_OAM                 (addr_OAM)
  );

  integer i;

  initial begin
    $display("\n=== Address Decoder Basic Tests ===");

    // Test1: chip_select=0 → all rw_* should be 0
    chip_select = 0; write = 1; addr = 12'hABC; write_data = 32'hDEADBEEF; #1;
    if (|{rw_tile_buffer,rw_tile_graphics,rw_sprite_graphics,
          rw_color_palettes,rw_OAM})
      $error("FAIL Test1: RW asserted when chip_select=0");

    // Test2: write=0 → all rw_* should be 0
    chip_select = 1; write = 0; addr = 12'h123; write_data = 32'hCAFEBABE; #1;
    if (|{rw_tile_buffer,rw_tile_graphics,rw_sprite_graphics,
          rw_color_palettes,rw_OAM})
      $error("FAIL Test2: RW asserted when write=0");

    // Test3: Tile Buffer (addr[11:9]==3'b00_)
    chip_select=1; write=1; addr=12'b000_1010_0101; write_data=32'hAAAA_AAAA; #1;
    if (!rw_tile_buffer) $error("FAIL Test3: rw_tile_buffer not asserted");
    if (addr_tile_buffer !== addr[8:0])
      $error("FAIL Test3: addr_tile_buffer = %h, expected %h", addr_tile_buffer, addr[8:0]);
    if (write_data_tile_buffer !== write_data)
      $error("FAIL Test3: write_data_tile_buffer = %h", write_data_tile_buffer);

    // Test4: Tile Graphics (01_)
    chip_select=1; write=1; addr=12'b010_1111_0000; write_data=32'hBBBB_BBBB; #1;
    if (!rw_tile_graphics) $error("FAIL Test4: rw_tile_graphics not asserted");
    if (addr_tile_graphics !== addr[10:0])
      $error("FAIL Test4: addr_tile_graphics = %h", addr_tile_graphics);

    // Test5: Sprite Graphics (10_)
    chip_select=1; write=1; addr=12'b100_0000_1111; write_data=32'hCCCC_CCCC; #1;
    if (!rw_sprite_graphics)$error("FAIL Test5: rw_sprite_graphics not asserted");
    if (addr_sprite_graphics !== addr[10:0])
      $error("FAIL Test5: addr_sprite_graphics = %h", addr_sprite_graphics);

    // Test6: Color Palettes (110)
    chip_select=1; write=1; addr=12'b110_0000_001; write_data=32'hDDDD_DDDD; #1;
    if (!rw_color_palettes) $error("FAIL Test6: rw_color_palettes not asserted");
    if (addr_color_palettes !== addr[2:0])
      $error("FAIL Test6: addr_color_palettes = %h", addr_color_palettes);

    // Test7: OAM (111)
    chip_select=1; write=1; addr=12'b111_1010_1010; write_data=32'hEEEE_EEEE; #1;
    if (!rw_OAM) $error("FAIL Test7: rw_OAM not asserted");
    if (addr_OAM !== addr[7:0])
      $error("FAIL Test7: addr_OAM = %h", addr_OAM);

    $display("\n=== All Tests Passed ===");
    $finish;
  end
endmodule
